#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "socket_utils.h"

#define BUFSIZE 1024

int main(int argc, char **argv)
{
    int connfd, n;
    char buf[BUFSIZE];

    if(argc != 4){
        printf("Usage: %s <host> <port> host_message\n", argv[0]);
        exit(-1);
    }

    connfd = createClientSock(argv[1], atoi(argv[2]), TRANSPORT_TYPE_TCP);

    if((n = write(connfd, argv[3], strlen(argv[3]))) == -1){
        perror("Error: write()\n");
    }
    printf("<-- Send to server. (client) -->\n%s\n\n", argv[3]);

    memset(buf, 0, BUFSIZE);

    if((n = read(connfd, buf, BUFSIZE)) == -1){
        perror("Error: read()\n");
    }
    printf("<-- Receive from server. (client) -->\n%s\n\n", buf);

    close(connfd);

    return 0;
}