#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // fork()
#include <signal.h>  // signal()
#include <sys/ipc.h>
#include <sys/shm.h> //for shared memory

#include <sys/wait.h>
#include <sys/sem.h>
#include "socket_utils.h"

#define BUFSIZE 1024
#define NUM_THREADS 10

#define SEM_KEY 1234
#define SEM_MODE 0666

typedef struct{
    int number ;       //process number
    int connfd;        //socket connfd
} info_transfer;

typedef struct AREA{
    int mild[9];       // number of mild case
    int severe[9];     // number of severe case
} AREA;

int sem, processNumber=0;
pid_t childpid;

/* shm parameter */
AREA *area;
int shmid;
key_t key;

/* ================================================ */
/*                  share memory                    */
/* ================================================ */
void shmHandler() {
    /* Detach the share memory segment */
    shmdt(area);
    
    /* Destroy the share memory segment */
    printf ("[INFO] Destroy the share memory.\n");

    int retval;
    retval = shmctl(shmid, IPC_RMID, NULL);
    if ( retval < 0)
    {
        fprintf (stderr , "[ERROR]: Remove share memory failed.\n");
        exit (1);
    }
    exit(1);
}


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

void semHandler()
{
  printf("[INFO]: Remove semaphore.\n");
  if(semctl(sem, 0, IPC_RMID, 0) < 0){
    perror("[ERROR]: Fail to remove sem.\n");
    exit(-1);
  }

  exit(0);
}

/* ================================================ */
/*                  process                         */
/* ================================================ */
void childProcess(info_transfer info)
{
    int i, n, substr_count = 0, which_area, n_report, n_case;
    char transmit_buf[BUFSIZE], receive_buf[BUFSIZE], buf[BUFSIZE];
    char* s;                        // use to seperate each command by '|' and ' '
    char* substr[30];               // store sub string

    // share memory attach
    if ((shmid = shmget(key, sizeof(AREA), 0666)) < 0) {
        perror("[ERROR]: Fail when shmget()");
        exit(1);
    }
    /* atach the segment to our data space */
    if ((area = shmat(shmid, NULL, 0)) == (AREA *) -1) {
        perror("[ERROR]: Fail when shmat()");
        exit(1);
    }
    //printf("[Process %d]: Attach to the share memory.\n", info.number);

    while (1)
    {
        memset(receive_buf, 0, BUFSIZE);
        if((n = read(info.connfd, receive_buf, BUFSIZE)) == -1){
            perror("[ERROR]: read()\n");
        }
        printf(" > [Process %d]: Receive %s\n", info.number, receive_buf);

        /* seperate command by the '|' and ' ' */
        substr_count = 0;
        s = strtok(receive_buf, " |");
        while(s != NULL){
            substr[substr_count++] = s;
            s = strtok(NULL, " |");
        }

        /* command: list (return categories) */
        if(strcmp(substr[0], "list") == 0){
            memset(transmit_buf, 0, BUFSIZE);
            n = sprintf(transmit_buf, "1. Confirmed case\n2. Reporting system\n3. Exit\n");
            if((n = write(info.connfd, transmit_buf, n)) == -1){
                perror("Error: write()\n");
            }

            printf(" > [Process %d]: Transmit\n%s\n", info.number, transmit_buf);
        }
        /* command: Confirmed case (return infomation of confirmed cases) */
        if(strcmp(substr[0], "Confirmed") == 0){
            /* command: Confirmed case */
            memset(transmit_buf, 0, BUFSIZE);
            P(sem);
            if(substr_count == 2){
                n = sprintf(transmit_buf, "0 : %d\n1 : %d\n2 : %d\n3 : %d\n4 : %d\n5 : %d\n6 : %d\n7 : %d\n8 : %d\n", 
                            (area->mild[0] + area->severe[0]), (area->mild[1] + area->severe[1]), (area->mild[2] + area->severe[2]),
                            (area->mild[3] + area->severe[3]), (area->mild[4] + area->severe[4]), (area->mild[5] + area->severe[5]), 
                            (area->mild[6] + area->severe[6]), (area->mild[7] + area->severe[7]), (area->mild[8] + area->severe[8]));
            }
            /* command: Confirmed case | Area x */
            else{
                which_area = atoi(substr[3]);
                n = sprintf(transmit_buf, "Area %d - Mild : %d | Severe : %d\n", which_area, area->mild[which_area], area->severe[which_area]);
            }

            if((n = write(info.connfd, transmit_buf, n)) == -1){
                perror("Error: write()\n");
            }
            V(sem);

            printf(" > [Process %d]: Transmit\n%s\n", info.number, transmit_buf);
        }
        /* command: Reporting system */
        if(strcmp(substr[0], "Reporting") == 0){
            /* command: Reporting sytem | Area x | Mild/Severe x */
            memset(transmit_buf, 0, BUFSIZE);
            P(sem);
            n = sprintf(transmit_buf, "The ambulance is on it's way...\n");
            if((n = write(info.connfd, transmit_buf, n)) == -1){
                perror("Error: write()\n");
            }
            printf(" > [Process %d]: Transmit\n%s\n", info.number, transmit_buf);

            n_report = ((substr_count - 2) / 4);
            for(i = 0; i < n_report; i++){
                which_area = atoi(substr[(3 + 4 * i)]);
                n_case = atoi(substr[(5 + 4 * i)]);
                if(strcmp(substr[(4 + 4 * i)], "Mild") == 0){
                    area->mild[which_area] += n_case;
                }
                else{
                    area->severe[which_area] += n_case;
                }
            }

            for(i = 0; i < n_report; i++){
                if(which_area < atoi(substr[(3 + 4 * i)])){
                    which_area = atoi(substr[(3 + 4 * i)]);
                }
            }
            V(sem);

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

            if((n = write(info.connfd, buf, strlen(buf))) == -1){
                perror("Error: write()\n");
            }
            printf(" > [Process %d]: Transmit\n%s\n", info.number, buf);
        }
        /* command: Exit (exit system) */
        if(strcmp(substr[0], "Exit") == 0){
            break;
        }
    }
    close(info.connfd);
    exit(0);
}

