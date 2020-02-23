/*
 * Created by victoria on 23.02.20.
*/

#include "record.h"

#include <malloc.h>

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
	record_t result = malloc(sizeof(struct record_s));

	result->def = def;
	result->values = calloc(def->attributes_count, sizeof(struct record_value_s));

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
