/*-------------------------------------------------------------------------------------------------/
 *  Symbol Table structure and functions related for a C- Compiler
 *  File: semantic.c
 *---------------------------------*/

#include "symtab.h"
#include "utils.h"

typedef struct LineListRec {
    int lineno;
    struct LineListRec *next;
} *LineList;

typedef struct BucketListRec {
    char *name;
    LineList lines;
    TreeNode *treeNode;
    struct BucketListRec *next;
} *BucketList;

#define HASH_SIZE 211
#define SHIFT 4

static BucketList hashTable[HASH_SIZE];

/*  hash() → transforms a string (identifier name) into a numeric table (to Symbol Table) index  */
static int hash(char *key) {
    int temp = 0;
    for (; *key != NULL; key++)
        temp = ((temp << SHIFT) + *key) % HASH_SIZE;
    return temp;
}

/*  st_insert() → Inserts or updates an identifier in the Symbol Table  */
void st_insert(char *name, int lineno, TreeNode *t) {
    int h = hash(name);
    BucketList l = hashTable[h];

    while ((l != NULL) && (strcmp(name, l->name) != 0)) {
        l = l->next;
    }

    if (l == NULL) {        // New Symbol
        l = malloc(sizeof(struct BucketListRec));

        l->name = strdup(name);

        l->lines = malloc(sizeof(struct LineListRec));
        l->lines->lineno = lineno;
        l->lines->next = NULL;

        l->treeNode = t;

        l->next = hashTable[h];

        hashTable[h] = l;
    } else {                // Symbol already exists, add a new line
        LineList ll = l->lines;

        while (ll->next != NULL) {
            ll = ll->next;
        }

        ll->next = malloc(sizeof(struct LineListRec));
        ll->next->lineno = lineno;
        ll->next->next = NULL;
    }
}

/*  st_lookup() → Checks if the identifier is already declared in the Symbol Table and return the result (NULL or treeNode pointer)  */
TreeNode *st_lookup(char *name) {
    int h = hash(name);
    BucketList l = hashTable[h];

    while ((l != NULL) && (strcmp(name, l->name) != 0)) {
        l = l->next;
    }

    return (l == NULL) ? NULL : l->treeNode;
}

/*  printSymbolTable() → Prints the Symbol Table for debugging and/or viewing*/
void printSymbolTable() {
    printf("Tabela de Símbolos:\n");
    printf("Nome\t\tTipo\t\tEscopo\n");
    printf("----------------------------------------------------\n");

    for (int i = 0; i < HASH_SIZE; i++) {
        BucketList l = hashTable[i];

        while (l != NULL) {
            printf("%s", l->name);
            printf("\t\tTipo");
            printf("\t\t%s", (l->treeNode != NULL ? "Local" : "Global"));
            newLine();

            LineList lines = l->lines;
            while (lines != NULL) {
                printf("  Used at line %d\n", lines->lineno);
                lines = lines->next;
            }

            l = l->next;
        }
    }
}