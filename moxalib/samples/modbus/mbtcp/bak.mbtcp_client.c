/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    mbtcp_client.c

    Routines to simulate a simple MODBUS TCP client.

    2009-05-22 Peter Wu	new release
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "connection.h"
#include "modbus.h"

static int
send_request(MHANDLE con, MBTCPHDR *mb)
{
	static unsigned short tid = 1;
	static unsigned short count = 1;
	unsigned char data[8];
	unsigned char mbtcp_buf[260];
	unsigned short addr, d;
	int len;

	mb->trans_identifier = tid;
	if (tid == 65535) tid = 1; else tid++;

	/* compose request PDU */
	data[0] = 10;	/* slave ID */
	data[1] = 0x03;	/* function code */
	addr = 100;
	d = BSWAP16(addr);
	memcpy(&data[2], &d, 2);
	d = BSWAP16(count);
	memcpy(&data[4], &d, 2);
	if (count == 10) count = 1; else count++;
	len = mbtcp_packet_format(mb, data, 6, mbtcp_buf);
	connection_send_data(con, (char *)mbtcp_buf, len);

	return 0;
}

/*  This user-defined function is called after the user program receives
	a data packet. It keeps sending a packet to the server */
static int	
mbtcp_client_dispatch (MHANDLE conp, void *private_data, DATAPKT *dpkt)
{
	MBTCPHDR *mb = (MBTCPHDR *) private_data;
	unsigned char *packet; /* PDU, including Unit Identifier */
	unsigned int plen = dpkt->packet_size; /* ADU length after PDU parsed */
	int consumed;
	unsigned short count, d;
	int i;

	/* obtain a MODBUS TCP response packet */
	packet = mbtcp_packet_digest((unsigned char *)dpkt->packet_data, &plen, mb);
	if (packet == NULL)
	{
		/* MODBUS ADU packet not complete */
		printf("mbtcp digest error plen=%d\n", plen);
		consumed = 0;
	}
	else
	{
		consumed = plen;
		/* display response PDU */
		packet++; /* skip slave ID */
		if (*packet > 0x80)
		{
			packet++;
			printf("exception=%x\n", *packet);
		}
		else
		{
			packet++;
			count = *packet;
			count >>= 1;
			packet++;
			for (i = 0; i < count; i++, packet +=2)
			{
				memcpy(&d, packet, 2);
				d = BSWAP16(d);
				printf("%d ", d);
			}
			printf("\n");
		}
#ifdef WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
		send_request(conp, mb);
	}
	dpkt->packet_consumed = consumed;

	return consumed; 
}

/* this user-defined function is called after the user program makes the connection */
static void*
mbtcp_client_open(MHANDLE conp, void *param, unsigned int *timer_interval)
{
	CLNTPRM *p = (CLNTPRM*) param;
	MBTCPHDR *mb;

	printf("tcp_client_open: host (%s) local port (%d)\n", p->host, p->local_port);
	mb = calloc(1, sizeof(MBTCPHDR));
	if (mb == NULL) connection_destroy(conp);

	return mb;
}

/* this user-defined function is called before the client is disconnected */
static void
mbtcp_client_release(MHANDLE conp, void *private_data)
{
	(void) conp;
	/* free what was malloced at tcp_client_open */
	if (private_data) free(private_data);
}

/* This is an initialization function where the user program opens connections */
static int
mbtcp_client_init (int argc, char **argv)
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
	funs.open = mbtcp_client_open;
	funs.dispatch = mbtcp_client_dispatch;
	funs.close = mbtcp_client_release;

	/* make a client connection */
	if ((con=connection_open (CONNECTION_TYPE_TCPCLIENT, &param, &funs, NULL)) == NULL)
		return -1;
	else
	{
		MBTCPHDR mb;

		memset(&mb, 0 , sizeof(MBTCPHDR));
		/* send 1st packet */
		send_request(con, &mb);
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

    connection_dispatch_loop(argc, argv, mbtcp_client_init, NULL, 0);

	return 0;
}
