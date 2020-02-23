/*
 * Created by victoria on 23.02.20.
*/

#include <parser.h>
#include <lexer.h>
#include "socket_input.h"

void
socket_input(char *buf, int *result, int max_size, int fd)
{
	*result = read(fd, buf, max_size);
	if (*result < 0)
	{
		*buf = 0;
		*result = 0;
	}
}
