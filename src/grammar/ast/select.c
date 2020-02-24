/*
 * Created by victoria on 24.02.20.
*/

#include <mhash.h>
#include "select.h"
#include "statement.h"
#include "shared.h"

struct from_ast_node_s *
ast_from(ast_name_node_t name_node)
{
	struct from_ast_node_s *result = calloc(1, sizeof(struct from_ast_node_s));
	result->name = name_node;
	return result;
}

struct select_ast_node_s *
ast_select(ast_name_list_node_t columns, struct from_ast_node_s *from, ast_condition_t condition)
{
	struct select_ast_node_s *result = calloc(1, sizeof(struct select_ast_node_s));
	result->columns = columns;
	result->from = from;
	result->condition = condition;
	return result;
}

ast_statement_node_t
ast_statement_select(struct select_ast_node_s *select)
{
	ast_statement_node_t result = calloc(1, sizeof(struct ast_statement_s *));
	result->type = AST_SELECT;
	result->body.select = select;
	return result;
}

void
ast_from_free(ast_from_node_t node)
{
	ast_name_free(node->name);
	free(node);
}

void
ast_select_free(ast_select_node_t node)
{
	ast_name_list_free(node->columns);
	ast_from_free(node->from);
	free(node);
}

ast_condition_t
ast_condition(ast_name_node_t name, enum ast_operator operator, ast_literal_t value)
{
	ast_condition_t result = malloc(sizeof(struct ast_condition_s));

	result->column = name;
	result->operator = operator;
	result->value = value;

	return result;
}

void
ast_condition_free(ast_condition_t node)
{
	ast_name_free(node->column);
	ast_literal_free(node->value);
	free(node);
}
