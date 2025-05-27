/*-------------------------------------------------------------------------------------------------/
 *  Symbol Table interface for a C- Compiler
 *  File: symbol_table.h
 *---------------------------------*/

#ifndef SYMTAB_H
#define SYMTAB_H

#include "globals.h"

/*--------------------------------------------/
 *  Symbol Table structure
 *---------------------------------*/

/*  LineList → Linked list containing all lines of code where the symbol was used  */
typedef struct LineListRec {
    int lineno;
    struct LineListRec *next;
} *LineList;

/*  BucketList → Bucket list that stores a symbol, its scope, usage lines, AST node, and points to the next symbol in case of a hash table collision  */
typedef struct BucketListRec {
    char *name;
    char *scope;
    LineList lines;
    TreeNode *treeNode;
    struct BucketListRec *next;
} *BucketList;

/*--------------------------------------------/
 *  Symbol Table functions
 *---------------------------------*/

/*  st_insert() → Inserts or updates an identifier in the Symbol Table  */
void st_insert(TreeNode *t);

/*  st_lookup() → Checks if the identifier is already declared in the Symbol Table and return the result (NULL or treeNode pointer)  */
TreeNode *st_lookup(TreeNode *t);

/*  printSymbolTable() → Prints the Symbol Table for debugging and/or viewing*/
void printSymbolTable(void);

#endif