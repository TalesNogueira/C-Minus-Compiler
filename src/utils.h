/*-------------------------------------------------------------------------------------------------/
 *  Utils Header functions for a C- Compiler
 *  File: utils.h
 *---------------------------------*/

#ifndef UTILS_H
#define UTILS_H

#include "globals.h"

void newLine(void);

void inputSelect(void);

/*--------------------------------------------/
 *  Abstract Syntax Tree (AST) functions (.h)
 *---------------------------------*/

TreeNode *newDeclNode(DeclKind kind);

TreeNode *newStmtNode(StmtKind kind);

TreeNode *newExpNode(ExpKind kind);

TreeNode *addSibling(TreeNode *t, TreeNode *sibling);

void printTree(TreeNode *tree);

#endif