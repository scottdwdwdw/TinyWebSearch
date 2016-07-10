#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>
#include "debug.h"

#define D_EPOLL_CTL_ADD EPOLL_CTL_ADD
#define D_EPOLL_CTL_MOD EPOLL_CTL_MOD
#define D_EPOLL_CTL_DEL EPOLL_CTL_DEL

#define D_EPOLLIN EPOLLIN
#define D_EPOLLOUT EPOLLOUT
#define D_EPOLLPRI EPOLLPRI
#define D_EPOLLERR EPOLLERR
#define D_EPOLLHUP EPOLLHUP
#define D_EPOLLET EPOLLET
#define D_EPOLLONESHOT EPOLLONESHOT

#define MAX_EVENTS 1024

class Epoll
{
   public:
		Epoll();     //construct ,allocate memory for events
		~Epoll();
		int EpollCreate();
		int EpollCTL(int mod,int fd);
		int EpollWait(int time) ;          //wait
		void SetEpollEvent(int fd,int op);
		void SetEpollEvent(void*ptr,int op);   //reload
		int GetReadyNum(){return nrfd;}
		int GetEpollFD(){return epfd;}
   private:
		int epfd;    //epoll fd
		int nrfd;    // the ready num of fd 
        struct epoll_event event;
        //struct epoll_event*event;
   public:
        struct epoll_event* events;   //save the return event
};



#endif