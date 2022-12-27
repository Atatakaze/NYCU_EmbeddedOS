#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // fork()
#include <signal.h>  // signal()
#include <pthread.h> // pthread_create, pthread_join, etc.

#include <sys/wait.h>
#include <sys/sem.h>
#include "socket_utils.h"

#define BUFSIZE 1024
#define NUM_THREADS 10

#define SEM_KEY 1234
#define SEM_MODE 0666

typedef struct
{
    long t;       //thread number
    int connfd;   //socket connfd
    char transmit_buf[BUFSIZE], receive_buf[BUFSIZE], buf[BUFSIZE];
    char* s;                        // use to seperate each command by '|' and ' '
    char* substr[30];               // store sub string
} info_transfer;

typedef struct AREA{
    int mild;       // number of mild case
    int severe;     // number of severe case
} AREA;

int sem;
AREA area[9];

/* ================================================ */
/*                  Semaphore                       */
/* ================================================ */
int P(int s){
  struct sembuf sop;
  sop.sem_num = 0;
  sop.sem_op = -1;
  sop.sem_flg = 0;

  if(semop(s, &sop, 1) < 0){
    perror("[ERROR]: P() sem failed\n");
    return -1;
  }
  else{
    return 0;
  }
}

int V(int s){
  struct sembuf sop;
  sop.sem_num = 0;
  sop.sem_op = 1;
  sop.sem_flg = 0;

  if(semop(s, &sop, 1) < 0){
    perror("[ERROR]: V() sem failed.\n");
    return -1;
  }
  else{
    return 0;
  }
}

void intHandler()
{
  printf("[INFO]: Remove semaphore.\n");
  if(semctl(sem, 0, IPC_RMID, 0) < 0){
    perror("[ERROR]: Fail to remove sem.\n");
    exit(-1);
  }

  exit(0);
}

/* ================================================ */
/*                  Thread                          */
/* ================================================ */
void *connectCallback(void *info)
{
    info_transfer *my_info = (info_transfer *)info;
    int i, n, substr_count = 0, which_area, n_report, n_case;

    printf("[INFO]: Create thread %ld.\n", my_info->t);

    while (1)
    {
        memset(my_info->receive_buf, 0, BUFSIZE);
        if((n = read(my_info->connfd, my_info->receive_buf, BUFSIZE)) == -1){
            perror("[ERROR]: read()\n");
        }
        printf(" > [command received](thread %ld): %s\n", my_info->t, my_info->receive_buf);

        /* seperate command by the '|' and ' ' */
        P(sem);
        substr_count = 0;
        my_info->s = strtok(my_info->receive_buf, " |");
        while(my_info->s != NULL){
            my_info->substr[substr_count++] = my_info->s;
            my_info->s = strtok(NULL, " |");
        }
        V(sem);

        /* command: list (return categories) */
        if(strcmp(my_info->substr[0], "list") == 0){
            n = sprintf(my_info->transmit_buf, "1. Confirmed case\n2. Reporting system\n3. Exit\n");
            if((n = write(my_info->connfd, my_info->transmit_buf, n)) == -1){
                perror("Error: write()\n");
            }
            printf(" > [command send](thread %ld): %s\n", my_info->t, my_info->transmit_buf);
        }
        /* command: Confirmed case (return infomation of confirmed cases) */
        if(strcmp(my_info->substr[0], "Confirmed") == 0){
            /* command: Confirmed case */
            P(sem);
            if(substr_count == 2){
                n = sprintf(my_info->transmit_buf, "0 : %d\n1 : %d\n2 : %d\n3 : %d\n4 : %d\n5 : %d\n6 : %d\n7 : %d\n8 : %d\n", 
                            (area[0].mild + area[0].severe), (area[1].mild + area[1].severe), (area[2].mild + area[2].severe),
                            (area[3].mild + area[3].severe), (area[4].mild + area[4].severe), (area[5].mild + area[5].severe), 
                            (area[6].mild + area[6].severe), (area[7].mild + area[7].severe), (area[8].mild + area[8].severe));
            }
            /* command: Confirmed case | Area x */
            else{
                which_area = atoi(my_info->substr[3]);
                n = sprintf(my_info->transmit_buf, "Area %d - Mild : %d | Severe : %d\n", which_area, area[which_area].mild, area[which_area].severe);
            }

            if((n = write(my_info->connfd, my_info->transmit_buf, n)) == -1){
                perror("Error: write()\n");
            }
            V(sem);
        }
        /* command: Reporting system */
        if(strcmp(my_info->substr[0], "Reporting") == 0){
            /* command: Reporting sytem | Area x | Mild/Severe x */
            P(sem);
            n = sprintf(my_info->transmit_buf, "Please wait a few seconds...\n");
            if((n = write(my_info->connfd, my_info->transmit_buf, n)) == -1){
                perror("Error: write()\n");
            }

            n_report = ((substr_count - 2) / 4);
            for(i = 0; i < n_report; i++){
                which_area = atoi(my_info->substr[(3 + 4 * i)]);
                n_case = atoi(my_info->substr[(5 + 4 * i)]);
                if(strcmp(my_info->substr[(4 + 4 * i)], "Mild") == 0){
                    area[which_area].mild += n_case;
                }
                else{
                    area[which_area].severe += n_case;
                }
            }

            for(i = 0; i < n_report; i++){
                if(which_area < atoi(my_info->substr[(3 + 4 * i)])){
                    which_area = atoi(my_info->substr[(3 + 4 * i)]);
                }
            }
            sleep(which_area);

            memset(my_info->buf, 0, BUFSIZE);
            for(i = 0; i < n_report; i++){
                strcat(my_info->buf, "Area ");
                strcat(my_info->buf, my_info->substr[(3 + 4 * i)]);
                strcat(my_info->buf, " | ");
                strcat(my_info->buf, my_info->substr[(4 + 4 * i)]);
                strcat(my_info->buf, " ");
                strcat(my_info->buf, my_info->substr[(5 + 4 * i)]);
                strcat(my_info->buf, "\n");
            }

            if((n = write(my_info->connfd, my_info->buf, strlen(my_info->buf))) == -1){
                perror("Error: write()\n");
            }
            V(sem);
        }
        /* command: Exit (exit system) */
        if(strcmp(my_info->substr[0], "Exit") == 0){
            break;
        }

    close(my_info->connfd);
    pthread_exit((void *)0);
    }
}


