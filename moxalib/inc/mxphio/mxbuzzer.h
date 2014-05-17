/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#ifndef BUZZER_H
#define BUZZER_H

#if !defined(_WIN32_WCE) && !defined(WIN32)
typedef int HANDLE; /* to have a uniform handler type presentation */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*  open a handle (device file) that operates the buzzer 
    Inputs:
		none
	Returns:
		none
*/
HANDLE  mxbuzzer_open(void);

/*  beep the buzzer
    Inputs:
        <fd> the handle of the buzzer
		<time> in milliseconds, the amount of time 
			that the buzzer beeps
	Returns:
		none
*/
#if !defined(_WIN32_WCE) && !defined(WIN32)
void mxbuzzer_beep(HANDLE fd, int time);
#else
void mxbuzzer_beep(HANDLE fd, int time, int freq);
#endif

/*  close the buzzer 
    Inputs:
        <fd> the handle of the buzzer
	Returns:
		none
*/
void mxbuzzer_close(HANDLE fd);

#ifdef __cplusplus
}
#endif

#endif
