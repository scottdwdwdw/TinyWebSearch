#include "rio.h"

ssize_t rio_readn(const int fd, void *usrbuf, size_t n) 
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = (char *)usrbuf;
        
    while (nleft > 0) {
        nread=read(fd, bufp, nleft);
    //    debug("nread is: %d",nread);
        if(nread< 0){
            //interrupted by sig handler return 
            if (errno == EINTR) 
                nread = 0;      // and call read() again 
            else if((errno == EAGAIN) ||(errno == EWOULDBLOCK)) //already read all,try
            {
                break;
            }
            else
            {
                debug_info("rio_readn");
                return -1;      // errno set by read()  
            }
                
        } 
        else if (nread == 0)//EOF
        {
            break;
        }
            
        nleft -= nread;
        bufp += nread;
    }
    
    return (n - nleft);         /* return >= 0 */
}

/*
 * rio_writen - robustly write n bytes (unbuffered)
 */
ssize_t rio_writen(const int fd, void *usrbuf, size_t n) 
{
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = (char *)usrbuf;
        
    while (nleft > 0) {
        if ((nwritten = write(fd, bufp, nleft)) <= 0) {
            if (errno == EINTR)  /* interrupted by sig handler return */
                nwritten = 0;    /* and call write() again */
            else {
                debug("write error.");
                return -1;       /* errorno set by write() */
            }
        }
        nleft -= nwritten;
        bufp += nwritten;
    }
    
    return n;
}