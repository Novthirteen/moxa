/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    mbrtu_master.c

    Routines to simulate a simple MODBUS RTU master.

    2009-05-25 Peter Wu	new release
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "connection.h"
#include "modbus.h"

static int
send_request(MHANDLE con)
{
	static unsigned short count = 1;
	unsigned char data[8];
	unsigned char mbrtu_buf[256];
	unsigned short addr, d;
	int len;

	/* compose request PDU */
	data[0] = 10;	/* slave ID */
	data[1] = 0x03;	/* function code */
	addr = 100;
	d = BSWAP16(addr);
	memcpy(&data[2], &d, 2);
	d = BSWAP16(count);
	memcpy(&data[4], &d, 2);
	if (count == 10) count = 1; else count++;
	/* format request ADU */
	len = mbrtu_packet_format(data, 6, mbrtu_buf);
	/* send MODBUS RTU request */
	if (len > 0) connection_send_data(con, (char *)mbrtu_buf, len);

	return 0;
}

/*  This user-defined function is called after the user program receives
	a data packet. It keeps sending a packet to the slave */
static int	
mbrtu_master_dispatch (MHANDLE conp, void *private_data, DATAPKT *dpkt)
{
	int consumed;
	unsigned char *packet;
	int len;
	unsigned short count, d;
	int i;

	/* obtain a MODBUS RTU response packet */
	len = mbrtu_packet_digest(dpkt->packet_data, dpkt->packet_size);
	if (len <= 0)
	{
		/* MODBUS ADU packet not complete */
		consumed = 0;
	}
	else
	{
		consumed = dpkt->packet_size;
		packet = dpkt->packet_data;
		/* display response */
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
		send_request(conp);
	}
	dpkt->packet_consumed = consumed;

	return consumed; 
}

/* this user-defined function is called after the user program makes the connection */
static void*
mbrtu_master_open(MHANDLE conp, void *param, unsigned int *timer_interval)
{
	UARTPRM *uart = (UARTPRM*) param;

	printf("mbrtu_master_open: port (%d)\n", uart->port);

	return NULL;
}

/* this user-defined function is called before the client is disconnected */
static void
mbrtu_master_close(MHANDLE conp, void *private_data)
{
	(void) conp;
	/* free what was malloced at mbrtu_master_open */
	if (private_data) free(private_data);
}

/* This is an initialization function where the user program opens connections */
static int
mbrtu_master_init (int argc, char **argv)
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
	funs.open = mbrtu_master_open;
	funs.dispatch = mbrtu_master_dispatch;
	funs.close = mbrtu_master_close;

	/* make a client connection */
	if ((con=connection_open (CONNECTION_TYPE_UARTPORT, &uart, &funs, NULL)) == NULL)
		return -1;
	else
	{
		/* send 1st packet */
		send_request(con);
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

    connection_dispatch_loop(argc, argv, mbrtu_master_init, NULL, 0);

	return 0;
}
