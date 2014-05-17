/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    serial_connection_master.c

    Routines to simulate a simple serial master.

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
serial_master_dispatch (MHANDLE conp, void *private_data, DATAPKT *dpkt)
{
	static int count = 1;
	char ch, *buf = (char*) private_data;

	dpkt->packet_consumed = dpkt->packet_size; /* consumed all */

	ch = dpkt->packet_data[dpkt->packet_size];
	dpkt->packet_data[dpkt->packet_size] = 0;
	printf("RECV [%d]: %s\n", dpkt->packet_size, dpkt->packet_data);
	dpkt->packet_data[dpkt->packet_size] = ch;

#ifdef WIN32
	Sleep(1000);
#else
	sleep(1);
#endif
	sprintf(buf, "I am a master %d", count++);
	connection_send_data(conp, buf, strlen(buf));
	return 0; 
}

/* this user-defined function is called after the user program makes the connection */
static void*
serial_master_open(MHANDLE conp, void *param, unsigned int *timer_interval)
{
	UARTPRM *uart = (UARTPRM*) param;

	(void) timer_interval;

	printf("serial_master_open: port (%d)\n", uart->port);

	return malloc(256);
}

/* this user-defined function is called before the client is disconnected */
static void
serial_master_close(MHANDLE conp, void *private_data)
{
	(void) conp;
	/* free what was malloced at serial_master_open */
	if (private_data) free(private_data);
}

/* This is an initialization function where the user program opens connections */
static int
serial_master_init (int argc, char **argv)
{
	MHANDLE con;
	UARTPRM uart;
	USERFUN funs;

	memset(&uart, 0, sizeof(UARTPRM));
	if (argc < 2)
	{
		printf("usage: <progname> <port>\n");
		return -1;
	}
	uart.port = atoi(argv[1]);
	uart.baudrate = 9600;
	uart.parity = MSP_PARITY_NONE;	/* 0~4: none, odd, even, mark, space */
	uart.data_bits = 8;	/* 5,6,7,8 */
	uart.stop_bits = 1;	/* 1,2,3 (1.5 bits) */
	uart.iface_mode = MSP_RS232_MODE;	/* 0~3: RS232, RS485_2WIRE, RS422, RS485_4WIRE */
	uart.flow_control = MSP_FLOWCTRL_HW;	/* 0~2: none, software, hardware  */

	/* callback functions */
	memset(&funs, 0 , sizeof(USERFUN));
	funs.open = serial_master_open;
	funs.dispatch = serial_master_dispatch;
	funs.close = serial_master_close;

	/* make a client connection */
	if ((con=connection_open (CONNECTION_TYPE_UARTPORT, &uart, &funs, NULL)) == NULL)
		return -1;
	else
	{
		char buf[256];

		mxsp_connection_purge(con);
		/* send 1st packet */
		strcpy(buf, "I am a master 0");
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

    connection_dispatch_loop(argc, argv, serial_master_init, NULL, 0);

	return 0;
}
