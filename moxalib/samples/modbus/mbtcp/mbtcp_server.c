/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    mbtcp_server.c

    Routines to simulate a simple MODBUS TCP server.

    2009-05-22 Peter Wu	new release
*/
#include <stdlib.h>
#include <stdio.h>
#include "connection.h"
#include "modbus.h"

#define SERVER_LISTEN_PORT 502

static int
mbtcp_parser(unsigned char *packet, unsigned short len, unsigned char *output, unsigned short size)
{
	unsigned short regs, dlen;
	unsigned char *buf;
	unsigned short d, n;

	output[0] = packet[0];

	if (packet[1] != 0x03)
	{
		/* function code not supported */
		output[1] = packet[1] | 0x80;
		output[2] = 0x01;
		return 3;
	}

	/* number of addresses, each is a unsigned short */
	regs = (packet[4]<<8)+packet[5];
	if (regs > 125 || regs == 0)
	{
		output[1] = packet[1] | 0x80;
		output[2] = 0x03;
		return 3;
	}
#ifdef DEBUG
	printf("regs=%d\n", regs);
#endif
	output[1] = packet[1];
	buf = output+2;

	dlen = regs*sizeof(unsigned short);
	buf[0] = dlen; /* byte count */
	/* simulate register values */
	for (d = 1, buf++; d <= regs; d++, buf+=2)
	{
		n = BSWAP16(d);
		memcpy(buf, &n, 2);
	}

	return dlen+3;
}

/*  This user-defined function is called after the user program receives
	a data packet. It processes the content of the packet and then send 
	the simulated result back to the client */
static int	
mbtcp_server_dispatch_client (MHANDLE conp, void *private_data, DATAPKT *dpkt)
{
    MBTCPHDR *mb = (MBTCPHDR *)private_data;
    unsigned char *packet; /* PDU, including Unit Identifier */
    unsigned int plen = dpkt->packet_size; /* ADU length after PDU parsed */
    int consumed;
    unsigned char buf[256];
    unsigned char mbtcp_buf[260];
    int len;

	/* obtain a MODBUS TCP request packet */
	packet = mbtcp_packet_digest(dpkt->packet_data, &plen, mb);
	if (packet == NULL)
	{
		/* MODBUS ADU packet not complete */
		printf("mbtcp digest error plen=%d\n", plen);
		consumed = 0;
	}
	else
	{
		consumed = plen;
		/* process MODBUS TCP request packet and get response PDU */
		len = mbtcp_parser(packet, mb->length, buf, sizeof(buf));
		/* format response ADU */
		len = mbtcp_packet_format(mb, buf, len, mbtcp_buf);
		/* send MODBUS TCP response */
		connection_send_data(conp, mbtcp_buf, len);
	}
	dpkt->packet_consumed = consumed;

	return consumed;
}

/* this user-defined function is called after the server accepts a client connection */
static void*
mbtcp_server_accept_client(MHANDLE conp, void *param, unsigned int *timer_interval)
{
	MBTCPHDR *mb = calloc(1, sizeof(MBTCPHDR));

	if (mb==NULL)
		connection_destroy(conp);

	return mb;
}

/* this user-defined function is called before the server disconnects a client */
static void
mbtcp_server_close_client(MHANDLE conp, void *private_data)
{
	(void) conp;
	/* free what was malloced at tcp_server_accept_client */
	if (private_data) free(private_data);
}

/* This is an initialization function where the user program opens connections */
static int
mbtcp_server_init (int argc, char **argv)
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
	funs.open = mbtcp_server_accept_client;
	funs.dispatch = mbtcp_server_dispatch_client;
	funs.close = mbtcp_server_close_client;

	/* make a client connection */
	if ((conp=connection_open (CONNECTION_TYPE_TCPSERVER, &srvr, &funs, NULL)) == NULL)
		return -1;

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

    connection_dispatch_loop(argc, argv, mbtcp_server_init, NULL, 0);

	return 0;
}
