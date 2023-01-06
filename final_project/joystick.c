#include"joystick.h"



int read_event(int fd, struct js_event *event)
{
    ssize_t bytes;

    bytes = read(fd, event, sizeof(*event));

    if (bytes == sizeof(*event))
        return 0;

    /* Error, could not read full event. */
    return -1;
}

struct axis_state get_axis_state(struct js_event *event)
{
    struct axis_state as;
    //size_t axis = event->number;
    int n = event->number;
    int v = event->value;
    //as.num = event->number;
    //as.value = event->value;
    
    //mushroom head1
    
    if(n==0 || n==1)
    {
        as.num=0;
        as.value=v+32767;
        as.value/=2;
    }
    //mushroom head2
    else if(n==3||n==4)
    {
        as.num=1;
        as.value=v+32767;
        as.value/=2;    
    }
    //LT RT button
    else if(n==2||n==5)
    {
        as.num=(n+4)/3;
        as.value=v;
    }
    //cross button
    else if(n==6||n==7)
    {
        as.num=4;
        //UP=0 DOWN=1 LEFT=2 RIGHT=3 CENTER=5
        if(v==0)
            as.value=5;
        if(n==7)
        {
            if(v<0)
                as.value=0;
            else if(v>0)
                as.value=1;
        }
        else
        {
            if(v<0)
                as.value=2;
            else if(v>0)
                as.value=3;
        }
    }
    else
    {
        as.num=100;
    }
    
    
    return as;
}


