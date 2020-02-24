/*
 * Created by victoria on 24.02.20.
*/

#include <malloc.h>
#include "insert.h"
#include "literal.h"
#include "shared.h"
#include "statement.h"

ast_insert_tuple_node_t
ast_insert_tuple_add(ast_insert_tuple_node_t tuple, ast_literal_t literal)
{
	if (tuple == NULL)
	{
		tuple = malloc(sizeof(struct ast_insert_tuple_s));
		tuple->count = 0;
		tuple->size = 1;
		tuple->values = calloc(tuple->size, sizeof(ast_literal_t));
	}
	else
	{
		if (tuple->count == tuple->size)
		{
			tuple->size *= 2;
			tuple->values = realloc(tuple->values, tuple->size * sizeof(ast_literal_t));
		}
	}

	tuple->values[tuple->count++] = literal;

	return tuple;
}

void
ast_insert_tuple_free(ast_insert_tuple_node_t node)
{
	unsigned i;

	for (i = 0; i < node->count; ++i)
		ast_literal_free(node->values[i]);

	free(node);
}

void
ast_insert_free(ast_insert_node_t node)
{
	ast_name_free(node->table);
	ast_name_list_free(node->columns);
	ast_insert_tuple_free(node->tuple);
	free(node);
}

struct ast_statement_s *
ast_statement_insert(ast_name_node_t table, ast_name_list_node_t columns, ast_insert_tuple_node_t tuple)
{
	struct ast_statement_s *result = malloc(sizeof(struct ast_statements_s *));

	result->type = AST_INSERT;
	result->body.insert = malloc(sizeof(struct ast_insert_s));
	result->body.insert->table = table;
	result->body.insert->columns = columns;
	result->body.insert->tuple = tuple;

	return result;
}