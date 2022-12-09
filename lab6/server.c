#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 

/*for socket*/
#include <arpa/inet.h> //htons, ntohs
#include <errno.h>     /* Errors */
#include <netdb.h>
#include <netinet/in.h> //struct sockaddr_in
#include <sys/socket.h> //the functions of socket
#include <sys/types.h>  /* Primitive System Data Types */

#include <pthread.h>
#include <sys/sem.h> 

#include <signal.h>

#define BUFSIZE 1024
#define NUM_THREADS 10

#define SEM_MODE 0666 /* rw(owner)-rw(group)-rw(other) permission */ 
#define key 1234

//the information for each thread
typedef struct
{
    long t; //thread number
    int connfd; //socket connfd
    // int s; //semaphore
} info_transfer;

int amount=0;

int s;

/* P () - returns 0 if OK; -1 if there was a problem */ 
int P(int s) 
{ 
    struct sembuf sop ;     /* the operation parameters */ 
    sop.sem_num = 0;        /* access the 1st (and only) sem in the array */ 
    sop.sem_op = -1;        /* wait..*/ 
    sop.sem_flg = 0;        /* no special options needed */ 
    
    if (semop (s , &sop , 1) < 0) 
    { 
        fprintf (stderr ,"P(): semop failed : %s\n" , strerror (errno) ); 
        return -1; 
    } 
    else 
    { 
        return 0; 
    } 

} 

/* V() - returns 0 if OK; -1 if there was a problem */ 
int V(int s) 
{ 
    struct sembuf sop ; /* the operation parameters */ 
    sop.sem_num = 0;    /* the 1st (and only) sem in the array */ 
    sop.sem_op = 1;     /* signal */ 
    sop.sem_flg = 0;    /* no special options needed */ 
    
    if (semop(s , &sop , 1) < 0) 
    { 
        fprintf (stderr ,"V(): semop failed : %s\n" , strerror (errno) ); 
        return -1; 
    } 
    else {
        return 0;
    }
} 

void *BusyWork(void *info)
{
    info_transfer *my_info = (info_transfer *)info;
    int n;
    char rcv[2];
    char str[20];

    printf("create thread %ld\n", my_info->t);    

    int index=0;
    while (1)
    {
        //read message from client
        if ((n = read(my_info->connfd, rcv, 1)) == -1)
        {
            printf("Error: read()\n");
            break; //client is disconnected
        }
        
        if(n==0){
            break;
        }

        // printf("Thread %ld rcv: %s\n", my_info->t, rcv);

        //deal with the message
        int value=0;
        if(rcv[0]!='\0'){
            strncat(str, &rcv[0], 1);
            // printf("str: %s\n", str);
        }
        else{
            if (str[0] == 'w') {
                int j = 9;
                int w_value=0;
                while (str[j] != '\0')
                {
                    w_value = w_value * 10 + (int)(str[j] - '0');
                    j++;

                    // printf("w_vlaue: %d", w_value);
                }
                value=-w_value;

                memset(str, 0, 20);

                //refresh the amount
                P(s);
                amount+=value;
                
                printf("After withdraw: %d\n", amount);
                
                V(s);
            }
            else if (str[0] == 'd')  //deposit
            {
                int j = 8;
                int d_value=0;
                while (str[j] != '\0')
                {
                    d_value = d_value * 10 + (int)(str[j] - '0');
                    j++;

                    // printf("d_vlaue: %d", d_value);
                }
                value=d_value;

                memset(str, 0, 20);

                //refresh the amount
                P(s);
                amount+=value;
                
                printf("After deposit: %d\n", amount);
                
                V(s);
                // printf("str: %s\n", str);
                // printf("Thread %ld the wrong input from client\n", my_info->t);
            }

        index++;

        

        }
    }

    close(my_info->connfd);

    pthread_exit((void *)0);
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

void sigroutine(){
    /*remove semaphore*/
    // printf("enter sigroutine\n");
    if(semctl(s, 0, IPC_RMID, 0) < 0){
        printf("unable to remove semaphore\n");
        exit(1);
    }

    exit(0);
}

int main(int argc , char *argv[]) { 
    /*SIGINT*/
    signal(SIGINT, sigroutine);

    /*set socket*/
    int sockfd, connfd; /* socket descriptor */
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln);

    if (argc != 2)
        printf("Usage: %s port\n", argv[0]);

    //create socket and bind socket to port
    sockfd = passivesock(argv[1], "tcp", 10);

    /*set thread*/
    pthread_t thread[NUM_THREADS];
    int rc;
    long t = 0;
    //initialize and set thread detached atrribute
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    /*create semaphone*/
    // int s;
    s = semget(key , 1, IPC_CREAT | IPC_EXCL | SEM_MODE); 
    if (s < 0) 
    { 
        fprintf (stderr , "%s : creation of semaphore %d failed : %s\n" , argv [0] , key , strerror (errno) ); 
        exit (1); 
    } 
    //printf("test2\n");
    //initialize semaphone
    semctl(s , 0, SETVAL, 1);

    info_transfer info[NUM_THREADS];
    while (1)
    {
        /* waiting for connection */
        //if a new client is created and linked to the server, accept
        connfd = accept(sockfd, (struct sockaddr *)&addr_cln, &sLen);
        if (connfd == -1)
        {
            printf("Error : accept ()\n");
            break; //the checker is disconnected
        }

        /* now create new thread */
        //the infomation will be transferred to the thread
        info[t].connfd = connfd; //the new thread's connfd
        info[t].t = t;           //tread number
        // info[t].s = s;

        // printf("Main: creating thread %ld\n", info[t].t);
        rc = pthread_create(&thread[t], &attr, BusyWork, (void *)&info[t]);
        if (rc)
        {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }  

        t++;
        
    }

    //signal(SIGCHLD, handler);
    
    /* close server socket */
    //close (sockfd ); 
    //signal(SIGCHLD, handler);
    return 0;
} 