#ifndef JOYSTICK_H
#define JOYSTICK_H


#include <unistd.h>
#include <fcntl.h>              //IO
#include <linux/joystick.h>     //joystick api

struct axis_state 
{
    short num;
    int value;
};

int read_event(int fd, struct js_event *event);
struct axis_state get_axis_state(struct js_event *event);


#endif
