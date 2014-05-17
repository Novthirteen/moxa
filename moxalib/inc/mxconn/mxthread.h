/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#ifndef _MXTHR_H
#define _MXTHR_H

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_WIN32_WCE) && !defined(WIN32)
#include <pthread.h>
#ifndef HANDLE
typedef int HANDLE;
#endif
#else
#include <windows.h>
#endif

#if defined(_WIN32_WCE) || defined(WIN32)
typedef DWORD (*thread_cb_t)(void*);
#else
typedef void* (*thread_cb_t) (void*);
#endif

HANDLE mxthread_create(thread_cb_t cb, void *arg);

#ifdef __cplusplus
}
#endif

#endif
