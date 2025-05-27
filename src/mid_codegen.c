/*-------------------------------------------------------------------------------------------------/
 *  Intermediate "Mid" Code Generator structure and functions for a C- Compiler
 *  File: mid_codegen.c
 *---------------------------------*/

#include "mid_codegen.h"
#include "utils.h"

/*  opString[] → [TODO]  */
const char *opString[] = {
  "ADD", "SUB", "MUL", "DIV", "SGT", "SLT", "SGET", "SLET", "SET", "SDT",
  "AllocVAR", "AllocARRAY", "StoreVAR", "StoreARRAY", "LoadVAR", "LoadARRAY", "MOVE",
  "IFfalse", "LABEL", "JUMP", 
  "FunBGN", "FunEND", "PARAM", "CALL", "RETURN",
  "PUSH", "POP", "HALT"
};

/*  codeGen() → [TODO]  */
static void codeGen(TreeNode *t);

#define REG_SIZE 32

/*  firstExecution → Variable to track the first freeRegister() to start "registers" (int*)  */
static bool firstExecution = true;

static int *registers;

/*  freeRegisters() → [TODO]  */
static void freeRegisters(void) {
  if (firstExecution) {
    firstExecution = false;
    registers = malloc(REG_SIZE*sizeof(int));
  }

	for(int i=0;i<REG_SIZE;i++) {
		registers[i] = 0;
	}
}

QuadList *quadruples = NULL;

/*  insertQuad() → [TODO]  */
static void insertQuad(Operation op, Address src, Address tgt, Address dst) {
  QuadList *newQuad = (QuadList*)malloc(sizeof(QuadList));
	newQuad->op = op;
	newQuad->src = src;
	newQuad->tgt = tgt;
	newQuad->dst = dst;
  newQuad->next = NULL;

  if (quadruples == NULL) {
    quadruples = newQuad;
  } else {
    QuadList *list = quadruples;
    
    while (list->next != NULL) {
      list = list->next;
    }
    list->next = newQuad;
  }
}

/*  declGen() → [TODO]  */
static void declGen(TreeNode *t) {
  Address src, tgt, dst;

  if (t == NULL) return;

  switch (t->kind.decl) {
    case DeclFunction:
      src.type = addrString;
      src.content.name = (char*) malloc(strlen(t->attr.name)*sizeof(char));
      strcpy(src.content.name, t->attr.name);	

      tgt.type = addrVoid;
      dst.type = addrVoid;

      insertQuad(FunBGN, src, tgt, dst);

      codeGen(t->child[0]);
      codeGen(t->child[1]);

      freeRegisters();

      insertQuad(FunEND, src, tgt, dst);
      break;
    case DeclParameter:
      /* code */
      break;
    case DeclVariable:
      src.type = addrString;
      src.content.name = (char*) malloc(strlen(t->attr.name)*sizeof(char));
      strcpy(src.content.name, t->attr.name);	

      
      tgt.type = addrVoid;
      dst.type = addrVoid;

      break;
    case DeclArray:
      /* code */
      break;
    default:
      break;
  }
}

/*  stmtGen() → [TODO]  */
static void stmtGen(TreeNode *t) {
  printf("\n> Statement Kind...\n");
}

/*  expGen() → [TODO]  */
static void expGen(TreeNode *t) {
  printf("\n> Expression Kind...\n");
}

/*  codeGen() → [TODO]  */
static void codeGen(TreeNode *t) {
  if (t != NULL) {
    switch (t->nodekind) {
      case NodeDeclaration:
        declGen(t);
        break;
      case NodeStatement:
        stmtGen(t);
        break;
      case NodeExpression:
        expGen(t);
        break;
    }
    codeGen(t->sibling);
  }
}

/*  traceMidCode() → Check TraceMidCode and print out the Quadruples List  */
static void traceMidCode(char *string) {
  if (TraceMidCode) {
    printf("%s", string);
  }
}

static int counter = 0;

/*  printQuadruplesList() → [TODO]  */
static void printQuadruplesList(void) {
  FILE *file = fopen("outputs/midcode.txt","w");

  if (quadruples == NULL) return;

  printf("\n> Intermediate Code Synthesis ----------------------------------------------\n\n");

  QuadList *list = quadruples;
  while (list->next != NULL) {
    char str[32];

    fprintf(file, "\t> %d: %s ", counter, opString[list->op]);
    sprintf(str, "\t> %d: %s ", counter, opString[list->op]);
    traceMidCode(str);

    switch (list->src.type) {
      case addrVoid:
        fprintf(file, "[---] ");
        traceMidCode("[---] ");
        break;
      case addrConst:
        fprintf(file, "%d ", list->src.content.value);
        sprintf(str, "%d ", list->src.content.value);
        traceMidCode(str);
        break;
      case addrString:
        fprintf(file, "%s ", list->src.content.name);
        sprintf(str, "%s ", list->src.content.name);
        traceMidCode(str);
        break;
    }
    switch (list->tgt.type) {
      case addrVoid:
        fprintf(file, "[---] ");
        traceMidCode("[---] ");
        break;
      case addrConst:
        fprintf(file, "%d ", list->tgt.content.value);
        sprintf(str, "%d ", list->tgt.content.value);
        traceMidCode(str);
        break;
      case addrString:
        fprintf(file, "%s ", list->tgt.content.name);
        sprintf(str, "%s ", list->tgt.content.name);
        traceMidCode(str);
        break;
    }
    switch (list->dst.type) {
      case addrVoid:
        fprintf(file, "[---] ");
        traceMidCode("[---] ");
        break;
      case addrConst:
        fprintf(file, "%d ", list->dst.content.value);
        sprintf(str, "%d ", list->dst.content.value);
        traceMidCode(str);
        break;
      case addrString:
        fprintf(file, "%s ", list->dst.content.name);
        sprintf(str, "%s ", list->dst.content.name);
        traceMidCode(str);
        break;
    }
    fprintf(file, "\n");
    traceMidCode("\n");

    counter++;

    list = list->next;
  }
  fclose(file);
}

/*  midCodeGenerate() → Call codeGen() and [TODO] ---> Traceable    */
void midCodeGenerate(TreeNode *AST)  {
  freeRegisters();
  codeGen(AST);
  printQuadruplesList();
}