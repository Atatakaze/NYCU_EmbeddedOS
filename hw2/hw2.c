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
    int sockfd, connfd, i, n, which_area, n_case, n_report, substr_count = 0;
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln);
    char transmit_buf[BUFSIZE], receive_buf[BUFSIZE], buf[BUFSIZE];
    AREA area[9];
    char* s;                        // use to seperate each command by '|' and ' '
    char* substr[30];               // store sub string

    if(argc != 2){
        printf("Usage: %s port\n", argv[0]);
        exit(-1);
    }

    /* initialization of infomation of 8 area */
    for(i = 0; i < 9; i++){
        area[i].mild = 0;
        area[i].severe = 0;
    }
    
    sockfd = createServerSock(atoi(argv[1]), TRANSPORT_TYPE_TCP);
    
    if (sockfd < 0){
        perror("Error create socket\n");
        exit(-1);
    }

    while(1){
        connfd = accept(sockfd, (struct sockaddr *)&addr_cln, &sLen);
        if(connfd == -1){
            perror("Error: accept()\n");
        }

        /* initialization of infomation of 8 area */
        for(i = 0; i < 9; i++){
            area[i].mild = 0;
            area[i].severe = 0;
        }

        while(1){
            memset(receive_buf, 0, BUFSIZE);
            if((n = read(connfd, receive_buf, BUFSIZE)) == -1){
                perror("Error: read()\n");
            }
            printf(" > [command received](server): %s\n", receive_buf);

            /* seperate command by the '|' and ' ' */
            substr_count = 0;
            s = strtok(receive_buf, " |");
            while(s != NULL){
                substr[substr_count++] = s;
                s = strtok(NULL, " |");
            }
            
            //printf(" > after seperated by ' |':\n");
            //for(i = 0; i < substr_count; i++){
            //   printf("substr[%d]: '%s'\n", i, substr[i]);
            //}

            /* command: list (return categories) */
            if(strcmp(substr[0], "list") == 0){
                n = sprintf(transmit_buf, "1. Confirmed case\n2. Reporting system\n3. Exit\n");
                if((n = write(connfd, transmit_buf, n)) == -1){
                    perror("Error: write()\n");
                }
            }
            /* command: Confirmed case (return infomation of confirmed cases) */
            if(strcmp(substr[0], "Confirmed") == 0){
                /* command: Confirmed case */
                if(substr_count == 2){
                    n = sprintf(transmit_buf, "0 : %d\n1 : %d\n2 : %d\n3 : %d\n4 : %d\n5 : %d\n6 : %d\n7 : %d\n8 : %d\n", 
                                (area[0].mild + area[0].severe), (area[1].mild + area[1].severe), (area[2].mild + area[2].severe),
                                (area[3].mild + area[3].severe), (area[4].mild + area[4].severe), (area[5].mild + area[5].severe), 
                                (area[6].mild + area[6].severe), (area[7].mild + area[7].severe), (area[8].mild + area[8].severe));
                }
                /* command: Confirmed case | Area x */
                else{
                    which_area = atoi(substr[3]);
                    n = sprintf(transmit_buf, "Area %d - Mild : %d | Severe : %d\n", which_area, area[which_area].mild, area[which_area].severe);
                }

                if((n = write(connfd, transmit_buf, n)) == -1){
                    perror("Error: write()\n");
                }
            }
            /* command: Reporting system */
            if(strcmp(substr[0], "Reporting") == 0){
                /* command: Reporting sytem | Area x | Mild/Severe x */
                n = sprintf(transmit_buf, "Please wait a few seconds...\n");
                if((n = write(connfd, transmit_buf, n)) == -1){
                    perror("Error: write()\n");
                }

                n_report = ((substr_count - 2) / 4);
                for(i = 0; i < n_report; i++){
                    which_area = atoi(substr[(3 + 4 * i)]);
                    n_case = atoi(substr[(5 + 4 * i)]);
                    if(strcmp(substr[(4 + 4 * i)], "Mild") == 0){
                        area[which_area].mild += n_case;
                    }
                    else{
                        area[which_area].severe += n_case;
                    }
                }

                for(i = 0; i < n_report; i++){
                    if(which_area < atoi(substr[(3 + 4 * i)])){
                        which_area = atoi(substr[(3 + 4 * i)]);
                    }
                }
                sleep(which_area);

                memset(buf, 0, BUFSIZE);
                for(i = 0; i < n_report; i++){
                    strcat(buf, "Area ");
                    strcat(buf, substr[(3 + 4 * i)]);
                    strcat(buf, " | ");
                    strcat(buf, substr[(4 + 4 * i)]);
                    strcat(buf, " ");
                    strcat(buf, substr[(5 + 4 * i)]);
                    strcat(buf, "\n");
                }

                if((n = write(connfd, buf, strlen(buf))) == -1){
                    perror("Error: write()\n");
                }
            }
            /* command: Exit (exit system) */
            if(strcmp(substr[0], "Exit") == 0){
                break;
            }
        }
        close(connfd);
    } 
    close(sockfd);
   
    return 0;
}
