/*-------------------------------------------------------------------------------------------------/
 *  Global Header for a C- Compiler
 *  File: globals.h
 *---------------------------------*/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define INPUT_DIR "inputs"

extern FILE* yyin;
extern int yylineno;
extern char* yytext;
extern int yylex(void);

extern int yydebug;
extern int yyparse(void);

#endif