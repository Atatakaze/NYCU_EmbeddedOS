#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include "sockop.h"

#define BUFSIZE 1024

typedef struct AREA{
    int mild;       // number of mild case
    int severe;     // number of severe case
} AREA;

int main(int argc, char *argv[])
{
    int sockfd, connfd, i, n;
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln);
    char snd[BUFSIZE], rcv[BUFSIZE];
    AREA* area[9];

    if(argc!=2){
        errexit("Usage: %s port\n", argv[0]);
    }

    // initialization of infomation of 8 area
    for(i = 0; i < 9; i++){
        area[i].mild = 0;
        area[i].servere = 0;
    }

    sockfd = passivesock(argv[1], "tcp", 10);
    
    while(1)
    {
        connfd = accept(sockfd, (struct sockaddr *)&addr_cln, &sLen);
        if(connfd == -1){
            errexit("Error: accept()\n");
        }

        if((n = read(connfd, rcv, BUFSIZE)) == -1){
            errexit("Error: read()\n");
        }
        
        // command: list (return categories)
        if(strcmp(rcv, "list") == 1){
            printf(" > [command received]: list\n");
            n = sprintf(snd, "1. Confirmed case\n2. Reporting system\n3. Exit\n");
            if((n = write(connfd, snd, n)) == -1){
                errexit("Error: write()\n");
            }
        }

        close(connfd);    
    }
    
   close(sockfd);
   
   return 0;
}
