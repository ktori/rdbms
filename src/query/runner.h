/*
 * Created by victoria on 23.02.20.
*/

#pragma once

#include <stdio.h>

struct ast_statement_s;
struct ast_create_table_s;
struct ast_insert_s;

int
execute(struct ast_statement_s *statement, FILE *sockf);

int
execute_create_table(struct ast_create_table_s *ast, FILE *sockf);

int
execute_insert(struct ast_insert_s *insert, FILE *sockf);
