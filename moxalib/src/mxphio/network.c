/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    network.c

    Routines of socket functions: such as server start up, client connection...

    2008-04-15	CF Lin
		new release
*/
#include "network.h"

char*
iptoa (unsigned int ip)
{
    struct in_addr a;

    a.s_addr = ip;
    return (inet_ntoa (a));
}

int
lookup_localhost(char *host, int size)
{
	return gethostname(host,size);
}

/*  convert a host name to an ip address 
	Inputs:
		<host> the name of the host
	Returns:
		0 on failure, ip value otherwise
*/
unsigned int
lookup_ip (const char *host)
{
    struct hostent *he;
    unsigned int ip;

	if (host)
		ip = inet_addr (host);
	else
		ip = INADDR_NONE;
    if (ip == INADDR_NONE)
    {
		char localhost[64];

		if (gethostname(localhost,64)==0 && (he = gethostbyname (localhost)))
		{
			memcpy (&ip, he->h_addr_list[0], he->h_length);
		}
		else
		{
			dbgprintf("lookup_ip: can't find ip for host %s", localhost);
			ip = 0;
		}
    }
    return ip;
}

/* set a socket to be nonblocking 
	Inputs:
		<fd> socket
	Returns:
		-1 on failure, 0 on success
*/
int
tcp_set_nonblocking (int fd)
{
#if defined (_WIN32_WCE) || defined (WIN32)
	long val = 1;
	if (ioctlsocket (fd, FIONBIO, &val) != 0)
#else
	if (fcntl (fd, F_SETFL, O_NONBLOCK) != 0)
#endif /* !WIN32 */
	{
		dbgprintf("tcp_set_nonblocking: ioctlsocket");
		return -1;
	}
	else
		return 0;
}

/* turn on/off TCP/IP keepalive messages 
	Inputs:
		<fd> socket
		<on> the on/off flag
	Returns:
		-1 on failure, 0 on success
*/
static int
tcp_set_keepalive (int fd, int on)
{
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*) & on, sizeof (on)) == -1)
    {
		dbgprintf("tcp_set_keepalive: setsockopt");
		return -1;
    }
    return 0;
}

int
tcp_set_buffer_size (int fd, int size)
{
    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*) &size, sizeof (size)) == -1 ||
		setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*) &size, sizeof (size)) == -1)
    {
        dbgprintf("tcp_set_buffer_size: setsockopt");
		return -1;
    }
    return 0;
}

int
tcp_set_linger(int fd, int on)
{
    if (setsockopt(fd, SOL_SOCKET, SO_LINGER, (void*) & on, sizeof (on)) == -1)
    {
        dbgprintf("tcp_set_linger: setsockopt");
	    return -1;
    }
    return 0;
}

/* return the local port a socket is bound to */
static int
tcp_get_local_port (int fd)
{
    struct sockaddr_in sin;
	unsigned int sinsize = sizeof (sin);

    if (getsockname (fd, (struct sockaddr *) &sin, &sinsize))
    {
        dbgprintf("tcp_get_local_port: getsockname");
		return 0;
    }
    return (int) (ntohs (sin.sin_port));
}

/* create a nonblocking and keepalive socket 
	Returns:
		-1 on failure, otherwise the socket
*/
static int
tcp_open_socket (int nonblocking)
{
    int fd, on=1;

    fd = (int) socket (AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
	    return -1;
    if(nonblocking && tcp_set_nonblocking (fd))
    {
    	closesocket (fd);
	    return -1;
    }
    if(tcp_set_keepalive (fd,1) || 
       setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*) & on, sizeof (on)) != 0)
    {
        closesocket (fd);
        return -1;
    }
    return fd;
}

