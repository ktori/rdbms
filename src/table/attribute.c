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

struct attribute_resolve_data
{
	const char *name;
	short rel;
	attribute_t result;
	int found;
};

static int
attribute_resolve_callback(unsigned id, record_t record, struct attribute_resolve_data *user)
{
	if (*(short *)record->values[AR_RELATION].data != user->rel)
		return 0;

	if (strcmp(record->values[AR_NAME].data, user->name) == 0)
	{
		user->result = calloc(1, sizeof(struct attribute_s));
		user->result->id = id;
		user->result->name = strdup(record->values[AR_NAME].data);
		if (record->values[AR_DOMAIN_DATA].null == 0)
		{
			user->result->domain_data = malloc(record->values[AR_DOMAIN_DATA].data_size);
			memcpy(user->result->domain_data, record->values[AR_DOMAIN_DATA].data, record->values[AR_DOMAIN_DATA].data_size);
			user->result->domain_data_size = record->values[AR_DOMAIN_DATA].data_size;
		}
		else
		{
			user->result->domain_data = NULL;
		}
		user->result->nullable = *(char *)record->values[AR_NULLABLE].data;
		user->result->domain = *(unsigned *)record->values[AR_DOMAIN].data;
		user->found = 1;
		return 1;
	}

	return 0;
}

attribute_t
attribute_resolve(short rel, const char *name)
{
	unsigned i;
	relation_t relation;
	struct attribute_resolve_data data = {0};
	data.rel = rel;
	data.name = name;

	/* SELECT * FROM "sys_attribute" WHERE relation = ? AND name = ? */
	store_for_each(SYS_REL_ATTRIBUTE, (store_for_each_callback_t) attribute_resolve_callback, &data);

	if (data.result != NULL)
	{
		relation = rel_get(rel);
		for (i = 0; i < relation->record_def->attributes_count; ++i)
		{
			if (relation->record_def->attributes[i] == data.result->id)
			{
				data.result->index = i;
				break;
			}
		}
		if (i == relation->record_def->attributes_count)
		{
			fprintf(stderr, "could not get attr index\n");
		}
	}

	return data.result;
}

void
attribute_free(attribute_t attr)
{
	free(attr->domain_data);
	free(attr->name);
	free(attr);
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
