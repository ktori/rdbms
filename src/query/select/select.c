/*
 * Created by victoria on 24.02.20.
*/

#include <stdlib.h>
#include <ast/ast.h>
#include <stdio.h>
#include <string.h>
#include <table/relation.h>
#include <storage/storage.h>
#include <table/record.h>
#include "ast/select.h"
#include "ast/shared.h"

typedef struct select_attr_array_s
{
	unsigned *attr_ids;
	size_t count;
	size_t size;
} select_attr_array_t, *select_attr_array_pt;

static void
select_attr_push(select_attr_array_pt array, unsigned attr)
{
	if (array->count == array->size)
	{
		array->size *= 2;
		array->attr_ids = realloc(array->attr_ids, array->size * sizeof(unsigned));
	}

	array->attr_ids[array->count] = attr;
	array->count += 1;
}

struct attr_find_callback_data_s
{
	select_attr_array_t attrs;
	ast_select_value_list_t list;
	short from_rel;
};

typedef struct resolve_attribute_callback_data_s
{
	const char *name;
	short rel;
	unsigned found;
} *resolve_attribute_callback_data_t;

static int
resolve_attribute_callback(unsigned id, record_t record, resolve_attribute_callback_data_t data)
{
	if (*(short *) record->values[4].data == data->rel)
	{
		if (strncmp(record->values[0].data, data->name, record->values[0].data_size) == 0)
		{
			data->found = id;
			return 1;
		}
	}

	return 0;
}

static unsigned
resolve_attribute(const char *name, short rel)
{
	struct resolve_attribute_callback_data_s data = {0};

	data.name = name;
	data.rel = rel;

	store_for_each(SYS_REL_ATTRIBUTE, (store_for_each_callback_t) resolve_attribute_callback, &data);

	return data.found;
}

typedef struct push_asterisk_callback_data_s
{
	select_attr_array_pt array;
	short rel;
} *push_asterisk_callback_data_t;

static int
push_asterisk_callback(unsigned id, record_t record, push_asterisk_callback_data_t data)
{
	if (*(short *) record->values[4].data == data->rel)
		select_attr_push(data->array, id);

	return 0;
}

static select_attr_array_t
resolve_select_attrs(ast_select_value_list_t list, short from_rel)
{
	struct attr_find_callback_data_s result = {0};
	struct push_asterisk_callback_data_s asterisk_callback_data = {0};
	ast_select_value_pt i, end;
	unsigned found_attr;

	result.list = list;
	result.from_rel = from_rel;
	result.attrs.count = 0;
	result.attrs.size = 2;
	result.attrs.attr_ids = calloc(result.attrs.size, sizeof(unsigned));

	for (i = list->array, end = list->array + list->count; i != end; ++i)
	{
		switch (i->type)
		{
			case AST_SELECT_COLUMN:
				/* find out attr id */
				found_attr = resolve_attribute(i->data.column.name, from_rel);
				if (found_attr == 0)
				{
					fprintf(stderr, "could not locate attribute %s of relation %hd", i->data.column.name, from_rel);
					free(result.attrs.attr_ids);
					result.attrs.attr_ids = NULL;
					return result.attrs;
				}
				select_attr_push(&result.attrs, found_attr);
				break;
			case AST_SELECT_ASTERISK:
				asterisk_callback_data.array = &result.attrs;
				asterisk_callback_data.rel = from_rel;
				store_for_each(SYS_REL_ATTRIBUTE, (store_for_each_callback_t) push_asterisk_callback,
							   &asterisk_callback_data);
				break;
		}
	}

	return result.attrs;
}

typedef struct resolved_condition_s
{
	attribute_t attribute;
	enum ast_operator operator;
	ast_literal_t value;
} *resolved_condition_t;

struct select_for_each_callback_data_s
{

	select_attr_array_t attrs;
	ast_select_value_list_t list;
	resolved_condition_t condition;
	FILE *sockf;
};

