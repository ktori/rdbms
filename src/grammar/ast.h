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

struct name_list_ast_node_s
{
  struct name_ast_node_s **array;
  unsigned count;
  unsigned size;
};

struct name_list_ast_node_s *ast_name_list_add(struct name_list_ast_node_s *list, struct name_ast_node_s *name);

struct from_ast_node_s
{
  struct name_ast_node_s *name;
};

struct from_ast_node_s *ast_from(struct name_ast_node_s *name_node);

typedef struct select_ast_node_s
{
  struct name_list_ast_node_s *columns;
  struct from_ast_node_s *from;
} *select_ast_node_t;

select_ast_node_t ast_select(struct name_list_ast_node_s *columns, struct from_ast_node_s *from);

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

ast_statement_t ast_statement_select(struct select_ast_node_s *select);

typedef struct ast_statements_s
{
	ast_statement_t *array;
	unsigned count;
	unsigned size;
} *ast_statements_t;

ast_statements_t ast_statements_add(ast_statements_t array, ast_statement_t statement);

void ast_print(ast_statement_t statement);

typedef void(*ast_callback_t)(ast_statement_t statement, void *user);