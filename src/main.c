#include "server/server.h"
#include <malloc.h>
#include <signal.h>
#include <storage/storage.h>
#include <table/attribute.h>
#include <table/domain.h>
#include <table/relation.h>

struct server_s *server;

void
signal_callback(int signal)
{
	fprintf(stderr, "server shutdown\n");
	server_stop(server);
}

int
main()
{
	server = calloc(1, sizeof(struct server_s));
	server->running = 0;
	server->fd = -1;

	signal(SIGTERM, signal_callback);
	signal(SIGINT, signal_callback);

	store_init();
	rel_init();
	attribute_init();
	domain_init();

	server_start(server);
	server_listen(server);
	rel_shutdown();
	store_shutdown();
	free(server);

	return 0;
}
