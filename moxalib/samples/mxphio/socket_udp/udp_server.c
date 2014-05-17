/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    udp_server.c

    This is a simple UDP server.

    2008-08-15	CF Lin
		new release
*/
#include <stdio.h>
#include <stdlib.h>
#include "network.h"

#define SERVER_LISTEN_PORT 502

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

/*	upon receiving a data packet, this server handles the packet 
*/
int
handle_packet(int fd)
{
	int bytes;
	char buffer[1024];
	struct sockaddr_in peer;

	/* get data from the socket */
	bytes = udp_recv((unsigned int) fd, buffer, 256, &peer);
	if (bytes > 0)
	{
		/* reverse the input */
		buffer[bytes] = 0;
printf("RECV: %s\n", buffer);
		reverse_bytes(buffer, bytes);
		buffer[bytes] = 0;
printf("SEND: %s\n", buffer);
		udp_send((unsigned int) fd, buffer, bytes, &peer);
	}
	return bytes;
}

int
#if defined(_WIN32_WCE)
WINAPI 
WinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
#else
main (int argc, char **argv)
#endif
{
    fd_set g_read_fds; /* the set of sockets that are checked by the select function */
	fd_set read_fds; /* a variable of g_read_fds */
	int max_fd = 0; /* record the maxinum socket fd in the selectable set */
	struct timeval to; /* time out on the select function */
	int srvfd; /* the server socket */
	int port = SERVER_LISTEN_PORT; /* the listen port of the server */
	int numfds;
	struct sockaddr_in serv_addr;
#if defined(_WIN32_WCE)
	int	argc;
	char cmdline[256], *argv[32];

	WideCharToMultiByte(CP_ACP, 0, (LPCTSTR)lpCmdLine, 255, cmdline, 256, NULL, NULL);
	argc = split_line(argv+1, 32, cmdline)+1;
#endif

	if (argc > 1)
	{
		port = atoi(argv[1]);
	}
	/* initialize possible Windows socket library */
	net_library_init();

	/* start up a server */
	srvfd = udp_startup_server(0, port, &serv_addr);
	if (srvfd <= 0)
	{
		printf("fail to start up a server at port %d\n", port);
		return -1;
	}
	/* clear the set first, important */
	FD_ZERO(&g_read_fds);
	/* set this server fd onto the selectable list */
	FD_SET (srvfd, &g_read_fds);
	/* currently, it is the maximum socket fd */
	max_fd = srvfd;
	while(1)
	{
		/* set timeout values */
		to.tv_sec = 3;
		to.tv_usec = 0;
		/* copy the socket selectable list */
		read_fds = g_read_fds; 
		/* wait for read events out of open sockets or timeout */
		numfds = select (max_fd + 1, &read_fds, 0, 0, &to);
		if (numfds < 0)
		{
			printf("select error\n");
			break;
		}
		else if (numfds==0) /* timeout */
			continue;
		/* some client is requesting a connection */
		if (FD_ISSET(srvfd, &read_fds))
		{
			handle_packet(srvfd);
		}
	}
	closesocket(srvfd);
	net_library_release();
	return 0;
}
