/*
 * Created by victoria on 22.02.20.
 */

#include "parser.h"

#include "lexer.h"

#include "ast/statement.h"

static void
ast_callback(ast_statement_node_t statement, void *user)
{
}

int
main(void)
{
	struct yy_extra_s extra = { 0 };
	yyscan_t scanner;
	if (yylex_init(&scanner))
	{
		fprintf(stderr, "scanner init fail\n");
		return EXIT_FAILURE;
	}
	yyset_extra(&extra, scanner);
	YY_BUFFER_STATE buf = yy_create_buffer(stdin, YY_BUF_SIZE, scanner);
	if (yyparse(scanner, ast_callback, NULL) != EXIT_SUCCESS)
	{
		fprintf(stderr, "parse error\n");
		return EXIT_FAILURE;
	}
	yy_delete_buffer(buf, scanner);
	yylex_destroy(scanner);
	return EXIT_SUCCESS;
}