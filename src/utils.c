/*-------------------------------------------------------------------------------------------------/
 *  Utils functions for a C- Compiler
 *  File: utils.c
 *---------------------------------*/

#define _DEFAULT_SOURCE

#include "utils.h"
#include "parser.tab.h"

#define MAX_NAME 256
#define MAX_FILES 100

char source[512];

static int indent = 0;

#define INDENT indent += 2
#define UNINDENT indent -= 2

/*--------------------------------------------/
 *  Indentation and Design functions
 *---------------------------------*/

void newLine(void) {
    printf("\n");
}

void printBars(void) {
    printf("\n----------------------------------------------------------------------------\n");
}

/*--------------------------------------------/
 *  Utility functions
 *---------------------------------*/

/*  inputSelect() → Searches for the input folder provided and lists the files, allowing you to select one to use   */
void inputSelect(void) {
  char files[MAX_FILES][MAX_NAME];
  int count = 0;

  newLine();

  const char *folder = INPUT_DIR;
  DIR *dir = opendir(folder);
  if (!dir) {
      perror("> Misc Error\n     Could not find the desired folder.\n");
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
      printf("> Misc Error\n     No files found.\n");
      exit(EXIT_FAILURE);
  }

  newLine();

  int choice;
  printf("> Enter the desired file number: ");
  scanf("%d", &choice);

  if (choice < 0 || choice >= count) {
      printf("> Misc Error\n     Invalid choice.\n");
      exit(EXIT_FAILURE);
  }

  printf("\n> Chosen file: [%s]\n", files[choice]);

  snprintf(source, sizeof(source), "%s/%s", folder, files[choice]);
  yyin = fopen(source, "r");

  if (!yyin) {
      perror("> Misc Error\n     Invalid file.\n");
      exit(EXIT_FAILURE);
  }
}

/*  tokenToString() → Transforms a numeric token into a string token based on the table generated by the parser (YACC-Bison)   */
const char *tokenToString(int token) {
  switch (token) {
    case INT: return "INT";
    case VOID: return "VOID";

    case IF: return "IF";
    case ELSE: return "ELSE";
    case WHILE: return "WHILE";
    case RETURN: return "RETURN";

    case ADD: return "ADD";
    case SUB: return "SUB";
    case MUL: return "MUL";
    case DIV: return "DIV";

    case GET: return "GET";
    case MORE: return "MORE";
    case LESS: return "LESS";
    case EQUALMORE: return "EQUALMORE";
    case EQUALLESS: return "EQUALLESS";
    case EQUAL: return "EQUAL";
    case DIFER: return "DIFER";

    case COMMA: return "COMMA";
    case SEMI: return "SEMI";

    case OPARENTHESIS: return "OPARENTHESIS";
    case CPARENTHESIS: return "CPARENTHESIS";
    case OBRACKETS: return "OBRACKETS";
    case CBRACKETS: return "CBRACKETS";
    case OKEYS: return "OKEYS";
    case CKEYS: return "CKEYS";

    case NUM: return "NUM";
    case ID: return "ID";

    case 0: return "EOF";

    default: return "UNKNOWN";
  }
}

/*  tokenToSymbol() → Transforms a numeric token of an symbol into a string refered to that symbol   */
static const char *tokenToSymbol(int token) {
  switch (token) {
    case ADD: return "+";
    case SUB: return "-";
    case MUL: return "*";
    case DIV: return "/";

    case GET: return "=";
    
    case MORE: return ">";
    case LESS: return "<";
    case EQUALMORE: return ">=";
    case EQUALLESS: return "<=";
    case EQUAL: return "==";
    case DIFER: return "!=";

    default: return "UNKNOWN";
  }
}

/*  expTokenToString() → Transforms a Expression Type enum into a string refered to that expression type   */
const char *expTypeToString(ExpType expType) {
  switch (expType) {
    case Void: return "void";
    case Integer: return "int";
    case Boolean: return "bool";

    default: return "UNKNOWN";
  }
} 

/*  declKindToString() → Transforms a Declaration Kind enum into a string refered to that declaration type   */
const char *declKindToString(DeclKind declKind) {
  switch (declKind) {
  case DeclFunction: return "Function";
  case DeclParameter: return "Parameter";
  case DeclVariable: return "Variable";
  case DeclArray: return "Array";
  default: return "UNKNOWN";
  }
}

/*--------------------------------------------/
 *  Abstract Syntax Tree (AST) functions
 *---------------------------------*/

