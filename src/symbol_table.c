/*-------------------------------------------------------------------------------------------------/
 *  Symbol Table structure and functions for a C- Compiler
 *  File: symbol_table.c
 *---------------------------------*/

#include "symbol_table.h"
#include "utils.h"

#define HASH_SIZE 211
#define SHIFT 4

static BucketList hashTable[HASH_SIZE];

/*  hash() → transforms a string (identifier name) into a numeric table (to Symbol Table) index  */
static int hash(char *key, char *salt) {
    int temp = 0;

    for (; *key != '\0'; key++) {
        temp = ((temp << SHIFT) + *key) % HASH_SIZE;
    }
    
    for (; *salt != '\0'; salt++) {
        temp = ((temp << SHIFT) + *salt) % HASH_SIZE;
    }

    return temp;
}

/*  st_insert() → Inserts or updates an identifier in the Symbol Table  */
void st_insert(TreeNode *t, char *scope) {
    int h = hash(t->attr.name, scope);
    BucketList l = hashTable[h];

    while (l != NULL) {
        if (strcmp(t->attr.name, l->name) == 0) {
            break;
        }
        l = l->next;
    }

    /* New Symbol   */
    if (l == NULL) {
        l = malloc(sizeof(struct BucketListRec));

        l->name = strdup(t->attr.name);
        l->scope = strdup(scope);

        l->lines = malloc(sizeof(struct LineListRec));
        l->lines->lineno = t->lineno;
        l->lines->next = NULL;

        l->treeNode = t;
        
        l->next = hashTable[h];

        hashTable[h] = l;

    /* Symbol already exists, add a new line   */
    } else {
        LineList ll = l->lines;

        while (ll->next != NULL) {
            ll = ll->next;
        }

        ll->next = malloc(sizeof(struct LineListRec));
        ll->next->lineno = t->lineno;
        ll->next->next = NULL;
    }
}

/*  st_lookup() → Checks if the identifier is already declared in the Symbol Table and return the result (NULL or treeNode pointer)  */
TreeNode *st_lookup(TreeNode *t) {
    int h = hash(t->attr.name, t->scope);
    BucketList l = hashTable[h];

    while (l != NULL) {
        if (strcmp(t->attr.name, l->name) == 0) {
            return l->treeNode;
        }
        l = l->next;
    }

    if (t->nodekind == NodeExpression &&
       (t->kind.exp == ExpID) ||
        t->kind.exp == ExpCall)
    {
        h = hash(t->attr.name, "global");
        l = hashTable[h];

        while (l != NULL) {
            if (strcmp(t->attr.name, l->name) == 0) {
                return l->treeNode;
            }
            l = l->next;
        }
    }

    return NULL;
}

/*  printSymbolTable() → Prints the Symbol Table for debugging and/or viewing   */
void printSymbolTable() {
    newLine();
    printf("> Semantic Analysis --------------------------------------------------------\n");
    printf("\t\t\t    Symbol Table                 [%s]", source);
    printBars();
    printf("↓ Kind\t\t↓ Type\t\t↓ Name\t\t↓ Scope\t\t↓ At line(s)");
    printBars();

    for (int i = 0; i < HASH_SIZE; i++) {
        BucketList l = hashTable[i];

        while (l != NULL) {
            if(l->treeNode->kind.decl == DeclArray) printf("%s[%d]", declKindToString(l->treeNode->kind.decl), l->treeNode->attr.arrayAttr.size);
            else printf("%s", declKindToString(l->treeNode->kind.decl));

            printf("\t%s", expTypeToString(l->treeNode->type));
            printf("\t\t%s", l->name);
            printf("\t\t%s\t\t", l->scope);

            int firstLoop = 1;
            LineList lines = l->lines;

            while (lines != NULL) {
                if (firstLoop) {
                    firstLoop = 0;
                    printf("~%d", lines->lineno);
                } else printf("%d", lines->lineno);
                
                if(lines->next != NULL) {
                    printf(", ");
                }

                lines = lines->next;
            }

            newLine();
            
            l = l->next;
        }
    }

    printf("\n*[~line]: \"line\" = declaration line.");
    printBars();
}