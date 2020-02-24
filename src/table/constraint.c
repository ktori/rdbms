/*
 * Created by victoria on 24.02.20.
*/

#include <storage/storage.h>
#include "constraint.h"
#include "relation.h"
#include "domain_enum.h"

static record_def_t sys_constraint_record_def_ptr = NULL;

record_def_t
sys_constraint_record_def()
{
	unsigned attr_id;

	if (sys_constraint_record_def_ptr)
		return sys_constraint_record_def_ptr;

	sys_constraint_record_def_ptr = record_def_create();

	attribute_create(SYS_REL_CONSTRAINT, "type", AD_BYTE, NULL, 0, 0, &attr_id);
	record_def_add_attribute(sys_constraint_record_def_ptr, attr_id);
	attribute_create(SYS_REL_CONSTRAINT, "rel", AD_SMALL_INTEGER, NULL, 0, 0, &attr_id);
	record_def_add_attribute(sys_constraint_record_def_ptr, attr_id);
	attribute_create(SYS_REL_CONSTRAINT, "f_rel", AD_SMALL_INTEGER, NULL, 0, 1, &attr_id);
	record_def_add_attribute(sys_constraint_record_def_ptr, attr_id);
	attribute_create(SYS_REL_CONSTRAINT, "attr", AD_SMALL_INTEGER, NULL, 0, 0, &attr_id);
	record_def_add_attribute(sys_constraint_record_def_ptr, attr_id);
	attribute_create(SYS_REL_CONSTRAINT, "f_attr", AD_SMALL_INTEGER, NULL, 0, 1, &attr_id);
	record_def_add_attribute(sys_constraint_record_def_ptr, attr_id);

	return sys_constraint_record_def_ptr;
}

int
constraint_init()
{
	rel_create(SYS_REL_CONSTRAINT, "sys_constraint", sys_constraint_record_def());

	return 0;
}

void
constraint_shutdown()
{
}

int
constraint_foreign_key(const char *name, short rel, short attr, short f_rel, short f_attr)
{
	record_t record = record_create(sys_constraint_record_def());
	char ch_value = CON_FOREIGN_KEY;
	int status;

	record->values[0] = record_value_str(name);
	record->values[1] = record_value_from(&ch_value, sizeof(ch_value));
	record->values[2] = record_value_from(&rel, sizeof(rel));
	record->values[3] = record_value_from(&f_rel, sizeof(f_rel));
	record->values[4] = record_value_from(&attr, sizeof(attr));
	record->values[5] = record_value_from(&f_attr, sizeof(attr));

	status = store_insert(SYS_REL_CONSTRAINT, NULL, record);

	record_free(&record);

	return status;
}

int
constraint_primary_key(const char *name, short rel, short attr)
{
	record_t record = record_create(sys_constraint_record_def());
	char ch_value = CON_PRIMARY_KEY;
	int status;

	record->values[0] = record_value_str(name);
	record->values[1] = record_value_from(&ch_value, sizeof(ch_value));
	record->values[2] = record_value_from(&rel, sizeof(rel));
	record->values[4] = record_value_from(&attr, sizeof(attr));

	status = store_insert(SYS_REL_CONSTRAINT, NULL, record);

	record_free(&record);

	return status;
}

int
constraint_unique(const char *name, short rel, short attr)
{
	record_t record = record_create(sys_constraint_record_def());
	char ch_value = CON_UNIQUE;
	int status;

	record->values[0] = record_value_str(name);
	record->values[1] = record_value_from(&ch_value, sizeof(ch_value));
	record->values[2] = record_value_from(&rel, sizeof(rel));
	record->values[4] = record_value_from(&attr, sizeof(attr));

	status = store_insert(SYS_REL_CONSTRAINT, NULL, record);

	record_free(&record);

	return status;
}
