/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#include <stdio.h>
#if !defined( _WIN32_WCE) && !defined(WIN32)
#include <unistd.h>
#else
#include <windows.h>
#endif
#include <time.h>
#include <string.h>
#include "mxlcm.h"

#ifdef WIN32
#define strftime	_strftime
#endif

void
lcm_clock(HANDLE fd)
{
#ifdef UC74XX /* UC-74xx */
	int i;
#endif
#ifdef _WIN32_WCE
	SYSTEMTIME now;
#else
    time_t now;
    struct tm *tm;
#endif
    char text[MAX_LCM_ROWS][MAX_LCM_COLS];
	char s[MAX_LCM_COLS+1];

#ifdef _WIN32_WCE
	GetLocalTime(&now);
	sprintf(s, "   %d-%02d-%02d   ", now.wYear, now.wMonth, now.wDay);
	strncpy(text[0], s, MAX_LCM_COLS);
	sprintf(s, "    %02d:%02d:%02d    ", now.wHour, now.wMinute, now.wSecond);
	strncpy(text[1], s, MAX_LCM_COLS);
#else
    now = time(0);
    tm = localtime(&now);
    strftime(s, MAX_LCM_COLS+1, "   %Y-%m-%d   ", tm);
	strncpy(text[0], s, MAX_LCM_COLS);
    strftime(s, MAX_LCM_COLS+1, "    %H:%M:%S    ", tm);
	strncpy(text[1], s, MAX_LCM_COLS);
#endif
#ifdef UC74XX /* UC-74xx */
	memset(s, ' ', MAX_LCM_COLS); s[MAX_LCM_COLS] = 0;
	for (i = 2; i < MAX_LCM_ROWS; i++)
		strncpy(text[i], s, MAX_LCM_COLS);
#endif
    mxlcm_write_screen(fd, text);
}

int
#if defined(_WIN32_WCE)
WINAPI
WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
#else
main(int argc, char *argv[])
#endif
{
	HANDLE fd;

#if !defined( _WIN32_WCE) && !defined(WIN32)
	(void) argc;
	(void) argv;
#endif

	fd = mxlcm_open();
	if (fd < 0)
		return -1;
	while(1)
	{
#if !defined( _WIN32_WCE) && !defined(WIN32)
		sleep(1);
#else
		Sleep(1000);
#endif
		lcm_clock(fd);
	}
	mxlcm_close(fd);

	return 0;
}
