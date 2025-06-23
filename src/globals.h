/*-------------------------------------------------------------------------------------------------/
 *  Global interface for a C- Compiler
 *  File: globals.h
 *---------------------------------*/

#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

/*--------------------------------------------/
 *  Lexical Analysis → Flex variables & functions
 *---------------------------------*/

/*  yyin → Flex's input filepath  */
extern FILE* yyin;
/*  yylineno → Real time Flex's line number   */
extern int yylineno;
/* getToken() → Call yylex(), treat the TOKEN and then return ---> Traceable   */
extern int getToken(void);
 
/*--------------------------------------------/
  *  Syntax Analysis → YACC-Bison variables & functions
  *---------------------------------*/

/*  syntaxAnalysis() → Call yyparse() and build the AST ---> Traceable    */
extern void syntaxAnalysis(void);

/*--------------------------------------------/
 *  Global significant variables
 *---------------------------------*/

/*  INPUT_DIR → Inputs folder name */
extern char *INPUT_DIR;

/*  source[] → Source filepath */
extern char source[512];

/*--------------------------------------------/
 *  Tracing Flags
 *---------------------------------*/

/* TraceScan → Trace Flex's scanner getToken() - yylex() - executions and prints it out   */
extern bool TraceScan;
/* TraceParse → Trace Bison's parser Abstract Syntax Tree (AST) and prints it out   */
extern bool TraceParse;
/* TraceSemantic → Trace Semantic's Analyzer and its Symbol Table; and prints it out   */
extern bool TraceSemantic;
/* TraceMidCode → Trace Intermediate "Mid" Code Generator and it's Quadruples List; and prints it out   */
extern bool TraceMidCode;

/*--------------------------------------------/
 *  Abstract Syntax Tree (AST) and related structures
 *---------------------------------*/

 typedef enum { 
    Void, Integer, Boolean
} ExpType;

#define MAXCHILDREN 3

typedef enum { NodeDeclaration, NodeStatement, NodeExpression } NodeKind;

typedef enum { 
    DeclFunction, DeclParameter, DeclVariable, DeclArray
} DeclKind;

typedef enum { 
    StmtAssign, StmtCompound, StmtIf, StmtWhile, StmtReturn
} StmtKind;

typedef enum { 
    ExpOperator, ExpConst, ExpID, ExpCall
} ExpKind;

/*  (struct treeNode) TreeNode → Standard tree node structure for constructing the Abstract Syntax Tree   */
typedef struct treeNode {
    struct treeNode *sibling;
    struct treeNode *child[MAXCHILDREN];
    
    int lineno;
    char *scope;

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

/*--------------------------------------------/
  *  Semantic Analysis variables & functions
  *---------------------------------*/

/*  semanticAnalysis() → Traverses the entire Abstract Syntax Tree and performs the Semantic Analysis  */
extern void semanticAnalysis(TreeNode *AST);

/*--------------------------------------------/
  *  Intermediate Code functions
  *---------------------------------*/

/*  midCodeGenerate() → Traverses the entire Abstract Syntax Tree and performs the Intermediate Code Generation  */
extern void midCodeGenerate(TreeNode *AST);

#endif