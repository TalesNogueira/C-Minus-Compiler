/*-------------------------------------------------------------------------------------------------/
 *  Utils functions for a C- Compiler
 *  File: utils.c
 *---------------------------------*/

#define _DEFAULT_SOURCE

#include "utils.h"
#include "globals.h"

char source[512];

#define MAX_FILES 100
#define MAX_NAME 256

void newLine(void) {
    printf("\n");
}

void inputSelect(void) {
    char files[MAX_FILES][MAX_NAME];
    int count = 0;

    newLine();

    const char* folder = INPUT_DIR;
    DIR *dir = opendir(folder);
    if (!dir) {
        perror("> Error [ Misc ]:\n    Could not find the desired folder.\n");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    printf("> Files available at \"%s\":\n", folder);

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            printf("[%d] %s\n", count, entry->d_name);
            strncpy(files[count], entry->d_name, MAX_NAME);
            count++;
        }
    }
    closedir(dir);

    if (count == 0) {
        printf("> Error [ Misc ]:\n    No files found.\n");
        exit(EXIT_FAILURE);
    }

    newLine();

    int choice;
    printf("> Enter the desired file number: ");
    scanf("%d", &choice);

    if (choice < 0 || choice >= count) {
        printf("> Error [ Misc ]:\n    Invalid choice.\n");
        exit(EXIT_FAILURE);
    }

    snprintf(source, sizeof(source), "%s/%s", folder, files[choice]);

    yyin = fopen(source, "r");

    if (!yyin) {
        perror("> Error [ Misc ]:\n    Invalid file.\n");
        exit(EXIT_FAILURE);
    }

    newLine();
}

/*--------------------------------------------/
 *  Abstract Syntax Tree (AST) functions (.c)
 *---------------------------------*/

TreeNode *newDeclNode(DeclKind kind) {
    TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));

    if (t == NULL) {
        printf("> Error [ Misc ]:\n    Line (%d) - Out of memory error. (Declaration Node)\n", yylineno);
    } else {
        for (int i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
        t->sibling = NULL;
        
        t->lineno = yylineno;

        t->nodekind = DeclarationK;
        t->kind.decl = kind;

        // t->type = -1;
    }
    return t;
}

TreeNode *newStmtNode(StmtKind kind) {
    TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));

    if (t == NULL) {
        printf("> Error [ Misc ]:\n    Line (%d) - Out of memory error. (Statement Node)\n", yylineno);
    } else {
        for (int i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
        t->sibling = NULL;
        
        t->lineno = yylineno;

        t->nodekind = StatementK;
        t->kind.stmt = kind;

        // t->type = -1;
    }
    return t;
}

TreeNode *newExpNode(ExpKind kind) {
    TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));

    if (t == NULL) {
        printf("> Error [ Misc ]:\n    Line (%d) - Out of memory error. (Expression Node)\n", yylineno);
    } else {
        for (int i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
        t->sibling = NULL;
        
        t->lineno = yylineno;

        t->nodekind = ExpressionK;
        t->kind.exp = kind;

        // t->type = -1;
    }
    return t;
}

TreeNode *addSibling(TreeNode *t, TreeNode *sibling) {
    if (t == NULL) return sibling;
    
    TreeNode *current = t;

    while (current->sibling != NULL)
        current = current->sibling;
    current->sibling = sibling;
    
    return t;
}

void printTree(TreeNode *tree) {
    printf("\n--- TESTING AST PRINTING ---\n");
}