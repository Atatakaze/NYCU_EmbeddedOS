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

//others
pid_t pid;
int upperbound;

int max;
int min;


void handler ( int signo , siginfo_t *info , void *context )
{ 

    //judge outcome
    if(strncmp(shm->result, "smaller", 6) == 0 )
    {
        max = shm->guess - 1;
    }
    else if(strncmp(shm->result, "bigger", 7) == 0)
    {
        min = shm->guess + 1;
    }
    else if(strncmp(shm->result, "bingo", 5) == 0)
    {
        shmdt(shm);
        exit(0);
    }

}


void timer_handler ( int signum)
{ 
    shm->guess = (max + min)/2;

    printf("[game] Guess %d\n", shm->guess);

    
    //send signal to game , notify game to process
    kill(pid, SIGUSR1);        

}


int main(int argc, char **argv)
{
    key_t key;

    key = atoi(argv[1]);
    upperbound = atoi(argv[2]);
    pid = atoi(argv[3]);

    max=upperbound;
    min=1;
    
    //signal setting
    /* register handler to SIGUSR1 */
    //signal parameter
    struct sigaction my_action ;

    memset(&my_action , 0 , sizeof (struct sigaction ));
    my_action.sa_flags = SA_SIGINFO;
    my_action.sa_sigaction = handler ;   
    sigaction (SIGUSR1, &my_action , NULL);


    //timer setting
    /* Install timer_handler as the signal handler for SIGVTALRM */
    //timer parameter
    struct sigaction sa ;
    struct itimerval timer ;

    memset (&sa , 0 , sizeof ( sa ));
    sa.sa_handler = &timer_handler ;
    sigaction (SIGALRM, &sa , NULL);
    /* Configure the timer to expire after 1000 msec */
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;
    /* Reset the timer back to 1000 msec after expired */
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;
    /* Start a virtual timer */
    setitimer (ITIMER_REAL, &timer , NULL);

    //shm setting
    /* We need to get the segment named "5678", created by the server */
    /* Locate the segment */
    //shm parameter
    int shmid;
    //char *shm;
    if ((shmid = shmget(key , sizeof(data), 0666)) < 0) {
        perror("shmget");
        exit (1);
    } 
    /* Now we attach the segment to our data space */ 
    if ((shm = (data *)shmat(shmid , NULL, 0)) == (data *) -1) {
        perror("shmat");
        exit (1);
    }
    
    //printf("start\n");
    /* Do busy work */
    while (1);

    return 0;
}