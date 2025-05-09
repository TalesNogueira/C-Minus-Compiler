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
                    if (t->type == Void) {
                        printBars();
                        printf("> Semantic Error\n     Line %d - Invalid Declaration: variable '%s' cannot be of type 'void'.", t->lineno, t->attr.name);
                        printBars();
                    } else if (st_lookup(t->attr.name) == NULL) {
                        st_insert(t->attr.name, t->lineno, t);
                    } else if (st_lookup(t->attr.name)->kind.decl != FunctionK){
                        printBars(); 
                        printf("> Semantic Error\n     Line %d - Redeclaration: variable '%s' was already declared.\n", t->lineno, t->attr.name);
                        printBars();
                    }
                    break;
                case FunctionK:
                    if (strcmp(t->attr.name, "main") == 0) mainDeclared = true;

                    if (st_lookup(t->attr.name) == NULL) {
                        currentScope = "global";
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
                    TreeNode *func = st_lookup(t->attr.name);

                    if (st_lookup(t->attr.name) == NULL) {
                        printBars(); 
                        printf("> Semantic Error\n     Line %d - Not Declared: function '%s' was not declared.", t->lineno, t->attr.name);
                        printBars();
                    } else {
                        t->type = func->type;
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
                        printf("> Semantic Error\n     Line %d - Incomplete Assignment: missing operand(s).", t->lineno);
                        printBars();
                    } else if (t->child[1]->kind.exp == CallK) {
                        if (t->child[0]->type != t->child[1]->type) {
                            printBars();
                            printf("> Semantic Error\n     Line %d - Mismatch Type: variable '%s' (%s) cannot receive return value\n from function '%s' (%s).", 
                                  t->lineno,
                                  t->child[0]->attr.name,
                                  expTypeToString(t->child[0]->type),
                                  t->child[1]->attr.name,
                                  expTypeToString(t->child[1]->type));
                            printBars();
                        }
                    } else if (t->child[0]->type != t->child[1]->type) {
                        printBars();
                        printf("> Semantic Error\n     Line %d - Mismatch Type: '%s → %s' and '%s → %s' types do not match.", t->lineno, t->child[0]->attr.name, expTypeToString(t->child[0]->type), t->child[1]->attr.name, expTypeToString(t->child[1]->type));
                        printBars();
                    }
                    break;

                case ReturnK:
                    if (currentFunctionType == Void && t->child[0] != NULL) {
                        printBars();
                        printf("> Semantic Error\n     Line %d - Invalid Return: cannot return a value from a void function.", t->lineno);
                        printBars();
                    }
                    else if (currentFunctionType == Integer && t->child[0] == NULL) {
                        printBars();
                        printf("> Semantic Error\n     Line %d - Invalid Return: function with return type 'int' must return a value.", t->lineno);
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
            printf("> Semantic Error\n     Line %d - Conflict: variable '%s' collided with a function with same\n name.", t->lineno, t->attr.name);
            printBars();
        }
    }
}

/*  initiPredefinedFunctions() → TODO  */
static void initPredefinedFunctions() {
    TreeNode *inputFunc = newDeclNode(FunctionK);
    inputFunc->attr.name = "input";
    inputFunc->type = Integer;
    st_insert("input", 0, inputFunc);
    
    TreeNode *outputFunc = newDeclNode(FunctionK);
    outputFunc->attr.name = "output";
    outputFunc->type = Void;
    st_insert("output", 0, outputFunc);
}

/*  semanticAnalysis() → Traverses the entire Abstract Syntax Tree and performs the Semantic Analysis  */
void semanticAnalysis(TreeNode *tree) {
    initPredefinedFunctions();

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