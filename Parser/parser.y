%{
#include <stdio.h>
#include <cmath>
#include <windows.h>   // For MessageBox
#include "lex.yy.h"
#include "../Globals.hpp"
#include "../Circle.hpp"
#include "../Ellipse.hpp"
#include "../Line.hpp"
#include "../Parabola.hpp"
#include "../Axis.hpp"

// Tools
#include "./Tools/Vertex.h"
#include "./Tools/Color.h"
#include "./Tools/Integer.h"
#include "./Tools/Float.h"

int yylex(void);
int yyerror(const char *s);
void semantic_error(const char *msg);
extern FILE *yyin; // Declare yyin for Flex
extern int yylineno; // Declare yylineno for Flex
%}

%union {
    float fval;
    int  ival;
    char* sval;
    Vertex* vertex;
    Color* clr;
    Integer* ivalObj;
    Float* fvalObj;
}

%token LINE CIRCLE ELLIPSE PARABOLA FILLCOLOR CLR VX FLT INTEGER
%token <fval> FLOAT_NUM
%token <ival> NUMBER
%token <sval> IDENTIFIER


%type <fvalObj> float_num
%type <vertex> vx
%type <clr> color
%%

input:
      /* empty */
    | input shape
    ;


float_num:
    FLOAT_NUM  { $$ = new Float($1); }
  | NUMBER     { $$ = new Float((float)$1); }
  ;

vx:
    '(' float_num ',' float_num ')' {$$=new Vertex($2->getValue(),$4->getValue());}
    | IDENTIFIER
        {
            std::string varName($1);
            try {
                Vertex v = globalVertexSymbolTable.getVertex(varName);
                $$ = new Vertex(v.getX(), v.getY());
            } catch (const std::runtime_error&) {
                semantic_error((std::string("Undefined variable: ") + varName).c_str());
                $$ = new Vertex(0.0f, 0.0f);
            }
            free($1);
        }
    | '(' IDENTIFIER ',' IDENTIFIER ')'
        {
            std::string varNameX($2);
            std::string varNameY($4);
            float xVal, yVal;
            try {
                Float fX = globalFloatSymbolTable.getFloat(varNameX);
                xVal = fX.getValue();
            } catch (const std::runtime_error&) {
                semantic_error((std::string("Undefined variable: ") + varNameX).c_str());
                xVal = 0.0f;
            }
            try {
                Float fY = globalFloatSymbolTable.getFloat(varNameY);
                yVal = fY.getValue();
            } catch (const std::runtime_error&) {
                semantic_error((std::string("Undefined variable: ") + varNameY).c_str());
                yVal = 0.0f;
            }
            $$ = new Vertex(xVal, yVal);
            free($2);
            free($4);
        }
    | '(' IDENTIFIER ',' float_num ')'
        {
            std::string varNameX($2);
            float xVal;
            try {
                Float fX = globalFloatSymbolTable.getFloat(varNameX);
                xVal = fX.getValue();
            } catch (const std::runtime_error&) {
                semantic_error((std::string("Undefined variable: ") + varNameX).c_str());
                xVal = 0.0f;
            }
            float yVal = $4->getValue();
            $$ = new Vertex(xVal, yVal);
            free($2);
            delete $4;
        }
    | '(' float_num ',' IDENTIFIER ')'
        {
            float xVal = $2->getValue();
            std::string varNameY($4);
            float yVal;
            try {
                Float fY = globalFloatSymbolTable.getFloat(varNameY);
                yVal = fY.getValue();
            } catch (const std::runtime_error&) {
                semantic_error((std::string("Undefined variable: ") + varNameY).c_str());
                yVal = 0.0f;
            }
            $$ = new Vertex(xVal, yVal);
            delete $2;
            free($4);
        }
    ;

color:
    '(' NUMBER ',' NUMBER ',' NUMBER ')' {$$=new Color($2,$4,$6);}
    | IDENTIFIER
        {
            std::string colorName($1);
            try {
                Color c = globalColorsSymbolTable.getColor(colorName);
                $$ = new Color(c.getR(), c.getG(), c.getB());
            } catch (const std::runtime_error&) {
                semantic_error((std::string("Undefined color: ") + colorName).c_str());
                $$ = new Color(0, 0, 0);
            }
            free($1);
        }
    ;

