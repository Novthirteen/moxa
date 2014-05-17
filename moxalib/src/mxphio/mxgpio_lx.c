/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    gpio.c

    Routines to opearate GPIO ports.

    2008-07-15	CF Lin
		new release
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define IOCTL_PIO_GET_MODE	1
#define IOCTL_PIO_SET_MODE	2
#define IOCTL_PIO_GET_DATA	3
#define IOCTL_PIO_SET_DATA	4

typedef struct _GPIO 
{
    int pin;
    int val;
} GPIO;

void
mxgpio_close(int fd)
{
	close(fd);
}

/*  open an access to GPIO pins
    Returns:
        a file descriptor of the access
*/
int
mxgpio_open(void)
{
    return open("/dev/pio", O_RDWR);
}

int 
mxgpio_get_mode(int fd, int pin)
{
    GPIO pset;

    pset.pin = pin;
    if ( ioctl(fd, IOCTL_PIO_GET_MODE, &pset) != 0 ) 
        return -1;
    else
        return pset.val;
}

int 
mxgpio_get_data(int fd, int pin)
{
    GPIO pset;

    pset.pin = pin;
    if ( ioctl(fd, IOCTL_PIO_GET_DATA, &pset) != 0 ) 
        return -1;
    else
        return pset.val;
}

int 
mxgpio_set_mode(int fd, int pin, int val)
{
    GPIO pset;

    pset.pin = pin;
    pset.val = val;
    if ( ioctl(fd, IOCTL_PIO_SET_MODE, &pset) != 0 ) 
        return -1;
    else
        return pset.val;
}

int 
mxgpio_set_data(int fd, int pin, int val)
{
    GPIO pset;

    pset.pin = pin;
    pset.val = val;
    if ( ioctl(fd, IOCTL_PIO_SET_DATA, &pset) != 0 ) 
        return -1;
    else
        return pset.val;
}
