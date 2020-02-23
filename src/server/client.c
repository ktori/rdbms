/*
 * Created by victoria on 23.02.20.
*/

#include "client.h"
#include "parser.h"
#include "lexer.h"
#include "../grammar/ast.h"
#include "../query/runner.h"

static void
parse_callback(ast_statement_node_t statement, FILE *sockf)
{
	ast_print(statement);
	execute(statement, sockf);
}

int
client_accept(int fd)
{
	struct yy_extra_s c = {0};
	yyscan_t scanner;
	FILE *f = fdopen(fd, "r+b");
	setvbuf(f, NULL, _IONBF, 0);
	if (f == NULL)
	{
		perror("fdopen()");
		return EXIT_FAILURE;
	}
	if (yylex_init(&scanner) != EXIT_SUCCESS)
	{
		fprintf(stderr, "yylex_init()\n");
		return EXIT_FAILURE;
	}
	c.socket_fd = fd;
	yyset_extra(&c, scanner);
	if (yyparse(scanner, (ast_callback_t) parse_callback, f) != EXIT_SUCCESS)
	{
		fprintf(stderr, "parse error\n");
	}
	yylex_destroy(scanner);
	fclose(f);

	return 0;
}
