/*
 * Created by victoria on 24.02.20.
*/

#include "literal.h"
#include "shared.h"

ast_literal_t
ast_literal_null()
{
	ast_literal_t result = {0};
	return result;
}

ast_literal_t
ast_literal_bool(int value)
{
	ast_literal_t result = {0};
	result.domain = AD_BOOLEAN;
	result.value.bool_val = value ? 1 : 0;
	return result;
}

ast_literal_t
ast_literal_int(int value)
{
	ast_literal_t result = {0};
	result.domain = AD_INTEGER;
	result.value.int_val = value;
	return result;
}

ast_literal_t
ast_literal_string(string_t string)
{
	ast_literal_t result = {0};
	result.domain = AD_VARCHAR;
	result.value.string_val = string->buffer;
	return result;
}

void
ast_literal_free(ast_literal_t node)
{

}

ast_literal_t
ast_literal_real(float value)
{
	ast_literal_t result = {0};
	result.value.real_val = value;
	result.domain = AD_REAL;
	return result;
}
