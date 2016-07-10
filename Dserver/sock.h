/****************************

*****************************/

#ifndef SOCK_H
#define SOCK_H


#include <sys/socket.h>
#include <strings.h>    //for bzero
#include <netinet/in.h>  //for htonl
#include <fcntl.h>     //for fcntl
#include "debug.h"

#define BACKLOG 1024
//sock class ,save sock fd,
// creat , listen , bind, accept
class Sock
{
   public:
   	  Sock(){}    //default construct
   	  Sock&operator=(Sock&)=delete;
   	  Sock&&operator=(Sock&&)=delete;
   	  ~Sock(){}    // destruct construct
      int create_socket();
      int Bind();
      int Listen();
      void BackLogSet(int back_log = BACKLOG){backlog = back_log;}
      int Accept(struct sockaddr *cliaddr, socklen_t *addrlen);
      int OpenListenfd(int port);
      void ServerAddrSet(int port);
      int GetFd() const {return listenfd;}

   private:
   	  int listenfd;      //listen
   	//  int acceptfd;      //accept
   	  int backlog;       
   	  struct sockaddr_in serveraddr;   
};


int make_sock_non_blocking(int sfd);


#endif