/*	bind a socket on a specific interface 
	Inputs:
		<fd> socket
		<ip> the ip address of the networking interface of the host machine
		<port> the listening port
	Returns:
		-1 on failure, 0 on success
*/
static int
bind_interface (int fd, unsigned int ip, int port)
{
    struct sockaddr_in sin;

    memset (&sin, 0, sizeof (sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = ip? ip:htonl(INADDR_ANY);
    sin.sin_port = htons ((unsigned short) port);
    if (bind (fd, (struct sockaddr *) &sin, sizeof (sin)) < 0)
    {
		dbgprintf("bind_interface: bind");
		return -1;
    }
    return 0;
}

/*	create a client connection to a specified host according to a specified host name or
	an ip address. 
	Inputs:
		<host> the name of the host
		<port> the listening port
		<ip> the ip value of the host, if host is a NULL value
	Outputs:
		<ip> the ip value of the host
	Returns:
		-1 on failure, otherwise the socket
*/
int
tcp_make_client (char *host, int *port, unsigned int *ip, int nonblocking)
{
    struct sockaddr_in sin;
	unsigned short listen_port;
    int     fd, st;
	char localhost[64];

	/* set everything up */
	listen_port = (unsigned short) *port;
    memset (&sin, 0, sizeof (sin));
    sin.sin_port = htons (listen_port);
    sin.sin_family = AF_INET;
    if (!host && !ip)
	{
		if (gethostname(localhost,64)==0)
			host = localhost;
		else
			return -1;
	}
    if (host) 
		*ip = lookup_ip (host);
    if (*ip == 0)
		return -2;
    sin.sin_addr.s_addr = *ip;

    /* create a socket */
    fd = tcp_open_socket (nonblocking);
    if (fd < 0)
		return -3;
	/* make the connection */
	st = connect (fd, (struct sockaddr *) &sin, sizeof (sin));
	/* get the local port bound to this socket */
 	*port = tcp_get_local_port(fd);
    if (st < 0)
    {
		/* for nonblocking, it is okay for the following error # */
		if (nonblocking && (NET_ERRNO == EINPROGRESS || NET_ERRNO == EWOULDBLOCK))
		{
			dbgprintf ("connecting to (%s:%hu) at port %hu", inet_ntoa (sin.sin_addr), listen_port, *port);
			return fd;
		}
		dbgprintf ("tcp_make_client : fail to connect to %s error %d", host, NET_ERRNO);
		closesocket (fd);
		return -4;
    } 
    else
    {
	    dbgprintf ("connected to (%s:%hu) at port %hu", inet_ntoa (sin.sin_addr), listen_port, *port);
    }

    return fd;
}

/*	create a client connection to a specified host 
	Inputs:
		<iface> the network interface that the server binds
		<port> the listening port
	Returns:
		-1 on failure, otherwise the socket
*/
int
tcp_startup_server(unsigned int iface, int port)
{
    int sockfd;

    /* create a socket */
    sockfd = tcp_open_socket (1);
    if (sockfd < 0)
	    return -1;
    /* bind the socket on to a specified interface */
    if (bind_interface (sockfd, iface, port) == -1)
	    return -2;
    if (listen (sockfd, 5) < 0)
	    return -3;
    dbgprintf ("startup a TCP server %s at port %d fd %d",iptoa (iface),port, sockfd);
    return sockfd;
}

/*	accept a new tcp client connection 
	Inputs:
		<fd> the server listen socket
	Outputs:
		<port> the assigned port for this client
		<ip> the ip address of the client
	Returns:
		-1 on failure, otherwise the socket of the client
*/
int
tcp_accept_client(int sock, int *port, unsigned int *ip, int nonblocking)
{
	unsigned int sinsize;
	struct sockaddr_in sin;
	int fd;

	sinsize = sizeof (sin);
	fd = (int) accept (sock, (struct sockaddr *) &sin, &sinsize);
	if(fd > 0)
	{
		*port = ntohs(sin.sin_port);
		*ip = ntohl (sin.sin_addr.s_addr);
		tcp_set_keepalive(fd, 1);
		if(nonblocking && tcp_set_nonblocking (fd))
		{
			closesocket (fd);
			fd = -1;
		}
	}
	return fd;
}

/*  write data to a nonblocking socket 
	Inputs:
		<fd> socket
		<buf> point to the data 
		<len> length of data
	Returns:
		< 0 on failure
		otherwise the number of bytes written
*/
int
tcp_nonblocking_write(int fd, char *buf, int len, void *dummy)
{
	int n;

	(void) dummy;

	n = WRITESOCKET (fd, buf, len);
	if (n == -1)
	{
		if (NET_ERRNO != EWOULDBLOCK && NET_ERRNO != ENOBUFS)
			return -1;
		n = 0;
	}
	return n;
}

/* read data from a nonblocking socket 
	Inputs:
		<fd> socket
		<buf> point to the buffer which stores data 
		<len> size of the buffer
	Returns:
		< 0 on failure
		otherwise the number of bytes read
*/
int
tcp_nonblocking_read(int fd, char *buf, int len, void *dummy)
{
	int n;

	(void) dummy;

	n = READSOCKET (fd, buf, len);
	if (n == -1)
    {
		return (NET_ERRNO != EWOULDBLOCK)? -1:0;
	}
	else if (n == 0) /* probable socket close */
	{
		return -1;
	}
	return n;
}

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
int
tcp_nonblocking_wait(int fd, int sec, int write)
{
    fd_set  wfds;
    struct timeval to;

	to.tv_sec = sec;
	to.tv_usec = 0;

	FD_ZERO(&wfds);
	FD_SET(fd, &wfds);
	if (write)
		return select ((int) fd + 1, NULL, &wfds, 0, &to);
	else
		return select ((int) fd + 1, &wfds, NULL, 0, &to);
}

/*	create an UDP socket
	Inputs:
		<ip> the IP address is associated with
		<port> the port #
	Outputs:
		<addr_in> the info of the socket
	Return:
		the socket
*/
static int
udp_open_socket(unsigned int ip, int port, struct sockaddr_in *addr_in)
{
	int fd;

	fd = (int) socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd > 0)
	{

		if (ip==0) ip = INADDR_ANY;
		memset(addr_in, 0, sizeof(struct sockaddr_in));
		addr_in->sin_family = AF_INET;
		addr_in->sin_port	= htons((u_short) port);
		addr_in->sin_addr.s_addr = ip;
		tcp_set_nonblocking(fd);
//		tcp_set_keepalive(fd,1);

#if 0
		{
		int on = 1;
		setsockopt( fd, SOL_SOCKET, SO_REUSEPORT, (char *) &on, sizeof(on) );
		}
#endif
	}
	return fd;
}

/*	make an UDP client connection to a host
	Inputs:
		<host> the host, host==NULL means broadcast
		<port> the port #
	Outputs:
		<serv_addr> the info of the client
	Return:
		the socket
*/
int
udp_make_client(char *host, int port, struct sockaddr_in *serv_addr)
{
	int fd;
	unsigned int ip;

	if (host)
		ip = inet_addr(host);
	else
		ip = 0xFFFFFFFF; /* broadcast */

	fd = udp_open_socket(ip, port, serv_addr);
	if (fd > 0)
	{
		if (bind_interface(fd, 0, 0))
		{
			/* fail to bind local address for this client */
			closesocket(fd);
			fd = -1;
		}
		else
		{
			if (!host)
			{
				const int val = 1;
				setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (const char*) &val, sizeof(val));
			}
			dbgprintf ("connected to %s at port %hu", inet_ntoa (serv_addr->sin_addr), port);
		}
	}
	return fd;
}

