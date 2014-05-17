/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#include <stdio.h>
#include <time.h>
#include "mxrtc.h"

int
main(int argc, char *argv[])
{
	unsigned int t[6];

    (void) argc;
    (void) argv;

	if (mxrtc_get(t) < 0)
	{
		printf("mxrtc_get() error\n");
		return -1;
	}
	printf("RTC time: %d-%d-%d %d:%d:%d\n", t[0], t[1], t[2], t[3], t[4], t[5]);

	if (mxrtc_set(t) < 0)
	{
		printf("mxrtc_set() error\n");
		return -2;
	}
	printf("set RTC time OK\n");

	if (mxrtc_set_system_time() < 0)
	{
		printf("mxrtc_set_system_time() error\n");
		return -3;
	}
	printf("sync RTC time into system time OK\n");

	if (mxrtc_get_system_time() < 0)
	{
		printf("mxrtc_get_system_time() error\n");
		return -4;
	}
	printf("sync system time into RTC time OK\n");

    return 0;
}
