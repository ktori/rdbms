/*
 * Created by victoria on 24.02.20.
*/

#include "statement.h"
#include "create_table.h"
#include "select.h"

#include <malloc.h>

void
ast_statement_free(ast_statement_node_t node)
{
	switch (node->type)
	{
		case AST_SELECT:
			ast_select_free(node->body.select);
			break;
		case AST_CREATE_TABLE:
			ast_create_table_free(node->body.create_table);
			break;
		case AST_INSERT:
			break;
	}
	free(node);
}

void
ast_statements_free(ast_statements_t node)
{
	unsigned i;

	for (i = 0; i < node->count; ++i)
		ast_statement_free(node->array[i]);

	free(node->array);
	free(node);
}
