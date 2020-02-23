#include <table/attribute.h>
#include <table/domain.h>
#include <table/relation.h>
#include "server/server.h"

int
main()
{
	struct server_s server = {0};

	rel_init();
	attribute_init();
	domain_init();

	server_start(&server);
	server_listen(&server);
	server_stop(&server);
	return 0;
}
