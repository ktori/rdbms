/*
 * Created by victoria on 23.02.20.
*/

#include "strdup.h"

#include <malloc.h>
#include <string.h>

char *
strdup(const char *string)
{
	char *result = malloc(strlen(string) + 1);
	strcpy(result, string);
	return result;
}
