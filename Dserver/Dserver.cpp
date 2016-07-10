#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <iostream>
#include <signal.h>    //sigpipe
#include "sock.h"
#include "debug.h"
#include "epoll.h"
#include "http.h"
#include "thread_pool.h"

using namespace std;

//1. program tcp/ip interface  
//2. epoll interface
//3. analyse the request

int main()
{
     
	Sock s;
	Epoll ep;
	int listenfd;
	s.OpenListenfd(80);
    listenfd = s.GetFd();
    if(make_sock_non_blocking(listenfd)==-1) //this is very important
    {
        debug("can't make listenfd non blocking");
        exit(1);
    }

    struct sigaction sa;
    memset(&sa,'\0',sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if(sigaction(SIGPIPE,&sa,NULL))
    {
        debug("install sigpiple fail.");
        return -1;
    }

    Pool pool(2);
    
	ep.EpollCreate();    //create epoll fd
   
    HttpRequest* r = new HttpRequest;
    r->InitHttpRequest(&ep,listenfd,ep.GetEpollFD());
	ep.SetEpollEvent((void*)r,D_EPOLLIN|D_EPOLLET);  //set event
	ep.EpollCTL(D_EPOLL_CTL_ADD,listenfd);           //add listen fd to the event modul

    //the event loop
    
    while(1)
    {
        //debug("in main loop.");
    	int n = ep.EpollWait(-1);
    	for(int i=0; i<n;++i)
    	{
            HttpRequest* query = (HttpRequest*)ep.events[i].data.ptr;
            int ready_fd = query->GetFd();
            //debug("ready fd is : %d",ready_fd);
    		if((ep.events[i].events & D_EPOLLERR) ||
              (ep.events[i].events & D_EPOLLHUP) ||
              (!(ep.events[i].events & D_EPOLLIN)))
    		{
    			/* An error has occured on this fd, or the socket is not
                 ready for reading */
    			debug("epoll error");
    			close(ready_fd);
    			continue;
    		}
    		else if(listenfd == ready_fd)
    		{
    			/* We have a notification on the listening socket, which
                 means one or more incoming connections. */
    			while(1)
    			{
    				int infd;
    			    struct sockaddr_in client_addr;   //client address
                    socklen_t in_len = 1;
                    memset(&client_addr, 0, sizeof(struct sockaddr_in));  //must initializer or there is 'invalid argument' error in accept
    			    infd = s.Accept((sockaddr*)&client_addr,&in_len);
    			    if (infd == -1)
                    {
                        if((errno == EAGAIN) ||(errno == EWOULDBLOCK))
                        {
                            /* We have processed all incoming connections. */
                              //debug("accept all");
                              break;
                        }
                        else
                        {
                            debug_info("accept error");
                            break;
                        }
                    }
                     /* Make the incoming socket non-blocking and add it to the
                     list of fds to monitor. */
                     //debug("accept a fd : %d",infd);
                     if(make_sock_non_blocking(infd)==-1)
                     {
                     	debug("can't make socket non blocking");
                     	exit(1);
                     }
                     //create context
                     HttpRequest* request = new HttpRequest;  //new request
                     request->InitHttpRequest(&ep,infd,ep.GetEpollFD());
                     ep.SetEpollEvent((void*)request,D_EPOLLIN|D_EPOLLET|D_EPOLLONESHOT);  //set event
                     ep.EpollCTL(D_EPOLL_CTL_ADD,infd);     //add fd to the event modul
    			}
    		}
    		else
    		{
    			/* We have data on the fd waiting to be read. Read and
                 display it. We must read whatever data is available
                 completely, as we are running in edge-triggered mode
                 and won't get a notification again for the same
                 data. */
                 // use the http analyse or request analyse
                 //debug("new data from %d",ready_fd);
                 //query->RequestParse(NULL);
                 
                 int rrr = pool.ThreadPoolAdd(query,NULL);
                 if(rrr==-1)
                 {
                    //debug("signal cond error.");
                 }
                 
    		}
    	}
    }

    close(listenfd);
    //the destroy of pool will run automatic
	return 0;
}
