rename parser.h parser.tab.c
bison -d parser.y
rename parser.tab.c parser.h