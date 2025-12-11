/*-------------------------------------------------------------------------------------------------/
 *  Intermediate "Mid" Code Generator structure and functions for a C- Compiler
 *  File: mid_codegen.c
 *---------------------------------*/

#include "mid_codegen.h"
#include "parser.tab.h"
#include "utils.h"

/*  opString[] → [TODO]  */
const char *opString[] = {
  "Add", "Sub", "Mul", "Div",
  "Or", "And",
  "Lshift", "Rshift",
  "SGT", "SLT", "SGET", "SLET", "SET", "SDT",
  "AllocVAR", "AllocARRAY", "StoreVAR", "StoreARRAY", "LoadVAR", "LoadARRAY",
  "IFfalse", "Label", "Jump", 
  "FunBGN", "FunEND", "Param", "Call", "Move", "Return",
  "Push", "Pop", "Halt"
};

/*  codeGen() → [TODO]  */
static void codeGen(TreeNode *t);

/*  firstExecution → Variable to track the first freeRegister() to start "registers" (int*)  */
static bool firstExecution = true;

#define REG_SIZE 32
static int *registers;

int usedRegisters = 0;

/*  useRegister() → [TODO]  */
static char *useRegister(int addr) {
  char reg[16];

  if (addr == -1) {
    for (int i = 5; i < 28; i++) {
      if (registers[i] == 0) {
        registers[i] = 1;
        
        sprintf(reg, "r%d", i);

                usedRegisters++;
        return strdup(reg);
      }
    }
  } else {
    if (registers[addr] == 0) {
      registers[addr] = 1;
      
      sprintf(reg, "r%d", addr);

      usedRegisters++;
      return strdup(reg);
    }
  }

  return NULL;
}

