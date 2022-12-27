#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "socket_utils.h"

int main(int argc, char **argv)
{
  int connfd, n, i;
  char msg_buf[100];

  if(argc != 6){
    printf("Usage: %s <host> <port> <deposit/withdraw> <amount> <times>\n", argv[0]);
    exit(-1);
  }

  connfd = createClientSock(argv[1], atoi(argv[2]), TRANSPORT_TYPE_TCP);

  for(i = 0; i < atoi(argv[5]); i++){
    memset(msg_buf, 0, 100);
    strcpy(msg_buf, argv[3]);
    strcat(msg_buf, " ");
    strcat(msg_buf, argv[4]);

    if((n = write(connfd , msg_buf , strlen(msg_buf) + 1)) == -1){
      printf("Error : write ()\n");
    }

    printf("[Client] Send: %s\n", msg_buf);
  } 

  close(connfd); 
  return 0;
}