void childHandler(int signum)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}


int main(int argc, char *argv[])
{
    int sockfd, connfd, i;

    /* socket */
    struct sockaddr_in cln_addr;
    socklen_t sLen = sizeof(cln_addr);

    if (argc != 2){
        printf("[ERROR]: Usage: %s <port>\n", argv[0]);
        exit(-1);
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

    signal(SIGINT, semHandler);
    signal(SIGCHLD, childHandler);
    signal(SIGINT, shmHandler);

    /* Create the segment */
    key = 4321;
    if ((shmid = shmget(key ,sizeof(AREA), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit (1);
    }
    /* Attach the segment */    
    if ((area = (AREA *)shmat(shmid, NULL, 0)) == (AREA *) -1) {
        perror("shmat");
        exit (1);
    }

    /* initialization of infomation of 8 area */
    for(i = 0; i < 9; i++){
        area->mild[i] = 0;
        area->severe[i] = 0;
    }

    info_transfer info;
    while (1)
    {
        /* waiting for connection */
        connfd = accept(sockfd, (struct sockaddr *)&cln_addr, &sLen);
        if (connfd == -1){
            printf("[ERROR]: Fail to accept client connection.\n");
            break;
        }

        /* create new process */
        childpid = fork();
        info.connfd = connfd;
        info.number = processNumber;

        /* child process */
        if (childpid == 0){
            childProcess(info);
            break;
        }
        /* parent process */
        else if (childpid > 0){ 
            //printf("[INFO] Process ID: %d\n", childpid);
        }
        else{
            perror("[ERROR]: Fail to fork.\n");
            break;
        }

        processNumber++;
    }
    close(sockfd);
    return 0;
}