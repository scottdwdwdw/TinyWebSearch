#ifndef HTTP_H
#define HTTP_H

//http file 
//1 . store the http require line
//2 . store the http header
//3 . get the http data

#include <iostream>
#include <vector>
#include <sstream>
#include <string>

#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include "rio.h"
#include "debug.h"
#include "http_data_structure.h"
#include "http_error.h"
#include "http_out_filter.h"
#include "epoll.h"

using namespace std;




class HttpRequest
{
    public:
        HttpRequest();
        HttpRequest(int f,int ef);
        ~HttpRequest();
        HttpRequest&operator=(HttpRequest&)=delete;
        HttpRequest&operator=(HttpRequest&&)=delete;
        void InitHttpRequest(Epoll*epp,int f,int ef);
        void CommandParse();
        void HeaderParse();
        void BodyParse();
        void* RequestParse(void*arg);
        int GetFd() const {return fd;}

        int StaticServer();
        int DynamicServer();
        void ParamParse();
        
    private:
        int fd;
        int epfd;
        char *usr_buf;
	    struct HttpCommandLine command;     //store command line
	    struct HttpHeader head;
	    struct HttpBody http_data;
        struct stat file_stat;
        string read_data;
        HttpOut out; 
        map<string,string>parameters;       //save the query parameters
        Epoll*ep;
};








#endif