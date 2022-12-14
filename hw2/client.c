#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "socket_utils.h"

#define BUFSIZE 1024

int main(int argc, char **argv)
{
    int connfd, n;
    char transmit_buf[BUFSIZE], receive_buf[BUFSIZE];

    if(argc < 4){
        printf("Usage1: %s <host> <port> host_message ...\n", argv[0]);
        exit(-1);
    }

    connfd = createClientSock(argv[1], atoi(argv[2]), TRANSPORT_TYPE_TCP);

    /* command: list */
    if(strcmp(argv[3], "command1") == 0){
        sprintf(transmit_buf, "list");
    }
    /* command: Confirmed case */
    else if(strcmp(argv[3], "command2") == 0){
        sprintf(transmit_buf, "Confirmed case");
    }
    /* command: Confirmed case | Area x */
    else if(strcmp(argv[3], "command3") == 0){
        sprintf(transmit_buf, "Confirmed case | Area %s", argv[4]);
    }
    /* command: Reporting system | Area x | Mild/Severe x */
    else if(strcmp(argv[3], "command4") == 0){
        sprintf(transmit_buf, "Reporting system | Area %s | %s %s", argv[4], argv[5], argv[6]);
    }
    /* command: Reporting system | Area x | Mild/Severe x | Area x | Mild/Severe x */
    else if(strcmp(argv[3], "command5") == 0){
        sprintf(transmit_buf, "Reporting system | Area %s | %s %s | Area %s | %s %s", argv[4], argv[5], argv[6], argv[7], argv[8], argv[9]);
    }
    /* command: Exit */
    else if(strcmp(argv[3], "command6") == 0){
        sprintf(transmit_buf, "Exit");
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

    if((strcmp(argv[3], "command4") == 0) || (strcmp(argv[3], "command5") == 0)){
        memset(receive_buf, 0, BUFSIZE);
        if((n = read(connfd, receive_buf, BUFSIZE)) == -1){
            perror("Error: read()\n");
        }
        printf("<======= Receive from server. (client) ======>\n%s\n\n", receive_buf);
    }

    close(connfd);

    return 0;
}