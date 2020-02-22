#include "ast.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct name_ast_node_s *ast_name(const char *name)
{
  struct name_ast_node_s *result = calloc(1, sizeof(struct name_ast_node_s));
  result->name = strdup(name);
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