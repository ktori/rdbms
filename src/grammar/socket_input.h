/*
 * Created by victoria on 23.02.20.
*/

#pragma once

void
socket_input(char *buf, int *result, int max_size, int fd);

#define YY_INPUT(buf, result, max_size) socket_input(buf, &result, max_size, yyextra->socket_fd)
