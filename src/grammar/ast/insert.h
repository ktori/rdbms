/*
 * Created by victoria on 24.02.20.
*/

#pragma once

#include "literal.h"

typedef struct ast_insert_tuple_s
{
	ast_literal_t *values;
	unsigned count;
	unsigned size;
} *ast_insert_tuple_node_t;

typedef struct ast_insert_s
{
	ast_name_node_t table;
	ast_name_list_node_t columns;
	ast_insert_tuple_node_t tuple;
} *ast_insert_node_t;

ast_insert_tuple_node_t
ast_insert_tuple_add(ast_insert_tuple_node_t tuple, ast_literal_t literal);

void
ast_insert_tuple_free(ast_insert_tuple_node_t node);

void
ast_insert_free(ast_insert_node_t node);

struct ast_statement_s *
ast_statement_insert(ast_name_node_t table, ast_name_list_node_t columns, ast_insert_tuple_node_t tuple);
