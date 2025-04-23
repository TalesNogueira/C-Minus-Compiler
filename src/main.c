/*-------------------------------------------------------------------------------------------------/
 *  Main for a C- Compiler
 *  File: main.c
 *---------------------------------*/

#include "globals.h"
#include "utils.h"

/*--------------------------------------------/
 *  Allocate and Set → Global Significant Variables
 *---------------------------------*/

 /* INPUT_DIR → Inputs folder name  */
 char *INPUT_DIR = "inputs";

/*--------------------------------------------/
 *  Allocate and Set → Tracing Flags
 *---------------------------------*/

 /* TraceScan → Trace Flex's scanner getToken() - yylex() - executions and prints it out   */
 bool TraceScan = true;
 /* TraceParse → Trace Bison's parser Abstract Syntax Tree (AST) and prints it out   */
 bool TraceParse = true;

int main(void) {
    inputSelect();
    
    parse();

    printf("\n> End of compilation.\n");
    return 0;
}