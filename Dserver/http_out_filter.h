#ifndef HTTP_OUT_FILTER_H
#define HTTP_OUT_FILTER_H

#include <time.h>
#include <sys/stat.h>
#include "http_data_structure.h"




class HttpOut
{

public:
	HttpOut(){}
   // HttpOut(struct )
    void SetOutLine();
    //char*SetOutLine_C();
    string SetOutLine_C();
    void SetOutHeaders(struct HttpHeader in);
    //char*SetOutHeaders_C(struct HttpHeader in);
    string SetOutHeaders_C(struct HttpHeader in);
    void SetOutBody();
    inline void SetState(int s){state = s;};
    inline int GetState() const {return state;}

    inline void SetModTime(time_t t){modif_time = t;}
    inline void SetHeadItems(string key,string val){head_out.head[key] = val;}
    inline time_t GetModTime(){return modif_time;}

    inline void SetFilePath(string&path){file_path = path;}
    inline string GetFilePath(){return file_path;}
    
    inline void SetStaticServer(bool state){static_server = state;}
    inline bool GetStaticServer() const {return static_server;}


    size_t GetFileSize()const{return file_length;}
    void SetFileSize(size_t n){file_length = n;}

    ~HttpOut(){}
    void clear();
private:
	struct HttpResponseLine out_line;      //out line
	struct HttpHeader head_out;            //response head
	struct HttpBody   data_out;            //responser data
	int state;
	time_t modif_time;
    string file_path;
	//struct stat file_stat;
    size_t file_length;
    bool static_server;
};


extern map<int,string>state_code;
extern map<string,string>server_mime;


#endif