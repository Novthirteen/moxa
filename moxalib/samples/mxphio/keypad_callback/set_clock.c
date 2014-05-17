/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#if defined( _WIN32_WCE)
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include "keypad_lcm.h"

typedef struct _LCMCLK
{
#ifdef _WIN32_WCE
	SYSTEMTIME clock;
#else
    struct tm clock;
#endif
    int digit;
} LCMCLK;

//LCMCLK gClock;
LCMCLK gClock;

/* cursor positions */
static int cursor_x[6] = { 6, 9, 12, 5, 8, 11 };
static int cursor_y[6] = { 0, 0, 0, 1, 1, 1 };

/*
0123456789012
...2009-03-12...
....13:24:56....
*/
static void
lcm_clock(LCMDAT *lcm, LCMCLK *clk)
{
	char s[MAX_LCM_COLS+1];
#ifdef UC74XX /* UC-74xx */
	int i;
#endif

#ifdef _WIN32_WCE
	sprintf(s, "   %d-%02d-%02d   ", clk->clock.wYear, clk->clock.wMonth, clk->clock.wDay);
	strncpy(lcm->text[0], s, MAX_LCM_COLS);
	sprintf(s, "    %02d:%02d:%02d    ", clk->clock.wHour, clk->clock.wMinute, clk->clock.wSecond);
	strncpy(lcm->text[1], s, MAX_LCM_COLS);
#else
    strftime(s, MAX_LCM_COLS+1, "   %Y-%m-%d   ", &clk->clock);
	strncpy(lcm->text[0], s, MAX_LCM_COLS);
    strftime(s, MAX_LCM_COLS+1, "    %H:%M:%S    ", &clk->clock);
	strncpy(lcm->text[1], s, MAX_LCM_COLS);
#endif
#ifdef UC74XX /* UC-74xx */
	memset(s, ' ', MAX_LCM_COLS); s[MAX_LCM_COLS] = 0;
	for (i = 2; i < MAX_LCM_ROWS; i++)
		strncpy(lcm->text[i], s, MAX_LCM_COLS);
#endif
    lcm->cursor_x = cursor_x[clk->digit];
	lcm->cursor_y = cursor_y[clk->digit];
}

static int days[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/* KEY_UP */
static void
clock_value(LCMDAT *lcm, void *data)
{
    LCMCLK *clk = (LCMCLK*) data;

    switch(clk->digit)
    {
    case 0: /*year*/
#ifdef _WIN32_WCE
		clk->clock.wYear++;
		if (clk->clock.wYear==125)
			clk->clock.wYear=100;
#else
		clk->clock.tm_year++;
		if (clk->clock.tm_year==125)
			clk->clock.tm_year=100;
#endif
        break;
    case 1:
#ifdef _WIN32_WCE
		clk->clock.wMonth++;
		if (clk->clock.wMonth==12)
			clk->clock.wMonth=0;
#else
		clk->clock.tm_mon++;
		if (clk->clock.tm_mon==12)
			clk->clock.tm_mon=0;
#endif
        break;
    case 2:
#ifdef _WIN32_WCE
		clk->clock.wDay++;
		if (clk->clock.wDay==days[clk->clock.wMonth]+1)
			clk->clock.wDay=1;
#else
		clk->clock.tm_mday++;
		if (clk->clock.tm_mday==days[clk->clock.tm_mon]+1)
			clk->clock.tm_mday=1;
#endif
        break;
    case 3:
#ifdef _WIN32_WCE
		clk->clock.wHour++;
		if (clk->clock.wHour==24)
			clk->clock.wHour=0;
#else
		clk->clock.tm_hour++;
		if (clk->clock.tm_hour==24)
			clk->clock.tm_hour=0;
#endif
        break;
    case 4:
#ifdef _WIN32_WCE
		clk->clock.wMinute++;
		if (clk->clock.wMinute==60)
			clk->clock.wMinute=0;
#else
		clk->clock.tm_min++;
		if (clk->clock.tm_min==60)
			clk->clock.tm_min=0;
#endif
        break;
    case 5:
#ifdef _WIN32_WCE
		clk->clock.wSecond++;
		if (clk->clock.wSecond==60)
			clk->clock.wSecond=0;
#else
		clk->clock.tm_sec++;
		if (clk->clock.tm_sec==60)
			clk->clock.tm_sec=0;
#endif
        break;
    }
    lcm_clock(lcm, clk);
}

/* KEY_DOWN */
static void
clock_shift(LCMDAT *lcm, void *data)
{
    LCMCLK *clk = (LCMCLK*) data;

    clk->digit++;
    if (clk->digit==6) clk->digit = 0;
    lcm_clock(lcm, clk);
}

/* KEY_MENU
*/
static void
clock_menu(LCMDAT *lcm, void *data)
{
    LCMCLK *clk = (LCMCLK*) data;
#ifndef _WIN32_WCE
    time_t now;
    struct tm *tm;
#endif

#ifdef _WIN32_WCE
	GetLocalTime(&clk->clock);
#else
    now = time(0);
    tm = localtime(&now);
    clk->clock = *tm;
#endif
    clk->digit = 0;

    lcm_clock(lcm, clk);
}

/* KEY_SEL */
static void
clock_set(LCMDAT *lcm, void *data)
{
    LCMCLK *clk = (LCMCLK*) data;
#ifndef _WIN32_WCE
    struct timeval tv;
#endif

#ifdef _WIN32_WCE
	SetLocalTime(&clk->clock);
#else
    tv.tv_sec = mktime(&clk->clock);
	tv.tv_usec = 0;
    settimeofday(&tv, NULL);
#endif
    clock_menu(lcm, clk);
}

static int
set_clock_menu_init(LCMKPD *hndl)
{
	lcm_show_t funs[4];

	funs[0] = clock_menu;
	funs[1] = clock_set;
	funs[2] = clock_shift;
	funs[3] = clock_value;

	return keypad_lcm_menu_add(hndl, funs, &gClock);
}

int
#if defined(_WIN32_WCE)
WINAPI
WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
#else
main(int argc, char *argv[])
#endif
{
	LCMKPD *hndl;

#if !defined( _WIN32_WCE) && !defined(WIN32)
    (void) argc;
    (void) argv;
#endif

	hndl = keypad_lcm_menu_init();
	if (!hndl)
	{
		printf("fail to create LCM/KEYPAD session\n");
		return -1;
    }
	if (set_clock_menu_init(hndl))
	{
		printf("failed to initialize set clock menu\n");
		return -2;
	}
	keypad_lcm_menu_dispatch(hndl);
	return 0;
}
