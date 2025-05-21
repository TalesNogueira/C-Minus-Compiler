/*-------------------------------------------------------------------------------------------------/
 *  Symbol Table structure and functions related for a C- Compiler
 *  File: symbol_table.c
 *---------------------------------*/

#include "symbol_table.h"
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

    while (l != NULL) {
        if (strcmp(name, l->name) == 0 && strcmp(currentScope, l->scope) == 0) {
            break;
        }
        l = l->next;
    }

    if (l == NULL) {        // New Symbol
        l = malloc(sizeof(struct BucketListRec));

        l->name = strdup(name);
        l->scope = strdup(currentScope);

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

    while (l != NULL) {
        if (strcmp(name, l->name) == 0) {
            if (strcmp(l->scope, currentScope) == 0 || strcmp(l->scope, "global") == 0) {
                return l->treeNode;
            }
        }
        l = l->next;
    }

    return NULL;
}

/*  printSymbolTable() → Prints the Symbol Table for debugging and/or viewing   */
void printSymbolTable() {
    newLine();
    printf("> Semantic Analysis --------------------------------------------------------\n");
    printf("\t\t\t    Symbol Table");
    printBars();
    printf("↓ Kind");
    printf("\t\t↓ Type");
    printf("\t\t↓ Name");
    printf("\t\t↓ Scope");
    printf("\t\t↓ At line(s)");
    printBars();

    for (int i = 0; i < HASH_SIZE; i++) {
        BucketList l = hashTable[i];

        while (l != NULL) {
            printf("%s", declKindToString(l->treeNode->kind.decl));
            printf("\t%s", expTypeToString(l->treeNode->type));
            printf("\t\t%s", l->name);
            printf("\t\t%s\t\t", l->scope);

            LineList lines = l->lines;
            while (lines != NULL) {
                printf("%d", lines->lineno);
                
                if(lines->next != NULL) {
                    printf(", ");
                }

                lines = lines->next;
            }

            newLine();
            
            l = l->next;
        }
    }
}