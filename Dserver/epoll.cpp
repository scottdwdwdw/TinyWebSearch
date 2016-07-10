#include "epoll.h"

Epoll::Epoll():epfd(0),nrfd(0)
{
	events = new epoll_event[MAX_EVENTS];  //array of epoll fd
	if(events==NULL)
	{
		debug("new epoll_event error");
		exit(1);
	}
}

int Epoll::EpollCreate()  //create epoll fd
{
	epfd = epoll_create1(0);
	if(epfd == -1)
	{
		debug("epoll createl fail");
		return -1;
	}
	return 1;
}

void Epoll::SetEpollEvent(int fd,int e)   //signal event
{
	event.data.fd = fd;
	event.events = e;
}


void Epoll::SetEpollEvent(void*ptr,int e)
{
	event.data.ptr = ptr;
	event.events = e;
}


int Epoll::EpollCTL(int mod,int fd)
{
	 int flag;
	 flag = epoll_ctl(epfd, mod,fd,&event);
	 if(flag == -1)
	 {
	 	debug("epoll ctl fail");
	 	return -1;
	 }

	 return 0;
}

/*
int Epoll::EpollCTL(int mod,int fd,struct epoll_event*event)
{
	 int flag;
	 flag = epoll_ctl(epfd, mod,fd,event);
	 if(flag == -1)
	 {
	 	debug("epoll ctl fail");
	 	return -1;
	 }

	 return 0;
}
*/
int Epoll::EpollWait(int time)
{
	nrfd = epoll_wait(epfd,events,MAX_EVENTS, time);
	return nrfd;
} 

Epoll::~Epoll()
{
	delete events;
}