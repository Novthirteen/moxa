/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#include <stdio.h>
#include <unistd.h>
#include "mxbuzzer.h"

int
main(int argc, char *argv[])
{
    int i, fd;

    (void) argc;
    (void) argv;
	
	/* open the handle to the buzzer device */
    fd = mxbuzzer_open();
    if (fd < 0)
    {
        printf("fail to open the buzzer\n");
	return -1;
    }
    i = 0;
    while (i++ < 10)
    {
		/* sound the buzzer */
        mxbuzzer_beep(fd, i*100);
	sleep(i);
    }
	/* close the handle */
    mxbuzzer_close(fd);
    return 0;
}
