/*
 * Created by victoria on 22.02.20.
*/

#include "parser.h"
#include "lexer.h"

static void
ast_callback(ast_statement_t statement, void *user)
{
	ast_print(statement);
}

int main(void)
{
	yyscan_t scanner;
	if (yylex_init(&scanner))
	{
		fprintf(stderr, "scanner init fail\n");
		return EXIT_FAILURE;
	}
	yylex_init_extra(&c, scanner);
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