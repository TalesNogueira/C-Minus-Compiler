/*-------------------------------------------------------------------------------------------------/
 *  Semantic Analysis functions for a C- Compiler
 *  File: semantic_analyzer.c
 *---------------------------------*/

#include "symbol_table.h"
#include "utils.h"

/*  mainDeclared → Flag to catch if main() was declared into code compiled  */
static bool mainDeclared = false;

/* currentFunctionType → Type of the current function being analyzed  */
static TreeNode *lastFunctionDeclared = NULL;

/*  traverse() → Traverse the Abstract Syntax Tree (AST) applying pre-order and post-order functions to each node */
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

/*  insertNode() → Inserts nodes into the Symbol Table */
static void insertNode(TreeNode *t) {
    switch (t->nodekind) {
        case NodeDeclaration:
            switch (t->kind.decl) {
                case DeclVariable:
                    if (t->type == Void) {
                        printBars();
                        printf("> Semantic Error\n     Line %d - Invalid Declaration: variable '%s' cannot be of type 'void'.", t->lineno, t->attr.name);
                        printBars();
                    } else if (st_lookup(t) == NULL) {
                        st_insert(t, t->scope);
                    } else {
                        printBars(); 
                        printf("> Semantic Error\n     Line %d - Redeclaration: variable '%s' was already declared.\n", t->lineno, t->attr.name);
                        printBars();
                    }
                    break;
                case DeclFunction:
                    if (strcmp(t->attr.name, "main") == 0) mainDeclared = true;

                    if (st_lookup(t) == NULL) {
                        st_insert(t, t->scope);
                        lastFunctionDeclared = t;
                    } else {
                        printBars(); 
                        printf("> Semantic Error\n     Line %d - Redeclaration: function '%s' was already declared.", t->lineno, t->attr.name);
                        printBars();
                    }
                    break;
                case DeclParameter:
                    if (st_lookup(t) == NULL) {
                        st_insert(t, t->scope);
                    } else {
                        printBars(); 
                        printf("> Semantic Error\n     Line %d - Redeclaration: parameter '%s' was already declared.", t->lineno, t->attr.name);
                        printBars();
                    }
                    break;
                case DeclArray:
                    if (st_lookup(t) == NULL) {
                        st_insert(t, t->scope);
                    } else {
                        printBars(); 
                        printf("> Semantic Error\n     Line %d - Redeclaration: array '%s' was already declared.", t->lineno, t->attr.name);
                        printBars();
                    }
                    break;
            }
            break;
        default:
            break;
    }
}

