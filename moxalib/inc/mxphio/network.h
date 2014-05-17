/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/

#ifndef _NETWORK_H
#define _NETWORK_H

#if defined (_WIN32_WCE) || defined (WIN32)
#include <winsock2.h>
#endif
#include "os-support.h"

#if defined (_WIN32_WCE) || defined (WIN32)

#define READSOCKET(f,b,l)	recv(f,b,l,0)
#define WRITESOCKET(f,b,l)	send(f,b,l,0)
#define NET_ERRNO		h_errno

#define EINPROGRESS		WSAEINPROGRESS
#define EWOULDBLOCK		WSAEWOULDBLOCK
#define ENOBUFS			WSAENOBUFS
#define ENOTSOCK		WSAENOTSOCK

#define tcp_blocking_write  send
#define tcp_blocking_read   recv

#else  /* _WIN32_WCE */

#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/resource.h>

#define READSOCKET			read
#define WRITESOCKET			write
#define NET_ERRNO			errno

#define closesocket(f)	close((int)f)

#define tcp_blocking_write(x,y,z,w) write(x,y,z)
#define tcp_blocking_read(x,y,z,w) read(x,y,z)

#endif /* _WIN32_WCE */

#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif

#ifdef __cplusplus
extern "C" {
#endif

char* iptoa (unsigned int ip);

/*	create a client connection to a specified host according to a specified host name or
	an ip address. 
	Inputs:
		<host> the name of the host
		<port> the listening port of the host
		<ip> the ip value of the host, if host is a NULL value
		<nonblocking> nonzero value for nonblocking socket
	Outputs:
		<ip> the ip value of the host
		<port> the local port of the client
	Returns:
		-1 on failure, otherwise the socket
*/
int tcp_make_client (char *host, int *port, unsigned int *ip, int nonblocking);


/*	start up a TCP server 
	Inputs:
		<ip> the network byte ordered IPv4 address that the server binds
		<port> the listening port
	Returns:
		-1 on failure, otherwise the socket
*/
int tcp_startup_server(unsigned int ip, int port);

/*	accept a new tcp client connection 
	Inputs:
		<fd> the server listen socket
		<nonblocking> nonzero value for nonblocking socket
	Outputs:
		<port> the assigned port for this client
		<ip> the ip address of the client
	Returns:
		-1 on failure, otherwise the socket of the client
*/
int tcp_accept_client (int fd, int *port, unsigned int *ip, int nonblocking );

/*  write data to a nonblocking socket 
	Inputs:
		<fd> socket
		<buf> point to the data 
		<len> length of data
	Returns:
		< 0 on failure
		otherwise the number of bytes written
*/
int tcp_nonblocking_write(int fd, char *buf, int len, void *dummy);

/* read data from a nonblocking socket 
	Inputs:
		<fd> socket
		<buf> point to the buffer which stores data 
		<size> size of the buffer
	Returns:
		< 0 on failure
		otherwise the number of bytes read
*/
int tcp_nonblocking_read(int fd, char *buf, int size, void *dummy);

/* wait for an open socket to be writeable ot readable 
	Inputs:
		<fd> socket
		<sec> maximum # of seconds 
		<write> wait for writeable
	Returns:
		> 0 on success
		==0 timeout
		otherwise, failure
*/
int tcp_nonblocking_wait(int fd, int sec, int write);

#define	tcp_nonblocking_writeable(f,s)	tcp_nonblocking_wait(f,s,1)
#define	tcp_nonblocking_readable(f,s)	tcp_nonblocking_wait(f,s,0)

/*	make an UDP client connection to a host
	Inputs:
		<host> the host, host==NULL means broadcast
		<port> the port #
	Outputs:
		<addr_in> the info of the client
	Return:
		the socket
*/
int udp_make_client(char *host, int port, struct sockaddr_in *addr_in);

/*	start an UDP server at a port
	Inputs:
		<ip> the IP address 
		<port> the port #
	Outputs:
		<addr_in> the info of the server
	Return:
		the socket
*/
int udp_startup_server(unsigned int ip, int port, struct sockaddr_in *addr_in);

/*  via an established UDP socket, send data to a networking peer 
	Inputs:
		<fd> the socket
		<data> point to the data
		<dlen> the length of the data
		<peer> peer information is stored in the structure
	Returns:
		the number of bytes sent
*/
int udp_send(int fd, char *data, int dlen, struct sockaddr_in *peer);

/*  via an established UDP connection, receive data from a peer 
	Inputs:
		<fd> the socket
		<buf> point to the buffer to store data
		<size> the length of the data
	Outputs:
		<peer> obtain peer information via this structure
	Returns:
		the number of bytes received
*/
int udp_recv(int fd, char *buf, int size, struct sockaddr_in *peer);

/* initialize the socket library (needed in Windows) */
void net_library_init(void);

/* release the socket library (needed in Windows) */
void net_library_release(void);

/* get the IP address of a host 
	Inputs:
		<host> hostname
	Returns:
		IP address
*/
unsigned int lookup_ip (const char *host);

int lookup_localhost(char *host, int size);

#define lookup_localhost_ip() lookup_ip(NULL)

#ifdef __cplusplus
}
#endif

#endif
