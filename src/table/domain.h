/*
 * Created by victoria on 23.02.20.
*/

#pragma once

struct domain_s
{
	char *name;
};

/*
 *	CREATE TABLE sys_domain
 *	(
 *		id		SMALLINT NOT NULL,
 *		name	VARCHAR NOT NULL
 *	);
 */
int
domain_init();
