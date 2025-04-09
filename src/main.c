/*-------------------------------------------------------------------------------------------------/
 *  Main for a C- Compiler
 *  File: main.c
 *---------------------------------*/

#include "globals.h"
#include "utils.h"
#include "parser.tab.h"

int main(void) {
    inputSelect();
    
    // yydebug = 1;
    yyparse();

    return 0;
}