%{
#include "ast.h"
#include "parser.h"
%}

%option caseless

%%
[ \t]           {}
\n              { return EOL; }
"select"        { return SELECT; }
"from"          { return FROM; }
[a-z_0-9]+      { yylval.name = ast_name(yytext); return ID; }
"("             { return BR_OPEN; }
","             { return COMMA; }
")"             { return BR_CLOSE; }
.               { printf("unexpected %c\n", *yytext); }
%%
