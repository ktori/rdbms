%{
#include "ast.h"

#include <stdio.h>

int yylex();
int yyerror(char *s);
%}

%union
{
  struct name_ast_node_s *name;
  struct name_list_ast_node_s *list;
  struct from_ast_node_s *from;
  struct select_ast_node_s *select;
}

%token SELECT
%token FROM
%token BR_OPEN BR_CLOSE
%token COMMA
%token ID
%token EOL

%type <name> name
%type <select> select
%type <from> from
%type <list> columns
%type <list> column_list

%%
select: SELECT columns from EOL { $$ = ast_select($2, $3); select_print($$); };

name: ID                      { $$ = yylval.name; }

from: FROM name               { $$ = ast_from($2); };

columns
  : BR_OPEN column_list BR_CLOSE  { $$ = $2; };
  | column_list                   { $$ = $1; };

column_list
  : ID                        { $$ = ast_name_list_add(NULL, yylval.name); }
  | column_list COMMA name    { $$ = ast_name_list_add($1, $3); }
  ;
%%

int yyerror(char *s)
{
  fprintf(stderr, "error: %s\n", s);
}