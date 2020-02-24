/*
 * Created by victoria on 23.02.20.
*/

#include "runner.h"
#include "ast/statement.h"
#include "ast/insert.h"
#include "ast/shared.h"

#include <stdlib.h>
#include <ast/ast.h>
#include <stdio.h>

int
execute(ast_statement_node_t statement, FILE *sockf)
{
	switch (statement->type)
	{
		case AST_SELECT:
			return execute_select(statement->body.select, sockf);
		case AST_CREATE_TABLE:
			return execute_create_table(statement->body.create_table, sockf);
		case AST_INSERT:
			return execute_insert(statement->body.insert, sockf);
	}

	fprintf(stderr, "unknown statement type: %d\n", statement->type);

	return EXIT_FAILURE;
}
