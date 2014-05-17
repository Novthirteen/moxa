/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#ifndef _SUPPORT_H
#define _SUPPORT_H

#if WORDS_BIGENDIAN
#define BSWAP16(c) c
#define BSWAP32(c) c
#else
/* convert the bytes of a 16-bit integer to big endian */
#define BSWAP16(c) (((((c) & 0xff) << 8) | (((c) >> 8) & 0xff)))
/* convert the bytes of a 32-bit integer to big endian */
#define BSWAP32(c) ((((c)>>24)&0xff)|(((c)>>8)&0xff00)|(((c)<<8)&0xff0000)|((c)<<24))
#endif

#if defined (_WIN32_WCE) || defined (WIN32)
#include <windows.h>
#if defined(_DEBUG)
#define DEBUG 1
#endif

typedef unsigned int u_int;

#ifdef DEBUG
#define dbgprintf log_message
#else
#define dbgprintf
#endif

#define strdup _strdup
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define vsnprintf _vsnprintf
#define snprintf _snprintf

//typedef int size_t;

#define DIR_SEP '\\'
#define DIR_SEP_OPPOSITE '/'

#else /* ************ LINUX ********************* */

#ifdef DEBUG
#define dbgprintf log_message
#else
#define dbgprintf( a... )
#endif

/*#define FD_SETSIZE	256*/
#define DIR_SEP '/'
#define DIR_SEP_OPPOSITE '\\'

//typedef unsigned int HANDLE;

#endif /* **** end LINUX *******/

#define MAX_CONNECTIONS	FD_SETSIZE

/* ************************ */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif


#endif

