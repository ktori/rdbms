/*
 * Created by victoria on 23.02.20.
*/

#include "runner.h"

#include <stdlib.h>
#include <ast.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <table/relation.h>
#include <storage/storage.h>
#include <table/record.h>

struct attr_find_callback_data_s
{
	unsigned *out_ids;
	struct name_list_ast_node_s *list;
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

struct select_for_each_callback_data_s
{
	unsigned *attr_ids;
	struct name_list_ast_node_s *list;
	FILE *sockf;
};

static int
select_for_each_callback(unsigned id, record_t record, struct select_for_each_callback_data_s *user)
{
	unsigned i, ai, j;
	struct record_s attr_rec = {0};

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

static int
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
		fclose(sockf);
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

	store_for_each(from_rel_id, (store_for_each_callback_t) select_for_each_callback, &select_data);

	free(attr_ids);

	return EXIT_SUCCESS;
}

static int
execute_create_table(ast_create_table_node_t ast, FILE *sockf)
{
	return 0;
}

int
execute(ast_statement_node_t statement, FILE *sockf)
{
	switch (statement->type)
	{
		case AST_SELECT:
			return execute_select(statement->body.select, sockf);
		case AST_CREATE_TABLE:
			return execute_create_table(statement->body.create_table, sockf);
	}

	fprintf(stderr, "unknown statement type: %d\n", statement->type);

	return EXIT_FAILURE;
}
