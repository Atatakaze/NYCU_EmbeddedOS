#include <stdio.h>   // fprintf(), perror()
#include <stdlib.h>  // exit()
#include <string.h>  // memset()
#include <signal.h>  // signal()
#include <fcntl.h>  // open()
#include <unistd.h> //unix standard -> driver read(), write(), close()

#include <sys/socket.h> // socket(), connect()
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h>  // htons()

int connfd, fd;

void sigint_handler(int signo)
{
    close(fd);
    close(connfd);
}

int main(int argc, char *argv[])
{
    if (argc != 4){
        fprintf(stderr, "Usage: ./reader <server_ip> <port> <device_path>"); 
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, sigint_handler); //當在terminal輸入ctrl+C，做sigint_handler這個函式

    if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Failed to open socket.");
        exit(EXIT_FAILURE);
    }

    //設定client端的ip和port
    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;                     //AF_INET:伺服器對伺服器傳輸
    cli_addr.sin_addr.s_addr = inet_addr(argv[1]);     //client IP
    cli_addr.sin_port = htons((u_short)atoi(argv[2])); //port

    if (connect(connfd, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) == -1){
        perror("Connection error");
        exit(EXIT_FAILURE);
    }

    if ((fd = open(argv[3], O_RDWR)) < 0){
        perror(argv[3]);
        exit(EXIT_FAILURE);
    }

    int ret;
    char buf[16] = {};

    while (1){
        if ((ret = read(fd, buf, sizeof(buf))) == -1){
            perror("Failed to read()");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < 16; ++i){
            printf("%d ", buf[i]);
        }
        printf("\n");

        if (write(connfd, buf, 16) == -1){
            perror("Failed to write()");
            exit(EXIT_FAILURE);
        }

        sleep(1);
    }

    close(fd);
    close(connfd);
    return 0;
}
