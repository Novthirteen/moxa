/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    mbasc_slave.c

    Routines to simulate a simple MODBUS ASCII slave.

    2009-05-25 Peter Wu	new release
*/
#include <stdlib.h>
#include <stdio.h>
#include "connection.h"
#include "modbus.h"

static int
mbasc_parser(unsigned char *packet, unsigned short len, unsigned char *output, unsigned short size)
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
	the simulated result back to the master */
static int	
mbasc_slave_dispatch (MHANDLE conp, void *private_data, DATAPKT *dpkt)
{
	unsigned char *buf = (unsigned char *) private_data;
	unsigned int plen = dpkt->packet_size;
	int consumed;
	unsigned char mbasc_buf[256];
	int len;
	unsigned short slave_id;
	unsigned short Modbus_id = 10;

	len = mbasc_packet_digest(dpkt->packet_data, &plen, mbasc_buf);
	if (len <= 0)
	{
		/* MODBUS ADU packet not complete */
		slave_id = mbasc_buf[0];
		if (slave_id == Modbus_id) /* wait */
			consumed = 0;
		else /* discard */
			consumed = plen;
	}
	else
	{
		consumed = plen;
		slave_id = mbasc_buf[0];
		if (slave_id == Modbus_id)
		{
			/* process MODBUS ASCII request packet and get response PDU */
			len = mbasc_parser(mbasc_buf, len, buf, sizeof(buf));
			/* format response ADU */
			len = mbasc_packet_format(buf, len, mbasc_buf);
			/* send MODBUS ASCII response */
			if (len > 0)
				connection_send_data(conp, mbasc_buf, len);
		}
	}
	dpkt->packet_consumed = consumed;

	return consumed; 
}

/* this user-defined function is called after the server accepts a client connection */
static void*
mbasc_slave_open(MHANDLE conp, void *param, unsigned int *timer_interval)
{
	UARTPRM *uart = (UARTPRM*) param;
	char *buf = malloc(256);

	(void) timer_interval;

	if (buf==NULL)
		connection_destroy(conp);
	else
		printf("Port %d\n", uart->port);

	return buf;
}

/* this user-defined function is called before the server disconnects a client */
static void
mbasc_slave_close(MHANDLE conp, void *private_data)
{
	(void) conp;
	/* free what was malloced at mbasc_slave_accept_client */
	if (private_data) free(private_data);
}

/* This is an initialization function where the user program opens connections */
static int
mbasc_slave_init (int argc, char **argv)
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
	funs.open = mbasc_slave_open;
	funs.dispatch = mbasc_slave_dispatch;
	funs.close = mbasc_slave_close;

	/* make a client connection */
	if ((conp=connection_open (CONNECTION_TYPE_UARTPORT, &uart, &funs, NULL)) == NULL)
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

	connection_dispatch_loop(argc, argv, mbasc_slave_init, NULL, 0);

	return 0;
}
