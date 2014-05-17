/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

/*
    rtc.c

    Routines to operate the RTC clock in an embedded computer.

    2008-07-01	CF Lin
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <linux/rtc.h>

/* device node of the RTC */
#define RTC_DEV_NODE "/dev/rtc"

static int
rtc_time(struct tm *tm, int code)
{
    int fd, rtn;

    fd = open(RTC_DEV_NODE, O_RDWR);
    if (fd > 0)
	{
		if(ioctl( fd, code, tm)==0)
			rtn = 0;
		else
			rtn = -2;
		close(fd);
	}
	else
		rtn = -1;
	return rtn;
}

int
mxrtc_set(unsigned int *timeA)
{
    struct tm rtc;

	rtc.tm_year = timeA[0] - 1900;
	rtc.tm_mon = timeA[1] - 1;
	rtc.tm_mday = timeA[2];
	rtc.tm_hour = timeA[3];
	rtc.tm_min = timeA[4];
	rtc.tm_sec = timeA[5];
    return rtc_time(&rtc, RTC_SET_TIME);
}

int
mxrtc_get(unsigned int *timeA)
{
    struct tm rtc;

    if (rtc_time(&rtc, RTC_RD_TIME)!=0)
        return -1;

	timeA[0] = rtc.tm_year + 1900;
	timeA[1] = rtc.tm_mon + 1;
	timeA[2] = rtc.tm_mday;
	timeA[3] = rtc.tm_hour;
	timeA[4] = rtc.tm_min;
	timeA[5] = rtc.tm_sec;
    return 0;
}

int
//rtc_from_system_time(void)
mxrtc_set_system_time(void)
{
	struct tm rtc;
    struct timeval tv;

    if (rtc_time(&rtc, RTC_RD_TIME)!=0)
		return -1;
	
	tv.tv_usec = 0;
	tv.tv_sec = mktime(&rtc);
	settimeofday(&tv, NULL);
	return 0;
}

int
//rtc_to_system_time(void)
mxrtc_get_system_time(void)
{
    time_t ts = time(0);
	struct tm *tm;

	tm = localtime(&ts);
    return rtc_time(tm, RTC_SET_TIME);
}


