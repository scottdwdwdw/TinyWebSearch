#ifndef RIO_H
#define RIO_H

//csap chapter 10 system I/O   P597

//rio means robust IO operatoion
//rio with buffer

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "debug.h"


#define RIO_BUFSIZE 8192      //max bufsize
/*
typedef struct
{
	int rio_fd;      				Descriptor for this internal buf
	int rio_cnt;     				Unread bytes in internal buf
	char *rio_bufptr;  				Next unread byte in internal buf
	char rio_buf[RIO_BUFSIZE]; 		Internal buffer
}rio_t;

void rio_readinitb(rio_t *rp, int fd);*/

extern ssize_t rio_readn(const int fd,void*usrbuf,size_t n);
extern ssize_t rio_writen(const int fd,void*usrbuf,size_t n);


#endif