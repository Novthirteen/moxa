/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#ifndef KEYPAD_CALLBACK_H
#define KEYPAD_CALLBACK_H

#include "os-support.h"
#include "mxkeypad.h"

typedef void (*keypad_callback_t) (void*, int);

/* define a callback function for key press of a keypad */
typedef struct _KYPDCB
{
    keypad_callback_t	func; /* callback function */
    void *arg; /* argument of the function */
} KYPDCB;

typedef struct _KEYPAD
{
    int fd; /* the descriptor for keypad operations */
    KYPDCB keys[MAX_KEYPADS]; /* array of callback functions */
    int dispatch; /* control the callback function dispatch loop */
} KEYPAD;

#ifdef __cplusplus
extern "C" {
#endif

/*	register a handle for keypad/callback operations
	Returns:
		a handle representing the callback mechanism
	
	Note: each key is associated with a callback function
*/
KEYPAD*
keypad_callback_init(void);

/*	associate a callback function with a keypad
	Inputs:
		<hndl> the handle representing a context 
		<key> the keypad num starting from 0 
		<func> the callback function 
		<arg> the parameter of the callback function 
	Returns:
		0 on success, otherwise failure
*/
int
keypad_callback_add(KEYPAD *hndl, int key, keypad_callback_t func, void *arg);

/*	dispatch all added callback functions when any keypad is pressed
	Inputs:
		<hndl> the handle of a context 
*/
void
keypad_callback_dispatch(KEYPAD *hndl);

/*	force a context of keypad operations to stop
	Inputs:
		<hndl> the handle representing the context 

	Note: if a thread is used to dispatch functions, one 
	can use this method to stop the dispatch work
*/
void
keypad_callback_quit(KEYPAD *hndl);

#ifdef __cplusplus
}
#endif

#endif
