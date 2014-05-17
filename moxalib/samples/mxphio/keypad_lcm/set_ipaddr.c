/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#include <stdio.h>
#include <stdlib.h>
#include "set_ipconfig.h"

int
#if defined(_WIN32_WCE)
WINAPI
WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
#else
main(int argc, char *argv[])
#endif
{
	LCMKPD *hndl;

#ifndef _WIN32_WCE
    (void) argc;
    (void) argv;
#endif

	/* initialize the process */
	hndl = keypad_lcm_menu_init();
	if (!hndl)
	{
		printf("fail to create a LCM/KEYPAD session\n");
		return -1;
	}
	if (ipconfig_menu_init(hndl, INDEX_IPADDR))
	{
		printf("fail to get address\n");
		return -3;
	}
	if (ipconfig_menu_init(hndl, INDEX_NETMASK))
	{
		printf("fail to get netmask\n");
		return -4;
	}
#ifndef _WIN32_WCE
	if (ipconfig_menu_init(hndl, INDEX_NETWORK))
	{
		printf("fail to get network\n");
		return -5;
	}
	if (ipconfig_menu_init(hndl, INDEX_BROADCAST))
	{
		printf("fail to get broadcast\n");
		return -6;
	}
#endif
	if (ipconfig_menu_init(hndl, INDEX_GATEWAY))
	{
		printf("fail to get gateway\n");
		return -7;
	}
	keypad_lcm_menu_dispatch(hndl);
	return 0;
}