/*	start an UDP server at a port
	Inputs:
		<iface> the binding address 
		<port> the port #
		<serv_addr> the info of the server
	Return:
		the socket
*/
int
udp_startup_server(unsigned int iface, int port, struct sockaddr_in *serv_addr)
{
	int	fd;

	fd = udp_open_socket(0, port, serv_addr);
	if (fd > 0)
	{
		if (bind_interface(fd, iface, port))
		{
			/* fail to bind to local */
			closesocket(fd);
			fd = -1;
		}
		else
		{
			serv_addr->sin_addr.s_addr = iface? iface:lookup_localhost_ip(); /* host ip */
			dbgprintf ("startup a UDP server at port %d fd %d",port,fd);
		}
	}
	return fd;
}

/*  via an established UDP socket, send data to a networking peer 
	Inputs:
		<fd> the socket
		<data> point to the data
		<dlen> the length of the data
		<peer> peer information is stored in the structure
	Returns:
		the number of bytes sent
*/
int
udp_send(int fd, char *data, int dlen, struct sockaddr_in *peer)
{
	return sendto(fd, data, dlen, 0, (struct sockaddr*) peer, sizeof(struct sockaddr));
}

/*  via an established UDP connection, receive data from a peer 
	Inputs:
		<fd> the socket
		<data> point to the buffer to store data
		<dlen> the length of the data
	Outputs:
		<peer> obtain peer information via this structure
	Returns:
		the number of bytes received
*/
int
udp_recv(int fd, char *data, int dlen, struct sockaddr_in *peer)
{
	int len, slen = sizeof(struct sockaddr);

	len = recvfrom(fd, data, dlen, 0, (struct sockaddr*) peer, &slen);
	return (len < 0)? 0:len;
}

void
net_library_init(void)
{
#if defined(WIN32) || defined(_WIN32_WCE)
	/* init possible windows socket dll */
	WSADATA wsa;
	WSAStartup (MAKEWORD (2, 2), &wsa);
#endif
}

void
net_library_release(void)
{
	/* release possible windows socket dll */
#if defined(WIN32) || defined(_WIN32_WCE)
    WSACleanup ();
#endif
}

