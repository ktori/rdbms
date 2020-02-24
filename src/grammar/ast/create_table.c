/*
 * Created by victoria on 24.02.20.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "literal.h"
#include "insert.h"
#include "statement.h"
#include "select.h"
#include "ast.h"
#include <malloc.h>
#include "create_table.h"
#include "shared.h"

ast_create_table_node_t
ast_create_table(ast_name_node_t name, ast_column_defs_node_t defs)
{
	ast_create_table_node_t result = malloc(sizeof(struct ast_create_table_s));

	result->name = name;
	result->columns = defs;

	return result;
}

void
ast_create_table_free(ast_create_table_node_t node)
{
	ast_name_free(node->name);
	ast_column_defs_free(node->columns);
	free(node);
}

void
ast_column_def_free(ast_column_def_node_t node)
{
	ast_name_free(node->name);
	free(node);
}

ast_column_def_node_t
ast_column_def(ast_name_node_t name, enum attribute_domain domain, int nullable)
{
	ast_column_def_node_t result = malloc(sizeof(struct ast_column_def_s));

	result->name = name;
	result->domain = domain;
	result->nullable = nullable;

	return result;
}

void
ast_column_defs_free(ast_column_defs_node_t node)
{
	unsigned i;

	for (i = 0; i < node->count; ++i)
		ast_column_def_free(node->array[i]);

	free(node->array);
	free(node);
}

ast_column_defs_node_t
ast_add_column_def(ast_column_defs_node_t defs, ast_column_def_node_t def)
{
	if (defs == NULL)
	{
		defs = malloc(sizeof(struct ast_column_defs_s));
		defs->size = 4;
		defs->count = 0;
		defs->array = malloc(defs->size * sizeof(ast_column_def_node_t));
	}
	else
	{
		if (defs->count == defs->size)
		{
			defs->size *= 2;
			defs->array = realloc(defs->array, defs->size * sizeof(ast_column_def_node_t));
		}
	}

	defs->array[defs->count++] = def;
	return defs;
}

ast_statement_node_t
ast_statement_create_table(ast_create_table_node_t create_table)
{
	ast_statement_node_t result = malloc(sizeof(struct ast_statement_s *));

	result->type = AST_CREATE_TABLE;
	result->body.create_table = create_table;

	return result;
}