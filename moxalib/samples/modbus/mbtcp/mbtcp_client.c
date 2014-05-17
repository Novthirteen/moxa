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
#include <time.h>
#include <string.h>

#include "connection.h"
#include "modbus.h"

#include "Mxthread.h"

FILE *fd = NULL;

#define SWAP32X(c)   ((c)>>8&0xff00ff) | ((c)<<8&0xff00ff00)

static void mbtcp_client_get_time (char* buffer, unsigned int len);

static int
send_request(MHANDLE con, MBTCPHDR *mb)
{
	static unsigned short tid = 1;
	static unsigned short count = 4;
	unsigned char data[8];
	unsigned char mbtcp_buf[260];
	unsigned short addr, d;
	int len;

	mb->trans_identifier = tid;
	if (tid == 65535) tid = 1; else tid++;

	/* compose request PDU */
	data[0] = 1;	/* slave ID : 10*/
	data[1] = 0x03;	/* 	function code */
	addr = 0; 
	
	d = BSWAP16(addr);
	memcpy(&data[2], &d, 2);
	
	d = BSWAP16(count);
	
	memcpy(&data[4], &d, 2);
	/*if (count == 10) count = 1; else count++;*/
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
	unsigned short count; // d;
//	int i;


	unsigned int tmp;
	float pv1, pv2;
	char buffer[30];
	char status[30];

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
		printf("receive length:%d\n", consumed);
		/* display response PDU */
		packet++; /* skip slave ID */
		if (*packet > 0x80)
		{
			packet++;
			printf("exception=%x\n", *packet);
		}
		else
		{
			mbtcp_client_get_time(buffer, sizeof(buffer));
			packet++;
			count = *packet;
			count >>= 1;
			packet++;

			memcpy(&tmp, packet, 4);
			//printf("tmp is : %d\n", tmp);
			tmp = SWAP32X(tmp);
			//printf("tmp is : %x\n", tmp);
			memcpy(&pv1, &tmp, 4);
			//printf("pv1 is : %x\n", (int)pv1);

			
			memcpy(&tmp, packet+4, 4);
			tmp = SWAP32X(tmp);
			memcpy(&pv2, &tmp, 4);


			if(pv1 <= 50){
				if(pv2 <= 50){
					strcpy(status, "OK");
				}
				else{
					strcpy(status, "SAFTY VALVE MISS ACTIVATED");
				}
			}
			else if(pv1 > 50){
				if(pv2 > 50){
					strcpy(status, "SAFTY VALVE ACTIVATED");
				}
				else{
					strcpy(status, "SAFTY VALVE NOT WORKING");
				}
			}

			if(!fd){
				fd = fopen("/mnt/sd/log.txt", "a+");
			}
			
			fprintf(fd, "%-24s%-12.2f%-12.2f%-s\n", buffer, pv1, pv2, status);
			fflush(fd);
#if 0
			for (i = 0; i < count*2; i++)
			{
				//memcpy(&d, packet, 2);
				
				//d = BSWAP16(d);
				tmp = *(char *)(packet+i);
				printf("%d ", tmp);
			}
			printf("\n");
#endif
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
	if (mb == NULL) {
		printf("connection_destroy");
		connection_destroy(conp);

		}

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

	sleep(5);
	
	/* Add a file to output the device PV */
	fd = fopen("/mnt/sd/log.txt", "a+");


	if(fd){
		//fprintf(fd, "time        pressure    travel      status\n");
		
	}
	else{
		printf("File open error.");
	}
	
	
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
	if ((con=connection_open (CONNECTION_TYPE_TCPCLIENT, &param, &funs, NULL)) == NULL){
		printf("connect error\n");
		return -1;
	}
	else
	{
		MBTCPHDR mb;

		memset(&mb, 0 , sizeof(MBTCPHDR));
		/* send 1st packet */
		//printf("Send request. \n");
		
	//	mxthread_create(mbtcp_client_timer_request, NULL);
		send_request(con, &mb);
		return 0;
	}
}


static void mbtcp_client_get_time (char* buffer, unsigned int len)
{
   time_t currentTime;
   struct tm *ts;

   currentTime = time(NULL);
   ts = localtime(&currentTime);
   
   strftime(buffer, len, "%Y%m%d %X", ts);
}

static void * mbtcp_client_timer_request(void * arg )
{
	while(1)
	{
		sleep(1);
		//send_request(con, &mb);	
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
