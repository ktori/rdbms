%{
#include "ast.h"
#include "parser.h"
%}

%option caseless
%option reentrant
%option bison-bridge
%option noyywrap nounput noinput
%option extra-type="unsigned *"

%x QUOTED_STRING

%%
[ \t\n]			{}
\"			{ *yyextra = 1; yylval->string = string_create(); yymore(); BEGIN(QUOTED_STRING); }
<QUOTED_STRING>\"	{ string_append(yylval->string, 0); BEGIN(INITIAL); return STRING; }
<QUOTED_STRING>.	{ string_append(yylval->string, yytext[(*yyextra)++]); yymore(); }
";"			{ return SEMICOLON; }
"select"		{ return SELECT; }
"from"			{ return FROM; }
[a-z_0-9]+		{ yylval->name = ast_name(yytext); return ID; }
"("			{ return BR_OPEN; }
","			{ return COMMA; }
")"			{ return BR_CLOSE; }
"\0"        { yyterminate(); }
<<EOF>>     { yyterminate(); }
.			{ printf("unexpected %c\n", *yytext); }
%%
