/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#ifndef _DIGIO_H
#define _DIGIO_H

#if !defined(_WIN32_WCE) && !defined(WIN32)
typedef int HANDLE; /* to have a uniform handler type presentation */
#else
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*  open an access to digit io pins
    Returns:
	a file descriptor of the access
*/
HANDLE
mxdgio_open(void);

/*  get the signal state of a digit input port
    Inputs:
	<fd> the access to the device
	<port> port #
    Returns:
	1 or 0 on success, -1 on failure
*/
int
mxdgio_get_input_signal(HANDLE fd, int port);

/*  get the signal state of a digit output port 
    Inputs:
	<fd> the access to the device
	<port> port #
    Returns:
	1 or 0 on success, -1 on failure
*/
int	
mxdgio_get_output_signal(HANDLE fd, int port);

/*  pass a high signal to a digit output port
    Inputs:
	<fd> the access to the device
	<port> port #
    Returns:
	0 on success, -1 on failure
*/
int	
mxdgio_set_output_signal_high(HANDLE fd, int port);

/*  pass a low signal to a digit output port
    Inputs:
	<fd> the access to the device
	<port> port #
    Returns:
	0 on success, -1 on failure
*/
int	
mxdgio_set_output_signal_low(HANDLE fd, int port);

/* close the access to the device */
void
mxdgio_close(HANDLE fd);

#ifdef __cplusplus
}
#endif

#endif

