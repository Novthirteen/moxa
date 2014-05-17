/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    mxdgio.c

    Routines to access digital I/O.

    2008-07-15	CF Lin
		new release
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#ifdef UC74XX
#define MXDIO_DEV	"/dev/lcm"
#else
#define MXDIO_DEV	"/dev/dio"
#endif

#define IOCTL_SET_DOUT	15
#define IOCTL_GET_DOUT	16
#define IOCTL_GET_DIN	17

typedef struct _DGTIO 
{
    int	port;
    int	data;
} DGTIO;

/*	open an access to digit io pins
	Returns:
		a file descriptor of the access
*/
int
mxdgio_open(void)
{
    return open(MXDIO_DEV, O_RDWR);
}

/*  get the signal state of a digit input 
    Inputs:
	<fd> the access to the device
	<port> port #
    Returns:
	1 or 0 on success, -1 on failure
*/
int
mxdgio_get_input_signal(int fd, int port)
{
    int n;
    DGTIO dio;

    dio.port = port;
    n = ioctl(fd, IOCTL_GET_DIN, &dio);
    return ((n>0)? -1:((dio.data>0)? 1:0));
}

/*	get the signal state of a digit output 
	Inputs:
		<fd> the access to the device
		<port> port #
	Returns:
		1 or 0 on success, -1 on failure
*/
int	
mxdgio_get_output_signal(int fd, int port)
{
    int n;
    DGTIO dio;

    dio.port = port;
    n = ioctl(fd, IOCTL_GET_DOUT, &dio);
    return ((n>0)? -1:((dio.data>0)? 1:0));
}

/*  pass a high signal to a digit output 
    Inputs:
	<fd> the access to the device
	<port> port #
    Returns:
	0 on success, -1 on failure
*/
int	
mxdgio_set_output_signal_high(int fd, int port)
{
    DGTIO dio;

    dio.port = port;
    dio.data = 1;
    return ioctl(fd, IOCTL_SET_DOUT, &dio);
}

/*  pass a low signal to a digit output 
    Inputs:
	<fd> the access to the device
	<port> port #
    Returns:
	0 on success, -1 on failure
*/
int	
mxdgio_set_output_signal_low(int fd, int port)
{
    DGTIO dio;

    dio.port = port;
    dio.data = 0;
    return ioctl(fd, IOCTL_SET_DOUT, &dio);
}

void
mxdgio_close(int fd)
{
    close(fd);
}

