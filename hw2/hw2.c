#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "socket_utils.h"

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
    char transmit_buf[BUFSIZE], receive_buf[BUFSIZE];
    AREA area[9];

    if(argc!=2){
        printf("Usage: %s port\n", argv[0]);
        exit(-1);
    }

    // initialization of infomation of 8 area
    for(i = 0; i < 9; i++){
        area[i]->mild = 0;
        area[i]->severe = 0;
    }
    
    sockfd = createServerSock(atoi(argv[1]), TRANSPORT_TYPE_TCP);
    
    if (sockfd < 0)
    {
        perror("Error create socket\n");
        exit(-1);
    }

    while(1)
    {
        connfd = accept(sockfd, (struct sockaddr *)&addr_cln, &sLen);
        if(connfd == -1){
            perror("Error: accept()\n");
        }

        if((n = read(connfd, receive_buf, BUFSIZE)) == -1){
            perror("Error: read()\n");
        }
        
        // command: list (return categories)
        if(strcmp(receive_buf, "list") == 1){
            printf(" > [command received (server)]: %s\n", receive_buf);
            n = sprintf(transmit_buf, "1. Confirmed case\n2. Reporting system\n3. Exit\n");
            if((n = write(connfd, transmit_buf, n)) == -1){
                perror("Error: write()\n");
            }
        }

        close(connfd);    
    }
    
   close(sockfd);
   
   return 0;
}
