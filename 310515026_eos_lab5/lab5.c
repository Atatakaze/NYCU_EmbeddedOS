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

pid_t childpid;

void handler(int signum)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void childfunc(int connfd)
{
    dup2(connfd, STDOUT_FILENO);
    execlp("sl", "sl", "-l", NULL);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, connfd; /* socket descriptor */
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln);

    if (argc != 2){
        printf("Usage: %s port\n", argv[0]);
    }

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        printf("Can't create socket : %s\n", strerror(errno));
    }

    memset(&addr_cln, '0', sLen);

    addr_cln.sin_family = AF_INET;
    addr_cln.sin_addr.s_addr = INADDR_ANY;
    addr_cln.sin_port = htons((unsigned short)atoi(argv[1]));

    bind(sockfd, (struct sockaddr *)&addr_cln, sizeof(addr_cln));
    listen(sockfd, 10);

    signal(SIGCHLD, handler);

    while (1)
    {
        connfd = accept(sockfd, (struct sockaddr *)&addr_cln, &sLen);
        if (connfd == -1){
            printf("Error : accept ()\n");
        }

        /* now create new process */
        childpid = fork();

        /* child process */
        if (childpid == 0){
            childfunc(connfd);
            close(connfd);
            break;
        }
        /* parent process */
        else if (childpid > 0){ 
            printf("Train ID: %d\n", childpid);
        }
        else{
            perror("fork error");
            break;
        }
    }

    close(sockfd);
    return 0;
}