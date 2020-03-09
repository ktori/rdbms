%code requires {
#include "ast/ast.h"
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
  fprintf(stderr, "error: %s (%s)\n", s, yyget_text(scanner));
  return 0;
}
}

%define api.pure full
%lex-param   { yyscan_t scanner }
%parse-param { yyscan_t scanner }
%parse-param { ast_callback_t callback }
%parse-param { void *user }

%token T_ASTERISK
%token T_DOT

%token <operator> OPERATOR
%token SELECT
%token FROM
%token BR_OPEN BR_CLOSE
%token COMMA
%token <name> ID
%token EOL
%token SEMICOLON
%token STRING
%token STRING_VALUE
%token <intval> INT_VALUE
%token <fval> FLOAT_VALUE

%token AS

%token INNER
%token LEFT
%token RIGHT
%token FULL
%token OUTER
%token JOIN
%token ON

%token CREATE
%token TABLE

%token INSERT
%token INTO
%token VALUES
%token WHERE

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

%token TRUE
%token FALSE

%type <operator> operator
%type <name> name
%type <statement> statement
%type <select> select_stmt
%type <select_list> select_items
%type <select_item> select_item
%type <from> from_item
%type <domain> column_type

%type <list> insert_columns
%type <column_def> column_def
%type <column_defs> column_defs
%type <column_defs> table_def
%type <intval> opt_nullable
%type <statement> create_stmt
%type <create_table> create_table_stmt
%type <literal> value
%type <insert_tuple> insert_tuple
%type <insert_tuple> value_list
%type <statement> insert_stmt
%type <condition> opt_where
%type <condition> condition

%type <select_item> select_expression

%type <join_type> join_type
%type <from> from_expression
%type <join> join

%type <table_name> table_name
%type <column_name> column_name

%destructor { ast_name_free($<name>$); }    ID
%destructor { ast_name_free($$); }          <name>
%destructor { ast_name_list_free($$); }     <list>
%destructor { ast_from_free(&$$); }          <from>
%destructor { ast_select_free($$); }        <select>
%destructor { ast_statement_free($$); }     <statement>
%destructor { ast_create_table_free($$); }  <create_table>
%destructor { ast_column_defs_free($$); }   <column_defs>
%destructor { ast_column_def_free($$); }    <column_def>

%%
statements
  : statement SEMICOLON             { callback($1, user); }
  | statements statement SEMICOLON  { callback($2, user); }
  ;

statement
  : select_stmt                 { $$ = ast_statement_select($1); }
  | create_stmt                 { $$ = $1; }
  | insert_stmt                 { $$ = $1; }
  ;

/* common */

name
  : ID            { $$ = $1; }
  | STRING        { $$ = ast_name_from_string(yylval.string); }
  ;

table_name: name { $$ = ast_table_name($1->name); };

column_name
  : table_name T_DOT name { $$ = ast_column_name($1, $3->name); }
  | name                  { $$ = ast_column_name(ast_table_name(NULL), $1->name); };

/* SELECT statement */

select_stmt
  : SELECT select_items FROM from_expression opt_where  { $$ = ast_select($2, $4, $5); }
  ;

from_expression
  : table_name            { $$ = ast_from($1); }
  | from_expression join  { ast_from_join(&$1, $2); $$ = $1; }
  ;

opt_where
  : /* none */                  { $$ = NULL; }
  | WHERE condition             { $$ = $2; }
  ;

select_items
  : BR_OPEN select_items BR_CLOSE         { $$ = $2; }
  | select_expression                     { $$ = ast_select_value_list_add(NULL, $1); }
  | select_items COMMA select_expression  { $$ = ast_select_value_list_add($1, $3); }
  ;

join: join_type JOIN table_name ON condition { $$ = ast_join($1, $3, $5); };

join_type
  : /* none */  { $$ = AST_JOIN_INNER; }
  | LEFT        { $$ = AST_JOIN_LEFT; }
  | RIGHT       { $$ = AST_JOIN_RIGHT; }
  | FULL OUTER  { $$ = AST_JOIN_FULL; }
  | INNER       { $$ = AST_JOIN_INNER; }
  ;

select_expression
  : select_item         { $$ = $1; }
  | select_item AS name { $$ = $1; };

select_item
  : column_name                 { $$ = ast_select_value_column($1); }
  | T_ASTERISK                  { $$ = ast_select_value_asterisk(); }
  | table_name T_DOT T_ASTERISK { $$ = ast_select_value_asterisk(); }
  ;

/* INSERT statement */

condition
  : name operator value         { $$ = ast_condition($1, $2, $3); }
  ;

operator: OPERATOR      { $$ = $1; };

/* CREATE statement */

create_stmt
  : create_table_stmt   { $$ = ast_statement_create_table($1); };
  ;

insert_stmt
    : INSERT INTO name insert_columns VALUES insert_tuple     { $$ = ast_statement_insert($3, $4, $6); };
    ;

insert_tuple
    : BR_OPEN value_list BR_CLOSE   { $$ = $2; };
    ;

value_list
    : value                     { $$ = ast_insert_tuple_add(NULL, $1); }
    | value_list COMMA value    { $$ = ast_insert_tuple_add($1, $3); }
    ;

value
    : NULL_T        { $$ = ast_literal_null(); }
    | TRUE          { $$ = ast_literal_bool(1); }
    | FALSE         { $$ = ast_literal_bool(0); }
    | STRING_VALUE  { $$ = ast_literal_string(yylval.string); }
    | INT_VALUE     { $$ = ast_literal_int(yylval.intval); }
    | FLOAT_VALUE   { $$ = ast_literal_real(yylval.fval); }
    ;

create_table_stmt
  : CREATE TABLE name table_def    { $$ = ast_create_table($3, $4); };
  ;

table_def: BR_OPEN column_defs BR_CLOSE    { $$ = $2; };

column_defs
  : column_def                      { $$ = ast_add_column_def(NULL, $1); };
  | column_defs COMMA column_def    { $$ = ast_add_column_def($1, $3); };
  ;

column_def
  : name column_type opt_nullable    { $$ = ast_column_def($1, $2, $3); };
  ;

column_type
  : BLOB              { $$ = AD_BLOB; }
  | BOOLEAN           { $$ = AD_BOOLEAN; }
  | CHAR              { $$ = AD_CHAR; }
  | VARCHAR           { $$ = AD_VARCHAR; }
  | BYTE              { $$ = AD_BYTE; }
  | SMALLINT          { $$ = AD_SMALL_INTEGER; }
  | INTEGER           { $$ = AD_INTEGER; }
  | REAL              { $$ = AD_REAL; }
  ;

opt_nullable
  :                   { $$ = 1; }
  | NOT NULL_T        { $$ = 0; };

insert_columns
  : BR_OPEN insert_columns BR_CLOSE { $$ = $2; }
  | name                            { $$ = ast_name_list_add(NULL, $1); }
  | insert_columns COMMA name       { $$ = ast_name_list_add($1, $3); };

%%
