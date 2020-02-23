#include "ast.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct name_ast_node_s *ast_name(const char *name)
{
  struct name_ast_node_s *result = calloc(1, sizeof(struct name_ast_node_s));
  result->name = malloc(strlen(name) + 1);
  strcpy(result->name, name);
  return result;
}

struct name_list_ast_node_s *ast_name_list_add(struct name_list_ast_node_s *list, struct name_ast_node_s *name)
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

struct from_ast_node_s *ast_from(struct name_ast_node_s *name_node)
{
  struct from_ast_node_s *result = calloc(1, sizeof(struct from_ast_node_s));
  result->name = name_node;
  return result;
}

struct select_ast_node_s *ast_select(struct name_list_ast_node_s *columns, struct from_ast_node_s *from)
{
  struct select_ast_node_s *result = calloc(1, sizeof(struct select_ast_node_s));
  result->columns = columns;
  result->from = from;
  return result;
}

void select_print(struct select_ast_node_s *select)
{
  unsigned i;
  printf("select\n");
  for (i = 0; i < select->columns->count; ++i)
    printf("  %s\n", select->columns->array[i]->name);
  printf("from %s\n", select->from->name->name);
}

ast_statement_t
ast_statement_select(struct select_ast_node_s *select)
{
	ast_statement_t result = calloc(1, sizeof(struct ast_statement_s));
	result->type = AST_SELECT;
	result->body.select = select;
	return result;
}

ast_statements_t
ast_statements_add(ast_statements_t array, ast_statement_t statement)
{
	if (array == NULL)
	{
		array = calloc(1, sizeof(struct ast_statements_s));
		array->size = 1;
		array->count = 1;
		array->array = calloc(array->size, sizeof(ast_statement_t));
		array->array[0] = statement;
		return array;
	}

	if (array->size == array->count)
	{
		array->size *= 2;
		array->array = realloc(array->array, array->size * sizeof(ast_statement_t));
	}

	array->array[array->count] = statement;
	array->count += 1;

	return array;
}

void
ast_print(ast_statement_t statement)
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
