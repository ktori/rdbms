/*
 * Created by victoria on 24.02.20.
*/

#include <stdlib.h>
#include <ast/ast.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <table/relation.h>
#include <storage/storage.h>
#include <table/record.h>
#include "runner.h"
#include "ast/select.h"
#include "ast/statement.h"
#include "ast/insert.h"
#include "ast/create_table.h"
#include "ast/shared.h"

struct attr_find_callback_data_s
{
	unsigned *out_ids;
	ast_name_list_node_t list;
	short from_rel;
};

static int
attr_find_callback(unsigned id, record_t record, struct attr_find_callback_data_s *user)
{
	unsigned i;

	if (*(short *) record->values[4].data != user->from_rel)
		return 0;

	for (i = 0; i < user->list->count; ++i)
	{

		if (strncmp(user->list->array[i]->name, record->values[0].data, record->values[0].data_size - 1) == 0)
		{
			user->out_ids[i] = id;
		}
	}

	return 0;
}

static record_t
record_from_tuple(record_def_t def, struct attr_find_callback_data_s *columns, ast_insert_tuple_node_t tuple, FILE *sockf)
{
	struct record_s attr;
	struct record_value_s value = {0};
	record_t result = record_create(def);
	unsigned i, j;

	for (i = 0; i < def->attributes_count; ++i)
	{
		for (j = 0; j < columns->list->count; ++j)
		{
			if (columns->out_ids[j] == def->attributes[i])
				break;
		}
		store_find_by_id(SYS_REL_ATTRIBUTE, def->attributes[i], &attr);
		if (j == columns->list->count)
		{
			if (*(char *)attr.values[1].data)
				result->values[i] = record_value_null();
			else
			{
				fprintf(stderr, "attribute %s is not nullable\n", (const char *)attr.values[0].data);
				fprintf(sockf, "attribute %s is not nullable\n", (const char *)attr.values[0].data);
				return NULL;
			}
		}
		else
		{
			switch (* (enum attribute_domain *) attr.values[2].data)
			{
				case AD_BOOLEAN:
					if (tuple->values[j].domain != AD_BOOLEAN)
					{
						fprintf(stderr, "unexpected domain\n");
						fprintf(sockf, "unexpected domain\n");
						return NULL;
					}
					value = record_value_from(&tuple->values[j].value.bool_val, sizeof(char));
					break;
				case AD_VARCHAR:
					if (tuple->values[j].domain != AD_VARCHAR)
					{
						fprintf(stderr, "unexpected domain\n");
						fprintf(sockf, "unexpected domain\n");
						return NULL;
					}
					value = record_value_str(tuple->values[j].value.string_val);
					break;
				case AD_INTEGER:
					if (tuple->values[j].domain != AD_INTEGER)
					{
						fprintf(stderr, "unexpected domain\n");
						fprintf(sockf, "unexpected domain\n");
						return NULL;
					}
					value = record_value_from(&tuple->values[j].value.int_val, sizeof(int));
					break;
				default:
					fprintf(stderr, "type not supported\n");
					fprintf(sockf, "type not supported\n");
					return NULL;
			}
			result->values[i] = value;
		}
	}

	return result;
}

int
execute_insert(ast_insert_node_t insert, FILE *sockf)
{
	unsigned i, out_id;
	short from_rel_id;
	relation_t rel;
	record_t record;
	unsigned *attr_ids = calloc(insert->columns->count, sizeof(unsigned));
	struct attr_find_callback_data_s data;
	printf("INSERT INTO %s (", insert->table->name);
	for (i = 0; i < insert->columns->count; ++i)
	{
		printf("%s", insert->columns->array[i]->name);
		if (i < insert->columns->count - 1)
			printf(", ");
	}
	printf(") VALUES (");
	for (i = 0; i < insert->tuple->count; ++i)
	{
		switch ((int) insert->tuple->values[i].domain)
		{
			case 0:
				printf("NULL");
				break;
			case AD_VARCHAR:
				printf("%s", insert->tuple->values[i].value.string_val);
				break;
			case AD_BOOLEAN:
				if (insert->tuple->values[i].value.bool_val)
					printf("TRUE");
				else
					printf("FALSE");
				break;
			case AD_INTEGER:
				printf("%d", insert->tuple->values[i].value.int_val);
				break;
			default:
				printf("<unknown>");
				break;
		}
		if (i < insert->tuple->count - 1)
			printf(", ");
	}
	printf(");\n");

	from_rel_id = rel_find_by_name(insert->table->name, 0);

	if (from_rel_id <= 0)
	{
		fprintf(sockf, "not found\n");
		free(attr_ids);
		return 1;
	}

	data.list = insert->columns;
	data.out_ids = attr_ids;
	data.from_rel = from_rel_id;

	store_for_each(SYS_REL_ATTRIBUTE, (store_for_each_callback_t) attr_find_callback, &data);

	rel = rel_get(from_rel_id);

	record = record_from_tuple(rel->record_def, &data, insert->tuple, sockf);

	if (record == NULL)
		return 1;

	store_insert(from_rel_id, &out_id, record);

	fprintf(sockf, "insert = %u\n", out_id);

	return 0;
}
