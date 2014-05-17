/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    udp_client.c

    This is a simple client program.  

    2008-08-15	CF Lin
		new release
*/
#include <stdio.h>
#include <stdlib.h>
#include "network.h"

/*	upon receiving a data packet, this client handles the packet 
*/
int
handle_packet(int fd)
{
	static int count = 1;
	int bytes;
	char buffer[1024];
	struct sockaddr_in peer;

	/* get data from the socket */
	bytes = udp_recv((unsigned int) fd, buffer, 256, &peer);
	if (bytes > 0)
	{
#ifdef WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
		buffer[bytes] = 0;
		printf("RECV: %s\n", buffer);
		sprintf(buffer, "I am a client %d", count++);
		printf("SEND: %s\n", buffer);
		/* send it to the server */
		bytes = strlen(buffer);
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
	int sock; /* the client socket */
	int port; /* the listen port of the server */
	int numfds;
	char *host;
	struct sockaddr_in serv_addr;
#if defined(_WIN32_WCE)
	int	argc;
	char cmdline[256], *argv[32];

	WideCharToMultiByte(CP_ACP, 0, (LPCTSTR)lpCmdLine, 255, cmdline, 256, NULL, NULL);
	argc = split_line(argv+1, 32, cmdline)+1;
#endif

	if (argc < 3)
	{
		printf("usage: <program name> <host> <port>\n");
		return -1;
	}
	host = argv[1];
	port = atoi(argv[2]);

	/* initialize possible Windows socket library */
	net_library_init();

	/* make a client to a server */
	sock = udp_make_client(host, port, &serv_addr);
	if (sock <= 0)
	{
		printf("fail to make a client connection to the server %s\n", host);
		return -1;
	}
	/* clear the set first, important */
	FD_ZERO(&g_read_fds);
	/* set this server fd onto the selectable list */
	FD_SET (sock, &g_read_fds);
	/* currently, it is the maximum socket fd */
	max_fd = sock;
	/* send a hello message to the server */
	udp_send(sock, "I am a client 0", 15, &serv_addr);
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
		/* the cleint socket have events, check on it */
		if (FD_ISSET(sock, &read_fds))
		{
			handle_packet(sock);
		}
	}
	closesocket(sock);
	net_library_release();
	return 0;
}
