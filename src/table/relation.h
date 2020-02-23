/*
 * Created by victoria on 23.02.20.
*/

#pragma once

#include "attribute.h"
#include "record.h"

enum sys_relation
{
	SYS_REL_DOMAIN = 1,
	SYS_REL_ATTRIBUTE,
	SYS_REL_RELATION
};

typedef struct relation_s
{
	short id;
	char *name;
	record_def_t record_def;
} *relation_t;

relation_t
rel_alloc();

int
rel_init();

short
rel_find_by_name(char *name, int case_sens);

relation_t
rel_get(short rel);

relation_t
rel_create(char *name);