/*  checkNode() → Check nodes inserted into the Symbol Table */
static void checkNode(TreeNode *t) {
    TreeNode *lookup;

    switch (t->nodekind) {
        case NodeExpression:
            switch (t->kind.exp) {
                case ExpID:
                    lookup = st_lookup(t);

                    if (st_lookup(t) == NULL) {
                        printBars(); 
                        printf("> Semantic Error\n     Line %d - Not Declared: variable '%s' was not declared.", t->lineno, t->attr.name);
                        printBars();
                    } else {
                        st_insert(t, lookup->scope);
                    }
                    break;

                case ExpCall:
                    lookup = st_lookup(t);

                    if (lookup == NULL) {
                        printBars(); 
                        printf("> Semantic Error\n     Line %d - Not Declared: function '%s' was not declared.", t->lineno, t->attr.name);
                        printBars();
                    } else {
                        t->type = lookup->type;
                        st_insert(t, lookup->scope);
                    }
                    break;

                default:
                break;
            }
            break;

        case NodeStatement:
            switch (t->kind.stmt) {
                case StmtAssign:
                    if (t->child[0] == NULL || t->child[1] == NULL) {
                        printBars();
                        printf("> Semantic Error\n     Line %d - Incomplete Assignment: missing operand(s).", t->lineno);
                        printBars();
                    } else if (t->child[0]->type != t->child[1]->type) {
                        printBars();
                        printf("> Semantic Error\n     Line %d - Mismatch Type: '%s → %s' and '%s → %s' types do not match.", 
                            t->lineno,
                            t->child[0]->attr.name,
                            expTypeToString(t->child[0]->type),
                            t->child[1]->attr.name,
                            expTypeToString(t->child[1]->type));
                        printBars();
                    }
                    break;

                case StmtReturn:
                    lookup = st_lookup(lastFunctionDeclared);

                    if (lookup == NULL) {
                        printBars();
                        printf("> Semantic Error\n     Line %d - Invalid Return: function was not declared.", t->lineno);
                        printBars();
                    }

                    if (lookup->type == Void && t->child[0] != NULL) {
                        printBars();
                        printf("> Semantic Error\n     Line %d - Invalid Return: cannot return a value from a void function.", t->lineno);
                        printBars();
                    }
                    else if (lookup->type == Integer && t->child[0] == NULL) {
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

    if (t->nodekind == NodeDeclaration && t->kind.decl == DeclVariable) {
        lookup = st_lookup(t);

        if (lookup != NULL && lookup->kind.decl == DeclFunction) {
            printBars();
            printf("> Semantic Error\n     Line %d - Conflict: variable '%s' collided with a function with same\n name.", t->lineno, t->attr.name);
            printBars();
        }
    }
}

/*  initiPredefinedFunctions() → Inserts predefined functions into the Symbol Table  */
static void initPredefinedFunctions() {
    TreeNode *inputFunc = newDeclNode(DeclFunction);    // Input()
    inputFunc->type = Integer;
    inputFunc->lineno = 0;
    inputFunc->attr.name = "input";
    inputFunc->scope = "global";
    inputFunc->child[0] = NULL;
    inputFunc->child[1] = NULL;
    st_insert(inputFunc, "global");
    
//----------------------------------------------------
    TreeNode *outputFunc = newDeclNode(DeclFunction);   // Output(value)
    outputFunc->type = Void;
    outputFunc->lineno = 0;
    outputFunc->attr.name = "output";
    outputFunc->scope = "global";

    TreeNode *out_value = newDeclNode(DeclParameter);
    out_value->type = Integer;
    out_value->attr.name = strdup("value");

    outputFunc->child[0] = out_value;
    outputFunc->child[1] = NULL;
    st_insert(outputFunc, "global");

//----------------------------------------------------
    TreeNode *loadHDFunc = newDeclNode(DeclFunction);   // LoadHD(offset, line)
    loadHDFunc->type = Integer;
    loadHDFunc->lineno = 0;
    loadHDFunc->attr.name = "loadHD";
    loadHDFunc->scope = "global";
    
    TreeNode *lHD_offset = newDeclNode(DeclParameter);
    lHD_offset->type = Integer;
    lHD_offset->attr.name = strdup("offset");

    TreeNode *lHD_line = newDeclNode(DeclParameter);
    lHD_line->type = Integer;
    lHD_line->attr.name = strdup("line");

    addSibling(lHD_offset, lHD_line);

    loadHDFunc->child[0] = lHD_offset;
    loadHDFunc->child[1] = NULL;
    st_insert(loadHDFunc, "global");

//----------------------------------------------------
    TreeNode *storeHDFunc = newDeclNode(DeclFunction);   // StoreHD(offset, line, value)
    storeHDFunc->type = Void;
    storeHDFunc->lineno = 0;
    storeHDFunc->attr.name = "storeHD";
    storeHDFunc->scope = "global";
    
    TreeNode *sHD_offset = newDeclNode(DeclParameter);
    sHD_offset->type = Integer;
    sHD_offset->attr.name = strdup("offset");

    TreeNode *sHD_line = newDeclNode(DeclParameter);
    sHD_line->type = Integer;
    sHD_line->attr.name = strdup("line");

    TreeNode *sHD_value = newDeclNode(DeclParameter);
    sHD_value->type = Integer;
    sHD_value->attr.name = strdup("value");

    addSibling(sHD_offset, sHD_line);
    addSibling(sHD_offset, sHD_value);

    storeHDFunc->child[0] = sHD_offset;
    storeHDFunc->child[1] = NULL;
    st_insert(storeHDFunc, "global");

//----------------------------------------------------
    TreeNode *HD2IMFunc = newDeclNode(DeclFunction);   // HD2IM(offset, line, address)
    HD2IMFunc->type = Void;
    HD2IMFunc->lineno = 0;
    HD2IMFunc->attr.name = "HDtoIM";
    HD2IMFunc->scope = "global";
    
    TreeNode *HD2IM_offset = newDeclNode(DeclParameter);
    HD2IM_offset->type = Integer;
    HD2IM_offset->attr.name = strdup("offset");

    TreeNode *HD2IM_line = newDeclNode(DeclParameter);
    HD2IM_line->type = Integer;
    HD2IM_line->attr.name = strdup("line");

    TreeNode *HD2IM_address = newDeclNode(DeclParameter);
    HD2IM_address->type = Integer;
    HD2IM_address->attr.name = strdup("address");

    addSibling(HD2IM_offset, HD2IM_line);
    addSibling(HD2IM_offset, HD2IM_address);

    HD2IMFunc->child[0] = HD2IM_offset;
    HD2IMFunc->child[1] = NULL;
    st_insert(HD2IMFunc, "global");
//----------------------------------------------------
    TreeNode *LCDwriteFunc = newDeclNode(DeclFunction);   // LCDwrite(c0, c1, ..., c14, c15, line)
    LCDwriteFunc->type = Void;
    LCDwriteFunc->lineno = 0;
    LCDwriteFunc->attr.name = "LCDwrite";
    LCDwriteFunc->scope = "global";

    TreeNode *LCD_c0  = newDeclNode(DeclParameter);
    LCD_c0->type = Integer;
    LCD_c0->attr.name = strdup("c0");

    TreeNode *LCD_c1  = newDeclNode(DeclParameter);
    LCD_c1->type = Integer;
    LCD_c1->attr.name = strdup("c1");

    TreeNode *LCD_c2  = newDeclNode(DeclParameter);
    LCD_c2->type = Integer;
    LCD_c2->attr.name = strdup("c2");

    TreeNode *LCD_c3  = newDeclNode(DeclParameter);
    LCD_c3->type = Integer;
    LCD_c3->attr.name = strdup("c3");

    TreeNode *LCD_c4  = newDeclNode(DeclParameter);
    LCD_c4->type = Integer;
    LCD_c4->attr.name = strdup("c4");

    TreeNode *LCD_c5  = newDeclNode(DeclParameter);
    LCD_c5->type = Integer;
    LCD_c5->attr.name = strdup("c5");

    TreeNode *LCD_c6  = newDeclNode(DeclParameter);
    LCD_c6->type = Integer;
    LCD_c6->attr.name = strdup("c6");

    TreeNode *LCD_c7  = newDeclNode(DeclParameter);
    LCD_c7->type = Integer;
    LCD_c7->attr.name = strdup("c7");

    TreeNode *LCD_c8  = newDeclNode(DeclParameter);
    LCD_c8->type = Integer;
    LCD_c8->attr.name = strdup("c8");

    TreeNode *LCD_c9  = newDeclNode(DeclParameter);
    LCD_c9->type = Integer;
    LCD_c9->attr.name = strdup("c9");

    TreeNode *LCD_c10 = newDeclNode(DeclParameter);
    LCD_c10->type = Integer;
    LCD_c10->attr.name = strdup("c10");

    TreeNode *LCD_c11 = newDeclNode(DeclParameter);
    LCD_c11->type = Integer;
    LCD_c11->attr.name = strdup("c11");

    TreeNode *LCD_c12 = newDeclNode(DeclParameter);
    LCD_c12->type = Integer;
    LCD_c12->attr.name = strdup("c12");

    TreeNode *LCD_c13 = newDeclNode(DeclParameter);
    LCD_c13->type = Integer;
    LCD_c13->attr.name = strdup("c13");

    TreeNode *LCD_c14 = newDeclNode(DeclParameter);
    LCD_c14->type = Integer;
    LCD_c14->attr.name = strdup("c14");

    TreeNode *LCD_c15 = newDeclNode(DeclParameter);
    LCD_c15->type = Integer;
    LCD_c15->attr.name = strdup("c15");

    TreeNode *LCD_c16 = newDeclNode(DeclParameter);
    LCD_c16->type = Integer;
    LCD_c16->attr.name = strdup("c16");

    TreeNode *LCD_line = newDeclNode(DeclParameter);
    HD2IM_line->type = Integer;
    HD2IM_line->attr.name = strdup("line");

    addSibling(LCD_c0, LCD_c1);
    addSibling(LCD_c0, LCD_c2);
    addSibling(LCD_c0, LCD_c3);
    addSibling(LCD_c0, LCD_c4);
    addSibling(LCD_c0, LCD_c5);
    addSibling(LCD_c0, LCD_c6);
    addSibling(LCD_c0, LCD_c7);
    addSibling(LCD_c0, LCD_c8);
    addSibling(LCD_c0, LCD_c9);
    addSibling(LCD_c0, LCD_c10);
    addSibling(LCD_c0, LCD_c11);
    addSibling(LCD_c0, LCD_c12);
    addSibling(LCD_c0, LCD_c13);
    addSibling(LCD_c0, LCD_c14);
    addSibling(LCD_c0, LCD_c15);
    addSibling(LCD_c0, LCD_c16);
    addSibling(LCD_c0, LCD_line);

    LCDwriteFunc->child[0] = LCD_c0;
    LCDwriteFunc->child[1] = NULL;
    st_insert(LCDwriteFunc, "global");
}

/*  traceSemantic() → Check TraceSemantic and print out the Symbol Table  */
static void traceSemantic(void) {
  if (TraceSemantic) {
    printSymbolTable();
  } else {
    printf("\n> Semantic Analysis completed.\n");
  }
}

/*  semanticAnalysis() → Traverses the entire Abstract Syntax Tree and performs the Semantic Analysis  */
void semanticAnalysis(TreeNode *AST) {
    initPredefinedFunctions(); // [TODO]: Add a bool in main to config this

    traverse(AST, insertNode, checkNode);

    if (!mainDeclared) {
        printBars(); 
        printf("> Semantic Error\n     Main Missing: function 'main' was not declared.");
        printBars();
    }
    
    traceSemantic();
}