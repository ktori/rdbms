#include "ast.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

struct from_ast_node_s *
ast_from(struct name_ast_node_s *name_node)
{
	struct from_ast_node_s *result = calloc(1, sizeof(struct from_ast_node_s));
	result->name = name_node;
	return result;
}

struct select_ast_node_s *
ast_select(struct name_list_ast_node_s *columns, struct from_ast_node_s *from)
{
	struct select_ast_node_s *result = calloc(1, sizeof(struct select_ast_node_s));
	result->columns = columns;
	result->from = from;
	return result;
}

void
select_print(struct select_ast_node_s *select)
{
	unsigned i;
	printf("select\n");
	for (i = 0; i < select->columns->count; ++i)
		printf("  %s\n", select->columns->array[i]->name);
	printf("from %s\n", select->from->name->name);
}

ast_statement_node_t
ast_statement_select(struct select_ast_node_s *select)
{
	ast_statement_node_t result = calloc(1, sizeof(struct ast_statement_s));
	result->type = AST_SELECT;
	result->body.select = select;
	return result;
}

ast_statements_t
ast_statements_add(ast_statements_t array, ast_statement_node_t statement)
{
	if (array == NULL)
	{
		array = calloc(1, sizeof(struct ast_statements_s));
		array->size = 1;
		array->count = 1;
		array->array = calloc(array->size, sizeof(ast_statement_node_t));
		array->array[0] = statement;
		return array;
	}

	if (array->size == array->count)
	{
		array->size *= 2;
		array->array = realloc(array->array, array->size * sizeof(ast_statement_node_t));
	}

	array->array[array->count] = statement;
	array->count += 1;

	return array;
}

void
ast_print(ast_statement_node_t statement)
{
	unsigned i;

	printf("statement: ");
	switch (statement->type)
	{
		case AST_SELECT:
			printf("SELECT ");
			for (i = 0; i < statement->body.select->columns->count; ++i)
			{
				printf("%s", statement->body.select->columns->array[i]->name);
				if (i + 1 < statement->body.select->columns->count)
					printf(", ");
			}
			printf(" FROM ");
			printf("%s\n", statement->body.select->from->name->name);
			break;
		case AST_CREATE_TABLE:
			printf("CREATE TABLE ");
			printf("%s\n", statement->body.create_table->name->name);
			break;
		case AST_INSERT:
			printf("INSERT INTO %s VALUES \n", statement->body.insert->table->name);
			break;
	}
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

void
ast_from_free(ast_from_node_t node)
{
	ast_name_free(node->name);
	free(node);
}

void
ast_select_free(ast_select_node_t node)
{
	ast_name_list_free(node->columns);
	ast_from_free(node->from);
	free(node);
}

void
ast_statement_free(ast_statement_node_t node)
{
	switch (node->type)
	{
		case AST_SELECT:
			ast_select_free(node->body.select);
			break;
		case AST_CREATE_TABLE:
			ast_create_table_free(node->body.create_table);
			break;
		case AST_INSERT:
			break;
	}
	free(node);
}

void
ast_statements_free(ast_statements_t node)
{
	unsigned i;

	for (i = 0; i < node->count; ++i)
		ast_statement_free(node->array[i]);

	free(node->array);
	free(node);
}

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
	ast_statement_node_t result = malloc(sizeof(struct ast_statement_s));

	result->type = AST_CREATE_TABLE;
	result->body.create_table = create_table;

	return result;
}

ast_literal_t
ast_literal_null()
{
	ast_literal_t result = {0};
	return result;
}

ast_literal_t
ast_literal_bool(int value)
{
	ast_literal_t result = {0};
	result.domain = AD_BOOLEAN;
	result.value.bool_val = value ? 1 : 0;
	return result;
}

ast_literal_t
ast_literal_int(int value)
{
	ast_literal_t result = {0};
	result.domain = AD_INTEGER;
	result.value.int_val = value;
	return result;
}

ast_literal_t
ast_literal_string(string_t string)
{
	ast_literal_t result = {0};
	result.domain = AD_VARCHAR;
	result.value.string_val = string->buffer;
	return result;
}

void
ast_literal_free(ast_literal_t node)
{

}

ast_insert_tuple_node_t
ast_insert_tuple_add(ast_insert_tuple_node_t tuple, ast_literal_t literal)
{
	if (tuple == NULL)
	{
		tuple = malloc(sizeof(struct ast_insert_tuple_s));
		tuple->count = 0;
		tuple->size = 1;
		tuple->values = calloc(tuple->size, sizeof(ast_literal_t));
	}
	else
	{
		if (tuple->count == tuple->size)
		{
			tuple->size *= 2;
			tuple->values = realloc(tuple->values, tuple->size * sizeof(ast_literal_t));
		}
	}

	tuple->values[tuple->count++] = literal;

	return tuple;
}

void
ast_insert_tuple_free(ast_insert_tuple_node_t node)
{
	unsigned i;

	for (i = 0; i < node->count; ++i)
		ast_literal_free(node->values[i]);

	free(node);
}

void
ast_insert_free(ast_insert_node_t node)
{
	ast_name_free(node->table);
	ast_name_list_free(node->columns);
	ast_insert_tuple_free(node->tuple);
	free(node);
}

ast_statement_node_t
ast_statement_insert(ast_name_node_t table, ast_name_list_node_t columns, ast_insert_tuple_node_t tuple)
{
	ast_statement_node_t result = malloc(sizeof(struct ast_statements_s));

	result->type = AST_INSERT;
	result->body.insert = malloc(sizeof(struct ast_insert_s));
	result->body.insert->table = table;
	result->body.insert->columns = columns;
	result->body.insert->tuple = tuple;

	return result;
}
