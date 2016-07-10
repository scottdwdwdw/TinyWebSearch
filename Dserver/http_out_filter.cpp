#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

//#include <time.h>
#include <math.h>
#include <string>
#include <map>
#include <iostream>
#include "http_out_filter.h"

using namespace std;

//header callback function
typedef int (*http_header_handler)(HttpOut&out,struct HttpHeader&in);

struct head_handle
{
    string name;
    http_header_handler hd;
};

//head process function
int http_header_connection_handle(HttpOut&out,struct HttpHeader&in);
int http_header_IfModified_handle(HttpOut&out,struct HttpHeader&in);
int http_header_server_handle(HttpOut&out,struct HttpHeader&in);
int http_header_date_handle(HttpOut&out,struct HttpHeader&in);
int http_header_content_type_handle(struct HttpOut&out,struct HttpHeader&in);
int http_heaer_Content_length_handle(struct HttpOut&out,struct HttpHeader&in);

//array of callback function
struct head_handle head_handle_list[] = 
    {
    	{"Connection",http_header_connection_handle},
    	{"If-Modified-Since",http_header_IfModified_handle},
        {"Server",http_header_server_handle},
        {"Content-Type",http_header_content_type_handle},
        {"Content-length",http_heaer_Content_length_handle},
        {"",NULL}
    };


 map<string,string>server_mime={
    {".html", "text/html"},
    {".xml", "text/xml"},
    {".xhtml", "application/xhtml+xml"},
    {".txt", "text/plain"},
    {".rtf", "application/rtf"},
    {".pdf", "application/pdf"},
    {".word", "application/msword"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".au", "audio/basic"},
    {".mpeg", "video/mpeg"},
    {".mpg", "video/mpeg"},
    {".avi", "video/x-msvideo"},
    {".gz", "application/x-gzip"},
    {".tar", "application/x-tar"},
    {".css", "text/css"},
    {".js","application/javascript"},
    {"" ,"text/plain"}
};

map<int,string>state_code={
    {200,"OK"},
    {304,"NOT MODIFIED"},
    {403,"NOT ALLOWED"},
    {404,"NOT FOUND"},
    {405,"METHOD NOT ALLOWED"},
    {505,"HTTP VERSION NOT SUPPORTED"}
};

//set keep-alive
int http_header_connection_handle(struct HttpOut&out,struct HttpHeader&in)
{
	if(in.head["Connection"]!="")
	{
		//out.head_out.head["Connection"] = "Keep-alive";
		out.SetHeadItems("Connection","Keep-alive");
	}

	return D_OK;
}

//set if modified
int http_header_IfModified_handle(struct HttpOut&out,struct HttpHeader&in)
{
	if(in.head["If-Modified-Since"]!="")
	{
		string require_time = in.head["If-Modified-Since"];  
	    struct tm tm;
        if (strptime(require_time.c_str(), "%a, %d %b %Y %H:%M:%S GMT", &tm) == (char *)NULL) {
        return D_OK;
        }
        time_t client_time = mktime(&tm);
        double time_diff = difftime(out.GetModTime(), client_time);
        if (fabs(time_diff) < 1e-6) {
        	out.SetState(D_NOT_MODIFIED);
        }
	}
	
    return D_OK;
}

//set servet name
int http_header_server_handle(struct HttpOut&out,struct HttpHeader&in)
{
	 //out.head_out.head["Server"] = server_name;
	out.SetHeadItems("Server",server_name);
	return D_OK;
}

int http_header_content_type_handle(struct HttpOut&out,struct HttpHeader&in)
{
    string path = out.GetFilePath();
    string suffix = "";
    size_t pos = path.find_last_of(".");
    if(pos==std::string::npos)suffix="";
    else
    {
       suffix = path.substr(pos);
    }
    //cout<<"suffix is: "<<suffix<<endl;
    if(server_mime.find(suffix)==server_mime.end())suffix="";
    out.SetHeadItems("Content-Type",server_mime[suffix]);
    return D_OK;
}

//content-length
int http_heaer_Content_length_handle(struct HttpOut&out,struct HttpHeader&in)
{
    int file_size = out.GetFileSize();
   // if(file_size<0)out.SetHeadItems("Content-length","-1");
    string file_length = to_string(file_size);
    out.SetHeadItems("Content-length",file_length);

    //cout<<"file length : "<<file_length<<endl; 
    return D_OK;
}

/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/

string HttpOut::SetOutLine_C()
{
	SetOutLine();
	string state = to_string(out_line.state);
	string line = out_line.version+" "+state+" "+out_line.code+"\r\n";

    //cout<<"Responseline is: "<<line<<endl;
	
	//return const_cast<char*>(line.c_str());
    return line;
}

void HttpOut::SetOutLine()
{
	out_line.version = global_http_version;
	out_line.state = state;
	out_line.code = state_code[state];   //this is not robust
}




//
void HttpOut::SetOutHeaders(struct HttpHeader in)
{
	//set header
	for(int i=0;head_handle_list[i].name!="";++i)
	{
		head_handle_list[i].hd(*this,in);
	}

}

string HttpOut::SetOutHeaders_C(struct HttpHeader in)
{
    SetOutHeaders(in);   //header set successful
    string head_s;
    for(auto&e:head_out.head)
    {
        head_s+=e.first+":"+e.second+"\r\n";
    }
    head_s+="\r\n";
   
    //cout<<"out header: "<<head_s;

    //return const_cast<char*>(head_s.c_str());
    return head_s;
}

/*
size_t HttpOut::GetFileSize()
{

    int r = stat(file_path.c_str(),&file_stat);
    if(r<0)
    {
        return -1;
    }
    return file_stat.st_size;
}
*/

void HttpOut::clear()
{

    head_out.head.clear();          //response head
    data_out.data = "";            //responser data
}


