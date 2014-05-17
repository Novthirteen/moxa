/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    udp_connection_client.c

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
udp_client_dispatch (MHANDLE conp, void *private_data, DATAPKT *dpkt)
{
	static int count = 1;
	char *buffer = (char*) private_data;

	dpkt->packet_consumed = dpkt->packet_size; /* consumed all */
	/* assume buffer size is enough */
	memcpy(buffer, dpkt->packet_data, dpkt->packet_size);
	buffer[dpkt->packet_size] = 0;
printf("RECV: %s\n", buffer);

#ifdef WIN32
	Sleep(200);
#else
	sleep(1);
#endif
	sprintf(buffer, "I am a client %d", count++);
	connection_send_data(conp, buffer, strlen(buffer));
printf("SEND: %s\n", buffer);
	return 0; 
}

/* this user-defined function is called after the user program makes the connection */
static void*
udp_client_open(MHANDLE conp, void *param, unsigned int *timer_interval)
{
	UDPXPRM *p = (UDPXPRM*) param;

	(void) timer_interval;

	printf("udp_client_open: host (%s) local port (%d)\n", p->host, p->local_port);

	return malloc(256);
}

/* this user-defined function is called before the client is disconnected */
static void
udp_client_close(MHANDLE conp, void *private_data)
{
	(void) conp;
	/* free what was malloced at udp_client_open */
	if (private_data) free(private_data);
}

/* This is an initialization function where the user program opens connections */
static int
udp_client_init (int argc, char **argv)
{
	MHANDLE con;
	UDPXPRM param;
	USERFUN funs;

	if (argc < 3)
	{
		printf("usage: <progname> <hostname> <listen port>\n");
		return -1;
	}
	/* setting the parameters of a server */
	memset(&param, 0 , sizeof(UDPXPRM));
	param.host = argv[1];
	param.listen_port = atoi(argv[2]);

	/* callback functions */
	memset(&funs, 0 , sizeof(USERFUN));
	funs.open = udp_client_open;
	funs.close = udp_client_close;
	funs.dispatch = udp_client_dispatch;

	/* make a client connection */
	if ((con=connection_open (CONNECTION_TYPE_UDPCLIENT, &param, &funs, NULL)) == NULL)
		return -1;
	else /* send 1st packet */
		connection_send_data(con, "haha", 4);
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

    connection_dispatch_loop(argc, argv, udp_client_init, NULL, 0);

	return 0;
}

