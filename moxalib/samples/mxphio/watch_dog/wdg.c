/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#include <stdio.h>
#if !defined( _WIN32_WCE) && !defined(WIN32)
#include <unistd.h>
#else
#include <windows.h>
#endif
#include "mxwdg.h"

int
#if defined(_WIN32_WCE)
WINAPI
WinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
#else
main(int argc, char *argv[])
#endif
{
    int i, fd;

#if !defined( _WIN32_WCE) && !defined(WIN32)
    (void) argc;
    (void) argv;
#endif

    /* set watch dog timer, must be refreshed in 60 seconds */
    fd = mxwdg_open(60*1000);
    if (fd < 0)
    {
        printf("fail to open the watch dog\n");
		return -1;
    }
    i = 0;
    while (i++ < 10)
    {
		/* refresh the timer */
        mxwdg_refresh(fd);
#if !defined( _WIN32_WCE) && !defined(WIN32)
		sleep(i);
#else
		Sleep(i*1000);
#endif
    }
    mxwdg_close(fd);

    return 0;
}
