/*
 * Created by victoria on 23.02.20.
*/

#pragma once

#include <stdio.h>

struct ast_statement_s;

int
execute(struct ast_statement_s *statement, FILE *sockf);