TreeNode *newDeclNode(DeclKind kind) {
    TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));

    if (t == NULL) {
      printf("> Misc Error\n   Line %d - Out of memory error. (Declaration Node)\n", yylineno);
    } else {
      for (int i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
      t->sibling = NULL;
      
      t->lineno = yylineno;

      t->nodekind = NodeDeclaration;
      t->kind.decl = kind;
    }
    return t;
}

TreeNode *newStmtNode(StmtKind kind) {
    TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));

    if (t == NULL) {
      printf("> Misc Error\n   Line %d - Out of memory error. (Statement Node)\n", yylineno);
    } else {
      for (int i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
      t->sibling = NULL;
      
      t->lineno = yylineno;

      t->nodekind = NodeStatement;
      t->kind.stmt = kind;
    }
    return t;
}

TreeNode *newExpNode(ExpKind kind) {
    TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));

    if (t == NULL) {
      printf("> Misc Error\n   Line %d - Out of memory error. (Expression Node)\n", yylineno);
    } else {
      for (int i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
      t->sibling = NULL;
      
      t->lineno = yylineno;

      t->nodekind = NodeExpression;
      t->kind.exp = kind;
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

/*  insertScope() → [TODO]   */
void insertScope(TreeNode *t, char *scope) {
  while (t != NULL) {
    t->scope = strdup(scope);

    for (int i=0; i<MAXCHILDREN; i++) {
      if (t->child[i] != NULL) {
        insertScope(t->child[i], scope);
      }
    }

    t = t->sibling;
  }
}

/*  printIndent() → Prints out indentation using the "indent" variable   */
static void printIndent(void) {
    for (int i = 0; i < indent; i++) printf(" ");
}

/*  printTree() → Prints the Abstract Syntax Tree (AST) in a hierarchical format   */
void printTree(TreeNode *t) {
  INDENT;
  while (t != NULL) {
    switch (t->nodekind) {
      case NodeDeclaration:
        switch (t->kind.decl) {
          case DeclVariable:
            printIndent();
            printf("%s %s;", expTypeToString(t->type), t->attr.name);
            printf(" → Variable declaration at line %d\n", t->lineno);
            break;
          case DeclFunction:
            printf("\n> Function declaration at line %d:\n", t->lineno);
            printIndent();
            printf("%s %s (...)\n", expTypeToString(t->type), t->attr.name);
            break;
          case DeclParameter:
            printIndent();
            if (t->flags.isArray) {
              printf("%s %s[]", expTypeToString(t->type), t->attr.arrayAttr.name);
              printf(" → Parameter [Array]\n");
            } else {
              printf("%s %s", expTypeToString(t->type), t->attr.name);
              printf(" → Parameter\n");
            }
            break;
            case DeclArray:
              printIndent();
              printf("%s %s[%d];", expTypeToString(t->type), t->attr.arrayAttr.name, t->attr.arrayAttr.size);
              printf(" → Variable [Array] declaration at line %d\n", t->lineno);
              break;
          default:
            printf("- Unknown Declaration\n");
            break;
          }
          break;

      case NodeStatement:
        switch (t->kind.stmt) {
          case StmtIf:
            newLine();
            printIndent();
            printf("If (...)\n");
            break;
          case StmtWhile:
            newLine();
            printIndent();
            printf("While (...):\n");
            break;
          case StmtAssign:
            newLine();
            printIndent();
            printf("Assign [=] at line %d:\n", t->lineno);
            break;
          case StmtReturn:
            newLine();
            printIndent();
            printf("Return at line %d:\n", t->lineno);
            break;
          case StmtCompound:
            printIndent();
            printf("{\n");
            break;
          default:
            printf("Unknown Statement\n");
            break;
        }
        break;

      case NodeExpression:
        printIndent();
        switch (t->kind.exp) {
          case  ExpOperator:
            printf("Operator [%s] at line %d:\n", tokenToSymbol(t->attr.operator), t->lineno);
            break;
          case ExpConst:
            printf("%d", t->attr.value);
            printf(" → Constant\n");
            break;
          case  ExpID:
            if (t->flags.isArray) {
              printf("%s[↓]", t->attr.arrayAttr.name);
              printf(" → ID [Array]\n");
            } else {
              printf("%s", t->attr.name);
              printf(" → ID\n");
            }
            break;
          case ExpCall:
            printf("%s(...);", t->attr.name);
            printf(" → Call at line %d\n", t->lineno);
            break;
          default:
            printf("Unknown Expression\n");
            break;
        }
        break;

      default:
        printf("Unknown Node Kind\n");
        break;
    }

    for (int i = 0; i < MAXCHILDREN; i++)
      printTree(t->child[i]);

    t = t->sibling;
  }
  UNINDENT;
}