static int
select_filter_condition(record_t record, resolved_condition_t condition)
{
	if (condition == NULL || condition->attribute == NULL)
		return 0;

	if (condition->value.domain != condition->attribute->domain)
	{
		fprintf(stderr, "TODO: coerce type (%u -> %u)\n", condition->value.domain, condition->attribute->domain);
		return 1;
	}

	switch (condition->attribute->domain)
	{
		case AD_INTEGER:
			switch (condition->operator)
			{
				case AST_OP_EQ:
					return condition->value.value.int_val == *(int *) record->values[condition->attribute->index].data
						   ? 0 : 1;
				default:
					fprintf(stderr, "TODO: implement op %u for %u\n", condition->operator,
							condition->attribute->domain);
					return 1;
			}
			break;
		default:
			fprintf(stderr, "TODO: implement compare for %u\n", condition->attribute->domain);
	}

	return 1;
}

static int
select_for_each_callback(unsigned id, record_t record, struct select_for_each_callback_data_s *user)
{
	unsigned i, ai, j;
	struct record_s attr_rec = {0};

	if (select_filter_condition(record, user->condition))
		return 0;

	fprintf(user->sockf, "id = %u\n", id);

	for (i = 0; i < user->attrs.count; ++i)
	{
		fprintf(user->sockf, "column (%d): ", i);
		for (ai = 0; ai < record->def->attributes_count; ++ai)
		{
			if (record->def->attributes[ai] == user->attrs.attr_ids[i])
				break;
		}
		if (ai == record->def->attributes_count)
		{
			fprintf(stderr, "no attribute found");
			fprintf(user->sockf, "<?>");
		}
		else
		{
			if (store_find_by_id(SYS_REL_ATTRIBUTE, record->def->attributes[ai], &attr_rec) != EXIT_SUCCESS)
			{
				fprintf(user->sockf, "unknown domain type, v = ");
				for (j = 0; j < record->values[ai].data_size; ++j)
				{
					fprintf(user->sockf, "%02x", ((unsigned char *) record->values[ai].data)[j]);
				}
			}
			else
			{
				if (record->values[ai].null)
					fprintf(user->sockf, "<null>");
				else
					switch (*(enum attribute_domain *) attr_rec.values[2].data)
					{
						case AD_VARCHAR:
							fprintf(user->sockf, "'%.*s'", (int) record->values[ai].data_size - 1,
									(char *) record->values[ai].data);
							break;
						case AD_INTEGER:
							fprintf(user->sockf, "%d", *(int *) record->values[ai].data);
							break;
						case AD_BOOLEAN:
							if (*(char *) record->values[ai].data)
								fprintf(user->sockf, "true");
							else
								fprintf(user->sockf, "false");
							break;
						default:
							for (j = 0; j < record->values[ai].data_size; ++j)
							{
								fprintf(user->sockf, "%02x", ((unsigned char *) record->values[ai].data)[j]);
							}
							break;
					}
			}
		}
		fprintf(user->sockf, "\n");
	}

	fprintf(user->sockf, "\n");

	return 0;
}

struct select_record_attr_s
{
	size_t value_idx;
	enum attribute_domain domain;
	void *domain_data;
	char *name;
};

struct select_record_s
{
	struct select_record_attr_s *attrs;
	size_t count;
	size_t size;

	short rel;
	char *alias;
};

int
fill_select_record_callback(unsigned id, record_t record, struct select_record_s *user)
{
	short rel = *(short *) record->values[4].data;

	if (rel == user->rel)
	{
		if (user->count == user->size)
		{
			user->size *= 2;
			user->attrs = realloc(user->attrs, user->size * sizeof(struct select_record_attr_s));
		}

		memset(user->attrs + user->count, 0, sizeof(struct select_record_attr_s));
		user->attrs[user->count].name = calloc(record->values[0].data_size, 1);
		memcpy(user->attrs[user->count].name, record->values[0].data, record->values[0].data_size);
		user->attrs[user->count].domain = *(enum attribute_domain *) record->values[1].data;
		user->attrs[user->count].domain_data = malloc(record->values[2].data_size);
		memcpy(user->attrs[user->count].domain_data, record->values[2].data, record->values[2].data_size);
		user->attrs[user->count].value_idx = user->count;

		user->count += 1;
	}

	return EXIT_SUCCESS;
}

