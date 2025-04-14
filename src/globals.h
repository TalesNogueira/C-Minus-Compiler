/*-------------------------------------------------------------------------------------------------/
 *  Global Header for a C- Compiler
 *  File: globals.h
 *---------------------------------*/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

#define INPUT_DIR "inputs"

// Flex Variables
extern FILE* yyin;
extern int yylineno;
extern int yylex(void);

// YACC-Bison Variables
extern int yydebug;
extern int yyparse(void);

/*--------------------------------------------/
 *  Abstract Syntax Tree (AST) structures
 *---------------------------------*/

#define MAXCHILDREN 3

typedef enum { 
    DeclarationK, StatementK, ExpressionK
} NodeKind;

typedef enum { 
    VariableK, FunctionK, ParameterK 
} DeclKind;

typedef enum { 
    IfK, WhileK, AssignK, ReturnK, CompoundK 
} StmtKind;

typedef enum { 
    OperatorK, ConstK, IdK, CallK 
} ExpKind;

typedef enum { 
    Void, Integer, Boolean
} ExpType;

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
    } attr;

    ExpType type;
} TreeNode;

extern TreeNode *savedTree;

#endif