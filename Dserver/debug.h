//debug.h

#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

#ifdef NDEBUG
#define debug(M,...)
#else
#define debug(M,...) fprintf(stderr,"DEBUG: %s %d " M "\n",__FILE__,__LINE__, ##__VA_ARGS__)  //note:the last argument is ##__VA_ARGS__
#endif

#define debug_info(M) perror(M)      //this will give the error infomation
#define debug_cpp(M)  cout<<__FILE__<<" "<<__LINE__<<" "<<M<<endl;




#endif