/*
 * Created by victoria on 09.03.20.
*/

#pragma once

#include <stdio.h>

struct select_ast_node_s;

int
execute_select(struct select_ast_node_s *select, FILE *sockf);
