/*
 * Created by victoria on 24.02.20.
*/

#pragma once

#include "shared.h"
#include "table/domain_enum.h"

typedef struct ast_literal_s
{
	union
	{
		char bool_val;
		short smallint_val;
		int int_val;
		char *string_val;
		float real_val;
	} value;
	enum attribute_domain domain;
} ast_literal_t;

ast_literal_t
ast_literal_null();

ast_literal_t
ast_literal_bool(int value);

ast_literal_t
ast_literal_int(int value);

ast_literal_t
ast_literal_string(string_t string);

void
ast_literal_free(ast_literal_t node);

ast_literal_t
ast_literal_real(float value);
