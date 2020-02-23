%{
#include "socket_input.h"
#include "ast.h"
#include "parser.h"
%}

%option caseless
%option reentrant
%option bison-bridge
%option noyywrap nounput noinput
%option extra-type="yy_extra_t"

%x QUOTED_STRING

%%

SELECT			{ return SELECT; }
FROM			{ return FROM; }
CREATE			{ return CREATE; }
TABLE			{ return TABLE; }

BLOB			{ return BLOB; }
BOOL(EAN)?		{ return BOOLEAN; }
CHAR(ACTER)?		{ return CHAR; }
VARCHAR(ACTER)?		{ return VARCHAR; }
BYTE			{ return BYTE; }
SMALLINT		{ return SMALLINT; }
INT(EGER)?		{ return INTEGER; }
REAL			{ return REAL; }

NOT			{ return NOT; }
NULL			{ return NULL_T; }

[ \t\n]			{}
\"			{ yyextra->string_length = 1; yylval->string = string_create(); yymore(); BEGIN(QUOTED_STRING); }
<QUOTED_STRING>\"	{ string_append(yylval->string, 0); BEGIN(INITIAL); return STRING; }
<QUOTED_STRING>.	{ string_append(yylval->string, yytext[yyextra->string_length++]); yymore(); }
";"			{ return SEMICOLON; }
[a-z_0-9]+		{ yylval->name = ast_name(yytext); return ID; }
"("			{ return BR_OPEN; }
","			{ return COMMA; }
")"			{ return BR_CLOSE; }
"\0"        { yyterminate(); }
<<EOF>>     { yyterminate(); }
.			{ printf("unexpected %c\n", *yytext); }
%%
