/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    tcp_connection_client.c

    Routines to simulate a simple TCP client.

    2008-09-25	CF Lin
		new release
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "connection.h"

/*  this user-defined function is called after the user program receives
	a data packet. It keeps sending a packet to the server */
static int	
tcp_client_dispatch (MHANDLE conp, void *private_data, DATAPKT *dpkt)
{
	static int count = 1;
	char ch, *buf = (char*) private_data;

	dpkt->packet_consumed = dpkt->packet_size; /* consumed all */

	ch = dpkt->packet_data[dpkt->packet_size];
	dpkt->packet_data[dpkt->packet_size] = 0;
	printf("RECV [%d]: %s\n", dpkt->packet_size, dpkt->packet_data);
	dpkt->packet_data[dpkt->packet_size] = ch;

#ifdef WIN32
	Sleep(200);
#else
	sleep(1);
#endif
	sprintf(buf, "I am a client %d", count++);
	connection_send_data(conp, buf, strlen(buf));
	return 0; 
}

/* this user-defined function is called after the user program makes the connection */
static void*
tcp_client_open(MHANDLE conp, void *param, unsigned int *timer_interval)
{
	CLNTPRM *p = (CLNTPRM*) param;

	(void) timer_interval;

	printf("tcp_client_open: host (%s) local port (%d)\n", p->host, p->local_port);

	return malloc(256);
}

/* this user-defined function is called before the client is disconnected */
static void
tcp_client_release(MHANDLE conp, void *private_data)
{
	(void) conp;
	/* free what was malloced at tcp_client_open */
	if (private_data) free(private_data);
}

/* This is an initialization function where the user program opens connections */
static int
tcp_client_init (int argc, char **argv)
{
	MHANDLE con;
	CLNTPRM param;
	USERFUN funs;

	if (argc < 3)
	{
		printf("usage: <progname> <hostname> <listen port>\n");
		return -1;
	}
	/* setting the parameters of a server */
	memset(&param, 0 , sizeof(CLNTPRM));
	param.host = argv[1];
	param.listen_port = atoi(argv[2]);

	/* callback functions */
	memset(&funs, 0 , sizeof(USERFUN));
	funs.open = tcp_client_open;
	funs.dispatch = tcp_client_dispatch;
	funs.close = tcp_client_release;

	/* make a client connection */
	if ((con=connection_open (CONNECTION_TYPE_TCPCLIENT, &param, &funs, NULL)) == NULL)
		return -1;
	else
	{
		char buf[256];

		/* send 1st packet */
		strcpy(buf, "I am a client 0");
		connection_send_data(con, buf, strlen(buf));
		return 0;
	}
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

    connection_dispatch_loop(argc, argv, tcp_client_init, NULL, 0);

	return 0;
}

