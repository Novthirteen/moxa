/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    udp_connection_broadcast_client.c

    Routines to simulate a simple UDP client which broadcasts a message to the LAN periodically.
	The client will receive the ip/port information of the repliers.

    2008-09-25	CF Lin
		new release
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "connection.h"

/*  get a ip/port information of a UDP server 
	<ip> <port>
*/
static int	
udp_broadcast_client_dispatch (MHANDLE hndl, void *private_data, DATAPKT *dpkt)
{
	unsigned char ch;

	(void) hndl;
	(void) private_data;

	dpkt->packet_consumed = dpkt->packet_size; /* consumed all */

	ch = dpkt->packet_data[dpkt->packet_size];
	dpkt->packet_data[dpkt->packet_size] = 0;
	printf("RECV [%d]: %s\n", dpkt->packet_size, dpkt->packet_data);
	dpkt->packet_data[dpkt->packet_size] = ch;

	return 0; 
}

/* after making a client connection, the program calls this user-defined function */
static void*
udp_broadcast_client_open(MHANDLE hndl, void *param, unsigned int *timer_interval)
{
	(void) hndl;
	(void) param;

	*timer_interval = 3000; /* set the period (in milliseconds) of the timer */

	return hndl;
}

/* periodically, the client send a broadcast message to LAN */
static void
udp_broadcast_client_timer(void *private_data)
{
	MHANDLE hndl = (MHANDLE) private_data;
	char buf[256];

	sprintf(buf, "I am a UDP broadcast client.");
printf("[SEND] %s\n", buf);
	connection_send_data(hndl, buf, strlen(buf));
}

static int
udp_broadcast_client_init (int argc, char **argv)
{
	MHANDLE con;
	UDPXPRM param;
	USERFUN funs;

	if (argc < 2)
	{
		printf("usage: <progname> <listen port>\n");
		return -1;
	}
	/* setting the parameters of a server */
	memset(&param, 0 , sizeof(UDPXPRM));
	param.host = NULL; /* this implies a broadcast client */
	param.listen_port = atoi(argv[1]);

	/* callback functions */
	memset(&funs, 0 , sizeof(USERFUN));
	funs.open = udp_broadcast_client_open;
	funs.dispatch = udp_broadcast_client_dispatch;
	funs.timer = udp_broadcast_client_timer;

	/* make a client connection */
	if ((con=connection_open (CONNECTION_TYPE_UDPCLIENT, &param, &funs, NULL)) == NULL)
		return -1;
	else
		return 0;
}

int
#if defined(_WIN32_WCE)
WINAPI 
WinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
#else
main (int argc, char **argv)
#endif
{
#if defined(_WIN32_WCE)
	int	argc;
	char cmdline[256], *argv[32];

	WideCharToMultiByte(CP_ACP, 0, (LPCTSTR)lpCmdLine, 255, cmdline, 256, NULL, NULL);
	argc = split_line(argv+1, 32, cmdline)+1;
#endif

    connection_dispatch_loop(argc, argv, udp_broadcast_client_init, NULL, 0);

	return 0;
}

