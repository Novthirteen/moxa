/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    serial_connection_slave.c

    Routines to simulate a simple serial communication slave.

    2008-11-25	CF Lin
		new release
*/
#include <stdlib.h>
#include <stdio.h>
#include "connection.h"

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
serial_slave_dispatch (MHANDLE conp, void *private_data, DATAPKT *dpkt)
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
serial_slave_open(MHANDLE conp, void *param, unsigned int *timer_interval)
{
	UARTPRM *uart = (UARTPRM*) param;
	char *buf = malloc(256);

	(void) timer_interval;

	if (buf==NULL)
		connection_destroy(conp);
	else
		sprintf(buf, "Port %d", uart->port);
	return buf;
}

/* this user-defined function is called before the server disconnects a client */
static void
serial_slave_close(MHANDLE conp, void *private_data)
{
	(void) conp;
	/* free what was malloced at serial_slave_accept_client */
	if (private_data) free(private_data);
}

static void
serial_slave_event_change(MHANDLE conp, unsigned int event)
{
	printf("serial_slave_event_change %X\n", event);
}

/* This is an initialization function where the user program opens connections */
static int
serial_slave_init (int argc, char **argv)
{
	MHANDLE conp;
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
	funs.open = serial_slave_open;
	funs.dispatch = serial_slave_dispatch;
	funs.close = serial_slave_close;

	/* make a client connection */
	if ((conp=connection_open (CONNECTION_TYPE_UARTPORT, &uart, &funs, NULL)) == NULL)
		return -1;
	else
	{
		mxsp_connection_set_mask(conp, serial_slave_event_change, 0x38, 10);
		return 0;
	}
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

    connection_dispatch_loop(argc, argv, serial_slave_init, NULL, 0);

	return 0;
}
