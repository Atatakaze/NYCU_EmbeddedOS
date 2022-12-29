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
int sockfd, connfd;

/* shm parameter */
AREA *area;
int shmid;
key_t key = 4321;

/* ================================================ */
/*                  signal                          */
/* ================================================ */
void intHandler() 
{
    /* close client connection */
    close(connfd);
    /* close server socket */
    close(sockfd);

    /* Destroy the share memory segment */
    int retval;
    retval = shmctl(shmid, IPC_RMID, NULL);
    if ( retval < 0){
        fprintf(stderr , "[ERROR]: Remove share memory failed.\n");
        exit (1);
    }
    printf ("[INFO] Destroy the share memory.\n");

    /* Destroy semaphore */
    if(semctl(sem, 0, IPC_RMID, 0) < 0){
        perror("[ERROR]: Fail to remove semaphore.\n");
        exit(1);
    }
    printf("[INFO]: Remove semaphore %d.\n", SEM_KEY);

    exit(0);
}

void childHandler()
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}


/* ================================================ */
/*                  Semaphore                       */
/* ================================================ */
int P(int s)
{
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

int V(int s)
{
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

/* ================================================ */
/*                  process                         */
/* ================================================ */
void childProcess(info_transfer info)
{
    int i, n, substr_count = 0, which_area, n_case;
    char transmit_buf[BUFSIZE], receive_buf[BUFSIZE];
    char* s;                        // use to seperate each command by '|' and ' '
    char* substr[30];               // store sub string
    AREA myArea;

    while (1)
    {
        /* read message from client */
        memset(receive_buf, 0, BUFSIZE);
        if((n = read(info.connfd, receive_buf, BUFSIZE)) == -1){
            perror("[ERROR]: read()\n");
        }
        if((strcmp(receive_buf, " ") == 0) || n == 0){
            printf("[INFO](Process %d): End process.\n", info.number);
            break;
        }
        printf(" > [Receive](process %d): %s\n", info.number, receive_buf);

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
            char temp[100] = {0};
            
            if(substr_count == 2){
                P(sem);
                // attach share memory
                if ((area = (AREA *)shmat(shmid, NULL, 0)) == (AREA *) -1){
                    printf("[ERROR](Process %d): Fail to attach share memory.", info.number);
                    exit(1);
                }

                n = sprintf(temp, "0 : %d\n1 : %d\n2 : %d\n3 : %d\n4 : %d\n5 : %d\n6 : %d\n7 : %d\n8 : %d\n", 
                            (area->mild[0] + area->severe[0]), (area->mild[1] + area->severe[1]), (area->mild[2] + area->severe[2]),
                            (area->mild[3] + area->severe[3]), (area->mild[4] + area->severe[4]), (area->mild[5] + area->severe[5]), 
                            (area->mild[6] + area->severe[6]), (area->mild[7] + area->severe[7]), (area->mild[8] + area->severe[8]));

                shmdt(area);
                V(sem);
            }
            /* command: Confirmed case | Area x */
            else{
                P(sem);
                // attach share memory
                if ((area = (AREA *)shmat(shmid, NULL, 0)) == (AREA *) -1){
                    printf("[ERROR](Process %d): Fail to attach share memory.", info.number);
                    exit(1);
                }

                which_area = atoi(substr[3]);
                n = sprintf(temp, "Area %d - Mild : %d | Severe : %d\n", which_area, area->mild[which_area], area->severe[which_area]);
                
                shmdt(area);
                V(sem);
            }

            memset(transmit_buf, 0, BUFSIZE);
            strcpy(transmit_buf, temp);
            if((n = write(info.connfd, transmit_buf, n)) == -1){
                perror("Error: write()\n");
            }
            printf(" > [Transmit](Process %d): Transmit (length %d)\n%s\n", info.number, n, transmit_buf);
        }
        /* command: Reporting system */
        if(strcmp(substr[0], "Reporting") == 0){
            /* command: Reporting sytem | Area x | Mild/Severe x */
            char temp[100] = {0};
            char wait[100] = {0};

            n = sprintf(wait, "The ambulance is on it's way...\n");
            memset(transmit_buf, 0, BUFSIZE);
            strcpy(transmit_buf, wait);
            if((n = write(info.connfd, transmit_buf, n)) == -1){
                perror("Error: write()\n");
            }
            printf(" > [Transmit](Process %d): %s\n", info.number, transmit_buf);

            // initialize the local case report
            for(i = 0; i < 9; i++){
                myArea.mild[i] = 0;
                myArea.severe[i] = 0;
            }

            for(i = 2; i < substr_count; i++){
                if(strcmp(substr[i], "Area") == 0){
                    which_area = atoi(substr[(i + 1)]);
                }
                if(strcmp(substr[i], "Mild") == 0){
                    n_case = atoi(substr[(i + 1)]);
                    myArea.mild[which_area] += n_case;
                }
                if(strcmp(substr[i], "Severe") == 0){
                    n_case = atoi(substr[(i + 1)]);
                    myArea.severe[which_area] += n_case;
                }
            }

            which_area = 8;
            while(i >= 0){
                if((myArea.mild[which_area] + myArea.severe[which_area]) != 0){
                    break;
                }
                else{
                    which_area -= 1;
                }
            }

            printf(" > [INFO](Process %d): Wait %d seconds...\n", info.number, which_area);
            sleep(which_area);

            // update local report to share memory
            P(sem);
            // attach share memory
            if ((area = (AREA *)shmat(shmid, NULL, 0)) == (AREA *) -1){
                printf("[ERROR](Process %d): Fail to attach share memory.", info.number);
                exit(1);
            }
            
            for(i = 0; i < 9; i++){
                area->mild[i] += myArea.mild[i];
                area->severe[i] += myArea.severe[i];
            }
            shmdt(area);
            V(sem);

            memset(transmit_buf, 0, BUFSIZE);
            for(i = 0; i < 9; i++){
                if((myArea.mild[i] + myArea.severe[i]) != 0){
                    memset(temp, 0, 100);
                    if((myArea.mild[i] != 0) && (myArea.severe[i] == 0)){
                        sprintf(temp, "Area %d | Mild %d\n", i, myArea.mild[i]);
                    }
                    if((myArea.mild[i] == 0) && (myArea.severe[i] != 0)){
                        sprintf(temp, "Area %d | Severe %d\n", i, myArea.severe[i]);
                    }
                    if((myArea.mild[i] != 0) && (myArea.severe[i] != 0)){
                        sprintf(temp, "Area %d | Severe %d | Mild %d\n", i, myArea.severe[i], myArea.mild[i]);
                    }
                    strcat(transmit_buf, temp);
                }
            }

            if((n = write(info.connfd, transmit_buf, strlen(transmit_buf))) == -1){
                perror("Error: write()\n");
            }
            printf(" > [Transmit](Process %d): \n%s\n", info.number, transmit_buf);
        }
        /* command: Exit (exit system) */
        if(strcmp(substr[0], "Exit") == 0){
            break;
        }
    }
    memset(receive_buf, 0, BUFSIZE);
    close(info.connfd);
    exit(0);
}

/* ================================================ */
/*                     main                         */
/* ================================================ */

int main(int argc, char *argv[])
{
    int i;
    pid_t childpid;
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

    /* create semaphore */
    sem = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | SEM_MODE);
    if (sem < 0){
        fprintf (stderr , "%s : creation of semaphore %d failed : %s\n" , argv [0] , SEM_KEY , strerror (errno) ); 
        exit (1); 
    }
    semctl(sem , 0, SETVAL, 1);
    printf("[INFO] Create semaphore %d.\n", SEM_KEY);

    signal(SIGINT, intHandler);
    signal(SIGCHLD, childHandler);

    /* Create the segment */
    if ((shmid = shmget(key ,sizeof(AREA), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit (1);
    }
    /* Attach the segment */    
    if ((area = (AREA *)shmat(shmid, NULL, 0)) == (AREA *) -1) {
        perror("shmat");
        exit (1);
    }
    printf("[INFO] Create and attach share memory.\n");

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