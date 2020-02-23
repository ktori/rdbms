/*
 * Created by victoria on 23.02.20.
*/

#include "client.h"
#include "parser.h"
#include "lexer.h"
#include "../grammar/ast.h"

static void
parse_callback(ast_statement_t statement, int *fd)
{
	ast_print(statement);
}

int
client_accept(int fd)
{
	unsigned c = 0;
	yyscan_t scanner;
	YY_BUFFER_STATE buf;
	FILE *f = fdopen(fd, "r");
	if (f == NULL)
	{
		perror("fdopen()");
		return EXIT_FAILURE;
	}
	if (yylex_init(&scanner) != EXIT_SUCCESS)
	{
		fprintf(stderr, "yylex_init()\n");
		fclose(f);
		return EXIT_FAILURE;
	}
	yylex_init_extra(&c, scanner);
	buf = yy_create_buffer(f, YY_BUF_SIZE, scanner);
	buf->yy_is_interactive = 1;
	yy_switch_to_buffer(buf, scanner);
	if (yyparse(scanner, (ast_callback_t) parse_callback, &fd) != EXIT_SUCCESS)
	{
		fprintf(stderr, "parse error\n");
	}
	yy_delete_buffer(buf, scanner);
	yylex_destroy(scanner);
	fclose(f);

	return 0;
}
