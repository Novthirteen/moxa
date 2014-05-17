/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    keypad.c

    Routines to operate the keypads in an embedded computer.

    2008-09-01	CF Lin
		new release
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>

#define IOCTL_KEYPAD_PRESSED  1
#define IOCTL_KEYPAD_GET_KEY    2
#define IOCTL_KEYPAD_SOUND_SET  3

static time_t gLastPressed = 0;

time_t
keypad_get_last_pressed_time(void)
{
    return gLastPressed;
}
/*	open a handle to the keypad device
	Returns:
		on success, a positive value for the handle
		otherwise, a negative value
*/
int
keypad_open(void)
{
	return open("/dev/keypad",O_RDWR);
}

/*	close a handle to the keypad device
	Inputs:
		<fd> the opend handle
*/
void
keypad_close(int fd)
{
	close(fd);
}

/*	obtain which key has been pressed
	Inputs:
		<fd> the opend handle
	Returns:
		key number starting from 1, or 0 meaning no key been pressed
*/
int
keypad_get_pressed_key(int fd)
{
    int num, key;
    int i = 0;

    key = -1;
    /* forced to do this way because releasing 
       the key generates a key==0. Thus, this
       method takes the 1st key and ignores
       the 2nd key
    */
    while(1) 
    {
	num = 0;
	if (ioctl( fd, IOCTL_KEYPAD_PRESSED, &num))
	    return -1;
	if (num)
	{
	    int tmp = -1;

	    if (ioctl( fd, IOCTL_KEYPAD_GET_KEY, &tmp))
	        return -1;
	    if (i==0)
	    {
	        i = 1;
		key = tmp;
	    }
	    else
	    {
		gLastPressed = time(0);
	        break;
	    }
	}
	usleep(10000);
    }
    return key;
}

