/*
 * Created by victoria on 23.02.20.
*/

#include "server.h"
#include "client.h"

#include <stdlib.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>

int
server_start(server_t server)
{
	struct addrinfo hints = { 0 }, *info = NULL, *info_it = NULL;
	struct timeval tv;
	int on = 1;

	server->running = 0;
	server->fd = -1;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, "8765", &hints, &info) != EXIT_SUCCESS)
	{
		perror("getaddrinfo()");
		return EXIT_FAILURE;
	}

	if (info == NULL)
	{
		printf("no addrinfo\n");
		return EXIT_FAILURE;
	}

	tv.tv_sec = 30;
	tv.tv_usec = 0;

	for (info_it = info; info_it != NULL; info_it = info_it->ai_next)
	{
		printf("%d %d %d\n", info_it->ai_family, info_it->ai_socktype, info_it->ai_protocol);
		server->fd = socket(info_it->ai_family, info_it->ai_socktype, info_it->ai_protocol);
		if (server->fd < 0)
		{
			perror("socket()");
			continue;
		}
		setsockopt(server->fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
		setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

		if (bind(server->fd, info_it->ai_addr, info_it->ai_addrlen) != EXIT_SUCCESS)
		{
			perror("bind()");
			close(server->fd);
			continue;
		}
		break;
	}

	freeaddrinfo(info);

	if (info_it == NULL)
	{
		printf("no info\n");
		return EXIT_FAILURE;
	}

	listen(server->fd, 16);
	server->running = 1;

	printf("listening (8765)\n");

	return EXIT_SUCCESS;
}

int
server_stop(server_t server)
{
	shutdown(server->fd, SHUT_RDWR);

	return EXIT_SUCCESS;
}

int
server_listen(server_t server)
{
	struct sockaddr client_addr;
	unsigned addr_size;
	int client;

	while (server->running)
	{
		client = accept(server->fd, &client_addr, &addr_size);
		if (client < 0)
		{
			if (errno != EAGAIN)
				perror("accept()");
			if (errno == EBADF)
			{
				server->running = 0;
				return EXIT_FAILURE;
			}
			continue;
		}
		printf("accepted client (%d)\n", client);

		if (client_accept(client) != EXIT_SUCCESS)
		{
			fprintf(stderr, "fail\n");
			continue;
		}

		printf("closing client (%d)\n", client);
		close(client);
	}

	return EXIT_SUCCESS;
}
