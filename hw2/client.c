#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "socket_utils.h"

#define BUFSIZE 1024

int main(int argc, char **argv)
{
    int connfd, n, area;
    char transmit_buf[BUFSIZE], receive_buf[BUFSIZE];

    if(argc != 4 || argc != 5){
        printf("Usage1: %s <host> <port> host_message\n", argv[0]);
        printf("Usage2: %s <host> <port> host_message host_message2\n", argv[0]);
        exit(-1);
    }

    connfd = createClientSock(argv[1], atoi(argv[2]), TRANSPORT_TYPE_TCP);

    /* list all available commads */
    if(strcmp(argv[3], "command1") == 0){
        sprintf(transmit_buf, "list");
    }
    /* check the number of confirmed case in every area */
    else if(strcmp(argv[3], "command2") == 0){
        sprintf(transmit_buf, "Confirmed case");
    }
    /* check the number of confirmed case in specific area */
    else if(strcmp(argv[3], "command3") == 0){
        area = atoi(argv[4]);
        sprintf(transmit_buf, "Confirmed case | Area %d");
    }
    else{
        sprintf(transmit_buf, "No this command");
    }

    if((n = write(connfd, transmit_buf, strlen(transmit_buf))) == -1){
            perror("Error: write()\n");
    }

    printf("<====== Send to server. (client) ======>\n%s\n\n", transmit_buf);

    memset(receive_buf, 0, BUFSIZE);
    if((n = read(connfd, receive_buf, BUFSIZE)) == -1){
        perror("Error: read()\n");
    }
    printf("<======= Receive from server. (client) ======>\n%s\n\n", receive_buf);

    close(connfd);

    return 0;
}