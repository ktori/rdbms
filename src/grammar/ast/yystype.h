/*
 * Created by victoria on 09.03.20.
*/

#pragma once

#include "shared.h"
#include "literal.h"
#include "select.h"
#include "insert.h"
#include "create_table.h"

typedef union YYSTYPE
{
	int intval;
	float fval;
	enum ast_operator operator;
	ast_literal_t literal;
	string_t string;
	ast_name_node_t name;
	ast_name_list_node_t list;
	ast_from_expression_t from;
	ast_select_node_t select;
	ast_statement_node_t statement;
	enum attribute_domain domain;
	ast_column_def_node_t column_def;
	ast_column_defs_node_t column_defs;
	ast_create_table_node_t create_table;
	ast_insert_tuple_node_t insert_tuple;
	ast_condition_t condition;

	ast_select_value_t select_item;
	ast_select_value_list_t select_list;

	ast_join_t join;
	enum ast_join_type_enum join_type;

	ast_column_name_t 	column_name;
	ast_table_name_t 	table_name;
} YYSTYPE;

#define YYSTYPE YYSTYPE
