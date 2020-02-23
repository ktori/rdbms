/*
 * Created by victoria on 23.02.20.
*/

#pragma once

#include <stddef.h>

#include "domain.h"

typedef struct record_def_s
{
	unsigned *attributes;
	size_t attributes_count;
	size_t attributes_size;
} *record_def_t;

typedef struct record_value_s
{
	void *data;
	size_t data_size;
} *record_value_t;

typedef struct record_s
{
	record_def_t def;
	record_value_t values;
} *record_t;

record_def_t
record_def_create();

int
record_def_add_attribute(record_def_t record_def, unsigned attribute);

void
record_def_free(record_def_t *record_def);

record_t
record_create(record_def_t def);

void
record_free(record_t *record);
