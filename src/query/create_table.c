/*
 * Created by victoria on 24.02.20.
*/

#include <table/record.h>
#include <table/relation.h>
#include <ast/ast.h>

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

static record_def_t
record_def_from_ast(short rel, ast_column_defs_node_t defs)
{
	attribute_t attributes = calloc(defs->count, sizeof(struct attribute_s));
	unsigned i;

	for (i = 0; i < defs->count; ++i)
	{
		attributes[i].name = defs->array[i]->name->name;
		attributes[i].nullable = defs->array[i]->nullable ? 1 : 0;
		attributes[i].domain = defs->array[i]->domain;
	}

	return record_def_create_from(rel, attributes, defs->count);
}

int
execute_create_table(ast_create_table_node_t ast, FILE *sockf)
{
	short rel_id = rel_alloc();
	record_def_t def = record_def_from_ast(rel_id, ast->columns);
	if (def == NULL)
	{
		fprintf(stderr, "record_def null\n");
		return EXIT_FAILURE;
	}
	printf("creating table '%s'...\n", ast->name->name);
	rel_create(rel_id, ast->name->name, def);
	return 0;
}
