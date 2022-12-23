#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h>
#include <string.h>

//for shared memory
#include <sys/ipc.h>
#include <sys/shm.h> 
//for signal
#include <signal.h>
//for nanosleep
#include <time.h>
//for timer
#include <sys/time.h>


typedef struct {
    int guess;
    char result[8];
}data;

//shm parameter
data *shm;
int shmid;
int ans;



void handler_destory_shm(int signum) {
    /* Detach the share memory segment */
    shmdt(shm);
    
    /* Destroy the share memory segment */
    printf ("Server destroy the share memory.\n");

    int retval;
    retval = shmctl(shmid , IPC_RMID, NULL);
    if ( retval < 0)
    {
        fprintf (stderr , "Server remove share memory failed\n");
        exit (1);
    }
    exit(1);
}



void handler ( int signo , siginfo_t *info , void *context )
{ 
    memset(shm->result, 0, sizeof(shm->result));
    if(shm->guess > ans){
        strcpy(shm->result, "smaller");
        printf("[game] Guess %d, %s\n", shm->guess, "smaller");
    }
    else if(shm->guess < ans){
        strcpy(shm->result, "bigger");
        printf("[game] Guess %d, %s\n", shm->guess, "bigger");
    }
    else{
        strcpy(shm->result, "bingo");
        printf("[game] Guess %d, %s\n", shm->guess, "bingo");
    }


    //send signal to guess    
    kill(info->si_pid, SIGUSR1);
}


int main(int argc, char **argv)
{
    //shm setting
    /* We need to get the segment named "5678", created by the server */
    key_t key;
    key = atoi(argv[1]);
    ans = atoi(argv[2]);

    //ctrl+c setting
    signal(SIGINT, handler_destory_shm);

    //signal parameter
    struct sigaction my_action ;
    memset(&my_action, 0, sizeof(struct sigaction));
    my_action.sa_flags=SA_SIGINFO;
    my_action.sa_sigaction=handler;
    sigaction(SIGUSR1, &my_action, NULL);

    printf("[game] Game PID : %d\n", getpid() );


    /* Create the segment */
    if ((shmid = shmget(key ,sizeof(data), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit (1);
    }
    /* Now we attach the segment to our data space */ 
    if ((shm = (data *)shmat(shmid , NULL, 0)) == (data *) -1) { //(char *)
        perror("shmat");
        exit (1);
    }

    //wait parameter
    struct timespec req;
    /* set the sleep time to 100 sec */
    memset(&req , 0 , sizeof (struct timespec));
    req.tv_sec = 100;
    req.tv_nsec = 0;

    int retval;
    /* sleep 100 sec */
    do{
        retval = nanosleep(&req , &req );
    }while( retval );


    return 0;
}
