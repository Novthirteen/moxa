/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    udp_connection_broadcast_server.c

    Routines to simulate a simple UDP server which receives broadcast messages and replies
	each message with the ip/port of the server.

    2008-09-25	CF Lin
		new release
*/
#include <stdlib.h>
#include <stdio.h>
#include "connection.h"

#define SERVER_LISTEN_PORT 502

/* got a broadcast message and reply it with the ip/port of the server. */
static int	
udp_broadcast_server_dispatch_client (MHANDLE hndl, void *private_data, DATAPKT *dpkt)
{
	unsigned char *info = (unsigned char*) private_data;
	unsigned char ch;

	dpkt->packet_consumed = dpkt->packet_size;
	ch = dpkt->packet_data[dpkt->packet_size];
	dpkt->packet_data[dpkt->packet_size] = 0;
	printf("[RECV] %s\n", dpkt->packet_data);
	dpkt->packet_data[dpkt->packet_size] = ch;

	/* send back <ip> <port> */
	printf("SEND [%d]: %s\n", strlen(info), info);
	connection_send_data(hndl, info, strlen(info));
	return 0;
}

/* after accepting a client connection, the program calls this user-defined function */
static void*
udp_broadcast_server_open(MHANDLE hndl, void *param, unsigned int *timer_interval)
{
	unsigned char *info;

	(void) timer_interval;

	info = malloc(256);
	if (info==NULL)
	{
		connection_destroy(hndl);
	}
	else /* form a string buffer containing the ip/port information of the server */
	{
		UDPXPRM *p = (UDPXPRM*) param;
		struct in_addr a;

		a.s_addr = p->ip;
		sprintf(info, "%s %d", inet_ntoa (a), p->listen_port);
	}
	return info;
}

/* before disconnecting a client, the program calls this user-defined function */
static void
udp_broadcast_server_close_client(MHANDLE hndl, void *private_data)
{
	(void) hndl;
	/* free what was malloced at udp_broadcast_server_open */
	if (private_data) free(private_data);
}

static int
udp_broadcast_server_init (int argc, char **argv)
{
	MHANDLE con;
	UDPXPRM srvr;
	USERFUN funs;

	memset(&srvr, 0, sizeof(UDPXPRM));
	if (argc > 1)
		srvr.listen_port = atoi(argv[1]);
	else
		srvr.listen_port = SERVER_LISTEN_PORT;

	/* callback functions */
	memset(&funs, 0 , sizeof(USERFUN));
	funs.open = udp_broadcast_server_open;
	funs.dispatch = udp_broadcast_server_dispatch_client;
	funs.close = udp_broadcast_server_close_client;

	/* make a client connection */
	if ((con=connection_open (CONNECTION_TYPE_UDPSERVER, &srvr, &funs, NULL)) == NULL)
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

    connection_dispatch_loop(argc, argv, udp_broadcast_server_init, NULL, 0);

	return 0;
}
