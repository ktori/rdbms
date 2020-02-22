%{
#include "ast.h"

#include <stdio.h>

int yylex();
int yyerror(char *s);
%}

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
	: statement SEMICOLON			{ $$ = ast_statements_add(NULL, $1); ast_print($1); };
	| statements statement			{ $$ = ast_statements_add($1, $2); ast_print($2); };
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

int yyerror(char *s)
{
  fprintf(stderr, "error: %s\n", s);
}