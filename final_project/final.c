#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <pthread.h>            //-lpthread
#include<stdbool.h>
#include<time.h>
#include "sockop.h"
#include "joystick.h"
#define BUFSIZE 1024
//char server_ip[11]="192.168.0.4";
char server_ip[9]="127.0.0.1";
char server_port[4]="1234";
bool p2=false;
double current_time=0;
struct thread_data
{
    int number;
    char device[14];
};

void *client_func(void*);
void *time_cnt(void*);


int main(int argc, char *argv[])
{
    char *device[2] = {"/dev/input/js0","/dev/input/js1"};
    printf("%s,%s",device[0],device[1]);
    time_t seconds = time(NULL);
    printf("%ld",seconds);
    pthread_t thread_id;
    printf("Before Thread\n");
    struct thread_data *data=malloc(sizeof(struct thread_data));
    data->number=1;
    strcpy(data->device,device[0]);
    pthread_create(&thread_id, NULL, client_func, data);
    pthread_t thread_time_id;
    pthread_create(&thread_time_id, NULL, time_cnt, NULL);
    //pthread_exit(NULL);
    pthread_join(thread_id, NULL);
    printf("After Thread\n");
    //pthread_exit(NULL);
    
    return 0;
}
void *client_func(void *init_data)
{
    // Initialize
    struct thread_data *data=init_data;
    printf("[INFO] Create thread %d\n",data->number);
    int state=0;  //0:opening  1:single-mode  2:dual-mode
    int tmp_state=1;  //1:single-select 2:dual-select
    
    
    //Joystick
    const char *device;
    int js;
    struct js_event event;
    struct axis_state axis;    
    while(1)
    {
        js = open(data->device, O_RDONLY);
        if (js == -1)
        {
            printf("Could not open joystick (Try again after 3 secs)\n");
            usleep(3000000);
        }
        else
        {
            printf("Joystick %d on\n",data->number);
            break;
        }
    }
    
    //Socket
    int connfd;
    int n;
    double start_time=0;
    char buf[BUFSIZE];
    char send_msg[BUFSIZE];
    char tmp[BUFSIZE];
    connfd = connectsock(server_ip,server_port,"tcp");
    printf("[INFO] Player %d connect success\n",data->number);
    sprintf(send_msg,"Joystick 1 on\n");
    write(connfd,send_msg,strlen(send_msg));
    
    while (read_event(js, &event) == 0)
    {
        strcpy(send_msg,"");
        switch (event.type)
        {
            case JS_EVENT_BUTTON:
                printf("Button %u %s\n", event.number, event.value ? "pressed" : "released");
                if(state==0)
                {
                    if(event.number==7 && event.value==1)
                    {
                        sprintf(send_msg,"Confirm Select %s mode",tmp_state==1? "single" : "Dual");
                        write(connfd,send_msg,strlen(send_msg));
                        //state=tmp_state;  // single or dual mode
                        start_time=0;
                        current_time=0;
                    }
                    if(event.number==8 && event.value==1)
                    {
                        sprintf(send_msg,"end");
                        write(connfd,send_msg,strlen(send_msg));
                    }
                    
                    if(event.value==1)
                    {
                        if(event.number<=3)
                        {
                            sprintf(send_msg,"1");
                            write(connfd,send_msg,strlen(send_msg));
                        }
                        else if(event.number<=5)
                        {
                            sprintf(send_msg,"2");
                            write(connfd,send_msg,strlen(send_msg));
                        }
                        else if(event.number==6)
                        {
                            sprintf(send_msg,"pause");
                            write(connfd,send_msg,strlen(send_msg));
                        }
                    }
                }
                /*
                if(event.number==8 && event.value==0 && !p2)
                {
                    struct thread_data *tmp_data = malloc(sizeof(struct thread_data));
                    tmp_data->number=2;
                    strcpy(tmp_data->device,"/dev/input/js1");
                    pthread_t thread_id;
                    pthread_create(&thread_id, NULL, client_func, tmp_data);
                    p2=true;
                }
                */
                /*
                if(event.value==1)
                {
                    sprintf(send_msg,"[Player %d] Press button %u\n",data->number,event.number);
                    write(connfd,send_msg,strlen(send_msg));
                }
                */   
                break;
            case JS_EVENT_AXIS:
                axis = get_axis_state(&event);
                printf("Axis %u at (%6d)\n", axis.num, axis.value);
                if(state==0)
                {
                    if(axis.num==4)
                    {
                        if(axis.value==2)
                        {
                            tmp_state=1;
                            sprintf(send_msg,"Select left\n");
                            write(connfd,send_msg,strlen(send_msg));
                        }
                        else if(axis.value==3)
                        {
                        
                            tmp_state=2;
                            sprintf(send_msg,"Select right\n");
                            write(connfd,send_msg,strlen(send_msg));
                        }
                    }
                
                }
                /*
                else if(state==1 || state==2)
                {
                    sprintf(send_msg,"[Player %d] Axis %d is spinning\n",data->number,axis.num);
                    write(connfd,send_msg,strlen(send_msg));                    
                }
                */
                /*
                if(axis.num<4)
                {
                    sprintf(send_msg,"[Player %d] Axis %d is spinning\n",data->number,axis.num);
                    write(connfd,send_msg,strlen(send_msg));
                }
                else
                {
                    switch (axis.value)
                    {
                        case 0:
                            sprintf(send_msg,"[Player %d ] Press UP\n",data->number);
                            write(connfd,send_msg,strlen(send_msg));
                            break;
                        case 1:
                            sprintf(send_msg,"[Player %d ] Press DOWN\n",data->number);
                            write(connfd,send_msg,strlen(send_msg));
                            break;
                        case 2:
                            sprintf(send_msg,"[Player %d ] Press LEFT\n",data->number);
                            write(connfd,send_msg,strlen(send_msg));
                            break;
                        case 3:
                            sprintf(send_msg,"[Player %d ] Press RIGHT\n",data->number);
                            write(connfd,send_msg,strlen(send_msg));
                            break;
                        default:
                            break;
                    }
                }
                */
                break;
            default:
                /* Ignore init events. */
                break;
        }
        
        fflush(stdout);
    }

    
    //memset(buf, 0, BUFSIZE);    
    close(connfd);
    return NULL;
}
void *time_cnt(void* data)
{
    while(1)
    {
        usleep(10000);
        current_time+=0.01;
    }
}

