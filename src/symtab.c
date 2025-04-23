/*-------------------------------------------------------------------------------------------------/
 *  Symbol Table structure and functions related for a C- Compiler
 *  File: symtab.c
 *---------------------------------*/

#include "symtab.h"
#include "utils.h"

#define HASH_SIZE 211
#define SHIFT 4

static BucketList hashTable[HASH_SIZE];

/*  hash() → transforms a string (identifier name) into a numeric table (to Symbol Table) index  */
static int hash(char *key) {
    int temp = 0;
    for (; *key != '\0'; key++)
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

/*  printSymbolTable() → Prints the Symbol Table for debugging and/or viewing   */
void printSymbolTable() {
    newLine();
    printf("> Semantic Analysis ----------------------------------------------------\n");
    printf("\t\t\t    Symbol Table");
    printBars();
    printf("\t\t↓ Name\t\t↓ Type\t\t↓ Scope");
    printBars();

    for (int i = 0; i < HASH_SIZE; i++) {
        BucketList l = hashTable[i];

        while (l != NULL) {
            printf("\t\t%s", l->name);
            printf("\t\t%s", expTypeToString(l->treeNode->type));
            printf("\t\t%s\n", (l->treeNode != NULL ? "Local" : "Global"));

            LineList lines = l->lines;
            while (lines != NULL) {
                printf("    > Used at line %d\n", lines->lineno);
                lines = lines->next;
            }

            newLine();
            
            l = l->next;
        }
    }
}