int main(int argc, char *argv[])
{
    signal(SIGINT, intHandler);

    int sockfd, connfd, rc, i;
    long n_thread = 0;

    /* socket */
    struct sockaddr_in cln_addr;
    socklen_t sLen = sizeof(cln_addr);
  
    /* thread */
    pthread_t thread[NUM_THREADS];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    if (argc != 2){
        printf("[ERROR]: Usage: %s <port>\n", argv[0]);
        exit(-1);
    }

    /* initialization of infomation of 8 area */
    for(i = 0; i < 9; i++){
        area[i].mild = 0;
        area[i].severe = 0;
    }

    /* create socket server */
    sockfd = createServerSock(atoi(argv[1]), TRANSPORT_TYPE_TCP);
    if(sockfd < 0){
        printf("[ERROR]: Fail to create socket.\n");
        exit(-1);
    }

    // Creating semaphore
    sem = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | SEM_MODE);
    if (sem < 0){
        fprintf (stderr , "%s : creation of semaphore %d failed : %s\n" , argv [0] , SEM_KEY , strerror (errno) ); 
        exit (1); 
    }
    printf("[INFO] sem created.\n");
    semctl(sem , 0, SETVAL, 1);

    info_transfer info[NUM_THREADS];
    while (1)
    {
        /* waiting for connection */
        connfd = accept(sockfd, (struct sockaddr *)&cln_addr, &sLen);
        if (connfd == -1){
            printf("[ERROR]: Fail to accept client connection.\n");
            break;
        }

        info[n_thread].connfd = connfd; //the new thread's connfd
        info[n_thread].t = n_thread;           //thread number

        rc = pthread_create(&thread[n_thread], &attr, connectCallback, (void *)&info[n_thread]);
        if(rc){
            printf("[ERROR]: Return code from pthread_create() is %d.\n", rc);
            exit(-1);
        }

        n_thread++;
    }
    return 0;
}