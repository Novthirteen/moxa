
/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    lcm.c

    Routines to operate the LCM panel on an embedded computer.

    2008-07-01	CF Lin
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "mxlcm.h"

typedef struct _LCMXY
{
    int x,y;
} LCMXY;

int
mxlcm_open(void)
{
	return open("/dev/lcm", O_RDWR);
}

void
mxlcm_close(int fd)
{
	close(fd);
}

int
mxlcm_get_cursor(int fd, int *x, int *y)
{
	LCMXY lcm_xy;
	int ret = ioctl( fd, IOCTL_LCM_GET_XY, &lcm_xy);

	if (ret==0)
	{
		*x= lcm_xy.x;
		*y= lcm_xy.y;
	}
	else
	{
		*x = *y = 0;
	}
	return ret;
}

int 
mxlcm_set_cursor(int fd, int x, int y)
{
	LCMXY lcm_xy;

	lcm_xy.x= x;
	lcm_xy.y= y;
	return ioctl( fd, IOCTL_LCM_GOTO_XY, &lcm_xy);
}

int
mxlcm_control(int fd, int code)
{
    return ioctl(fd, code, NULL);
}

int 
mxlcm_write(int fd, int x, int y, char* str, int len)
{
	int i;

	for (i=0; i < MAX_LCM_COLS; i++)
	{
	    if (str[i]==0)
	    {
	        while(i<MAX_LCM_COLS) str[i++] = ' ';
				break;
	    }
	}
	if (mxlcm_set_cursor(fd, x, y)==0)
		return write( fd, str, len);
	else
		return -1;
}

int
mxlcm_write_screen(int fd, char text[MAX_LCM_ROWS][MAX_LCM_COLS])
{
	int y;

	mxlcm_clear_screen(fd);
	for(y=0; y < MAX_LCM_ROWS; y++)
		mxlcm_write(fd, 0, y, text[y], MAX_LCM_COLS);
	return 0;
}
