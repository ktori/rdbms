%code requires {
#include "ast.h"
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
typedef struct yy_extra_s
{
    unsigned string_length;
    int socket_fd;
} *yy_extra_t;
}

%code {
#include "lexer.h"
#include <stdio.h>
int yyerror(yyscan_t scanner, ast_callback_t callback, void *user, char *s)
{
  fprintf(stderr, "error: %s\n", s);
  return 0;
}
}

%define api.pure full
%lex-param   { yyscan_t scanner }
%parse-param { yyscan_t scanner }
%parse-param { ast_callback_t callback }
%parse-param { void *user }

%union
{
  int intval;
  string_t string;
  ast_name_node_t name;
  ast_name_list_node_t list;
  ast_from_node_t from;
  ast_select_node_t select;
  ast_statement_node_t statement;
  ast_statements_t statements;
  enum attribute_domain domain;
  ast_column_def_node_t column_def;
  ast_column_defs_node_t column_defs;
  ast_create_table_node_t create_table;
}

%token SELECT
%token FROM
%token BR_OPEN BR_CLOSE
%token COMMA
%token ID
%token EOL
%token SEMICOLON
%token STRING

%token CREATE
%token TABLE

%token NOT
%token NULL_T

%token BOOLEAN
%token BYTE
%token SMALLINT
%token INTEGER
%token CHAR
%token VARCHAR
%token BLOB
%token REAL

%type <name> name
%type <statements> statements
%type <statement> statement
%type <select> select_stmt
%type <list> select_items
%type <name> select_item
%type <from> from_item
%type <domain> column_type

%type <column_def> column_def
%type <column_defs> column_defs
%type <column_defs> table_def
%type <intval> opt_nullable
%type <statement> create_stmt
%type <create_table> create_table_stmt

%destructor { ast_name_free($<name>$); }		ID
%destructor { ast_name_free($$); }		<name>
%destructor { ast_name_list_free($$); }		<list>
%destructor { ast_from_free($$); }		<from>
%destructor { ast_select_free($$); }		<select>
%destructor { ast_statement_free($$); }		<statement>
%destructor { ast_statements_free($$); }	<statements>
%destructor { ast_create_table_free($$); } <create_table>
%destructor { ast_column_defs_free($$); } <column_defs>
%destructor { ast_column_def_free($$); } <column_def>

%%
statements
	: statement SEMICOLON			{ $$ = ast_statements_add(NULL, $1); callback($1, user); };
	| statements statement SEMICOLON	{ $$ = ast_statements_add($1, $2); callback($2, user); };
	;
	
statement
	: select_stmt				{ $$ = ast_statement_select($1); };
	| create_stmt				{ $$ = $1; };
	;

select_stmt
	: SELECT select_items FROM from_item	{ $$ = ast_select($2, $4); };

create_stmt
	: create_table_stmt			{ $$ = ast_statement_create_table($1); };
	;
	
create_table_stmt
	: CREATE TABLE name table_def		{ $$ = ast_create_table($3, $4); };
	;
	
table_def: BR_OPEN column_defs BR_CLOSE		{ $$ = $2; };
	
column_defs
	: column_def				{ $$ = ast_add_column_def(NULL, $1); };
	| column_defs COMMA column_def		{ $$ = ast_add_column_def($1, $3); };
	;
	
column_def
	: name column_type opt_nullable		{ $$ = ast_column_def($1, $2, $3); };
	;

column_type
	: BLOB					{ $$ = AD_BLOB; };
	| BOOLEAN				{ $$ = AD_BOOLEAN; };
	| CHAR					{ $$ = AD_CHAR; };
	| VARCHAR				{ $$ = AD_VARCHAR; };
	| BYTE					{ $$ = AD_BYTE; };
	| SMALLINT				{ $$ = AD_SMALL_INTEGER; };
	| INTEGER				{ $$ = AD_INTEGER; }; 
	| REAL					{ $$ = AD_REAL; };
	;
	
opt_nullable
	:					{ $$ = 0; };
	| NOT NULL_T				{ $$ = 1; };
	;

name
	: ID					{ $$ = yylval.name; };
	| STRING				{ $$ = ast_name_from_string(yylval.string); };

select_item
	: name					{ $$ = $1; };

from_item
	: name					{ $$ = ast_from($1); };

select_items
  : BR_OPEN select_items BR_CLOSE	{ $$ = $2; };
  | select_item				{ $$ = ast_name_list_add(NULL, $1); };
  | select_items COMMA select_item	{ $$ = ast_name_list_add($1, $3); };
%%
