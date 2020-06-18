/*
 * Created by victoria on 24.02.20.
*/

#pragma once

#include "literal.h"
#include "statement.h"
#include "shared.h"

enum ast_join_type_enum {
	AST_JOIN_UNKNOWN,
	AST_JOIN_INNER,
	AST_JOIN_LEFT,
	AST_JOIN_RIGHT,
	AST_JOIN_FULL
};

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

typedef struct ast_join_s {
	enum ast_join_type_enum type;
	ast_table_name_t table;
	ast_condition_t condition;
	ast_name_node_t alias;

	struct ast_join_s *next;
} *ast_join_t;

ast_join_t
ast_join(enum ast_join_type_enum type, ast_table_name_t table, ast_condition_t condition, ast_name_node_t alias);

void
ast_join_free(ast_join_t join);

typedef struct ast_from_expression_s {
	ast_table_name_t table;
	ast_name_node_t alias;
	ast_join_t head;
	ast_join_t tail;
} ast_from_expression_t, *ast_from_expression_pt;

ast_from_expression_t
ast_from(ast_table_name_t table, ast_name_node_t alias);

void
ast_from_join(ast_from_expression_pt from, ast_join_t join);

void
ast_from_free(ast_from_expression_pt node);

typedef struct ast_select_value_s {
	union {
		ast_column_name_t column;
		ast_table_name_t table;
	} data;
	enum {
		AST_SELECT_COLUMN,
		AST_SELECT_ASTERISK
	} type;
	ast_name_node_t alias;
} ast_select_value_t, *ast_select_value_pt;

ast_select_value_t
ast_select_value_column(ast_column_name_t name);

ast_select_value_t
ast_select_value_asterisk(ast_table_name_t name);

void
ast_select_value_free(ast_select_value_pt value);

typedef struct ast_select_value_list_s {
	ast_select_value_pt array;
	size_t count;
	size_t size;
} *ast_select_value_list_t;

ast_select_value_list_t
ast_select_value_list_add(ast_select_value_list_t list, ast_select_value_t value);

void
ast_select_value_list_free(ast_select_value_list_t list);

typedef struct select_ast_node_s
{
	ast_select_value_list_t values;
	ast_from_expression_t from;
	ast_condition_t condition;
} *ast_select_node_t;

ast_select_node_t
ast_select(ast_select_value_list_t values, ast_from_expression_t from, ast_condition_t condition);

ast_statement_node_t
ast_statement_select(ast_select_node_t select);

void
ast_select_free(ast_select_node_t node);
