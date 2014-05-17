/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    tcp_connection_server.c

    Routines to simulate a simple TCP server.

    2008-09-25	CF Lin
		new release
*/
#include <stdlib.h>
#include <stdio.h>
#include "connection.h"

#define SERVER_LISTEN_PORT 502

/* reverse the content of a buffer */
void
reverse_bytes(char *buffer, int bytes)
{
	int i, m;
	char ch;

	m = (bytes+1)/2;
	bytes--;
	for (i=0; i < m; i++, bytes--)
	{
		ch = buffer[i];
		buffer[i] = buffer[bytes];
		buffer[bytes] = ch;
	}
}

/*  this user-defined function is called after the user program receives
	a data packet. It reverses the content of the packet and then send 
	the result back to the client */
static int	
tcp_server_dispatch_client (MHANDLE conp, void *private_data, DATAPKT *dpkt)
{
	char buffer[1024];

	(void) private_data;

	dpkt->packet_consumed = dpkt->packet_size;
	memcpy(buffer, dpkt->packet_data, dpkt->packet_size);
	/* reverse the received data */
	reverse_bytes(buffer, dpkt->packet_size);
	/* send data back to the client */
	connection_send_data(conp, buffer, dpkt->packet_size);
	return 0;
}

/* this user-defined function is called after the server accepts a client connection */
static void*
tcp_server_accept_client(MHANDLE conp, void *param, unsigned int *timer_interval)
{
	TCPCPRM *p = (TCPCPRM*) param;
	char *buf = malloc(256);

	(void) timer_interval;

	if (buf==NULL)
		connection_destroy(conp);
	else
		sprintf(buf, "IP %d", p->ip);
	return buf;
}

/* this user-defined function is called before the server disconnects a client */
static void
tcp_server_close_client(MHANDLE conp, void *private_data)
{
	(void) conp;
	/* free what was malloced at tcp_server_accept_client */
	if (private_data) free(private_data);
}

/* This is an initialization function where the user program opens connections */
static int
tcp_server_init (int argc, char **argv)
{
	MHANDLE conp;
	SRVRPRM srvr;
	USERFUN funs;

	memset(&srvr, 0, sizeof(SRVRPRM));
	if (argc > 1)
		srvr.listen_port = atoi(argv[1]);
	else
		srvr.listen_port = SERVER_LISTEN_PORT;

	/* callback functions */
	memset(&funs, 0 , sizeof(USERFUN));
	funs.open = tcp_server_accept_client;
	funs.dispatch = tcp_server_dispatch_client;
	funs.close = tcp_server_close_client;

	/* make a client connection */
	if ((conp=connection_open (CONNECTION_TYPE_TCPSERVER, &srvr, &funs, NULL)) == NULL)
		return -1;
	else
		return 0;
}

/* usage: program_name [<listen port>] */
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

    connection_dispatch_loop(argc, argv, tcp_server_init, NULL, 0);

	return 0;
}
