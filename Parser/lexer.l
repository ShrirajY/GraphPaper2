%{
#include "parser.tab.h"
%}

%%
"//".*           { /* skip comment lines */ }
-?[0-9]+\.[0-9]+ { yylval.fval = atof(yytext); return FLOAT_NUM; }
-?[0-9]+         { yylval.ival = atoi(yytext); return NUMBER; }
LINE             { return LINE; }
CIRCLE           { return CIRCLE; }
ELLIPSE          { return ELLIPSE; }
PARABOLA         { return PARABOLA; }
FILLCOLOR        { return FILLCOLOR; }
":"              { return ':'; }
","              { return ','; }
"("              { return '('; }
")"              { return ')'; }
[ \t\r\n]+       { /* skip whitespace */ }
.                { /* ignore other characters */ }
%%

int yywrap() { return 1; }