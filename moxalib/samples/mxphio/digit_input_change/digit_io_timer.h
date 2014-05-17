/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
#ifndef _digit_io_TIMER
#define _digit_io_TIMER

#include "mxdgio.h"

/* the prototype of callback functions */
typedef int (*digit_io_cb_t) (int, int, void *);

/* the operation mode on a digital i/o port*/
enum
{
    DGTIO_GET_INPUT,	/* get the signal from a DI */
	DGTIO_GET_OUTPUT,	/* get the signal from a DO */
	DGTIO_SET_OUTPUT,	/* set the signal from a DO */
	DGTIO_GET_INPUT_STATE_CHANGE /* get the signal change at a DI */
};

typedef struct _DGIOITEM 
{
	int mode;			/* DGTIO_GET_INPUT, DGTIO_GET_OUTPUT, DGTIO_SET_OUTPUT, or, DGTIO_GET_INPUT_STATE_CHANGE */
	int port;			/* io port # */
	int last_signal;	/* last signal */
    digit_io_cb_t	cb; /* callback function */
    void *arg;			/* argument for the callback */
	unsigned int interval;	/* timer interval */
	unsigned int next_time; /* next trigger time */
	struct _DGIOITEM *next;
} DGIOITEM;

typedef struct _DGIOMNGR
{
    HANDLE fd;			/* the device file */
    int dispatch;	/* */
	unsigned int now_time;
    struct _DGIOITEM *list;	/* the list of digital io timer */
} DGIOMNGR;

/* this function initilizes a timer manager 
	Returns:
		Return a pointer to the manager.
*/
DGIOMNGR* digit_io_timer_init(void);

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
int digit_io_timer_add_callback (DGIOMNGR *mngr, int port, int mode, int interval, digit_io_cb_t cb, void *arg);

/*  start and dispatch the timer operations
    Inputs:
        <mngr> the manager
	Returns:
		none
*/
void digit_io_timer_dispatch(DGIOMNGR *mngr);

/* quit the routine digit_io_timer_dispatch 
	Inputs:
		<mngr> timer manager
	Returns:
		none
*/
void digit_io_timer_dispatch_quit(DGIOMNGR *mngr);

#endif
