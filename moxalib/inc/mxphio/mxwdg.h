/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#ifndef _WDG_H
#define _WDG_H

#ifdef __cplusplus
extern "C" {
#endif

/*  start operating a watch dog timer
    Inputs:
	<time> in milliseconds, within a specified time period, the watch dog time 
		needs to be refreshed in order to avoid reset
    Returns:
	positive value of a file descriptor on success, otherwise on failure
*/
int	mxwdg_open(unsigned long time);

/*  stop operating a watch dog timer
    Inputs:
	<fd> file descriptor respresenting the timer
*/
void mxwdg_close(int fd);

/*  refresh a watch dog timer, this must be called periodically
    Inputs:
	<fd> file descriptor respresenting the timer
    Returns:
	0 on success, failure otherwise
*/
int	mxwdg_refresh(int fd);

#ifdef __cplusplus
}
#endif

#endif
