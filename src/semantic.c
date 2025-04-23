/*-------------------------------------------------------------------------------------------------/
 *  Semantic Analysis functions for a C- Compiler
 *  File: utils.c
 *---------------------------------*/

#include "globals.h"
#include "symtab.h"
#include "utils.h"

/*  mainDeclared → Flag to catch if main() was declared into code compiled  */
static bool mainDeclared = false;

/*  currentScope → Current scope of analyzed tree node  */
char* currentScope = "global";

static ExpType currentFunctionType = Void;

/*  traverse() → TODO */
static void traverse(TreeNode *t, void (*preProc)(TreeNode *), void (*postProc)(TreeNode *)) {
    if (t != NULL) {
        preProc(t);

        for (int i = 0; i < MAXCHILDREN; i++) {
            traverse(t->child[i], preProc, postProc);
        }

        postProc(t);

        traverse(t->sibling, preProc, postProc);
    }
}

/*  insertNode() → TODO */
static void insertNode(TreeNode *t) {
    switch (t->nodekind) {
        case DeclarationK:
            switch (t->kind.decl) {
                case VariableK:
                    if (st_lookup(t->attr.name) == NULL) {
                        st_insert(t->attr.name, t->lineno, t);
                    } else {
                        printBars(); 
                        printf("> Semantic Error\n     Line %d - Redeclaration: variable '%s' was already declared.", t->lineno, t->attr.name);
                        printBars();
                    }
                    break;
                case FunctionK:
                    if (strcmp(t->attr.name, "main") == 0) mainDeclared = true;

                    if (st_lookup(t->attr.name) == NULL) {
                        st_insert(t->attr.name, t->lineno, t);
                        currentScope = t->attr.name;
                        currentFunctionType = t->type;
                    } else {
                        printBars(); 
                        printf("> Semantic Error\n     Line %d - Redeclaration: function '%s' was already declared.", t->lineno, t->attr.name);
                        printBars();
                    }
                    break;
                case ParameterK:
                    if (st_lookup(t->attr.name) == NULL) {
                        st_insert(t->attr.name, t->lineno, t);
                    } else {
                        printBars(); 
                        printf("> Semantic Error\n     Line %d - Redeclaration: parameter '%s' was already declared.", t->lineno, t->attr.name);
                        printBars();
                    }
                    break;
            }
            break;
        default:
            break;
    }
}

/*  checkNode() → TODO */
static void checkNode(TreeNode *t) {
    switch (t->nodekind) {
        case ExpressionK:
            switch (t->kind.exp) {
                case IdK:
                    if (st_lookup(t->attr.name) == NULL) {
                        printBars(); 
                        printf("> Semantic Error\n     Line %d - Not Declared: variable '%s' was not declared.", t->lineno, t->attr.name);
                        printBars();
                    }
                    break;

                case CallK:
                    if (st_lookup(t->attr.name) == NULL) {
                        printBars(); 
                        printf("> Semantic Error\n     Line %d - Function Not Declared: function '%s' was not declared.", t->lineno, t->attr.name);
                        printBars();
                    }
                    break;

                default:
                break;
            }
            break;

        case StatementK:
            switch (t->kind.stmt) {
                case AssignK:
                    if (t->child[0] == NULL || t->child[1] == NULL) {
                        printBars();
                        printf("> Semantic Error\n     Line %d - Incomplete Assignment: missing operand(s).\n", t->lineno);
                        printBars();
                    }
                    else if ((t->child[0]->kind.exp == IdK || t->child[0]->kind.exp == CallK) && st_lookup(t->child[0]->attr.name) == NULL) {
                        printBars();
                        printf("> Semantic Error\n     Line %d - Undeclared Variable: '%s' wasn't previously declared.\n", t->child[0]->lineno, t->child[0]->attr.name);
                        printBars();
                    }
                    break;

                case ReturnK:
                    if (currentFunctionType == Void && t->child[0] != NULL) {
                        printBars();
                        printf("> Semantic Error\n     Line %d - Void Function: cannot return a value from a void function.\n", t->lineno);
                        printBars();
                    }
                    else if (currentFunctionType == Integer && t->child[0] == NULL) {
                        printBars();
                        printf("> Semantic Error\n     Line %d - Missing Return Value: function with return type 'int' must return a value.\n", t->lineno);
                        printBars();
                    }
                    break;

                default:
                    break;
            }
            break;
    
        default:
            break;
    }

    if (t->nodekind == DeclarationK && t->kind.decl == VariableK) {
        TreeNode *lookup = st_lookup(t->attr.name);
        if (lookup != NULL && lookup->kind.decl == FunctionK) {
            printBars();
            printf("> Semantic Error\n     Line %d - Conflict: variable '%s' collided with a function with same name.\n", t->lineno, t->attr.name);
            printBars();
        }
    }
}

/*  semanticAnalysis() → Traverses the entire Abstract Syntax Tree and performs the Semantic Analysis  */
void semanticAnalysis(TreeNode *tree) {
    traverse(tree, insertNode, checkNode);

    if (!mainDeclared) {
        printBars(); 
        printf("> Semantic Error\n     Main Missing: function 'main' was not declared.");
        printBars();
    }
    
    if(TraceSemantic) {
        printSymbolTable();
    }
}