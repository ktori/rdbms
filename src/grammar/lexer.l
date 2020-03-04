%{
#include "socket_input.h"
#include "ast/ast.h"
#include "parser.h"
%}

%option caseless
%option reentrant
%option bison-bridge
%option noyywrap nounput noinput
%option extra-type="yy_extra_t"

%x QUOTED_STRING
%x QUOTED_STRING_VALUE

%%

SELECT          { return SELECT; }
FROM            { return FROM; }
CREATE          { return CREATE; }
TABLE           { return TABLE; }
INSERT          { return INSERT; }
INTO            { return INTO; }
VALUES          { return VALUES; }
WHERE           { return WHERE; }

BLOB            { return BLOB; }
BOOL(EAN)?      { return BOOLEAN; }
CHAR(ACTER)?    { return CHAR; }
VARCHAR(ACTER)? { return VARCHAR; }
BYTE            { return BYTE; }
SMALLINT        { return SMALLINT; }
INT(EGER)?      { return INTEGER; }
REAL            { return REAL; }

TRUE            { return TRUE; }
FALSE           { return FALSE; }

NOT  { return NOT; }
NULL { return NULL_T; }

"<="        { yylval->operator = AST_OP_LE; return OPERATOR; }
">="        { yylval->operator = AST_OP_GE; return OPERATOR; }
"<"         { yylval->operator = AST_OP_LT; return OPERATOR; }
">"         { yylval->operator = AST_OP_GT; return OPERATOR; }
"="         { yylval->operator = AST_OP_EQ; return OPERATOR; }

"-"?[0-9]+"."[0-9]+"F"?     { yylval->fval = atof(yytext); return FLOAT_VALUE; }

[ \t\n]     {}

\"                  { yyextra->string_length = 1; yylval->string = string_create(); yymore(); BEGIN(QUOTED_STRING); }
<QUOTED_STRING>\"   { string_append(yylval->string, 0); BEGIN(INITIAL); return STRING; }
<QUOTED_STRING>.    { string_append(yylval->string, yytext[yyextra->string_length++]); yymore(); }

\'                          { yyextra->string_length = 1; yylval->string = string_create(); yymore(); BEGIN(QUOTED_STRING_VALUE); }
<QUOTED_STRING_VALUE>\'     { string_append(yylval->string, 0); BEGIN(INITIAL); return STRING_VALUE; }
<QUOTED_STRING_VALUE>.      { string_append(yylval->string, yytext[yyextra->string_length++]); yymore(); }

"-"?[1-9][0-9]*      { yylval->intval = atoi(yytext); return INT_VALUE; }

"*"         { return T_ASTERISK; }
";"         { return SEMICOLON; }
[a-z_0-9]+  { yylval->name = ast_name(yytext); return ID; }
"("         { return BR_OPEN; }
","         { return COMMA; }
")"         { return BR_CLOSE; }
"\0"        { yyterminate(); }
<<EOF>>     { yyterminate(); }
.           { printf("unexpected %c\n", *yytext); }
%%
