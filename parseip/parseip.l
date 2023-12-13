%option nodefault noyywrap noinput nounput

%{
#include "parseip.tab.h"
// Each time a rule is matched, advance the end cursor/position.
#define YY_USER_ACTION                          \
  yylloc->last_column += (int) yyleng;

  // Move the first position onto the last.
#define LOCATION_STEP()                         \
  do {                                          \
    yylloc->first_line = yylloc->last_line;     \
    yylloc->first_column = yylloc->last_column; \
  } while (0)
%}

%%
%{
    LOCATION_STEP ();
%}
[0-9]+  { 
    unsigned int n = atoi(yytext);
    yylval->num = n; 
    return TOK_NUMBER; 
}
"."     { 
    return TOK_DOT; 
}
\n      { 
    yylloc->last_line++; 
    yylloc->last_column = 0; 
    return TOK_EOL;
}

[ \t]+  {
    LOCATION_STEP(); 
    continue;
}

.   { 
    // printf("Invalid charact %c\n", *yytext); 
    yyerror(yylloc, nerrs, "Invalid character %c\n", *yytext);
    break;
}
%%