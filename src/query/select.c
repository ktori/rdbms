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

typedef struct resolved_condition_s
{
	attribute_t attribute;
	enum ast_operator operator;
	ast_literal_t value;
} *resolved_condition_t;

struct select_for_each_callback_data_s
{
	unsigned *attr_ids;
	ast_name_list_node_t list;
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
					fprintf(stderr, "TODO: implement op %u for %u\n", condition->operator, condition->attribute->domain);
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

	for (i = 0; i < user->list->count; ++i)
	{
		fprintf(user->sockf, "%s: ", user->list->array[i]->name);
		for (ai = 0; ai < record->def->attributes_count; ++ai)
		{
			if (record->def->attributes[ai] == user->attr_ids[i])
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

int
execute_select(ast_select_node_t select, FILE *sockf)
{
	short from_rel_id;
	unsigned *attr_ids = calloc(select->columns->count, sizeof(unsigned));
	struct attr_find_callback_data_s data;
	struct select_for_each_callback_data_s select_data;

	fprintf(sockf, "select\n");

	from_rel_id = rel_find_by_name(select->from->name->name, 0);

	if (from_rel_id <= 0)
	{
		fprintf(sockf, "not found\n");
		free(attr_ids);
		return 1;
	}

	data.list = select->columns;
	data.out_ids = attr_ids;
	data.from_rel = from_rel_id;

	store_for_each(SYS_REL_ATTRIBUTE, (store_for_each_callback_t) attr_find_callback, &data);

	select_data.list = data.list;
	select_data.attr_ids = data.out_ids;
	select_data.sockf = sockf;
	if (select->condition)
	{
		select_data.condition = calloc(1, sizeof(struct resolved_condition_s));
		select_data.condition->value = select->condition->value;
		select_data.condition->operator = select->condition->operator;
		select_data.condition->attribute = attribute_resolve(from_rel_id, select->condition->column->name);

		if (select_data.condition->attribute == NULL)
		{
			fprintf(stderr, "could not resolve attribute\n");
		}
	}

	store_for_each(from_rel_id, (store_for_each_callback_t) select_for_each_callback, &select_data);

	free(attr_ids);

	return EXIT_SUCCESS;
}
