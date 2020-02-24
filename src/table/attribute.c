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
#include <stdlib.h>

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

static int
sys_attribute_create(const char *name, unsigned domain, void *domain_data, unsigned domain_data_size,
					 char nullable, unsigned sys_id);

enum attr_record_value
{
	AR_NAME,
	AR_NULLABLE,
	AR_DOMAIN,
	AR_DOMAIN_DATA,
	AR_RELATION
};

static record_def_t sys_attribute_record_def_ptr = NULL;

static record_def_t
sys_attribute_record_def()
{
	if (sys_attribute_record_def_ptr != NULL)
		return sys_attribute_record_def_ptr;

	sys_attribute_record_def_ptr = record_def_create();

	record_def_add_attribute(sys_attribute_record_def_ptr, ATTR_SYS_ATTRIBUTE_NAME);
	record_def_add_attribute(sys_attribute_record_def_ptr, ATTR_SYS_ATTRIBUTE_NULLABLE);
	record_def_add_attribute(sys_attribute_record_def_ptr, ATTR_SYS_ATTRIBUTE_DOMAIN);
	record_def_add_attribute(sys_attribute_record_def_ptr, ATTR_SYS_ATTRIBUTE_DOMAIN_DATA);
	record_def_add_attribute(sys_attribute_record_def_ptr, ATTR_SYS_ATTRIBUTE_RELATION);

	return sys_attribute_record_def_ptr;
}

int
attribute_init()
{
	relation_t rel = rel_create(SYS_REL_ATTRIBUTE, "sys_attribute", sys_attribute_record_def());

	sys_attribute_create("name", AD_VARCHAR, NULL, 0, 0, ATTR_SYS_ATTRIBUTE_NAME);
	sys_attribute_create("nullable", AD_BOOLEAN, NULL, 0, 0, ATTR_SYS_ATTRIBUTE_NULLABLE);
	sys_attribute_create("domain", AD_INTEGER, NULL, 0, 0, ATTR_SYS_ATTRIBUTE_DOMAIN);
	sys_attribute_create("domain_data", AD_BLOB, NULL, 0, 1, ATTR_SYS_ATTRIBUTE_DOMAIN_DATA);
	sys_attribute_create("relation", AD_SMALL_INTEGER, NULL, 0, 0, ATTR_SYS_ATTRIBUTE_RELATION);

	store_insert_in_place(SYS_REL_ATTRIBUTE, ATTR_SYS_RELATION_ID,
						  attribute_record_create(SYS_REL_RELATION, "id", AD_SMALL_INTEGER, NULL, 0, 0));
	store_insert_in_place(SYS_REL_ATTRIBUTE, ATTR_SYS_RELATION_NAME,
						  attribute_record_create(SYS_REL_RELATION, "name", AD_VARCHAR, NULL, 0, 0));

	return rel != NULL;
}

int
attribute_register(short rel, attribute_t attribute, unsigned *out_id)
{
	return attribute_create(rel, attribute->name, attribute->domain, attribute->domain_data,
							attribute->domain_data_size, attribute->nullable, out_id);
}

record_t
attribute_record_create(short rel, const char *name, unsigned domain, void *domain_data, unsigned domain_data_size,
						char nullable)
{
	record_t record = record_create(sys_attribute_record_def());

	record->values[AR_NAME] = record_value_str(name);
	record->values[AR_DOMAIN] = record_value_from(&domain, sizeof(unsigned));
	record->values[AR_DOMAIN_DATA] = domain_data ? record_value_from(domain_data,
																	 domain_data_size)
												 : record_value_null();
	record->values[AR_NULLABLE] = record_value_from(&nullable, sizeof(char));
	record->values[AR_RELATION] = record_value_from(&rel, sizeof(short));

	return record;
}

int
attribute_create(short rel, const char *name, unsigned domain, void *domain_data, unsigned domain_data_size,
				 char nullable, unsigned *out_id)
{
	record_t record = attribute_record_create(rel, name, domain, domain_data, domain_data_size, nullable);
	int status = store_insert(SYS_REL_ATTRIBUTE, out_id, record);
	record_free(&record);
	return status;
}

static int
sys_attribute_create(const char *name, unsigned domain, void *domain_data, unsigned domain_data_size,
					 char nullable, unsigned sys_id)
{
	record_t record = attribute_record_create(SYS_REL_ATTRIBUTE, name, domain, domain_data, domain_data_size, nullable);
	int status = store_insert_in_place(SYS_REL_ATTRIBUTE, sys_id, record);
	record_free(&record);
	return status;
}
