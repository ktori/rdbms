/*
 * Created by victoria on 23.02.20.
*/

#include "attribute.h"

#include "relation.h"
#include "lib/strdup.h"
#include "record.h"
#include "domain_enum.h"

#include <malloc.h>
#include <string.h>
#include <storage/storage.h>

/*
 * 	CREATE TABLE "sys_attribute"
 * 	(
 * 		"name" VARCHAR NOT NULL,
 * 		"nullable" BOOLEAN NOT NULL,
 * 		"domain" INTEGER NOT NULL REFERENCES "sys_domain"("id")
 * 		"domain_data" BLOB,
 * 		"relation" SMALLINT NOT NULL
 * 	);
 */

static void
init_attr_name(record_def_t record_def)
{
	record_t record;
	struct record_value_s record_value;
	char false = 0;

	record = record_create(record_def);
	record_value.data = strdup("name");
	record_value.data_size = sizeof("name");
	record->values[0] = record_value;
	record_value.data = malloc(sizeof(char));
	record_value.data_size = sizeof(char);
	*(char *)(record_value.data) = false;
	record->values[1] = record_value;
	record_value.data = malloc(sizeof(unsigned));
	record_value.data_size = sizeof(unsigned);
	*(unsigned *)(record_value.data) = AD_VARCHAR;
	record->values[2] = record_value;
	record_value.data = NULL;
	record_value.data_size = 0;
	record->values[3] = record_value;
	record_value.data = malloc(sizeof(short));
	record_value.data_size = sizeof(short);
	*(short *)(record_value.data) = SYS_REL_ATTRIBUTE;
	record->values[4] = record_value;

	store_insert_in_place(SYS_REL_ATTRIBUTE, ATTR_SYS_ATTRIBUTE_NAME, record);
	record_free(&record);
}

static void
init_attr_nullable(record_def_t record_def)
{
	record_t record;
	struct record_value_s record_value;
	char false = 0;

	record = record_create(record_def);
	record_value.data = strdup("nullable");
	record_value.data_size = sizeof("nullable");
	record->values[0] = record_value;
	record_value.data = malloc(sizeof(char));
	record_value.data_size = sizeof(char);
	*(char *)(record_value.data) = false;
	record->values[1] = record_value;
	record_value.data = malloc(sizeof(unsigned));
	record_value.data_size = sizeof(unsigned);
	*(unsigned *)(record_value.data) = AD_BOOLEAN;
	record->values[2] = record_value;
	record_value.data = NULL;
	record_value.data_size = 0;
	record->values[3] = record_value;
	record_value.data = malloc(sizeof(short));
	record_value.data_size = sizeof(short);
	*(short *)(record_value.data) = SYS_REL_ATTRIBUTE;
	record->values[4] = record_value;

	store_insert_in_place(SYS_REL_ATTRIBUTE, ATTR_SYS_ATTRIBUTE_NULLABLE, record);
	record_free(&record);
}

static void
init_attr_domain(record_def_t record_def)
{
	record_t record;
	struct record_value_s record_value;
	char false = 0;

	record = record_create(record_def);
	record_value.data = strdup("domain");
	record_value.data_size = sizeof("domain");
	record->values[0] = record_value;
	record_value.data = malloc(sizeof(char));
	record_value.data_size = sizeof(char);
	*(char *)(record_value.data) = false;
	record->values[1] = record_value;
	record_value.data = malloc(sizeof(unsigned));
	record_value.data_size = sizeof(unsigned);
	*(unsigned *)(record_value.data) = AD_INTEGER;
	record->values[2] = record_value;
	record_value.data = NULL;
	record_value.data_size = 0;
	record->values[3] = record_value;
	record_value.data = malloc(sizeof(short));
	record_value.data_size = sizeof(short);
	*(short *)(record_value.data) = SYS_REL_ATTRIBUTE;
	record->values[4] = record_value;

	store_insert_in_place(SYS_REL_ATTRIBUTE, ATTR_SYS_ATTRIBUTE_DOMAIN, record);
	record_free(&record);
}

