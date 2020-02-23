#pragma once

typedef struct string_s
{
	char *buffer;
	unsigned count;
	unsigned size;
} *string_t;

string_t string_create();
string_t string_append(string_t string, char c);

typedef struct name_ast_node_s
{
  char *name;
} *ast_name_node_t;

ast_name_node_t ast_name(const char *name);
ast_name_node_t ast_name_from_string(string_t string);
void ast_name_free(ast_name_node_t node);

typedef struct name_list_ast_node_s
{
  ast_name_node_t *array;
  unsigned count;
  unsigned size;
} *ast_name_list_node_t;

ast_name_list_node_t ast_name_list_add(ast_name_list_node_t list, ast_name_node_t name);
void ast_name_list_free(ast_name_list_node_t node);

typedef struct from_ast_node_s
{
  struct name_ast_node_s *name;
} *ast_from_node_t;

ast_from_node_t ast_from(ast_name_node_t name_node);
void ast_from_free(ast_from_node_t node);

typedef struct select_ast_node_s
{
  struct name_list_ast_node_s *columns;
  struct from_ast_node_s *from;
} *select_ast_node_t;

select_ast_node_t ast_select(ast_name_list_node_t columns, ast_from_node_t from);
void ast_select_free(select_ast_node_t node);

typedef struct ast_statement_s
{
	union
	{
		select_ast_node_t select;
	} body;
	enum {
		AST_SELECT
	} type;
} *ast_statement_t;

ast_statement_t ast_statement_select(select_ast_node_t select);
void ast_statement_free(ast_statement_t node);

typedef struct ast_statements_s
{
	ast_statement_t *array;
	unsigned count;
	unsigned size;
} *ast_statements_t;

ast_statements_t ast_statements_add(ast_statements_t array, ast_statement_t statement);
void ast_statements_free(ast_statements_t node);

void ast_print(ast_statement_t statement);

typedef void(*ast_callback_t)(ast_statement_t statement, void *user);
