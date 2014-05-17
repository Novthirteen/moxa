/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    main.c

    main routines and callbacks to operate timer functions on digital IO ports.

    2009-02-15	CF Lin
		new release
*/
#include <stdio.h>
#include <stdlib.h>
#include "digit_io_timer.h"

static int
input_chg_cb(int port, int sig, void *arg)
{
	printf("input_chg_cb() port %d sig %d\n", port, sig);
	return 0;
}

static int
input_get_cb(int port, int sig, void *arg)
{
	printf("input_get_cb() port %d sig %d\n", port, sig);
	return 0;
}

static int
output_set_cb(int port, int last_sig, void *arg)
{
	printf("output_set_cb() port %d last sig %d\n", port, last_sig);
	last_sig++;
	last_sig %= 2;
	printf("new sig=%d\n", last_sig);

	return last_sig;
}

static int
output_get_cb(int port, int sig, void *arg)
{
	printf("output_get_cb() port %d sig %d\n", port, sig);
	return 0;
}

#define INTERVAL	10000

int
#if defined(_WIN32_WCE)
WINAPI
WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
#else
main(int argc, char *argv[])
#endif
{
	DGIOMNGR *mngr;
	int port;
	int interval;
#if defined(_WIN32_WCE)
	int	argc;
	char cmdline[256], *argv[32];

	WideCharToMultiByte(CP_ACP, 0, (LPCTSTR)lpCmdLine, 255, cmdline, 256, NULL, NULL);
	argc = split_line(argv+1, 32, cmdline)+1;
#endif

	if (argc > 1) interval = atoi(argv[1]);
	else interval = INTERVAL;

	mngr = digit_io_timer_init();
	if (mngr == NULL) {
		printf("digit_io_timer_init() error\n");
		return -1;
	}
	for (port = 0; port < 4; port++) {
		/* since list is LIFO last callbacks are added first */
		if (digit_io_timer_add_callback(mngr, port, DGTIO_GET_INPUT_STATE_CHANGE, interval, input_chg_cb, &port) < 0) {
			printf("add %d input change callback error\n", port);
			return -2;
		}
		if (digit_io_timer_add_callback(mngr, port, DGTIO_GET_INPUT, interval, input_get_cb, &port) < 0) {
			printf("add %d input callback error\n", port);
			return -3;
		}
		if (digit_io_timer_add_callback(mngr, port, DGTIO_SET_OUTPUT, interval, output_set_cb, &port) < 0) {
			printf("add %d set output callback error\n", port);
			return -4;
		}
		if (digit_io_timer_add_callback(mngr, port, DGTIO_GET_OUTPUT, interval, output_get_cb, &port) < 0) {
			printf("add %d get output callback error\n", port);
			return -5;
		}
	}
	digit_io_timer_dispatch(mngr);
	return 0;
}
