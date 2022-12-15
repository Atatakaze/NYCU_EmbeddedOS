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

int sem, s;
int amount = 0;

typedef struct
{
  long t; //thread number
  int connfd; //socket connfd
} info_transfer;

int P(int s){
  struct sembuf sop;
  sop.sem_num = 0;
  sop.sem_op = -1;
  sop.sem_flg = 0;

  if(semop(s, &sop, 1) < 0){
    perror("P(): sem failed\n");
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
    perror("V(): sem failed\n");
    return -1;
  }
  else{
    return 0;
  }
}

void intHandler()
{
  printf("remove sem\n");
  if(semctl(sem, 0, IPC_RMID, 0) < 0){
    perror("Error removing sem\n");
    exit(-1);
  }

  exit(0);
}

void *connectCallback(void *info)
{
  info_transfer *my_info = (info_transfer *)info;
  int n, index = 0;
  char rcv[2], str[20];

  printf("create thread %ld\n", my_info->t);

  while (1)
  {
    if((n = read(my_info->connfd, rcv, 1)) == -1){
      printf("Error: read()\n");
      break;
    }

    if(n == 0){
      break;
    }

    int value=0;
    if(rcv[0]!='\0'){
      strncat(str, &rcv[0], 1);
    }
    else{
      if(str[0] == 'w'){
        int j = 9;
        int w_value=0;
        while(str[j] != '\0'){
          w_value = w_value * 10 + (int)(str[j] - '0');
          j++;
        }

        value = -w_value;

        memset(str, 0, 20);
        P(s);
        amount += value;

        printf("After withdraw: %d\n", amount);
        V(s);
      }
      else if(str[0] == 'd'){
        int j = 8;
        int d_value = 0;
        while(str[j] != '\0'){
          d_value = d_value * 10 + (int)(str[j] - '0');
          j++;
        }
        value = d_value;

        memset(str, 0, 20);
        P(s);
        amount += value;

        printf("After deposit: %d\n", amount);
        V(s);
      }

      index++;
    }
  }

  close(my_info->connfd);
  pthread_exit((void *)0);
}

int main(int argc, char **argv)
{
  signal(SIGINT, intHandler);

  int sockfd, connfd, rc;
  long t = 0;
  struct sockaddr_in cln_addr;
  socklen_t sLen = sizeof(cln_addr);
  pthread_t thread[NUM_THREADS];
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  if (argc != 2){
    printf("Usage: %s <port>\n", argv[0]);
    exit(-1);
  }

  sockfd = createServerSock(atoi(argv[1]), TRANSPORT_TYPE_TCP);
  if(sockfd < 0){
    printf("Error create socket\n");
    exit(-1);
  }

  // Creating semaphore
  sem = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | SEM_MODE);
  if (sem < 0){
    fprintf (stderr , "%s : creation of semaphore %d failed : %s\n" , argv [0] , SEM_KEY , strerror (errno) ); 
    exit (1); 
  }
  printf("sem created\n");
  semctl(s , 0, SETVAL, 1);

  info_transfer info[NUM_THREADS];
  while (1)
  {
    /* waiting for connection */
    connfd = accept(sockfd, (struct sockaddr *)&cln_addr, &sLen);
    if (connfd == -1){
      printf("Error : accept ()\n");
      break;
    }

    info[t].connfd = connfd; //the new thread's connfd
    info[t].t = t;           //tread number

    rc = pthread_create(&thread[t], &attr, connectCallback, (void *)&info[t]);
    if(rc){
      printf("ERROR: return code from pthread_create() is %d\n", rc);
      exit(-1);
    }

    t++;
  }
  return 0;
}