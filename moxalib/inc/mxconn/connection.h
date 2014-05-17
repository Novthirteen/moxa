/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#ifndef _CONNECTION_H
#define _CONNECTION_H

#include "network.h"
#include "mserial_port.h"

#define CONNECTION_VERSION_NUMBER	"V1.0"

typedef void* MHANDLE;

typedef struct _DATAPKT
{
    char *packet_data;	/* data to be consumed and forwarded */
    int  packet_size;	/* bytes in packet_data */
    int  packet_consumed;/* bytes consumed in packet_data after the execution of a driver function*/
} DATAPKT;

/*  user-defined function, called at the initialization stage
*/
typedef int (*app_init_t) (int argc, char **argv);

/*  user-defined function, called at the release stage
*/
typedef void (*app_release_t) (void);

/*  user-defined function, called after a connection receives a data packet
    Inputs:
        <hndl> the connection
	<private_data> point to the area of user-defined data
        <dpkt> the data packet
    Returns:
        the bytes that is consumed in each call
*/
typedef int (*conn_dispatch_t) (MHANDLE hndl, void *private_data, DATAPKT *dpkt); 

/*  user-defined timer function that is associated with a connection  
    Inputs:
        <hndl> the connection
*/
typedef void (*conn_timer_t) (void *private_data); 

/*  user-defined function, called when a connection is established 
    Inputs:
        <hndl> the connection
    Outputs:
	<timer_interval> 0, initially, timer interval in milliseconds 
    Returns:
	point to an area of user-defined data (could be NULL)
    Notes: 
	if <timer_interval> has a value returned, a timer is initiated. its
	timer function will be called periodically every <timer_interval>
	milliseconds.
*/
typedef void* (*conn_open_t) (MHANDLE hndl, void *param, unsigned int *timer_interval);

/*  user-defined function, called when a client connection is disconnected
    Inputs:
        <hndl> the connection
        <private_data> the data created when calling accept function 
*/
typedef void (*conn_close_t) (MHANDLE hndl, void *private_data);

/*  user-defined function, called when a serial port connection detects
	events or errors
    Inputs:
        <hndl> the connection
        <event>  
*/
typedef void (*conn_event_t) (MHANDLE hndl, unsigned int event);

enum
{
    CONNECTION_TYPE_NONE,
	CONNECTION_TYPE_TCPACCEPT, 	/* a client connection accepted by a TCP server */
    CONNECTION_TYPE_TCPSERVER,	/* a connection of a TCP server */
    CONNECTION_TYPE_TCPCLIENT,	/* a TCP client making the connection */
    CONNECTION_TYPE_UDPCLIENT,	/* a UDP client making the connection */
    CONNECTION_TYPE_UDPSERVER,	/* a connection of a UDP server */
    CONNECTION_TYPE_UARTPORT,	/* a connection of a serial port */
};

typedef struct _USERFUN
{
    /* being called after the connection is established */
    conn_open_t open; 
    /* being called after the connection receives data */
    conn_dispatch_t	dispatch;
    /* being called after the connection is established and indicated a timer interval */
    conn_timer_t timer;
    /* being called after the connection disconnects */
    conn_close_t close;
} USERFUN;

/* data struct for setting communication parameters of a serial port */
typedef struct _UARTPRM
{
	unsigned int    port;		/* port number starting from 1 */
	unsigned int    baudrate;	/* e.g, ..., 1200, 2400, ..., 9600, ... */
	unsigned int	parity:3;	/* 0~4: none, odd, even, mark, space */
	unsigned int    data_bits:4;	/* 5,6,7,8 */
	unsigned int    stop_bits:2;	/* 1,2,3 (1.5 bits) */
	unsigned int	iface_mode:3;	/* 0~3: RS232, RS485_2WIRE, RS422, RS485_4WIRE */
	unsigned int	flow_control:2;	/* 0~2: none, software, hardware  */
	unsigned int	xxx:18;		/* un-used or reserved */
} UARTPRM;

/* parameters for a TCP server */
typedef struct _SRVRPRM
{
    /* the server listening port */
    unsigned int listen_port:16; 
    /* the maximum number of clients allowed, 0 for unlimited */
    unsigned int max_clients:16;
    /* the IP range of clients allowed, 0 for unlimited */
	unsigned char ip_range[16];
} SRVRPRM;

/* parameters for a client accepted by a TCP server */
typedef struct _TCPCPRM
{
    /* the server listening port */
    unsigned int listen_port:16; 
    /* the local port the connection is on */
    unsigned int local_port:16;
    /* the IP of the host */
    unsigned int ip;
} TCPCPRM;

/* parameters for a client making a TCP connection */
typedef struct _CLNTPRM
{
    /* the hostname of the server */
    char *host;
    /* the IP of the server */
    unsigned int server_ip;
    /* the listening port of the server */
    unsigned int listen_port:16;
    /* the local port of the connection */
    unsigned int local_port:16;
    /* the # of connection trials before it is established */
    unsigned int connection_retrials:8;
    /* the timeout value (in seconds) waiting for a connection being established */
    unsigned int connection_timeout:8;
    /* reconnect to the server if the connection is disconnected */
    unsigned int reconnect_interval:8;
    unsigned int xxx:8;
} CLNTPRM;

/* parameters for a UDP connection */
typedef struct _UDPXPRM
{
    /* the hostname of the server */
    char *host;
    /* the IP of the server or client */
    unsigned int ip;
    /* the server listening port */
    unsigned int listen_port:16;
    unsigned int local_port:16;
} UDPXPRM;

#ifdef __cplusplus
extern "C" {
#endif

/*  inform the main routine to close a connection
    Inputs:
        <con> the connection
*/
void connection_destroy(MHANDLE con);

/*  send data onto a connection
    Inputs:
        <con> the connection
        <buf> the buffer with data
        <len> the length of data to be sent
	Returns:
		the number of bytes that are sent
*/
int connection_send_data (MHANDLE con, char *buf, int len);

/*  open a connection
    Inputs:
	<type> type of connection, CONNECTION_TYPE_XXXXXXX
	<param> point to its parameters
	<funs> point to its user-defined functions
    Returns:
	a handler to its connection on success, otherwise NULL for failure  
*/
MHANDLE connection_open (unsigned int type, void *param, USERFUN *funs, void **);

/*  your application calls this main routine to enter an infinitive loop
    where read/write operations of all types of connections are	dispatched
    Inputs:
		<argc> main function argc
		<argv> main function argv
		<init> a user-defined initialization function
		<release> a user-defined initialization release
	Returns:
		None
*/
int connection_dispatch_loop(int argc, char **argv, app_init_t app_init, app_release_t app_release, int thread);

/*  call this function to quit from the loop of main routine */
void connection_dispatch_quit(void);

/*	call this function to purge the receiving and sending buffers 
	of a serial connection
*/
void mxsp_connection_purge(MHANDLE *conp);

/*	call this function to add a timer that responds to the change of
	the events (line status and error status) of a serial connection
	Inputs:
		<hndl> Specify the serial connection.
		<cb> Specify the user-defined function that is called when events change.
		<mask> Specify the events to be enabled. 
		<interval> Specify the timer interval.
*/
int mxsp_connection_set_mask(MHANDLE *hndl, conn_event_t cb, unsigned int mask, unsigned int interval);

#ifdef __cplusplus
}
#endif

#endif
