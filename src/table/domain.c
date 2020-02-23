/*
 * Created by victoria on 23.02.20.
*/

#include <storage/storage.h>
#include "domain_enum.h"
#include "domain.h"
#include "relation.h"

#include <string.h>
#include "lib/strdup.h"
#include "record.h"

static int
domain_init_type(record_def_t record_def, const char *name, enum attribute_domain domain)
{
	record_t record = record_create(record_def);
	struct record_value_s value = {0};
	int status;
	value.data = strdup(name);
	value.data_size = strlen(name) + 1;
	record->values[0] = value;
	status = store_insert_in_place(SYS_REL_DOMAIN, domain, record);
	record_free(&record);
	return status;
}

static record_def_t domain_record_def_ptr = NULL;

static record_def_t
domain_record_def()
{
	if (domain_record_def_ptr)
		return domain_record_def_ptr;

	domain_record_def_ptr = record_def_create();
	store_insert_in_place(SYS_REL_ATTRIBUTE, ATTR_SYS_DOMAIN_NAME,
						  attribute_record_create(SYS_REL_DOMAIN, "name", AD_VARCHAR, NULL, 0, 0));
	record_def_add_attribute(domain_record_def_ptr, ATTR_SYS_DOMAIN_NAME);

	return domain_record_def_ptr;
}

int
domain_init()
{
	record_def_t def = domain_record_def();
	relation_t rel = rel_create(SYS_REL_DOMAIN, "sys_domain", def);

	if (rel == NULL)
		return 1;

	domain_init_type(def, "BOOL", AD_BOOLEAN);
	domain_init_type(def, "CHAR", AD_CHAR);
	domain_init_type(def, "VARCHAR", AD_VARCHAR);
	domain_init_type(def, "BYTE", AD_BYTE);
	domain_init_type(def, "SMALLINT", AD_SMALL_INTEGER);
	domain_init_type(def, "INT", AD_INTEGER);
	domain_init_type(def, "REAL", AD_REAL);
	domain_init_type(def, "BLOB", AD_BLOB);

	return 0;
}
