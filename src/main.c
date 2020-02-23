#include "server/server.h"

int
main()
{
	struct server_s server = {0};
	server_start(&server);
	server_listen(&server);
	server_stop(&server);
	return 0;
}
