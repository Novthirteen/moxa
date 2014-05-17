/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#ifndef _KEYPAD_LCM_MENU_H
#define _KEYPAD_LCM_MENU_H

#include "keypad_callback.h"
#include "mxlcm.h"

typedef struct _LCMDAT
{
    char text[MAX_LCM_ROWS][MAX_LCM_COLS];
    int cursor_x, cursor_y;
} LCMDAT;

typedef void (*lcm_show_t)(LCMDAT*, void*);

typedef struct _MLAYER
{
    lcm_show_t funs[MAX_KEYPADS]; /* MAX_KEYPADS menu functions */
    void   *data; /* the argument of the function */
    struct _MLAYER *next;
} MLAYER;

typedef struct _LCMKPD
{
    HANDLE lcm_fd;
    KEYPAD *kypd;
    int last_key;
    MLAYER *toper, *layer;
} LCMKPD;

#ifdef __cplusplus
extern "C" {
#endif
/*  initialize a LCM/KEYPAD session 
    Inputs:
        None
    Returns:
        a handler representing the session 
*/
LCMKPD*
keypad_lcm_menu_init(void);

/*  add menu items onto a session
    Inputs:
        <hndl> the session
		<funs> an array of menu functions , the number of them must be MAX_KEYPADS
		<data> argument of the functions
    Returns
        0, on okay
*/
int
keypad_lcm_menu_add(LCMKPD *hndl, lcm_show_t *funs, void *data);

/*  dispatch lcm/keypad operation associated with defined menu items
    Inputs:
        <hndl> the handler reprsenting a sesseion
    Returns:
        0, on okay
*/
int
keypad_lcm_menu_dispatch(LCMKPD *hndl);

/*  quit a LCM/KEYPAD session
    Inputs:
        <hndl> the handler reprsenting the session 
    Returns:
        None
*/
void
keypad_lcm_menu_quit(LCMKPD *hndl);

HANDLE
keypad_lcm_menu_get_lcm_fd(LCMKPD *hndl);

#ifdef __cplusplus
}
#endif

#endif
