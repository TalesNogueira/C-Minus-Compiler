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
 /* TraceSemantic → Trace Semantic's Analyzer and it's Symbol Table; and prints it out   */
 bool TraceSemantic = true;
 /* TraceMidCode → Trace Intermediate "Mid" Code Generator and it's Quadruples List; and prints it out   */
 bool TraceMidCode = true;

int main(void) {
    inputSelect();
    
 /* lexicalAnalysis(); ← Syntax Analysis includes it    */
    syntaxAnalysis();
    semanticAnalysis(abstractSyntaxTree);

    midCodeGenerate(abstractSyntaxTree);

    printf("\n> End of compilation → [%s]\n", source);
    return 0;
}