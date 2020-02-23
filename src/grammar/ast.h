#pragma once

#include <table/domain_enum.h>

typedef struct string_s
{
	char *buffer;
	unsigned count;
	unsigned size;
} *string_t;

string_t
string_create();

string_t
string_append(string_t string, char c);

typedef struct ast_literal_s
{
	union
	{
		char bool_val;
		short smallint_val;
		int int_val;
		char *string_val;
		float real_val;
	} value;
	enum attribute_domain domain;
} ast_literal_t;

ast_literal_t
ast_literal_null();

ast_literal_t
ast_literal_bool(int value);

ast_literal_t
ast_literal_int(int value);

ast_literal_t
ast_literal_string(string_t string);

ast_literal_t
ast_literal_real(float value);

void
ast_literal_free(ast_literal_t node);

typedef struct name_ast_node_s
{
	char *name;
} *ast_name_node_t;

ast_name_node_t
ast_name(const char *name);

ast_name_node_t
ast_name_from_string(string_t string);

void
ast_name_free(ast_name_node_t node);

typedef struct name_list_ast_node_s
{
	ast_name_node_t *array;
	unsigned count;
	unsigned size;
} *ast_name_list_node_t;

ast_name_list_node_t
ast_name_list_add(ast_name_list_node_t list, ast_name_node_t name);

void
ast_name_list_free(ast_name_list_node_t node);

typedef struct from_ast_node_s
{
	struct name_ast_node_s *name;
} *ast_from_node_t;

ast_from_node_t
ast_from(ast_name_node_t name_node);

void
ast_from_free(ast_from_node_t node);

typedef struct select_ast_node_s
{
	struct name_list_ast_node_s *columns;
	struct from_ast_node_s *from;
} *ast_select_node_t;

ast_select_node_t
ast_select(ast_name_list_node_t columns, ast_from_node_t from);

void
ast_select_free(ast_select_node_t node);


typedef struct ast_column_def_s
{
	ast_name_node_t name;
	enum attribute_domain domain;
	int nullable;
} *ast_column_def_node_t;

ast_column_def_node_t
ast_column_def(ast_name_node_t name, enum attribute_domain domain, int nullable);

void
ast_column_def_free(ast_column_def_node_t node);

typedef struct ast_column_defs_s
{
	ast_column_def_node_t *array;
	unsigned count;
	unsigned size;
} *ast_column_defs_node_t;

ast_column_defs_node_t
ast_add_column_def(ast_column_defs_node_t defs, ast_column_def_node_t def);

void
ast_column_defs_free(ast_column_defs_node_t node);

typedef struct ast_create_table_s
{
	ast_name_node_t name;
	ast_column_defs_node_t columns;
} *ast_create_table_node_t;

ast_create_table_node_t
ast_create_table(ast_name_node_t name, ast_column_defs_node_t defs);

void
ast_create_table_free(ast_create_table_node_t node);

typedef struct ast_insert_tuple_s
{
	ast_literal_t *values;
	unsigned count;
	unsigned size;
} *ast_insert_tuple_node_t;

ast_insert_tuple_node_t
ast_insert_tuple_add(ast_insert_tuple_node_t tuple, ast_literal_t literal);

void
ast_insert_tuple_free(ast_insert_tuple_node_t node);

typedef struct ast_insert_s
{
	ast_name_node_t table;
	ast_name_list_node_t columns;
	ast_insert_tuple_node_t tuple;
} *ast_insert_node_t;

void
ast_insert_free(ast_insert_node_t node);

typedef struct ast_statement_s
{
	union
	{
		ast_select_node_t select;
		ast_create_table_node_t create_table;
		ast_insert_node_t insert;
	} body;
	enum
	{
		AST_SELECT,
		AST_CREATE_TABLE,
		AST_INSERT
	} type;
} *ast_statement_node_t;

ast_statement_node_t
ast_statement_select(ast_select_node_t select);

ast_statement_node_t
ast_statement_create_table(ast_create_table_node_t create_table);

ast_statement_node_t
ast_statement_insert(ast_name_node_t table, ast_name_list_node_t columns, ast_insert_tuple_node_t tuple);

void
ast_statement_free(ast_statement_node_t node);

typedef struct ast_statements_s
{
	ast_statement_node_t *array;
	unsigned count;
	unsigned size;
} *ast_statements_t;

ast_statements_t
ast_statements_add(ast_statements_t array, ast_statement_node_t statement);

void
ast_statements_free(ast_statements_t node);

void
ast_print(ast_statement_node_t statement);

typedef void(*ast_callback_t)(ast_statement_node_t statement, void *user);

