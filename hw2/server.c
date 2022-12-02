#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include "sockop.h"

#define BUFSIZE 1024

int main(int argc, char *argv[])
{
    int sockfd, connfd;
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln);
    int n;
    char snd[BUFSIZE], rcv[BUFSIZE];
    
    if(argc!=2)
        errexit("Usage: %s port\n",argv[0]);
    
    sockfd = passivesock(argv[1],"tcp",10);
    
    while(1)
    {
        connfd = accept(sockfd, (struct sockaddr *)&addr_cln, &sLen);
        if(connfd == -1)
            errexit("Error: accept()\n");
        
        if((n = read(connfd, rcv, BUFSIZE)) == -1)
            errexit("Error: read()\n");
        
        n = sprintf(snd, "Server: %.*s", n, rcv);
        if((n = write(connfd, snd, n)) == -1)
            errexit("Error: write()\n");
        
        close(connfd);    
    }
    
   close(sockfd);
   
   return 0;
}
