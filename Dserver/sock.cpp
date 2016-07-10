#include "sock.h"

int Sock::create_socket()
{
	int fd;
	fd = socket(AF_INET,SOCK_STREAM,0);
	if(fd==-1)
	{
		debug("socket create fail...");
	}

	debug("create socket fd is: %d",fd);

	return fd;
}


int Sock::Bind()
{
	if(bind(listenfd,(const sockaddr*)&serveraddr,sizeof(serveraddr))==-1)
	{
		debug("bind fail...");
		exit(1);
	}

	return 0;
}

int Sock::Listen()
{
	BackLogSet();
	if(listen(listenfd,backlog)==-1)
	{
		debug("listen fail...");
		exit(1);
	}

	return 0;
}


int Sock::Accept(struct sockaddr *cliaddr, socklen_t *addrlen)
{
	int accept_fd=-1;
	accept_fd = accept(listenfd,cliaddr,addrlen);
	//debug("accept fd is :%d",accept_fd);
	if(accept_fd ==-1)
	{
		return -1;
	}

	//debug("accept successful, accept socket is:%d",accept_fd);
	return  accept_fd;
}

void Sock::ServerAddrSet(int port)
{
	bzero((char*)&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;   //ip4
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);   //ip4 is 32bits
	serveraddr.sin_port = htons(port);   //port is 16bits
}

int Sock::OpenListenfd(int port)
{
	int optval = 1;
    listenfd = create_socket();    
    if(setsockopt(listenfd,SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(optval))==-1)
    {
    	debug("setsockopt fail...");
    	exit(1);
    }

    ServerAddrSet(port);
    Bind();    //bind address and port
    Listen();  //listen on this port

    return 0;
}


int make_sock_non_blocking(int sfd)
{
   int flags,state;
   flags = fcntl(sfd,F_GETFL,0);     //get flags;
   if(flags==-1)
   {
      debug("fcntl get flag error");
      return -1;
   }
   flags |= O_NONBLOCK;
   state = fcntl(sfd,F_SETFL,flags); //set flags
   if(state == -1)
   {
      debug("fcntl set flag error");
      return -1;
   }

   return 0;
}