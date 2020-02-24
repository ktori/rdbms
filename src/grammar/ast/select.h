/*
 * Created by victoria on 24.02.20.
*/

#pragma once

#include "ast.h"
#include "statement.h"
#include "shared.h"

typedef struct ast_condition_s
{
	ast_name_node_t column;
	enum ast_operator operator;
	ast_literal_t value;
} *ast_condition_t;

ast_condition_t
ast_condition(ast_name_node_t name, enum ast_operator operator, ast_literal_t value);

void
ast_condition_free(ast_condition_t node);

typedef struct from_ast_node_s
{
	ast_name_node_t name;
} *ast_from_node_t;

typedef struct select_ast_node_s
{
	ast_name_list_node_t columns;
	struct from_ast_node_s *from;
	ast_condition_t condition;
} *ast_select_node_t;

ast_from_node_t
ast_from(ast_name_node_t name_node);

ast_select_node_t
ast_select(ast_name_list_node_t columns, ast_from_node_t from, ast_condition_t condition);

ast_statement_node_t
ast_statement_select(ast_select_node_t select);

void
ast_from_free(ast_from_node_t node);

void
ast_select_free(ast_select_node_t node);