static void
init_attr_domain_data(record_def_t record_def)
{
	record_t record;
	struct record_value_s record_value;
	char true = 1;

	record = record_create(record_def);
	record_value.data = strdup("domain_data");
	record_value.data_size = sizeof("domain_data");
	record->values[0] = record_value;
	record_value.data = malloc(sizeof(char));
	record_value.data_size = sizeof(char);
	*(char *)(record_value.data) = true;
	record->values[1] = record_value;
	record_value.data = malloc(sizeof(unsigned));
	record_value.data_size = sizeof(unsigned);
	*(unsigned *)(record_value.data) = AD_BLOB;
	record->values[2] = record_value;
	record_value.data = NULL;
	record_value.data_size = 0;
	record->values[3] = record_value;
	record_value.data = malloc(sizeof(short));
	record_value.data_size = sizeof(short);
	*(short *)(record_value.data) = SYS_REL_ATTRIBUTE;
	record->values[4] = record_value;

	store_insert_in_place(SYS_REL_ATTRIBUTE, ATTR_SYS_ATTRIBUTE_DOMAIN_DATA, record);
	record_free(&record);
}

static void
init_attr_relation(record_def_t record_def)
{
	record_t record;
	struct record_value_s record_value;
	char false = 0;

	record = record_create(record_def);
	record_value.data = strdup("relation");
	record_value.data_size = sizeof("relation");
	record->values[0] = record_value;
	record_value.data = malloc(sizeof(char));
	record_value.data_size = sizeof(char);
	*(char *)(record_value.data) = false;
	record->values[1] = record_value;
	record_value.data = malloc(sizeof(unsigned));
	record_value.data_size = sizeof(unsigned);
	*(unsigned *)(record_value.data) = AD_INTEGER;
	record->values[2] = record_value;
	record_value.data = NULL;
	record_value.data_size = 0;
	record->values[3] = record_value;
	record_value.data = malloc(sizeof(short));
	record_value.data_size = sizeof(short);
	*(short *)(record_value.data) = SYS_REL_ATTRIBUTE;
	record->values[4] = record_value;

	store_insert_in_place(SYS_REL_ATTRIBUTE, ATTR_SYS_ATTRIBUTE_RELATION, record);
	record_free(&record);
}

static void
init_domain_name(record_def_t record_def)
{
	record_t record;
	struct record_value_s record_value;
	char false = 0;

	record = record_create(record_def);
	record_value.data = strdup("name");
	record_value.data_size = sizeof("name");
	record->values[0] = record_value;
	record_value.data = malloc(sizeof(char));
	record_value.data_size = sizeof(char);
	*(char *)(record_value.data) = false;
	record->values[1] = record_value;
	record_value.data = malloc(sizeof(unsigned));
	record_value.data_size = sizeof(unsigned);
	*(unsigned *)(record_value.data) = AD_VARCHAR;
	record->values[2] = record_value;
	record_value.data = NULL;
	record_value.data_size = 0;
	record->values[3] = record_value;
	record_value.data = malloc(sizeof(short));
	record_value.data_size = sizeof(short);
	*(short *)(record_value.data) = SYS_REL_DOMAIN;
	record->values[4] = record_value;

	store_insert_in_place(SYS_REL_ATTRIBUTE, ATTR_SYS_DOMAIN_NAME, record);
	record_free(&record);
}

int
attribute_init()
{
	relation_t rel = rel_alloc();
	record_def_t record_def;

	record_def = record_def_create();
	record_def_add_attribute(record_def, ATTR_SYS_ATTRIBUTE_NAME);
	record_def_add_attribute(record_def, ATTR_SYS_ATTRIBUTE_NULLABLE);
	record_def_add_attribute(record_def, ATTR_SYS_ATTRIBUTE_DOMAIN);
	record_def_add_attribute(record_def, ATTR_SYS_ATTRIBUTE_DOMAIN_DATA);
	record_def_add_attribute(record_def, ATTR_SYS_ATTRIBUTE_RELATION);

	rel->id = SYS_REL_ATTRIBUTE;
	rel->name = strdup("sys_attribute");
	rel->record_def = record_def;

	init_attr_name(record_def);
	init_attr_nullable(record_def);
	init_attr_domain(record_def);
	init_attr_domain_data(record_def);
	init_attr_relation(record_def);

	init_domain_name(record_def);

	return 0;
}
