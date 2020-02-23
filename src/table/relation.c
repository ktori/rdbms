/*
 * Created by victoria on 23.02.20.
*/

#include "relation.h"

#include <stdlib.h>
#include <string.h>

static relation_t relations = NULL;
static unsigned relations_count = 0;
static unsigned relations_size = 0;

short next_user_id = 1000;

relation_t
rel_alloc()
{
	if (relations_size == relations_count)
	{
		relations_size *= 2;
		relations = realloc(relations, relations_size * sizeof(struct relation_s));
	}

	memset(relations + relations_count, 0, sizeof(struct relation_s));
	relations_count += 1;
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

relation_t
rel_create(char *name)
{
	relation_t result = rel_alloc();

	result->id = next_user_id;
	next_user_id += 1;
	result->name = malloc(strlen(name) + 1);
	strcpy(result->name, name);

	return result;
}

int
rel_init()
{
	relations_size = 8;
	relations_count = 0;
	relations = calloc(relations_size, sizeof(struct relation_s));

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
