%{
#include <stdio.h>
#include "lex.yy.h"
#include "../Globals.hpp"
#include "../Circle.hpp"
#include "../Ellipse.hpp"
#include "../Line.hpp"
int yylex(void);
int yyerror(const char *s);
extern FILE *yyin; // Declare yyin for Flex
%}

%union {
    float fval;
    int  ival;
}

%token LINE CIRCLE ELLIPSE
%token <fval> FLOAT_NUM
%token <ival> NUMBER

%type <fval> num

%%

input:
    /* empty */
  | input shape
  ;

num:
    FLOAT_NUM  { $$ = $1; }
  | NUMBER { $$ = (float)$1; }
  ;

shape:
    LINE ':' num ',' num ',' num ',' num ',' '(' NUMBER ',' NUMBER ',' NUMBER ')'
        { printf("Found LINE: start=(%.2f,%.2f) end=(%.2f,%.2f) color=(%d,%d,%d)\n", $3, $5, $7, $9, $12, $14, $16);
          Line line = Line::Create($3, $5, $7, $9, RGB($12, $14, $16));
          AddLine(line);}
  | CIRCLE ':' num ',' num ',' num ',' '(' NUMBER ',' NUMBER ',' NUMBER ')'
        { printf("Found CIRCLE: center=(%.2f,%.2f) radius=%.2f color=(%d,%d,%d)\n", $3, $5, $7, $10, $12, $14);
          Circle circle = Circle::Create($3, $5, $7, RGB($10, $12, $14));
          AddCircle(circle);}
  | ELLIPSE ':' num ',' num ',' num ',' num ',' num ',' '(' NUMBER ',' NUMBER ',' NUMBER ')'
        { printf("Found ELLIPSE: center=(%.2f,%.2f) a=%.2f b=%.2f angle=%.2f color=(%d,%d,%d)\n", $3, $5, $7, $9, $11, $14, $16, $18);
          Ellipse_ ellipse = Ellipse_::Create($3, $5, $7, $9, $11, RGB($14, $16, $18));
          AddEllipse(ellipse);}
;

%%

// int main(int argc, char *argv[]) {
//     if (argc > 1) {
//         FILE *file = fopen(argv[1], "r");
//         if (!file) {
//             perror("Error opening file");
//             return 1;
//         }
//         yyin = file;
//     }
//     return yyparse();
// }

void parse(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }
    yyin = file;
    yyparse();
    fclose(file);
}

int yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
    return 0;
}