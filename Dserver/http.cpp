#include <memory.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/epoll.h>
#include "http.h"


HttpRequest::HttpRequest():fd(0),epfd(0)
{
	usr_buf = new char[RIO_BUFSIZE];
}

HttpRequest::HttpRequest(int f,int ef):fd(f),epfd(ef)
{
   usr_buf = new char[RIO_BUFSIZE];
}

void HttpRequest::InitHttpRequest(Epoll*epp,int f,int ef)
{
	fd = f;
	epfd = ef;
    ep = epp;
    if(usr_buf == NULL)usr_buf = new char[RIO_BUFSIZE];
}


void HttpRequest::CommandParse()
{
    if(!read_data.size())return;
    istringstream data(read_data);
    string line;
    getline(data,line);      //getline not include the last \n
    read_data = read_data.substr(line.size()+1);  //delete the request line,
	istringstream request_line(line);
    string word;
    vector<string>store_temp;
    while(request_line>>word)
    {
        store_temp.push_back(word);
    }
    command.method = store_temp[0];
    command.uri = store_temp[1];
    command.version = store_temp[2];
}

void HttpRequest::HeaderParse()
{
    if(!read_data.size())return;
    istringstream data(read_data);
    string line;
    int count = 0;
    while(getline(data,line))
    {
        count+=line.size()+1;          //record the lenght of data;/r/n
        istringstream word(line);
        string name;
        string value;
        while(word>>name)
        {
            name=name.substr(0,name.size()-1);
            getline(word,value);
            head.head[name] = value.substr(1); //delete the space
        }
    }
    //delete the head data;
    read_data = read_data.substr(count);
}

void HttpRequest::BodyParse()
{
    if(!read_data.size())return;
    http_data.data = read_data;
}

//
void* HttpRequest::RequestParse(void*arg)
{
	int n;
    int again_flag = true;   //again flag decide to close the fd or try again
    while(true)
    {
        errno = 0;                        //this is important
        memset(usr_buf,0,RIO_BUFSIZE);    //reset the buf
        n = rio_readn(fd,(void*)usr_buf,RIO_BUFSIZE-1);
        
        if(n==-1)
        {
            debug("error.");
            again_flag = false;
    	    break;
        }
        else if(n==0)
        {
            if(errno==EAGAIN)
            {
                //debug("again read");
                again_flag = true;
            }
            else  //EOF
            {
                //debug("EOF read close fd");
                again_flag = false;
            }
            //debug("there is not any more data,errno is: %d",errno);
            break;
        }

        read_data = usr_buf;
        /*
        cout<<"read data:--------------------------------------------"<<endl;
        cout<<read_data<<endl;
        cout<<"-------------------------------------------------------"<<endl;
        */
        //debug_cpp(read_data.size());
        CommandParse();    //requese line parse
        HeaderParse();     //head parse

        //int data_n = read_data.size();
        if(read_data.size()!=0)
        {
            BodyParse();   //body parse
        }

        /*parse complete,do method,url ,version check*/
        HttpError e;
        int check_res = e.RequireCheck(command,head,out,file_stat);


        if(check_res!=D_OK)
        {

            //debug("require error: %d",check_res);    //then send the wrong information page 
            command.uri = "/40x.html";
            // reset the file_stat;
            string file_name = file_root+command.uri;
            stat(file_name.c_str(),&file_stat);
            out.SetStaticServer(true);
        }
        
        /**********check over,send the result to the client********/

        if(out.GetStaticServer())  
        {
              //set the length
            out.SetFileSize(file_stat.st_size);
        
            auto out_line = out.SetOutLine_C();
            out_line += out.SetOutHeaders_C(head); 
            char* res_line_buf = const_cast<char*>(out_line.c_str());
            n = rio_writen(fd,(void*)res_line_buf,strlen(res_line_buf));  //write command line

            //debug("send Response line and Out Header.");
            StaticServer();
            //debug("----------static server over-----------");
        }
        else //dynamic server
        {
            //debug("----------dynamic server on-------------");
            DynamicServer();
            //debug("----------dynamic server over-----------");
        }

        //server -------**over**----------
        
        if(head.head.find("Connection")==head.head.end()) //keep alive
        {
            again_flag = false;
            //debug("-----------no keep alive,will break---------");
            break;
        }
        else
        {
            again_flag = true;
        }
        
    }
    
    if(!again_flag)
    {
      //debug("close fd ,%d",fd);
      close(fd);    //close fd ,there is no other request
    }
    else
    {
        
        head.head.clear();                //clear the request head
        read_data="";
        again_flag = true;
        out.clear();                      //clear the output data
        

        InitHttpRequest(ep,fd,epfd);      //no need to initialize.because the ep,fd,and the epfd etc do not change
        ep->SetEpollEvent((void*)this,D_EPOLLIN|D_EPOLLET|D_EPOLLONESHOT);  //set event
        ep->EpollCTL(D_EPOLL_CTL_MOD,fd); //add fd to the event module

      
    }

    return NULL;
}


int HttpRequest::StaticServer()
{
    string file_name = file_root+command.uri;
    //char* file_name_c = const_cast<char*>(file_name.c_str());
    int srcfd = open(file_name.c_str(),O_RDONLY,0);   //open file
    if(srcfd<0)
    {
        debug("open file error.");
        return -1;
    }
    char *srcptr = (char*)mmap(NULL,file_stat.st_size,PROT_READ,MAP_PRIVATE,srcfd,0);
    if(srcptr==(void*)-1)
    {
        debug("mmap error");
        return -1;
    }
    
    //successful
    int n = rio_writen(fd,(void*)srcptr,file_stat.st_size);
    //debug("write file successful,write num :%d  file_size:%ld",n,file_stat.st_size);
    
    close(srcfd);

    munmap(srcptr,file_stat.st_size);


    return 0;
}

//dynamic server
int HttpRequest::DynamicServer()
{
    //first analyse the parameter
    ParamParse();
    //call the function ,and send the parameter
    for(auto&e:parameters)
    {
        cout<<e.first<<" : "<<e.second<<endl;
    }

    return 0;
}


void HttpRequest::ParamParse()
{
     size_t pos = command.uri.find_first_of("?");
    string cgi_name = command.uri.substr(0,pos);
    string file_name = cgi_file_root+cgi_name;
    string parameter_s = command.uri.substr(pos+1);
    string key="";
    string value = "";
    while(parameter_s!="")
    {
        pos = parameter_s.find_first_of("&");
        if(pos==string::npos)//the last word
        {
            size_t pos_temp = parameter_s.find_first_of("=");
            if(pos_temp==string::npos)
            {
                debug("parameter pharse error.");
                key="";
                value="";
            }
            else
            {
                key = parameter_s.substr(0,pos_temp);
                value = parameter_s.substr(pos_temp+1);
            }
             parameter_s="";
        }
        else
        {
            string key_value = parameter_s.substr(0,pos);
            size_t pos_temp = key_value.find_first_of("=");
            if(pos_temp==string::npos)
            {
                debug("parameter pharse error.");
                key=value="";
            }
            else
            {
                key=key_value.substr(0,pos_temp);
                if(pos_temp+1==key_value.size())value="";
                else
                {
                    value=key_value.substr(pos_temp+1);
                }
            }
            parameter_s = parameter_s.substr(pos+1);
        }

        parameters[key]=value;
    }
}


