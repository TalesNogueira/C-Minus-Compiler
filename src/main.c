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

 /* TraceScan → Trace Flex's scanner getToken() - yylex() - executions; and prints it out   */
 bool TraceScan = true;
 /* TraceParse → Trace Bison's parser Abstract Syntax Tree (AST); and prints it out   */
 bool TraceParse = true;
 /* TraceSemantic → Trace Semantic's Analyzer and its Symbol Table; and prints it out   */
 bool TraceSemantic = true;

int main(void) {
    inputSelect();
    
    syntaxAnalysis();
    semanticAnalysis(abstractSyntaxTree);

    printf("\n> End of compilation → %s\n", source);
    return 0;
}