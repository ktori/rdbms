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

struct attribute_s
{
	char *name;
	int nullable;
	unsigned domain;
	void *domain_data;
};

int
attribute_init();
