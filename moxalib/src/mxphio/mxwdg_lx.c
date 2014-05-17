/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    mxwdg.c

    Routines to operate a watch dog timer.

    2008-07-15	CF Lin
		new release
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "mxwdg.h"

#define IOCTL_SWATCHDOG_ENABLE	1
#define IOCTL_SWATCHDOG_DISABLE	2
#define IOCTL_SWATCHDOG_GET		3
#define IOCTL_SWATCHDOG_ACK		4

/*	start operating a watch dog timer
*/
int	
mxwdg_open(unsigned long time)
{
	int fd;
	
	fd = open("/dev/swtd", O_RDWR);
	if (fd<0)
	return -1;
	else
	{
	if (ioctl(fd, IOCTL_SWATCHDOG_ENABLE, &time))
	return -2;
	}
	return fd;
}

/*	stop operating a watch dog timer
	Inputs:
		<fd> file descriptor respresenting the timer
	Returns:
		0 on success, failure otherwise
*/
void	
mxwdg_close(int fd)
{
	ioctl(fd, IOCTL_SWATCHDOG_DISABLE, NULL);
	close(fd);
}

/*	get the status of a watch dog timer
	Inputs:
		<fd> file descriptor respresenting the timer
	Outputs:
		<mode> the status
		<time> the timer
	Returns:
		0 on success, failure otherwise
*/
int	
mxwdg_get_status(int fd, int *mode, unsigned long *time)
{
	int			ret;
	struct {
		int		mode;
		unsigned long	time;
	} nowset;

	ret = ioctl(fd, IOCTL_SWATCHDOG_GET, &nowset);
	*mode = nowset.mode;
	*time = nowset.time;
	return ret;
}

/*	refresh a watch dog timer, this must be called periodically
	Inputs:
		<fd> file descriptor respresenting the timer
	Returns:
		0 on success, failure otherwise
*/
int	
mxwdg_refresh(int fd)
{
	return ioctl(fd, IOCTL_SWATCHDOG_ACK, NULL);
}
