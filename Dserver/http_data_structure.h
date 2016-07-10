#ifndef HTTP_DATA_STRUCTURE_H
#define HTTP_DATA_STRUCTURE_H

#include <string>
#include <map>
#include <set>
#include <initializer_list>

using namespace std;

#define D_OK 1
#define D_ERROR 2

#define D_OUT_OK 200
#define D_NOT_MODIFIED 304
#define D_NOT_ALLOWED 403
#define D_METHOD_NOT_ALLOWED 405
#define D_NOT_FOUND 404
#define D_HTTP_VERSION_NOT_SUPPORTED 505

const string server_name = "DServer/v0.1";
const set<string>global_require_method = {"GET","POST","HEAD"};  
const string global_http_version = "HTTP/1.1";   
const string file_root = "/home/scottdw/web_server";    //should read from configure file
const string cgi_file_root = "/home/scottdw/web_server/cgi-bin";

/*
map<int,string>state_code={
    {200,"OK"},
    {304,"NOT MODIFIED"},
    {403,"NOT ALLOWED"},
    {404,"NOT FOUND"},
    {405,"METHOD NOT ALLOWED"},
    {505,"HTTP VERSION NOT SUPPORTED"}
};
*/

//http require line
struct HttpCommandLine
{
    string method;
    string uri;
    string version;
};

//responser line
struct HttpResponseLine
{
    string version;
    int state;
    string code;
};

//http header ,use a list to store elements
struct HttpHeader
 {
    map<string,string>head;
 };

 //http body
 struct HttpBody
 {
    string data;
 };




 #endif    
