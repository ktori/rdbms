/*
 * Created by victoria on 24.02.20.
*/

#pragma once

typedef struct ast_statement_s
{
	union
	{
		struct select_ast_node_s *select;
		struct ast_create_table_s *create_table;
		struct ast_insert_s *insert;
	} body;
	enum
	{
		AST_SELECT,
		AST_CREATE_TABLE,
		AST_INSERT
	} type;
} *ast_statement_node_t;

typedef struct ast_statements_s
{
	ast_statement_node_t *array;
	unsigned count;
	unsigned size;
} *ast_statements_t;

typedef void(*ast_callback_t)(ast_statement_node_t statement, void *user);

void
ast_statement_free(ast_statement_node_t node);

void
ast_statements_free(ast_statements_t node);
