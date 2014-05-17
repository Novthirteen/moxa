/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    tcp_server.c

    This is a simple server which handle multiple clients.
	It reverses the data sent by any client.

    2008-08-15	CF Lin
		new release
*/
#include <stdio.h>
#include <stdlib.h>
#include "network.h"

#define MAX_CLIENTS 8
#define SERVER_LISTEN_PORT 502

int clifd[MAX_CLIENTS]; /* the socket of clients */
int num_clients = 0; /* keep tracking the number of clients */

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

/*	upon detecting data from a client, this server handles the data packet 
	<fd> socket
*/
int
handle_packet(int fd)
{
	int bytes;
	char buffer[1024];
	
	/* get data from the socket */
	bytes = tcp_nonblocking_read((unsigned int) fd, buffer, 1024, NULL);
	if (bytes > 0)
	{
		buffer[bytes] = 0;
		printf("RECV[%d]: %s\n", bytes, buffer);
		/* reverse the input */
		reverse_bytes(buffer, bytes);
		bytes = tcp_nonblocking_write((unsigned int) fd, buffer, bytes, NULL);
		buffer[bytes] = 0;
		printf("SEND[%d]: %s\n", bytes, buffer);
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
	int sock; /* the server socket */
	int port = SERVER_LISTEN_PORT; /* the listen port of the server */
	int i, numfds, fd;
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

	/* clean up client fds */
	for (i = 0; i < MAX_CLIENTS; i++) 
		clifd[i] = 0;

	/* start up a TCP server */
	sock = tcp_startup_server(0, port);
	if (sock <= 0)
	{
		printf("fail to start up a TCP server at port %d\n", port);
		return -1;
	}
	/* clear the set first, important */
	FD_ZERO(&g_read_fds);
	/* set this server fd onto the selectable list */
	FD_SET (sock, &g_read_fds);
	/* currently, it is the maximum socket fd */
	max_fd = sock;
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
		/* some sockets have events, check on them */
		for (i = 0; i < num_clients; i++) 
		{
			fd = clifd[i];
			if (FD_ISSET(fd, &read_fds) && handle_packet(fd) < 0)
			{
				FD_CLR((unsigned int) fd, &g_read_fds);
				closesocket(fd);
				/* move the last one to this index */
				if (num_clients > 0)
				clifd[i] = clifd[--num_clients]; 
			}
		}
		/* some client is requesting a connection */
		if (FD_ISSET(sock, &read_fds))
		{
			unsigned int ip;

			/* accept this client */
			fd = tcp_accept_client(sock, &port, &ip, 1);
			if (fd > 0)
			{
				if (num_clients==MAX_CLIENTS)
				{
					/* no more room */
					closesocket(fd);
				}
				else
				{
					/* add it into the client list */
					clifd[num_clients++] = fd;
					/* add this client socket into the list */
					FD_SET (fd, &g_read_fds);
					/* update the maximum socket fd */
					if (fd > max_fd) 
						max_fd = fd;
				}
			}
		}
	}
	closesocket(sock);
	net_library_release();
	return 0;
}
