/*
 * Created by victoria on 23.02.20.
*/

#pragma once

typedef struct server_s
{
	int fd;
	int running;
} *server_t;

int
server_start(server_t server);

int
server_listen(server_t server);

int
server_stop(server_t server);
