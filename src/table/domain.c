/*
 * Created by victoria on 23.02.20.
*/

#include <storage/storage.h>
#include "domain.h"
#include "relation.h"

#include "lib/strdup.h"
#include "record.h"

static int
domain_init_type(record_def_t record_def, const char *name, enum attribute_domain domain)
{
	record_t record = record_create(record_def);
	struct record_value_s value = {0};
	int status;
	value.data = strdup("BOOL");
	value.data_size = sizeof("BOOL");
	record->values[0] = value;
	status = store_insert_in_place(SYS_REL_DOMAIN, AD_BOOLEAN, record);
	record_free(&record);
	return status;
}

int
domain_init()
{
	relation_t rel = rel_alloc();
	record_def_t record_def;

	record_def = record_def_create();
	record_def_add_attribute(record_def, ATTR_SYS_DOMAIN_NAME);

	rel->id = SYS_REL_DOMAIN;
	rel->name = strdup("sys_domain");
	rel->record_def = record_def;

	domain_init_type(record_def, "BOOL", AD_BOOLEAN);
	domain_init_type(record_def, "CHAR", AD_CHAR);
	domain_init_type(record_def, "VARCHAR", AD_VARCHAR);
	domain_init_type(record_def, "BYTE", AD_BYTE);
	domain_init_type(record_def, "SMALLINT", AD_SMALL_INTEGER);
	domain_init_type(record_def, "INT", AD_INTEGER);
	domain_init_type(record_def, "REAL", AD_REAL);
	domain_init_type(record_def, "BLOB", AD_BLOB);

	return 0;
}
