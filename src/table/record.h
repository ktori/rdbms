/*
 * Created by victoria on 23.02.20.
*/

#pragma once

#include <stddef.h>

#include "domain.h"
#include "attribute.h"

typedef struct record_def_s
{
	unsigned *attributes;
	size_t attributes_count;
	size_t attributes_size;
} *record_def_t;

typedef struct record_value_s
{
	int null;
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

record_def_t
record_def_create_from(short rel, attribute_t attributes, unsigned count);

int
record_def_add_attribute(record_def_t record_def, unsigned attribute);

void
record_def_free(record_def_t *record_def);

record_t
record_create(record_def_t def);

void
record_free(record_t *record);

struct record_value_s
record_value_str(const char *str);

struct record_value_s
record_value_from(void *data, size_t size);

struct record_value_s
record_value_null();