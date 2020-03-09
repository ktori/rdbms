/*
 * Created by victoria on 24.02.20.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "create_table.h"
#include "literal.h"
#include "insert.h"
#include "statement.h"
#include "select.h"
#include "ast.h"
#include "shared.h"

struct name_ast_node_s *
ast_name(const char *name)
{
	struct name_ast_node_s *result = calloc(1, sizeof(struct name_ast_node_s));
	result->name = malloc(strlen(name) + 1);
	strcpy(result->name, name);
	return result;
}

struct name_list_ast_node_s *
ast_name_list_add(struct name_list_ast_node_s *list, struct name_ast_node_s *name)
{
	if (list == NULL)
	{
		list = calloc(1, sizeof(struct name_list_ast_node_s));
		list->array = calloc(1, sizeof(struct name_ast_node_s *));
		list->count = 1;
		list->size = 1;
		list->array[0] = name;
		return list;
	}

	if (list->count == list->size)
	{
		list->size *= 2;
		list->array = realloc(list->array, list->size * sizeof(struct name_ast_node_s *));
	}

	list->array[list->count] = name;
	list->count += 1;

	return list;
}

string_t
string_create()
{
	string_t result = calloc(1, sizeof(struct string_s));
	result->size = 16;
	result->count = 0;
	result->buffer = calloc(result->size, sizeof(char));
	return result;
}

string_t
string_append(string_t string, char c)
{
	if (string->count == string->size)
	{
		string->size *= 2;
		string->buffer = realloc(string->buffer, string->size);
	}

	string->buffer[string->count] = c;
	string->count += 1;

	return string;
}

ast_name_node_t
ast_name_from_string(string_t string)
{
	ast_name_node_t result = calloc(1, sizeof(struct name_ast_node_s));
	result->name = string->buffer;
	return result;
}

void
ast_name_free(ast_name_node_t node)
{
	free(node->name);
	free(node);
}

void
ast_name_list_free(ast_name_list_node_t node)
{
	unsigned i;

	for (i = 0; i < node->count; ++i)
		ast_name_free(node->array[i]);

	free(node->array);
	free(node);
}

ast_table_name_t
ast_table_name(char *table)
{
	ast_table_name_t result = {0};
	result.name = table;
	return result;
}

void
ast_table_name_free(ast_table_name_pt table_name)
{
	free(table_name->name);
	table_name->name = NULL;
}

ast_column_name_t
ast_column_name(ast_table_name_t table, char *column)
{
	ast_column_name_t result = {0};
	result.table = table;
	result.name = column;
	return result;
}

void
ast_column_name_free(ast_column_name_pt column_name)
{
	ast_table_name_free(&column_name->table);
	free(column_name->name);
	column_name->name = NULL;
}
