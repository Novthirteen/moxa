
/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    keypad_lcm.c

    Routines to operate the LCM panel and the keypad buttons on an embedded computer.

    2008-07-01	CF Lin
*/
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "keypad_lcm.h"

HANDLE
keypad_lcm_menu_get_lcm_fd(LCMKPD *lcmk)
{
    if (lcmk)
    return lcmk->lcm_fd;
    else
    return 0;
}

int
keypad_lcm_menu_add(LCMKPD *lcmk, lcm_show_t *funs, void *data)
{
    MLAYER *mlyr, **dptr;
    int i;

    if (!lcmk || !funs)
    {
        return -1;
    }
    /* generate a layer */
    mlyr = (MLAYER*) malloc(sizeof(MLAYER));
    if (mlyr==NULL)
        return -1;
    mlyr->next = NULL;
    if (lcmk->toper==NULL)
    {
        lcmk->toper = mlyr;
    }
    else
    {
        dptr = &(lcmk->toper);
        while((*dptr)->next) dptr = &(*dptr)->next;
	(*dptr)->next = mlyr;
    }
    for (i=0; i < MAX_KEYPADS; i++)
        mlyr->funs[i] = funs[i];
    mlyr->data = data;
    return 0;
}

static void
keypad_lcm_show(LCMKPD *lkpd, int last_key)
{
    LCMDAT lcm;
	lcm_show_t show;

    show = lkpd->layer->funs[last_key];
    if (show)
	{
		lkpd->last_key = last_key;
		show(&lcm, lkpd->layer->data);
		mxlcm_write_screen(lkpd->lcm_fd, lcm.text);
		mxlcm_set_cursor(lkpd->lcm_fd, lcm.cursor_x, lcm.cursor_y);
	}
}

static void
keypad_lcm_menu(LCMKPD *lkpd, int key)
{
    if (lkpd->last_key==key)
    {
        /* next menu */
        if (lkpd->layer->next)
	    lkpd->layer = lkpd->layer->next;
	else
	    lkpd->layer = lkpd->toper;
    }
    else
    {
        /* current menu */
    }
    keypad_lcm_show(lkpd, key);
}

void
keypad_lcm_menu_quit(LCMKPD *lkpd)
{
    MLAYER *mlyr, *ltmp;

    if (lkpd)
    {
        if (lkpd->lcm_fd>0) 
	    mxlcm_close(lkpd->lcm_fd);

		mlyr = lkpd->toper;
		while(mlyr)
		{
			ltmp = mlyr;
			mlyr = mlyr->next;
			free(ltmp);
		}
		if (lkpd->kypd) 
	    keypad_callback_quit(lkpd->kypd);
		free(lkpd);
    }
}

int
keypad_lcm_menu_dispatch(LCMKPD *lkpd)
{
    int key;

    if (lkpd->toper==NULL)
    return -1;

    keypad_callback_add(lkpd->kypd, 0, keypad_lcm_menu, (void *)lkpd);
    for (key=1; key < MAX_KEYPADS; key++)
      keypad_callback_add(lkpd->kypd, key, keypad_lcm_show, (void *)lkpd);

    lkpd->last_key = -1;
    lkpd->layer = lkpd->toper;
#if !defined( _WIN32_WCE) && !defined(WIN32)
	mxlcm_blink_on(lkpd->lcm_fd);
#endif
    keypad_lcm_menu(lkpd, 0);

    keypad_callback_dispatch(lkpd->kypd);
    return 0;
}

LCMKPD*
keypad_lcm_menu_init(void)
{
    LCMKPD *lkpd;

    /* create a body */
    lkpd = (LCMKPD*) malloc(sizeof(LCMKPD));
    if (lkpd)
    {
        memset(lkpd, 0, sizeof(LCMKPD));
		/* get the LCM handle */
        lkpd->lcm_fd = mxlcm_open();
        /* init a keypad session */
        lkpd->kypd = keypad_callback_init();
	if (lkpd->lcm_fd < 0 || lkpd->kypd==NULL)
	{
	    keypad_lcm_menu_quit(lkpd);
	    lkpd = NULL;
	}
    }
    return lkpd;
}
