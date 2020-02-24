/*
 * Created by victoria on 24.02.20.
*/

#pragma once

#include <table/domain_enum.h>
#include "ast.h"
#include "statement.h"
#include "shared.h"

typedef struct ast_column_def_s
{
	ast_name_node_t name;
	enum attribute_domain domain;
	int nullable;
} *ast_column_def_node_t;
typedef struct ast_column_defs_s
{
	ast_column_def_node_t *array;
	unsigned count;
	unsigned size;
} *ast_column_defs_node_t;
typedef struct ast_create_table_s
{
	ast_name_node_t name;
	ast_column_defs_node_t columns;
} *ast_create_table_node_t;

ast_create_table_node_t
ast_create_table(ast_name_node_t name, ast_column_defs_node_t defs);

void
ast_create_table_free(ast_create_table_node_t node);

void
ast_column_def_free(ast_column_def_node_t node);

ast_column_def_node_t
ast_column_def(ast_name_node_t name, enum attribute_domain domain, int nullable);

void
ast_column_defs_free(ast_column_defs_node_t node);

ast_column_defs_node_t
ast_add_column_def(ast_column_defs_node_t defs, ast_column_def_node_t def);

ast_statement_node_t
ast_statement_create_table(ast_create_table_node_t create_table);