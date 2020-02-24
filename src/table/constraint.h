/*
 * Created by victoria on 24.02.20.
*/

#pragma once

enum constraint_type
{
	CON_PRIMARY_KEY = 1,
	CON_FOREIGN_KEY,
	CON_UNIQUE
};

/*
 *  CREATE TABLE sys_constraint
 *  (
 *  	name 	VARCHAR NOT NULL,
 * 		type 	BYTE NOT NULL,
 *		rel  	SMALLINT REFERENCES sys_relation(id),
 *		f_rel 	SMALLINT REFERENCES sys_relation(id),
 *		attr	SMALLINT,
 *		f_attr	SMALLINT
 *  );
 */
int
constraint_init();

int
constraint_foreign_key(const char *name, short rel, short attr, short f_rel, short f_attr);

int
constraint_primary_key(const char *name, short rel, short attr);

int
constraint_unique(const char *name, short rel, short attr);

void
constraint_shutdown();
