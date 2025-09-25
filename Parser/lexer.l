%{
#include "parser.tab.h"
#include <stdio.h>
#include "./Tools/Vertex.h"
#include "./Tools/Color.h"
#include "./Tools/Integer.h"
#include "./Tools/Float.h"
int yylineno = 1;   // define here
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
CLR              { return CLR; }
VX               { return VX; }
FLT              { return FLT; }
INTEGER              { return INTEGER; }
":"              { return ':'; }
","              { return ','; }
"("              { return '('; }
")"              { return ')'; }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.sval = strdup(yytext); return IDENTIFIER; }
[ \t\r\n]+       { /* skip whitespace */ }
.                { /* ignore other characters */ }
%%

int yywrap() { return 1; }