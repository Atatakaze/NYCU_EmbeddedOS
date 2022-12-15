#include <arpa/inet.h> //htons, ntohs
#include <errno.h>     /* Errors */
#include <netdb.h>
#include <netinet/in.h> //struct sockaddr_in
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h> //the functions of socket
#include <sys/types.h>  /* Primitive System Data Types */
#include <unistd.h> //sleep()

#define BUFSIZE 128

int connectsock(const char *host, const char *service, const char *transport)
{
    struct hostent *phe;
    struct servent *pse;
    struct sockaddr_in sin;
    int s, type;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;

    /* Map service name to port number */
    if ((pse = getservbyname(service, transport)))
        sin.sin_port = pse->s_port;
    else if ((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
        printf("Can’t get \"%s\" service entry\n", service);

    if((phe=gethostbyname(host))){
        memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
    }
    else if((sin.sin_addr.s_addr=inet_addr(host))==INADDR_NONE){
        printf("Can't get \"%s\" host entry\n", host);
    }

    if(strcmp(transport, "udp")==0){
        type=SOCK_DGRAM;
    }
    else
    {
        type=SOCK_STREAM;
    }
    

    /* Allocate a socket */
    s = socket(PF_INET, type, 0);
    if (s < 0)
        printf("Can't create socket : %s\n", strerror(errno));

    if(connect(s, (struct sockaddr *)&sin, sizeof(sin))<0){
        printf("Can't connect to %s.%s: %s\n", host, service, strerror(errno));
    }

    return s;
}

int main(int argc , char *argv [])
{ 
    int connfd ; /* socket descriptor */ 
    int n; 
    char buf [BUFSIZE]; 
    int t;
    char send_data[100];


    if (argc != 6) 
        printf("Usage: %s host address host port message\n" , argv [0]); 
    
    /* create socket and connect to server */ 
    connfd = connectsock(argv [1] , argv [2] , "tcp");
    

    for(t=0;t<atoi(argv[5]);t++)
    {
        //combine reposit/withdraw and money
        //strcpy(send_data, "");
        memset(send_data, 0, 100);
        strcpy (send_data,argv[3]);
        strcat (send_data," ");
        strcat (send_data,argv[4]);
        // strcat (send_data,"@");


        /* write message to server */ 
        if ((n = write(connfd , send_data , strlen(send_data)+1 ) ) == -1) 
            printf("Error : write ()\n"); 

        printf("send_data: %s\n", send_data);
    }    


    /* close client socket */ 
    close (connfd ); 
    
    return 0;
}