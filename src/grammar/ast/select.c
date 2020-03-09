/*
 * Created by victoria on 24.02.20.
*/

#include <mhash.h>
#include "select.h"
#include "statement.h"
#include "shared.h"
#include "ast.h"

ast_from_expression_t
ast_from(ast_table_name_t table, ast_name_node_t alias)
{
	ast_from_expression_t result = {0};
	result.table = table;
	result.alias = alias;
	return result;
}

void
ast_from_free(ast_from_expression_pt node)
{
	ast_table_name_free(&node->table);

	if (node->head)
		ast_join_free(node->head);
}

struct select_ast_node_s *
ast_select(ast_select_value_list_t values, ast_from_expression_t from, ast_condition_t condition)
{
	struct select_ast_node_s *result = calloc(1, sizeof(struct select_ast_node_s));
	result->values = values;
	result->from = from;
	result->condition = condition;
	return result;
}

ast_statement_node_t
ast_statement_select(struct select_ast_node_s *select)
{
	ast_statement_node_t result = calloc(1, sizeof(struct ast_statement_s *));
	result->type = AST_SELECT;
	result->body.select = select;
	return result;
}

void
ast_select_free(ast_select_node_t node)
{
	ast_select_value_list_free(node->values);
	ast_from_free(&node->from);
	free(node);
}

ast_condition_t
ast_condition(ast_name_node_t name, enum ast_operator operator, ast_literal_t value)
{
	ast_condition_t result = malloc(sizeof(struct ast_condition_s));

	result->column = name;
	result->operator = operator;
	result->value = value;

	return result;
}

void
ast_condition_free(ast_condition_t node)
{
	ast_name_free(node->column);
	ast_literal_free(node->value);
	free(node);
}

ast_select_value_t
ast_select_value_column(ast_column_name_t name)
{
	ast_select_value_t result = {0};
	result.type = AST_SELECT_COLUMN;
	result.data.column = name;
	return result;
}

ast_select_value_t
ast_select_value_asterisk()
{
	ast_select_value_t result = {0};
	result.type = AST_SELECT_ASTERISK;
	return result;
}

void
ast_select_value_free(ast_select_value_pt value)
{
	if (value->type == AST_SELECT_COLUMN)
		ast_column_name_free(&value->data.column);

	if (value->alias)
		ast_name_free(value->alias);

	memset(value, 0, sizeof(*value));
}

ast_select_value_list_t
ast_select_value_list_add(ast_select_value_list_t list, ast_select_value_t value)
{
	if (list == NULL)
	{
		list = calloc(1, sizeof(struct ast_select_value_list_s));
		list->size = 1;
		list->count = 0;
		list->array = calloc(list->size, sizeof(struct ast_select_value_s));
	}

	if (list->count == list->size)
	{
		list->size *= 2;
		list->array = realloc(list->array, list->size * sizeof(struct ast_select_value_s));
	}

	memcpy(list->array + list->count, &value, sizeof(value));
	list->count += 1;

	return list;
}

void
ast_select_value_list_free(ast_select_value_list_t list)
{
	size_t i;

	for (i = 0; i < list->count; ++i)
		ast_select_value_free(list->array + i);

	free(list->array);
	free(list);
}

void
ast_from_join(ast_from_expression_pt from, ast_join_t join)
{
	if (from->tail == NULL)
	{
		if (from->head != NULL)
		{
			fprintf(stderr, "illegal state: from->head != NULL while from->tail is NULL\n");
			return;
		}

		from->tail = join;
		from->head = join;
	}

	from->tail->next = join;
	from->tail = join;
}

ast_join_t
ast_join(enum ast_join_type_enum type, ast_table_name_t table, ast_condition_t condition, ast_name_node_t alias)
{
	ast_join_t result = calloc(1, sizeof(struct ast_join_s));

	result->type = type;
	result->table = table;
	result->condition = condition;
	result->alias = alias;

	return result;
}

void
ast_join_free(ast_join_t join)
{
	ast_table_name_free(&join->table);

	if (join->condition)
		ast_condition_free(join->condition);

	if (join->alias)
		ast_name_free(join->alias);

	if (join->next)
		ast_join_free(join->next);

	free(join);
}
