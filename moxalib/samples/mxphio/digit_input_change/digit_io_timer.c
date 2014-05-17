/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    digit_io_timere.c

    Routines to operate timer function on digital IO port.

    2009-02-15	CF Lin
		new release
*/
#include <stdio.h>
#include <stdlib.h>
#if !defined(_WIN32_WCE) && !defined(WIN32)
#include <time.h>
#endif
#include "digit_io_timer.h"

/* callback function */
static void
dgio_input_change_exec(HANDLE fd, DGIOITEM *item)
{
    int sig;

    switch(item->mode)
    {
    case DGTIO_GET_INPUT:
		sig = mxdgio_get_input_signal(fd, item->port);
		item->cb(item->port, sig, item->arg);
		break;
    case DGTIO_GET_OUTPUT:
		sig = mxdgio_get_output_signal(fd, item->port);
		item->cb(item->port, sig, item->arg);
		break;
    case DGTIO_GET_INPUT_STATE_CHANGE:
		sig = mxdgio_get_input_signal(fd, item->port);
		if (item->last_signal!=sig) 
		{
			item->cb(item->port, sig, item->arg);
		}
		break;
    case DGTIO_SET_OUTPUT:
		sig = item->cb(item->port, item->last_signal, item->arg);
		if (sig)
		{
			mxdgio_set_output_signal_high(fd, item->port);
		}
		else
		{
			mxdgio_set_output_signal_low(fd, item->port);
		}
		break;
    default:
		return;
    }
    item->last_signal = sig;
}

/* release the timer operation 
*/
static void
dgio_input_change_release(DGIOMNGR *mngr)
{
    DGIOITEM *item, *next;

	item=mngr->list;
	while(item)
	{
		next = item->next;
		free(item);
		item = next;
	}
    if (mngr->fd) mxdgio_close(mngr->fd);
}

/* this function initilizes a timer manager 
	Returns:
		Return a pointer to the manager.
*/
DGIOMNGR*
digit_io_timer_init(void)
{
    DGIOMNGR *mngr;

    mngr = (DGIOMNGR*) calloc(1, sizeof(DGIOMNGR));
    if (mngr)
    {
    	mngr->fd = mxdgio_open();
		if (mngr->fd < 0)
		{
			free(mngr);
			mngr = NULL;
		}
    }
    return mngr;
}

/*	add a digital io timer with a selected operation mode
	Inputs:
		<mngr> timer manager
		<port> specify which DIO pin
		<mode> the operation mode on the port
		<interval> the interval (in milliseconds) between 2 calls to a user-defined function
		<cb> the user-defined callback function 
		<arg> argument to the function
	Returns:
		0 on sucess, otherwise failure
*/
int
digit_io_timer_add_callback(DGIOMNGR *mngr, int port, int mode, int interval, digit_io_cb_t cb, void *arg)
{
    DGIOITEM *item;

    item = (DGIOITEM*) calloc (1, sizeof (DGIOITEM));
    if (!item)
        return -1;
    item->next = mngr->list;
	mngr->list = item;
    item->cb  = cb;
    item->arg = arg;
    item->port = port;
    item->mode = mode;
	item->interval = interval;
	item->next_time = interval;
	item->last_signal = mxdgio_get_input_signal(mngr->fd, port);
    return 0;
}

void
digit_io_timer_dispatch_quit(DGIOMNGR *mngr)
{
    if (mngr) mngr->dispatch = 0;
}

#define MAX_TIME 0XFFFFFFFF

/*  start and dispatch the timer operations
    Inputs:
        <mngr> the manager
	Returns:
		none
*/
void
digit_io_timer_dispatch(DGIOMNGR *mngr)
{
    DGIOITEM *item;
	unsigned int ms_sleep, n;
#if !defined(_WIN32_WCE) && !defined(WIN32)
    struct timeval to;
#endif

	mngr->dispatch = 1;

    while(mngr->list && mngr->dispatch)
    {
		for (item = mngr->list; item != NULL; item = item->next)
		{
			if (mngr->now_time < item->next_time)  /* not yet */
				continue;
			n = mngr->now_time - item->next_time;
			/* over due, executable */
			item->next_time = mngr->now_time+item->interval-n; /* move to the next time */
			dgio_input_change_exec(mngr->fd, item);
		}
		ms_sleep = MAX_TIME;
		/* get the amount of time to sleep */
		for (item = mngr->list; item != NULL; item = item->next)
		{
			if (mngr->now_time < item->next_time)  /* not yet */
			{
				n = item->next_time - mngr->now_time;
				if (n < ms_sleep) ms_sleep = n;
				continue;
			}
		}
		if (ms_sleep!=MAX_TIME)
		{
#if !defined(_WIN32_WCE) && !defined(WIN32)
			to.tv_sec  = ms_sleep/1000;
			to.tv_usec = (ms_sleep%1000)*1000;
			if (select (0, NULL, NULL, 0, &to) != 0) /* sleep */
				break;
#else
			Sleep(ms_sleep);
#endif
			mngr->now_time += ms_sleep;
		}
    }
    dgio_input_change_release(mngr);
}