int
fill_select_record(struct select_record_s *out)
{
	out->count = 0;
	out->size = 2;
	out->attrs = calloc(out->size, sizeof(struct select_record_attr_s));

	store_for_each(SYS_REL_ATTRIBUTE, (store_for_each_callback_t) fill_select_record_callback, out);

	return EXIT_SUCCESS;
}

struct select_column_s
{
	size_t from_table;
	size_t from_index;
	char *alias;
};

struct select_result_s
{
	struct
	{
		struct select_record_s *array;
		size_t count;
		size_t size;
	} from;

	struct
	{
		struct select_column_s *array;
		size_t count;
		size_t size;
	} columns;
};

int
describe_from(ast_from_expression_pt from, struct select_result_s *result)
{
	ast_join_t join = from->head;
	struct select_record_s *rec;

	result->from.count = 1;
	result->from.size = 1;
	result->from.array = calloc(1, sizeof(struct select_record_s));

	rec = result->from.array;

	rec->alias = from->alias ? from->alias->name : NULL;
	rec->rel = rel_find_by_name(from->table.name, 0);

	if (rec->rel < 0)
	{
		fprintf(stderr, "could not find rel: %s\n", from->table.name);
		return EXIT_FAILURE;
	}

	fill_select_record(rec);

	while (join != NULL)
	{
		if (result->from.count == result->from.size)
		{
			result->from.size *= 2;
			result->from.array = realloc(result->from.array, result->from.size * sizeof(struct select_result_s));
		}

		rec = result->from.array + result->from.count;

		memset(rec, 0, sizeof(struct select_result_s));
		rec->alias = join->alias ? join->alias->name : NULL;
		rec->rel = rel_find_by_name(join->table.name, 0);

		if (rec->rel < 0)
		{
			fprintf(stderr, "could not find join rel: %s\n", join->table.name);
			return EXIT_FAILURE;
		}

		join = join->next;
		fill_select_record(rec);
		result->from.count += 1;
	}

	return EXIT_SUCCESS;
}

int
add_select_column(struct select_result_s *result, ast_column_name_t column, char *alias)
{
	struct select_record_s *i, *end;
	struct select_record_attr_s *j, *j_end;
	struct select_column_s *out;
	int found = 0;

	if (result->columns.size == result->columns.count)
	{
		result->columns.size *= 2;
		result->columns.array = realloc(result->columns.array, result->columns.size * sizeof(struct select_column_s));
	}

	out = result->columns.array + result->columns.count;
	out->alias = alias;

	for (i = result->from.array, end = result->from.array + result->from.count; i != end; ++i)
	{
		if (found)
		{
			break;
		}

		if (column.table.name != NULL)
		{
			if ((i->alias == NULL || strcmp(i->alias, column.table.name) != 0) &&
				strcmp(rel_get(i->rel)->name, column.table.name) != 0)
			{
				continue;
			}
		}

		for (j = i->attrs, j_end = i->attrs + i->count; j != j_end; ++j)
		{
			if (strcmp(j->name, column.name) == 0)
			{
				found = 1;
				out->from_table = i - result->from.array;
				out->from_index = j->value_idx;
			}
		}
	}

	if (!found)
	{
		fprintf(stderr, "could not find attribute %s\n", column.name);
		return EXIT_FAILURE;
	}

	result->columns.count += 1;

	return EXIT_SUCCESS;
}

