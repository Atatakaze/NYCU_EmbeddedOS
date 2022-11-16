#include <arpa/inet.h> //htons, ntohs
#include <errno.h>     /* Errors */
#include <netdb.h>
#include <netinet/in.h> //struct sockaddr_in
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h> //the functions of socket
#include <sys/types.h>  /* Primitive System Data Types */

#include <unistd.h> //execlp()

#include <sys/wait.h>   //waitpid
#include <signal.h> //signal

#define BUFSIZE 1024

pid_t childpid; /* variable to store the child’s pid */

void handler(int signum) //kill the zombie process
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void childfunc(int connfd)
{

    /* duplicate stdout to s */
    dup2(connfd, STDOUT_FILENO);
    //close(connfd);
    execlp("sl", "sl", "-l", NULL);
    exit(0);
}

int passivesock(const char *service, const char *transport, int qlen)
{
    struct servent *pse;    /* pointer to service information entry */
    struct sockaddr_in sin; /* an Internet endpoint address */
    int s, type;            /* socket descriptor and socket type */

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    /* Map service name to port number */
    if ((pse = getservbyname(service, transport)))
        sin.sin_port = htons(ntohs((unsigned short)pse->s_port));
    else if ((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
        printf("Can’t find \"%s\" service entry\n", service);

    /* Use protocol to choose a socket type */
    if (strcmp(transport, "udp") == 0)
        type = SOCK_DGRAM; //
    else
        type = SOCK_STREAM; //

    /* Allocate a socket */
    s = socket(PF_INET, type, 0);
    if (s < 0)
        printf("Can't create socket : %s\n", strerror(errno));

    /* Bind the socket */
    if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        printf("Can't bind to port %s : %s\n", service, strerror(errno));

    /* Set the maximum number of waiting connection */
    if (type == SOCK_STREAM && listen(s, qlen) < 0)
        printf("Can't listen on port %s : %s\n", service, strerror(errno));

    return s;
}

int main(int argc, char *argv[])
{
    int sockfd, connfd; /* socket descriptor */
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln);
    //char snd[BUFSIZE], rcv[BUFSIZE];

    if (argc != 2)
        printf("Usage: %s port\n", argv[0]);

    /* create socket and bind socket to port */
    sockfd = passivesock(argv[1], "tcp", 10);

    while (1)
    {
        /* waiting for connection */
        //if a new client is created and linked to the server, accept
        connfd = accept(sockfd, (struct sockaddr *)&addr_cln, &sLen);
        if (connfd == -1)
            printf("Error : accept ()\n");

        /* now create new process */
        childpid = fork();

        if (childpid >= 0)
        { /* fork succeeded */
            if (childpid == 0)
            { /* fork() returns 0 to the child process */
                childfunc(connfd);
                /* close client connection */
                close(connfd);
                break;
            }
            else
            { /* fork() returns new pid to the parent process */
                printf("Train ID: %d\n", childpid);
                signal(SIGCHLD, handler); //SIGCHLD: when childprocess exits, it sends a SIGCHLD signal to parent process
            }
        }
        else
        {                   /* fork returns -1 on failure */
            perror("fork"); /* display error message */
            exit(0);
        }
    }

    /* close server socket */
    close(sockfd);
    return 0;
}