/*
 * Created by victoria on 24.02.20.
*/

#pragma once

#include <stddef.h>
#include <table/domain_enum.h>

typedef struct string_s
{
	char *buffer;
	unsigned count;
	unsigned size;
} *string_t;
typedef struct name_ast_node_s
{
	char *name;
} *ast_name_node_t;
typedef struct name_list_ast_node_s
{
	ast_name_node_t *array;
	unsigned count;
	unsigned size;
} *ast_name_list_node_t;
enum ast_operator
{
	AST_OP_EQ = 1, 	/* = */
	AST_OP_GT,	   	/* > */
	AST_OP_GE,		/* >= */
	AST_OP_LT,		/* < */
	AST_OP_LE,		/* <= */
	AST_OP_NEQ		/* <> */
};

ast_name_node_t
ast_name(const char *name);

ast_name_list_node_t
ast_name_list_add(ast_name_list_node_t list, ast_name_node_t name);

string_t
string_create();

string_t
string_append(string_t string, char c);

ast_name_node_t
ast_name_from_string(string_t string);

void
ast_name_free(ast_name_node_t node);

void
ast_name_list_free(ast_name_list_node_t node);