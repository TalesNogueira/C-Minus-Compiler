/*-------------------------------------------------------------------------------------------------/
 *  Global interface for a C- Compiler
 *  File: globals.h
 *---------------------------------*/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

/*--------------------------------------------/
 *  Flex Variables
 *---------------------------------*/

/*  yyin → Flex's input filepath  */
extern FILE* yyin;
/*  yylineno → Real time Flex's line number   */
extern int yylineno;
/* getToken() → Call yylex(), treat the TOKEN and then return ---> Traceable   */
extern int getToken(void);
 
/*--------------------------------------------/
  *  YACC-Bison Variables
  *---------------------------------*/

/*  parse() → Call yyparse() - Syntax Analysis - and build the AST ---> Traceable    */
extern void parse(void);

/*--------------------------------------------/
 *  Global Significant Variables
 *---------------------------------*/

/*  INPUT_DIR → Inputs folder name */
extern char *INPUT_DIR;

/*--------------------------------------------/
 *  Tracing Flags
 *---------------------------------*/

/* TraceScan → Trace Flex's scanner getToken() - yylex() - executions and prints it out   */
extern bool TraceScan;
/* TraceParse → Trace Bison's parser Abstract Syntax Tree (AST) and prints it out   */
extern bool TraceParse;

/*--------------------------------------------/
 *  Abstract Syntax Tree (AST) and related structures
 *---------------------------------*/

 typedef enum { 
    Void, Integer, Boolean
} ExpType;

#define MAXCHILDREN 3

typedef enum { DeclarationK, StatementK, ExpressionK } NodeKind;

typedef enum { 
    VariableK, FunctionK, ParameterK 
} DeclKind;

typedef enum { 
    AssignK, CompoundK, IfK, WhileK, ReturnK
} StmtKind;

typedef enum { 
    OperatorK, ConstK, IdK, CallK 
} ExpKind;

/*  (struct treeNode) TreeNode → Standard tree node structure for constructing the Abstract Syntax Tree   */
typedef struct treeNode {
    struct treeNode *child[MAXCHILDREN];
    struct treeNode *sibling;
    
    int lineno;
    
    NodeKind nodekind;
    
    union {
        DeclKind decl;
        StmtKind stmt;
        ExpKind exp;
    } kind;

    union {
        char *name;
        int value;
        int operator;

        struct {
            char *name;
            int size;
        }arrayAttr;
    } attr;

    struct {
        bool isArray;
    } flags;

    ExpType type;
} TreeNode;

/*  AbstractSyntaxTree → Pointer to TreeNode base of Abstract Syntax Tree   */
extern TreeNode *abstractSyntaxTree;

#endif