shape:
    LINE ':' vx ',' vx ',' color
    {
        float x1=$3->getX();
        float y1=$3->getY();
        float x2=$5->getX();
        float y2=$5->getY();

        int r=$7->getR();
        int g=$7->getG();
        int b=$7->getB();
        const float EPSILON = 1e-6;
        if (fabs(x1 - x2) < EPSILON && fabs(y1 - y2) < EPSILON) {
              semantic_error("LINE startpoint and endpoint are identical");
          }
          else if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
              semantic_error("Invalid RGB values, must be in range 0-255");
          }
          else {
              Line line = Line::Create(x1,y1,x2,y2,RGB(r,g,b));
              AddLine(line);
              delete $3;
              delete $5;
              delete $7;
          }

        Line line = Line::Create(x1,y1,x2,y2,RGB(r,g,b));
        AddLine(line);
    }

  | CIRCLE ':' vx ',' float_num ',' color
  {
        float centerX = $3->getX();
        float centerY = $3->getY();
        float radius = $5->getValue();
        int r = $7->getR();
        int g = $7->getG();
        int b = $7->getB();
        const float EPSILON = 1e-6;
        if (radius <= 0) {
            semantic_error("CIRCLE radius must be positive");
        }
        else if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
            semantic_error("Invalid RGB values, must be in range 0-255");
        }
        else {
            Circle circle = Circle::Create(centerX, centerY, radius, RGB(r, g, b));
            AddCircle(circle);
            delete $3;
            delete $5;
            delete $7;
        }
  }
  | ELLIPSE ':' vx ',' float_num ',' float_num ',' float_num ',' color
  {
        float centerX = $3->getX();
        float centerY = $3->getY();
        float majorAxis = $5->getValue();
        float minorAxis = $7->getValue();
        float rotation = $9->getValue();
        int r = $11->getR();
        int g = $11->getG();
        int b = $11->getB();
        const float EPSILON = 1e-6;
        if (majorAxis <= 0 || minorAxis <= 0) {
            semantic_error("ELLIPSE axes must be positive");
        }
        else if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
            semantic_error("Invalid RGB values, must be in range 0-255");
        }
        else {
            Ellipse_ ellipse = Ellipse_::Create(centerX, centerY, majorAxis, minorAxis, rotation, RGB(r, g, b));
            AddEllipse(ellipse);
            delete $3;
            delete $5;
            delete $7;
            delete $9;
            delete $11;
        }
  }

 | PARABOLA ':' vx ',' vx ',' vx ',' color
 {
        float x1 = $3->getX();
        float y1 = $3->getY();
        float x2 = $5->getX();
        float y2 = $5->getY();
        float x3 = $7->getX();
        float y3 = $7->getY();
        int r = $9->getR();
        int g = $9->getG();
        int b = $9->getB();
        const float EPSILON = 1e-6;
        if (fabs((x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1)) < EPSILON) {
            semantic_error("PARABOLA points are collinear");
        }
        else if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
            semantic_error("Invalid RGB values, must be in range 0-255");
        }
        else {
            Parabola parabola = Parabola::CreateFrom3Points(x1, y1, x2, y2, x3, y3, RGB(r, g, b));
            AddParabola(parabola);
            delete $3;
            delete $5;
            delete $7;
            delete $9;
        }
 }

  | FILLCOLOR ':' vx ',' color
        { POINT point = { (LONG)$3->getX(), (LONG)$3->getY() };
          COLORREF currColor = RGB($5->getB(), $5->getG(), $5->getR());
          std::pair<POINT, COLORREF> floodFillPoint(point, currColor);
          AddFloodFillPoint(floodFillPoint);
          delete $3;
          delete $5;
        }

  | VX IDENTIFIER ':' vx
        {
            std::string varName($2);

            try {
                Vertex v = globalVertexSymbolTable.getVertex(varName);
                // If we reach here → variable exists
                semantic_error((std::string("Variable redefinition: ") + varName).c_str());
            } catch (const std::runtime_error&) {
                // Variable does not exist → safe to add
                globalVertexSymbolTable.addVertex(varName, *$4);
                delete $4;
            }

            free($2);
        }

    | CLR IDENTIFIER ':' color
            {
                std::string colorName($2);
    
                try {
                    Color c = globalColorsSymbolTable.getColor(colorName);
                    // If we reach here → color exists
                    semantic_error((std::string("Color redefinition: ") + colorName).c_str());
                } catch (const std::invalid_argument&) {
                    // Color does not exist → safe to add
                    globalColorsSymbolTable.addColor(colorName, *$4);
                    delete $4;
                }

    
                free($2);
            }
    | FLT IDENTIFIER ':' float_num
            {
                std::string floatName($2);
    
                try {
                    Float f = globalFloatSymbolTable.getFloat(floatName);
                    // If we reach here → float exists
                    semantic_error((std::string("Float variable redefinition: ") + floatName).c_str());
                } catch (const std::invalid_argument&) {
                    // Float does not exist → safe to add
                    globalFloatSymbolTable.addFloat(floatName, *$4);
                    delete $4;
                }

    
                free($2);
            }
;

%%

void parse(char *filename) {
    globalColorsSymbolTable.clear();
    globalVertexSymbolTable.clear();
    globalFloatSymbolTable.clear();
    globalIntegerSymbolTable.clear();
    FILE *file = fopen(filename, "r");
    if (!file) {
        char buf[512];
        snprintf(buf, sizeof(buf), "Error opening file: %s", filename);
        MessageBoxA(NULL, buf, "File Error", MB_ICONERROR | MB_OK);
        return;
    }
    yyin = file;
    if (yyparse() != 0) {
        MessageBoxA(NULL, "Parsing failed. Please check the input file format.", "Parse Error", MB_ICONERROR | MB_OK);
    }
    fclose(file);
}

int yyerror(const char *s) {
    char buf[128];
    snprintf(buf, sizeof(buf), "Syntax Error at line no: %d", yylineno);
    MessageBoxA(NULL, buf, "Syntax Error", MB_ICONERROR | MB_OK);
    return 0;
}


void semantic_error(const char *msg) {
    char buf[256];
    snprintf(buf, sizeof(buf), "Error at line no: %d: %s", yylineno, msg);
    MessageBoxA(NULL, buf, "Semantic Error", MB_ICONERROR | MB_OK);
}