/*  freeRegisters() → [TODO]  */
static void freeRegisters(char *reg) {
  if (firstExecution) {
    firstExecution = false;
    registers = malloc(REG_SIZE * sizeof(int));
    
    for (int i = 0; i < REG_SIZE; i++) {
      registers[i] = 0;
    }
  }

  if (reg == NULL) {
    for(int i=0; i<REG_SIZE; i++) {
      registers[i] = 0;
    }
    usedRegisters = 0;
  } else {
    if (reg[0] == 'r') {
      int regIndex = atoi(&reg[1]);
      if (regIndex >= 0 && regIndex < REG_SIZE) {
        registers[regIndex] = 0;
        usedRegisters--;
      }
    }
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

  if (op == Add || op == Sub || op == Mul || op == Div || op == Or || op == And || op == Lshift || op == Rshift || op == SGT || op == SLT || op == SGET || op == SLET || op == SET || op == SDT) {
    if (src.type == addrString) freeRegisters(src.content.name);
    if (tgt.type == addrString) freeRegisters(tgt.content.name);
  } else if (op == StoreVAR || op == StoreARRAY) {
    if (src.type == addrString) freeRegisters(src.content.name);
  }
}

/*  pushRegister() → [TODO]  */
static void pushRegister(int paramCounter) {
  char reg[16];
	Address src, empty;
  
	empty.type = addrVoid;

	for(int i = 0; i<REG_SIZE; i++) {
		if (registers[i] == 1) {
      if (paramCounter == usedRegisters) {
        sprintf(reg, "r%d", i);

        src.type = addrString;
        src.content.name = strdup(reg);

        insertQuad(Push, src, empty, empty);
      } else {
        paramCounter++;
      }
		}
	}
}

/*  popRegister() → [TODO]  */
static void popRegister(int paramCounter, int addsub) {
	char reg[16];
	Address src, dontSUB, empty;
  
	empty.type = addrVoid;

	for(int i = REG_SIZE - 1; i>=0; i--) {
		if (registers[i] == 1) {
      if (paramCounter > 0) {
        sprintf(reg, "r%d", i);

        src.type = addrString;
        src.content.name = strdup(reg);

        if (addsub) {
          dontSUB.type = addrConst;
          dontSUB.content.value = 1;
          insertQuad(Pop, src, empty, dontSUB);
        } else {
          insertQuad(Pop, src, empty, empty);
        }    
        freeRegisters(src.content.name);
        paramCounter--;
      }
		}
	}
}

static int labelsCounter = 0;

/*  useLabel() → [TODO]  */
static char *useLabel(void) {
    char label[16];
    sprintf(label, "l%d", labelsCounter++);

    return strdup(label);
}

/*  tokenToOperation() → [TODO]  */
static const Operation tokenToOperation(int token) {
	switch(token) {
		case ADD:
			return Add;
			break;
		case SUB:
			return Sub;
			break;
    case MUL:
      return Mul;
      break;
		case DIV:
			return Div;
			break;
    case OR:
      return Or;
      break;
    case AND:
      return And;
      break;
    case LSHIFT:
      return Lshift;
      break;
    case RSHIFT:
      return Rshift;
      break;
		case MORE:
			return SGT;
			break;
		case LESS:
			return SLT;
			break;
		case EQUALMORE:
			return SGET;
			break;
		case EQUALLESS:
			return SLET;
			break;
		case EQUAL:
			return SET;
			break;
    case DIFER:
			return SDT;
			break;
	}
}

/*  declGen() → [TODO]  */
static void declGen(TreeNode *t) {
  Address src, tgt, dst;

  if (t == NULL) return;

  switch (t->kind.decl) {
    case DeclFunction:
      src.type = addrString;
      src.content.name = strdup(t->attr.name);	

      tgt.type = addrString;
      tgt.content.name = strdup(expTypeToString(t->type));

      dst.type = addrVoid;

      insertQuad(FunBGN, src, tgt, dst);

      codeGen(t->child[0]);
      codeGen(t->child[1]);

      insertQuad(FunEND, src, tgt, dst);

      if (!strcmp(src.content.name, "main")) {
        src.type = addrVoid;
        
        insertQuad(Halt, src, tgt, dst);
      }

      freeRegisters(NULL);

      break;
    case DeclParameter:
      src.type = addrString;
      src.content.name = strdup(t->scope);

      if (t->flags.isArray) {
        tgt.type = addrString;
        tgt.content.name = strdup(t->attr.arrayAttr.name);

        dst.type = addrConst;
        dst.content.value = t->attr.arrayAttr.size;

        insertQuad(AllocARRAY, src, tgt, dst);
      } else {
        tgt.type = addrString;
        tgt.content.name = strdup(t->attr.name);

        dst.type = addrVoid;

        insertQuad(AllocVAR, src, tgt, dst);
      }
      break;
    case DeclVariable:
      src.type = addrString;
      src.content.name = strdup(t->scope);	

      tgt.type = addrString;
      tgt.content.name = strdup(t->attr.name);	

      dst.type = addrVoid;

      insertQuad(AllocVAR, src, tgt, dst);
      break;
    case DeclArray:
      src.type = addrString;
      src.content.name = strdup(t->scope);	

      tgt.type = addrString;
      tgt.content.name = strdup(t->attr.arrayAttr.name);	

      dst.type = addrConst;
      dst.content.value = t->attr.arrayAttr.size;	

      insertQuad(AllocARRAY, src, tgt, dst);
      break;
  }
}

static Address current;

/*  stmtGen() → [TODO]  */
static void stmtGen(TreeNode *t) {
  Address src, tgt, dst;
  Address empty;
  Address condition;

  char *labelStart;
  char *labelElse;
  char *labelEnd;

  char *regTemp;

  empty.type = addrVoid;
  
  if (t == NULL) return;

  switch (t->kind.stmt) {
    case StmtAssign:
      codeGen(t->child[1]);  
      Address right = current;

      if (t->child[0]->flags.isArray) {
        src.type = addrString;
        src.content.name = strdup(t->child[0]->scope);	

        tgt.type = addrString;
        tgt.content.name = strdup(t->child[0]->attr.arrayAttr.name);	

        regTemp = useRegister(-1);
        dst.type = addrString;
        dst.content.name = strdup(regTemp);
        
        insertQuad(LoadVAR, src, tgt, dst);

        codeGen(t->child[0]->child[0]);
        
        Address temp;

        regTemp = useRegister(-1);
        temp.type = addrString;
        temp.content.name = strdup(regTemp);

        insertQuad(Add, dst, current, temp);

        tgt.type = addrString;
        tgt.content.name = strdup(t->child[0]->scope);	

        insertQuad(StoreARRAY, right, tgt, temp);

        freeRegisters(temp.content.name);
      } else {
        src.type = addrString;
        src.content.name = strdup(t->child[0]->scope);

        tgt.type = addrString;
        tgt.content.name = strdup(t->child[0]->attr.name);

        insertQuad(StoreVAR, right, src, tgt);
      }
      break;
    case StmtCompound:
      codeGen(t->child[0]);
      codeGen(t->child[1]);
      break;
    case StmtIf:
      labelElse = useLabel();

      codeGen(t->child[0]);
      condition = current;

      tgt.type = addrString;
      tgt.content.name = labelElse;

      insertQuad(IFfalse, condition, tgt, empty);

      freeRegisters(condition.content.name);

      codeGen(t->child[1]);

      if (t->child[2] != NULL) {
        labelEnd = useLabel();

        src.type = addrString;
        src.content.name = labelEnd;

        insertQuad(Jump, src, empty, empty);
        
        src.type = addrString;
        src.content.name = labelElse;

        insertQuad(Label, src, empty, empty);

        codeGen(t->child[2]);

        src.type = addrString;
        src.content.name = labelEnd;

        insertQuad(Label, src, empty, empty);
      } else {
        src.type = addrString;
        src.content.name = labelElse;

        insertQuad(Label, src, empty, empty);
      }
    break;
    case StmtWhile:
      labelStart = useLabel();
      labelEnd = useLabel();

      src.type = addrString;
      src.content.name = labelStart;

      insertQuad(Label, src, empty, empty);

      codeGen(t->child[0]);
      condition = current;

      tgt.type = addrString;
      tgt.content.name = labelEnd;  

      insertQuad(IFfalse, condition, tgt, empty);

      freeRegisters(condition.content.name);

      codeGen(t->child[1]);

      src.type = addrString;
      src.content.name = labelStart;

      insertQuad(Jump, src, empty, empty);

      src.type = addrString;
      src.content.name = labelEnd;

      insertQuad(Label, src, empty, empty);
    break;
    case StmtReturn:
      if (t->child[0] != NULL) {
        Address rtn;

        codeGen(t->child[0]);

        regTemp = useRegister(2);
        rtn.type = addrString;
        rtn.content.name = regTemp;

        insertQuad(Move, current, rtn, empty);   
        insertQuad(Return, rtn, empty, empty);

        if (current.type == addrString) {
          freeRegisters(current.content.name);
        }

        freeRegisters(rtn.content.name);
      } else {
        insertQuad(Return, empty, empty, empty);
      }
    break;
  }
}

/*  expGen() → [TODO]  */
static void expGen(TreeNode *t) {
  Address src, tgt, dst;
  Address empty;
  char *regTemp;

  empty.type = addrVoid;

  if (t == NULL) return;

  switch (t->kind.exp) {
    case ExpOperator:
      Address left, right;

      if (t->child[0]->nodekind == NodeExpression && t->child[0]->kind.exp == ExpCall) {
        codeGen(t->child[0]);
        left = current;

        codeGen(t->child[1]);
        right = current;
      } else if (t->child[1]->nodekind == NodeExpression && t->child[1]->kind.exp == ExpCall) {
        codeGen(t->child[1]);
        right = current;

        codeGen(t->child[0]);
        left = current;
      } else {
        codeGen(t->child[0]);
        left = current;
  
        codeGen(t->child[1]);
        right = current;
      }

      regTemp = useRegister(-1);
      current.type = addrString;
      current.content.name = strdup(regTemp);

      insertQuad(tokenToOperation(t->attr.operator), left, right, current);
      break;
    case ExpConst:
      current.type = addrConst;
      current.content.value = t->attr.value;
      break;
    case ExpID:
      if (t->flags.isArray) {
        src.type = addrString;
        src.content.name = strdup(t->scope);	

        tgt.type = addrString;
        tgt.content.name = strdup(t->attr.arrayAttr.name);	

        regTemp = useRegister(-1);
        dst.type = addrString;
        dst.content.name = strdup(regTemp);

        insertQuad(LoadVAR, src, tgt, dst);

        codeGen(t->child[0]);

        Address temp;

        regTemp = useRegister(-1);
        temp.type = addrString;
        temp.content.name = strdup(regTemp);

        insertQuad(Add, dst, current, temp);

        regTemp = useRegister(-1);
        current.type = addrString;
        current.content.name = strdup(regTemp);

        insertQuad(LoadARRAY, src, temp, current);

        freeRegisters(temp.content.name);
      } else {
        src.type = addrString;
        src.content.name = strdup(t->scope);
        
        tgt.type = addrString;
        tgt.content.name = strdup(t->attr.name);

        regTemp = useRegister(-1);
        current.type = addrString;
        current.content.name = strdup(regTemp);
        
        insertQuad(LoadVAR, src, tgt, current);
      }
      break;
    case ExpCall:
      int addsub = 0;
      int paramCounter = 0;
      Address rf_temp;
      TreeNode *parameters = t->child[0];
      
      while (parameters != NULL) {
        paramCounter++;
        
        if (parameters->nodekind == NodeStatement) stmtGen(parameters);
        else if (parameters->nodekind = NodeExpression) expGen(parameters);
        
        if (current.type == addrConst) {
          regTemp = useRegister(-1);
          dst.type = addrString;
          dst.content.name = strdup(regTemp);
          
          insertQuad(Move, current, dst, empty);
          insertQuad(Param, dst, empty, empty);
        } else {
          insertQuad(Param, current, empty, empty);
        }
        
        parameters = parameters->sibling;
      }
      
      src.type = addrString;
      src.content.name = strdup(t->attr.name);
      
      tgt.type = addrConst;
      tgt.content.value = paramCounter;
      
      dst.type = addrVoid;

      if (strcmp(t->attr.name, "output") == 0 ||
          strcmp(t->attr.name, "loadHD") == 0 ||
          strcmp(t->attr.name, "LCDwrite") == 0 ||
          strcmp(t->attr.name, "storeHD") == 0 ||
          strcmp(t->attr.name, "HDtoIM") == 0
        )
      {
        addsub = 1;
      } else {
        pushRegister(paramCounter);
      }

      insertQuad(Call, src, tgt, dst);
      
      popRegister(paramCounter, addsub);

      if (t->type != Void) {
        if (strcmp(src.content.name, "input") == 0) {
          regTemp = useRegister(3);
        } else if (strcmp(src.content.name, "loadHD") == 0) {
          regTemp = useRegister(4);
        } else {
          regTemp = useRegister(2);
        }

        rf_temp.type = addrString;
        rf_temp.content.name = strdup(regTemp);
        
        regTemp = useRegister(-1);
        current.type = addrString;
        current.content.name = strdup(regTemp);

        insertQuad(Move, rf_temp, current, empty);
        freeRegisters(rf_temp.content.name);
      }
      break;
  }
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

  printf("\n> Intermediate Code Synthesis ----------------------------------------------");
  printBars();

  fprintf(file, "%s\n", source);

  QuadList *list = quadruples;
  while (list != NULL) {
    char str[32];

    fprintf(file, "%s|", opString[list->op]);

    sprintf(str, "\t> %d:\t%-10s ", counter, opString[list->op]);
    traceMidCode(str);


    sprintf(str, "→");
    traceMidCode(str);
    

    sprintf(str, "   ");
    traceMidCode(str);

    switch (list->src.type) {
      case addrVoid:
        fprintf(file, "---|");
        // traceMidCode("-----  ");   // Optional hifens
        break;
      case addrConst:
        fprintf(file, "%d|", list->src.content.value);
        sprintf(str, "%-6d ", list->src.content.value);
        traceMidCode(str);
        break;
      case addrString:
        fprintf(file, "%s|", list->src.content.name);
        sprintf(str, "%-6s ", list->src.content.name);
        traceMidCode(str);
        break;
    }
    switch (list->tgt.type) {
      case addrVoid:
        fprintf(file, "---|");
        // traceMidCode("-----  ");   // Optional hifens
        break;
      case addrConst:
        fprintf(file, "%d|", list->tgt.content.value);
        sprintf(str, "%-6d ", list->tgt.content.value);
        traceMidCode(str);
        break;
      case addrString:
        fprintf(file, "%s|", list->tgt.content.name);
        sprintf(str, "%-6s ", list->tgt.content.name);
        traceMidCode(str);
        break;
    }
    switch (list->dst.type) {
      case addrVoid:
        fprintf(file, "---");
        // traceMidCode("-----  ");   // Optional hifens
        break;
      case addrConst:
        fprintf(file, "%d", list->dst.content.value);
        sprintf(str, "%-6d ", list->dst.content.value);
        traceMidCode(str);
        break;
      case addrString:
        fprintf(file, "%s", list->dst.content.name);
        sprintf(str, "%-6s ", list->dst.content.name);
        traceMidCode(str);
        break;
    }
    fprintf(file, "\n");
    traceMidCode("\n");

    counter++;

    list = list->next;
  }
  fclose(file);
  newLine();
}

/*  midCodeGenerate() → Call codeGen() and [TODO] ---> Traceable    */
void midCodeGenerate(TreeNode *AST)  {
  freeRegisters(NULL);
  codeGen(AST);
  printQuadruplesList();
}