/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#ifndef LCM_H
#define LCM_H

/* the size of the LCM screen */
#if !defined(_WIN32_WCE) && !defined(WIN32)
#ifndef MAX_LCM_ROWS
#define MAX_LCM_ROWS 2
#endif
#else
#ifdef UC74XX /* UC-74xx */
#define MAX_LCM_ROWS 8
#else /* DA-66x */
#define MAX_LCM_ROWS 2
#endif
#endif
#define MAX_LCM_COLS 16 

#if !defined(_WIN32_WCE) && !defined(WIN32)
typedef int HANDLE; /* to have a uniform handler type presentation */
#else
#include <windows.h>
#endif

/*
 *  LCM IO control commands
 */
#define IOCTL_LCM_GOTO_XY               1
#define IOCTL_LCM_CLEAN_SCREEN          2	
#define IOCTL_LCM_CLEAN_LINE            3
#define IOCTL_LCM_GET_XY                4
#define IOCTL_LCM_BACK_LIGHT_ON         5
#define IOCTL_LCM_BACK_LIGHT_OFF        6	
#define IOCTL_LCM_CURSOR_ON				9
#define IOCTL_LCM_CURSOR_OFF			10
#define IOCTL_LCM_AUTO_SCROLL_ON		13
#define IOCTL_LCM_AUTO_SCROLL_OFF       14
#define IOCTL_LCM_BLINK_ON				15
#define IOCTL_LCM_BLINK_OFF				16

#define mxlcm_clear_screen(fd)   mxlcm_control(fd, IOCTL_LCM_CLEAN_SCREEN)
#define mxlcm_clear_line(fd)     mxlcm_control(fd, IOCTL_LCM_CLEAN_LINE)
#define mxlcm_back_light_on(fd)  mxlcm_control(fd, IOCTL_LCM_BACK_LIGHT_ON)
#define mxlcm_back_light_off(fd) mxlcm_control(fd, IOCTL_LCM_BACK_LIGHT_OFF)
#define mxlcm_cursor_on(fd)      mxlcm_control(fd, IOCTL_LCM_CURSOR_ON)
#define mxlcm_cursor_off(fd)     mxlcm_control(fd, IOCTL_LCM_CURSOR_OFF)
#define mxlcm_blink_on(fd)       mxlcm_control(fd, IOCTL_LCM_BLINK_ON)
#define mxlcm_blink_off(fd)      mxlcm_control(fd, IOCTL_LCM_BLINK_OFF)

#ifdef __cplusplus
extern "C" {
#endif

/*	open a fd associated with the LCM
*/
HANDLE  mxlcm_open(void);

/*	get the position of the cursor
	Inputs:
		<fd> the LCM
	Outputs
		<x> the x of the curosr
		<y> the y of the curosr
	Returns:
		0, on success, otherwise failure		
*/
int  mxlcm_get_cursor(HANDLE fd, int *x, int *y);

/*	set the position of the cursor
	Inputs:
		<fd> the LCM
		<x> the x of the curosr
		<y> the y of the curosr
	Returns:
		0, on success, otherwise failure		
*/
int  mxlcm_set_cursor(HANDLE fd, int x, int y);

/*	control the LCM
	Inputs:
		<fd> the LCM
		<flag> flag to control the LCM
	Returns:
		0, on success, otherwise failure		
*/
int  mxlcm_control(HANDLE fd, int flag);

/*  output data onto the LCM starting from a position
    Inputs:
    	<fd> the LCM
	<x><y> the position of the output
	<data> data
	<len> the length of the data
    Returns:
    	0 on success, otherwise failure
*/
int mxlcm_write(HANDLE fd, int x, int y, char *data, int len);

/*  output data onto the whole screen of the LCM
    Inputs:
    	<fd> the LCM
	<text> the content
    Returns:
    	0 on success, otherwise failure
*/
int mxlcm_write_screen(HANDLE fd, char text[MAX_LCM_ROWS][MAX_LCM_COLS]);

void mxlcm_close(HANDLE fd);

#ifdef __cplusplus
}
#endif

#endif
