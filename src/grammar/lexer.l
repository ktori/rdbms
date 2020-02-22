%{
#include "ast.h"
#include "parser.h"

unsigned string_offset = 0;
%}

%option caseless
%x QUOTED_STRING

%%
[ \t\n]			{}
\"			{ string_offset = 1; yylval.string = string_create(); yymore(); BEGIN(QUOTED_STRING); }
<QUOTED_STRING>\"	{ string_append(yylval.string, 0); BEGIN(INITIAL); return STRING; }
<QUOTED_STRING>.	{ string_append(yylval.string, yytext[string_offset++]); yymore(); }
";"			{ return SEMICOLON; }
"select"		{ return SELECT; }
"from"			{ return FROM; }
[a-z_0-9]+		{ yylval.name = ast_name(yytext); return ID; }
"("			{ return BR_OPEN; }
","			{ return COMMA; }
")"			{ return BR_CLOSE; }
.			{ printf("unexpected %c\n", *yytext); }
%%
