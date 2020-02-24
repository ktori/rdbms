/*
 * Created by victoria on 23.02.20.
*/

#include "relation.h"

#include <stdlib.h>
#include <string.h>
#include <storage/storage.h>
#include <stdio.h>
#include "lib/strdup.h"
#include "domain_enum.h"

static relation_t relations = NULL;
static unsigned relations_count = 0;
static unsigned relations_size = 0;

short next_user_id = 1000;

static record_def_t rel_record_def_ptr = NULL;

record_def_t
static rel_record_def()
{
	if (rel_record_def_ptr != NULL)
		return rel_record_def_ptr;

	rel_record_def_ptr = record_def_create();

	record_def_add_attribute(rel_record_def_ptr, ATTR_SYS_RELATION_ID);
	record_def_add_attribute(rel_record_def_ptr, ATTR_SYS_RELATION_NAME);

	return rel_record_def_ptr;
}

relation_t
rel_create(short id, const char *name, record_def_t record_def)
{
	record_t record = record_create(rel_record_def());

	if (relations_size == relations_count)
	{
		relations_size *= 2;
		relations = realloc(relations, relations_size * sizeof(struct relation_s));
	}

	memset(relations + relations_count, 0, sizeof(struct relation_s));

	if (id < 0)
		relations[relations_count].id = next_user_id++;
	else
		relations[relations_count].id = id;

	relations[relations_count].name = strdup(name);
	relations[relations_count].record_def = record_def;
	relations_count += 1;

	record->values[0] = record_value_from(&id, sizeof(short));
	record->values[1] = record_value_str(name);

	store_insert(SYS_REL_RELATION, NULL, record);
	return relations + relations_count - 1;
}

short
rel_find_by_name(char *name, int case_sens)
{
	relation_t i = relations, end = relations + relations_count;

	for (; i < end; ++i)
	{
		if (strcmp(i->name, name) == 0)
			return i->id;
	}

	return -1;
}

int
rel_init()
{
	relations_size = 8;
	relations_count = 0;
	relations = calloc(relations_size, sizeof(struct relation_s));
	rel_create(SYS_REL_RELATION, "sys_relation", rel_record_def());

	return EXIT_SUCCESS;
}

relation_t
rel_get(short rel)
{
	relation_t i = relations, end = relations + relations_count;

	for (; i < end; ++i)
	{
		if (i->id == rel)
			return i;
	}

	return NULL;
}

void
rel_shutdown()
{
	relation_t i = relations, end = relations + relations_count;

	for (; i < end; ++i)
	{
		record_def_free(&i->record_def);
		free(i->name);
	}

	free(relations);
}

short
rel_alloc()
{
	return next_user_id++;
}
