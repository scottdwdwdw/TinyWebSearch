
#include <iostream>
#include "http_error.h"
using namespace std;


int HttpError::RequireCheck(HttpCommandLine&command_line,HttpHeader&header,HttpOut&out,struct stat&stat_buf)
{
	//check method
	if(global_require_method.find(command_line.method)==global_require_method.end())  //illegal method
    {
        out.SetState(D_METHOD_NOT_ALLOWED);
		return D_METHOD_NOT_ALLOWED;
    }
    //check http method
	if(global_http_version != command_line.version)
    {
        out.SetState(D_HTTP_VERSION_NOT_SUPPORTED);
        return D_HTTP_VERSION_NOT_SUPPORTED;
    }
		

    //check if it is static serer or dynamic server
    //TO-DO   check if it is dynamic server
    //first if it is static server
    size_t pos = command_line.uri.find_first_of("?");
    string file_path="";
    if(pos!=string::npos)//this is dynamic server
    {
        out.SetStaticServer(false);
        string uri_temp = command_line.uri.substr(0,pos);
        file_path=cgi_file_root+uri_temp;    //get the cgi file 
    }
    else
    {
         if(command_line.uri == "/")command_line.uri = "/test.html";  // note the '/'
         file_path = file_root+command_line.uri; 
         out.SetStaticServer(true);
    }

   
    int file_stat = stat(file_path.c_str(),&stat_buf);
    //debug_cpp("file path "+file_path);
    if(file_stat<0)
    {
        out.SetState(D_NOT_FOUND);
        return D_NOT_FOUND;
    }
    if(!S_ISREG(stat_buf.st_mode)||!(S_IRUSR&stat_buf.st_mode))
    {
        out.SetState(D_NOT_ALLOWED);
        return D_NOT_ALLOWED;
    }

    out.SetState(D_OUT_OK);
    out.SetModTime(stat_buf.st_mtime);
    out.SetFilePath(file_path);
	return D_OK;
}

/*
int HttpError::DynamicServerCheck(HttpCommandLine&command_line,HttpHeader&header,HttpOut&out,struct stat&stat_buf)
{

}
*/
string HttpError::ClientError(int state)
{
    //string error_message;
    string error_num = to_string(state);
    error_message+="<html><title>Dserver Error</title>";
    error_message+="<body bgcolor=""ffffff"">\r\n";
    error_message+=error_num+" :"+state_code[state]+"\r\n";
    error_message+="<p>sorry , there exist some problem.\r\n";
    error_message+="<hr><em>"+server_name+"</em>\r\n";

    return error_message;
}