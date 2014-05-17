/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

/*
    buzzer.c

    Routines to operate the buzzer in an embedded computer.

    2008-07-01	CF Lin
*/
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<sys/ioctl.h>
#include	<fcntl.h>
#include	<sys/kd.h>   /* for buzzer functions */

int
mxbuzzer_open(void)
{
    return open("/dev/console", O_RDWR);
}

void 
mxbuzzer_beep(int fd, int time)
{
    ioctl( fd, KDMKTONE, (time<<16) | 100);
}

void
mxbuzzer_close(int fd)
{
    close(fd);
}

