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
  string_t string;
  struct name_ast_node_s *name;
  struct name_list_ast_node_s *list;
  struct from_ast_node_s *from;
  struct select_ast_node_s *select;
  ast_statement_t statement;
  ast_statements_t statements;
}

%token SELECT
%token FROM
%token BR_OPEN BR_CLOSE
%token COMMA
%token ID
%token EOL
%token SEMICOLON
%token STRING

%type <name> name
%type <statements> statements
%type <statement> statement
%type <select> select_stmt
%type <list> select_items
%type <name> select_item
%type <from> from_item


%%
statements
	: statement SEMICOLON			{ $$ = ast_statements_add(NULL, $1); callback($1, user); };
	| statements statement SEMICOLON	{ $$ = ast_statements_add($1, $2); callback($2, user); };
	;
	
statement
	: select_stmt				{ $$ = ast_statement_select($1); };
	;

select_stmt
	: SELECT select_items FROM from_item	{ $$ = ast_select($2, $4); };

name
	: ID					{ $$ = yylval.name; };
	| STRING				{ $$ = ast_name_from_string(yylval.string); };

select_item
	: name					{ $$ = $1; };

from_item
	: name					{ $$ = ast_from($1); };

select_items
  : BR_OPEN select_items BR_CLOSE	{ $$ = $2; };
  | select_item				{ $$ = ast_name_list_add(NULL, yylval.name); };
  | select_items COMMA select_item	{ $$ = ast_name_list_add($1, $3); };
%%
