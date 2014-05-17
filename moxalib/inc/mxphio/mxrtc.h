/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#ifndef _RTC_H
#define _RTC_H

#ifdef __cplusplus
extern "C" {
#endif

/*  get the RTC time, 
    Inputs:
        <timeA> point to an empty array (at least 6 elements) of time values
			year, month, day, hour, minute, seconds
    Returns:
    	0 on success, otherwise failure
*/
int mxrtc_get(unsigned int *timeA);

/*  set RTC time
    Inputs:
        <timeA> point to time values, (year, month, day, hour, minute, seconds)
    Returns:
    	0 on success, otherwise failure
*/    
int mxrtc_set(unsigned int *timeA);

/*  get the rtc time and then sync its value onto the system 
    time
    Returns:
    	0 on success, otherwise failure
*/
int mxrtc_set_system_time(void);

/*  get the system time and then sync this value onto the rtc
 *  time
    Returns:
    	0 on success, otherwise failure
 */
int mxrtc_get_system_time(void);

#ifdef __cplusplus
}
#endif

#endif

