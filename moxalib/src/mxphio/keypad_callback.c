/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    keypad_callback.c

    Routines to operate keypads in an embedded computer by
    a callback mechanism.

    2008-09-01	CF Lin
		new release
*/

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <mxkeypad.h>
#include <keypad_callback.h>

/*	register a handle for keypad/callback operations
	Returns:
		a handle representing the callback mechanism
	
	Note: each key is associated with a callback function
*/
KEYPAD*
keypad_callback_init(void)
{
    KEYPAD *kypd;
	
    kypd = (KEYPAD*) malloc(sizeof(KEYPAD));
    if (kypd)
    {
        memset(kypd,0,sizeof(KEYPAD));
	kypd->fd = keypad_open();
	if (kypd->fd < 0)
	{
	    free(kypd);
	    kypd = NULL;
	}
    }
    return kypd;
}

/*	associate a callback function with a keypad
	Inputs:
		<hndl> the handle representing a context 
		<key> the keypad num starting from 1
		<func> the callback function 
		<arg> the parameter of the callback function 
	Returns:
		0 on success, otherwise failure
*/
int
keypad_callback_add(KEYPAD *kypd, int key, keypad_callback_t func, void *arg)
{
	if (!func || key < 0 || key >= MAX_KEYPADS)
		return -1;

	kypd->keys[key].func = func;
	kypd->keys[key].arg = arg;
	return 0;
}

/*  dispatch all added callback functions when any keypad is pressed
    Inputs:
	<hndl> the handle representing a context 
*/
void
keypad_callback_dispatch(KEYPAD *kypd)
{
    int key;

    /* no function is defined, return the key */
    for (key=0; key<MAX_KEYPADS;key++)
    {
        if (kypd->keys[key].func)
	break;
    }
    if (key==MAX_KEYPADS)
        return;
    key = -1;
    kypd->dispatch = 1;
    /* dispatch functions */
    while(kypd->dispatch)
    {
	key = keypad_get_pressed_key(kypd->fd);
	if ( key < 0 )
	    return;
	if (key < MAX_KEYPADS && kypd->keys[key].func)
	    kypd->keys[key].func(kypd->keys[key].arg, key);
    }
}

/*	quit function dispatch 
	Inputs:
		<hndl> the handle representing a context 
*/
void
keypad_callback_quit(KEYPAD *kypd)
{
    if (kypd)
    {
    kypd->dispatch = 0;
    if (kypd->fd>0) keypad_close(kypd->fd);
    free(kypd);
    }
}

