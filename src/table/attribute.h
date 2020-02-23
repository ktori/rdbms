/*
 * Created by victoria on 23.02.20.
*/

#pragma once

#include <stddef.h>

enum attribute_builtin
{
	ATTR_SYS_ATTRIBUTE_NAME = 1,
	ATTR_SYS_ATTRIBUTE_NULLABLE,
	ATTR_SYS_ATTRIBUTE_DOMAIN,
	ATTR_SYS_ATTRIBUTE_DOMAIN_DATA,
	ATTR_SYS_ATTRIBUTE_RELATION,

	ATTR_SYS_DOMAIN_NAME
};

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

typedef struct attribute_s
{
	char *name;
	char nullable;
	unsigned domain;
	void *domain_data;
	unsigned domain_data_size;
} *attribute_t;

int
attribute_init();

int
attribute_register(short rel, attribute_t attribute, unsigned *out_id);

int
attribute_create(short rel, const char *name, unsigned domain, void *domain_data, unsigned domain_data_size,
				 char nullable, unsigned *out_id);

struct record_s *
attribute_record_create(short rel, const char *name, unsigned domain, void *domain_data, unsigned domain_data_size,
						char nullable);
