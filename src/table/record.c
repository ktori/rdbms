/*
 * Created by victoria on 23.02.20.
*/

#include "record.h"

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "lib/strdup.h"

record_def_t
record_def_create()
{
	return calloc(1, sizeof(struct record_def_s));
}

int
record_def_add_attribute(record_def_t record_def, unsigned attribute)
{
	if (record_def->attributes == NULL)
	{
		record_def->attributes_count = 0;
		record_def->attributes_size = 1;
		record_def->attributes = malloc(record_def->attributes_size * sizeof(unsigned));
	}
	else if (record_def->attributes_count == record_def->attributes_size)
	{
		record_def->attributes_size *= 2;
		record_def->attributes = realloc(record_def->attributes, record_def->attributes_size * sizeof(unsigned));
	}

	record_def->attributes[record_def->attributes_count++] = attribute;

	return 0;
}

void
record_def_free(record_def_t *record_def)
{
	free((*record_def)->attributes);
	free(*record_def);
	*record_def = NULL;
}

record_t
record_create(record_def_t def)
{
	unsigned i;
	record_t result = malloc(sizeof(struct record_s));

	result->def = def;
	result->values = calloc(def->attributes_count, sizeof(struct record_value_s));

	for (i = 0; i < def->attributes_count; ++i)
		result->values[i].null = 1;

	return result;
}

void
record_free(record_t *record)
{
	unsigned i;

	for (i = 0; i < (*record)->def->attributes_count; ++i)
	{
		/* TODO */
		free((*record)->values[i].data);
	}

	free((*record)->values);
	free(*record);
	*record = NULL;
}

record_def_t
record_def_create_from(short rel, attribute_t attributes, unsigned count)
{
	unsigned i;
	record_def_t result = malloc(sizeof(struct record_def_s));

	result->attributes = calloc(count, sizeof(unsigned));
	result->attributes_size = count;
	result->attributes_count = count;

	for (i = 0; i < count; ++i)
		if (attribute_register(rel, attributes + i, result->attributes + i) != EXIT_SUCCESS)
		{
			fprintf(stderr, "error registering attribute\n");
			return NULL;
		}

	return result;
}

struct record_value_s
record_value_str(const char *str)
{
	struct record_value_s result = {0};
	result.null = 0;
	result.data = strdup(str);
	result.data_size = strlen(str) + 1;
	return result;
}

struct record_value_s
record_value_from(void *data, size_t size)
{
	struct record_value_s result = {0};
	result.null = 0;
	result.data = malloc(size);
	memcpy(result.data, data, size);
	result.data_size = size;
	return result;
}

struct record_value_s
record_value_null()
{
	struct record_value_s result = {0};
	result.null = 1;
	return result;
}
