/*-------------------------------------------------------------------------------------------------/
 *  Symbol Table interface for a C- Compiler
 *  File: symtab.h
 *---------------------------------*/

#ifndef SYMTAB_H
#define SYMTAB_H

#include "globals.h"

/*--------------------------------------------/
 *  Symbol Table functions
 *---------------------------------*/

/*  st_insert() → Inserts or updates an identifier in the Symbol Table  */
void st_insert(char *name, int lineno, TreeNode *t);

/*  st_lookup() → Checks if the identifier is already declared in the Symbol Table and return the result (NULL or treeNode pointer)  */
TreeNode *st_lookup(char *name);

/*  printSymbolTable() → Prints the Symbol Table for debugging and/or viewing*/
void printSymbolTable();

#endif