int
add_select_asterisk(struct select_result_s *result, ast_table_name_t table)
{
	struct select_record_s *i, *end;
	struct select_record_attr_s *j, *j_end;
	struct select_column_s *k;

	for (i = result->from.array, end = result->from.array + result->from.count; i != end; ++i)
	{
		if (!((i->alias == NULL || strcmp(i->alias, table.name) != 0) &&
			  strcmp(rel_get(i->rel)->name, table.name) != 0))
		{
			while (result->columns.size <= result->columns.count + i->count)
			{
				result->columns.size *= 2;
				result->columns.array = realloc(result->columns.array,
												result->columns.size * sizeof(struct select_column_s));
			}

			for (j = i->attrs, j_end = i->attrs + i->count, k = result->columns.array + result->columns.count;
				 j != j_end; ++j, ++k)
			{
				k->alias = NULL;
				k->from_index = j->value_idx;
				k->from_table = i - result->from.array;
			}

			return EXIT_SUCCESS;
		}
	}

	return EXIT_FAILURE;
}

int
describe_columns(ast_select_node_t select, struct select_result_s *result)
{
	ast_select_value_pt i, end;

	result->columns.count = 0;
	result->columns.size = 2;
	result->columns.array = calloc(result->columns.size, sizeof(struct select_column_s));

	for (i = select->values->array, end = select->values->array + select->values->count; i != end; ++i)
	{
		switch (i->type)
		{
			case AST_SELECT_ASTERISK:
				add_select_asterisk(result, i->data.table);
				break;
			case AST_SELECT_COLUMN:
				add_select_column(result, i->data.column, i->alias->name);
				break;
		}
	}

	return EXIT_SUCCESS;
}

int
describe_result(ast_select_node_t select, struct select_result_s *result)
{
	if (describe_from(&select->from, result) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	if (describe_columns(select, result) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

struct select_ex_callback_data_s
{
	struct select_result_s *result;
	FILE *sock;
};

int
select_2_for_each_callback(unsigned id, record_t record, struct select_ex_callback_data_s *result)
{

	
	return EXIT_SUCCESS;
}

int
execute_select(ast_select_node_t select, FILE *sockf)
{
	/*
	 * 	Executing SELECT:
	 * 		* Build a temporary table (create record_def)
	 * 		* Populate temporary table
	 * 		* Filter temporary table
	 * 		* Return rows
	 */

//	short from_rel_id;
//	unsigned *attr_ids = calloc(select->values->count, sizeof(unsigned));
//	struct attr_find_callback_data_s data;
//	struct select_for_each_callback_data_s select_data;
	struct select_result_s result = {0};
	struct select_ex_callback_data_s data = {0};

	fprintf(sockf, "select\n");

	if (describe_result(select, &result) != EXIT_SUCCESS)
	{
		fprintf(sockf, "error");
		return EXIT_FAILURE;
	}

	data.sock = sockf;
	data.result = &result;

	store_for_each(result.from.array[0].rel, (store_for_each_callback_t) select_2_for_each_callback, &data);

//	from_rel_id = rel_find_by_name(select->from.table.name, 0);
//
//	if (from_rel_id <= 0)
//	{
//		fprintf(sockf, "not found\n");
//		free(attr_ids);
//		return 1;
//	}
//
//	data.list = select->values;
//	data.from_rel = from_rel_id;
//	data.attrs = resolve_select_attrs(select->values, from_rel_id);
//
//	select_data.list = data.list;
//	select_data.attrs = data.attrs;
//	select_data.sockf = sockf;
//	if (select->condition)
//	{
//		select_data.condition = calloc(1, sizeof(struct resolved_condition_s));
//		select_data.condition->value = select->condition->value;
//		select_data.condition->operator = select->condition->operator;
//		select_data.condition->attribute = attribute_resolve(from_rel_id, select->condition->column->name);
//
//		if (select_data.condition->attribute == NULL)
//		{
//			fprintf(stderr, "could not resolve attribute\n");
//		}
//	}
//
//	store_for_each(from_rel_id, (store_for_each_callback_t) select_for_each_callback, &select_data);
//
//	free(attr_ids);

	return EXIT_SUCCESS;
}
