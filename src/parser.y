/*-------------------------------------------------------------------------------------------------/
 *  YACC-Bison Specifications & Parser for a C- Compiler
 *  File: parser.y
 *---------------------------------*/

%debug

%{

#include "globals.h"
#include "utils.h"

/*  yylex() → Call getToken()  */
static int yylex(void);

/*  yyerror() → Print Syntax error messages  */
static void yyerror(const char *msg);

/*  globalScope → Global scope of analyzed tree node  */
char *globalScope = "global";

/*  AbstractSyntaxTree → Pointer to TreeNode base of Abstract Syntax Tree    */
TreeNode *abstractSyntaxTree;

%}

%code requires {
  #include "globals.h"
}

%define parse.error verbose
%expect 1

%union {
  int op;
  int num;

  struct {
    char *name;
    int lineno;
  } id;
  
  TreeNode *node;
  ExpType type;
}

%token <type> INT VOID

%token IF WHILE RETURN

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%left ADD SUB
%left MUL DIV

%right GET
%nonassoc MORE LESS EQUALMORE EQUALLESS EQUAL DIFER

%token COMMA SEMI
%token OPARENTHESIS CPARENTHESIS
%token OBRACKETS CBRACKETS
%token OKEYS CKEYS

%token <num> NUM
%token <id> ID

%type <node> declaration_list declaration
  %type <node> variable_declaration
  %type <node> function_declaration
    %type <node> function_params
      %type <node>  parameter_list parameter
    %type <node> compound_stmt
      %type <node> local_declarations
      %type <node> statement_list statement
        %type <node> expression_stmt 
          %type <node> variable expression
          %type <node> simple_expression add_expression term factor
            %type <node> call args argument_list
        %type <node> selection_stmt iteration_stmt return_stmt

%type <op> relational sum_sub mul_div
%type <type> type

%start program

%%

program:
  declaration_list {
    abstractSyntaxTree = $1;
  }
;

declaration_list:
  declaration_list declaration {
    $$ = addSibling($1, $2);
  }
| declaration {
    $$ = $1;
  }
;

declaration:
  variable_declaration {
    $$ = $1;
  }
| function_declaration {
    $$ = $1;
  }
;

variable_declaration:
  type ID SEMI {
    TreeNode *t = newDeclNode(DeclVariable);
    t->type = $1;
    t->flags.isArray = false;
    t->attr.name = strdup($2.name);
    t->scope = strdup(globalScope);
    $$ = t;
  }
| type ID OBRACKETS NUM CBRACKETS SEMI {
    TreeNode *t = newDeclNode(DeclArray);
    t->type = $1;
    t->flags.isArray = true;
    t->attr.arrayAttr.name = strdup($2.name);
    t->attr.arrayAttr.size = $4;
    t->scope = strdup(globalScope);
    $$ = t;
  }
| error SEMI {
    yyerror("Invalid variable declaration");
    yyerrok;
    $$ = NULL;
  }
;

type:
  INT   { $$ = Integer; }
| VOID  { $$ = Void; }
;

function_declaration:
  type ID OPARENTHESIS function_params CPARENTHESIS compound_stmt {
    TreeNode *t = newDeclNode(DeclFunction);
    t->type = $1;
    t->attr.name = strdup($2.name);
    t->scope = strdup("global");
    t->lineno = $2.lineno; 
    insertScope(t->child[0] = $4, t->attr.name);
    insertScope(t->child[1] = $6, t->attr.name);
    $$ = t;
  }
;

function_params:
  parameter_list  { $$ = $1; }
| VOID  { $$ = NULL; }
;

parameter_list: 
  parameter_list COMMA parameter {
    $$ = addSibling($1, $3);
  }
| parameter {
    $$ = $1;
  }
;

parameter:
  type ID {
    TreeNode *t = newDeclNode(DeclParameter);
    t->type = $1;
    t->attr.name = strdup($2.name);
    $$ = t;
  }
| type ID OBRACKETS CBRACKETS {
    TreeNode *t = newDeclNode(DeclParameter);
    t->type = $1;
    t->flags.isArray = true;
    t->attr.arrayAttr.name = strdup($2.name);
    $$ = t;
  }
;

compound_stmt:
OKEYS local_declarations statement_list CKEYS {
    TreeNode *t = newStmtNode(StmtCompound);
    t->child[0] = $2;
    t->child[1] = $3;
    $$ = t;
  }
| OKEYS local_declarations error CKEYS {
      yyerror("Invalid compound statement → error in statements");
      yyerrok;
      $$ = NULL;
  }
| OKEYS error CKEYS {
      yyerror("Invalid compound statement → error in statements");
      yyerrok;
      $$ = NULL;
  }
;

local_declarations:
  local_declarations variable_declaration {
    if ($1 != NULL) {
      $$ = addSibling($1, $2);
    } else {
      $$ = $2;
    }
  }
| /* Empty */ {
    $$ = NULL;
  }
;

statement_list:
  statement_list statement {
    if ($1 != NULL) {
      $$ = addSibling($1, $2);
    } else {
      $$ = $2;
    }
  }
| /* Empty */ {
    $$ = NULL;
  }
;

statement:
  expression_stmt { $$ = $1; }
| compound_stmt   { $$ = $1; }
| selection_stmt  { $$ = $1; }
| iteration_stmt  { $$ = $1; }
| return_stmt     { $$ = $1; }
;

expression_stmt:
  expression SEMI {
    $$ = $1;
  }
| SEMI {
    $$ = NULL;
  }
;

selection_stmt:
  IF OPARENTHESIS expression CPARENTHESIS statement %prec LOWER_THAN_ELSE{
    TreeNode *t = newStmtNode(StmtIf);
    t->child[0] = $3;   // if   → ( Expression )
    t->child[1] = $5;   // if   → { Statement }
    t->child[2] = NULL; // else → NULL
    $$ = t;
  }
| IF OPARENTHESIS expression CPARENTHESIS statement ELSE statement {
    TreeNode *t = newStmtNode(StmtIf);
    t->child[0] = $3;   // if   → ( Expression )
    t->child[1] = $5;   // if   → { Statement }
    t->child[2] = $7;   // else → { Statement }
    $$ = t;
  }
| IF error CPARENTHESIS statement {
    yyerror("Invalid selection (IF-ELSE) statement");
    yyerrok;
    $$ = NULL;  // Continua o código após o erro
  }
;

iteration_stmt:
  WHILE OPARENTHESIS expression CPARENTHESIS statement {
    TreeNode *t = newStmtNode(StmtWhile);
    t->child[0] = $3;   // while → ( Expression )
    t->child[1] = $5;   // while → { Statement }
    $$ = t;
  }
| WHILE error CPARENTHESIS statement {
    yyerror("Invalid iteration (WHILE) statement");
    yyerrok;
    $$ = NULL;
  }
;

return_stmt:
  RETURN SEMI {
    TreeNode *t = newStmtNode(StmtReturn);
    t->child[0] = NULL; // return → NULL
    $$ = t;
  }
| RETURN expression SEMI {
    TreeNode *t = newStmtNode(StmtReturn);
    t->child[0] = $2;   // return → Expression
    $$ = t;
  }
;

expression:
  variable GET expression {
    TreeNode *t = newStmtNode(StmtAssign);
    t->child[0] = $1;       // Expression → Variable
    t->attr.operator = GET; // Expression → Operator (=)
    t->child[1] = $3;       // Expression → Expression
    $$ = t;
  }
| simple_expression {
    $$ = $1;
  }
;

variable:
  ID {
    TreeNode *t = newExpNode(ExpID);
    t->type = Integer;
    t->flags.isArray = false;
    t->attr.name = strdup($1.name);  // Variable → <id> (Name)
    $$ = t;
  }
| ID OBRACKETS expression CBRACKETS {
    TreeNode *t = newExpNode(ExpID);
    t->type = Integer;
    t->flags.isArray = true;
    t->attr.arrayAttr.name = strdup($1.name);  // Variable → <id> (Name)
    t->child[0] = $3;                          // Variable → [ Expression ]
    $$ = t;
 }
;

simple_expression:
  add_expression relational add_expression {
    TreeNode *t = newExpNode(ExpOperator);
    t->type = Boolean;
    t->child[0] = $1;       // Simple Expression → Expression
    t->attr.operator = $2;  // Simple Expression → Relational Symbol
    t->child[1] = $3;       // Simple Expression → Expression
    $$ = t;
  }
| add_expression {
    $$ = $1;
  }
;

relational:
  MORE      { $$ = MORE; }
| LESS      { $$ = LESS; }
| EQUALMORE { $$ = EQUALMORE; }
| EQUALLESS { $$ = EQUALLESS; }
| EQUAL     { $$ = EQUAL; }
| DIFER     { $$ = DIFER; }
;

add_expression:
  add_expression sum_sub term {
    TreeNode *t = newExpNode(ExpOperator);
    t->type = Integer;
    t->child[0] = $1;       // Add Expression → Add Expression
    t->attr.operator = $2;  // Add Expression → Operator (+ or -)
    t->child[1] = $3;       // Add Expression → Term
    $$ = t;
  }
| term {
    $$ = $1;
  }
;

sum_sub:
  ADD { $$ = ADD; }
| SUB { $$ = SUB; }
;

term:
  term mul_div factor {
    TreeNode *t = newExpNode(ExpOperator);
    t->type = Integer;
    t->child[0] = $1;       // Term → Term
    t->attr.operator = $2;  // Term → Operator (* or /)
    t->child[1] = $3;       // Term → Factor
    $$ = t;
  }
| factor {
    $$ = $1;
  }
;

mul_div:
  MUL { $$ = MUL; }
| DIV { $$ = DIV; }
;

factor:
  OPARENTHESIS expression CPARENTHESIS {
    $$ = $2;
  }
| variable {
    $$ = $1;
  }
| call {
    $$ = $1;
  }
| NUM {
    TreeNode *t = newExpNode(ExpConst);
    t->type = Integer;
    t->attr.value = $1;
    $$ = t;
  }
;

call:
  ID OPARENTHESIS args CPARENTHESIS {
    TreeNode *t = newExpNode(ExpCall);
    t->attr.name = strdup($1.name);
    t->child[0] = $3; // Call → Arguments
    $$ = t;
  }
;

args:
  argument_list {
    $$ = $1;
  }
| /* Empty */ {
    $$ = NULL;
  }
;

argument_list:
  argument_list COMMA expression {
    $$ = addSibling($1, $3);
  }
| expression {
     $$ = $1;
  }
;

%%

/*  traceParser() → Check TraceParse and print out the AST  */
static void traceParser(void) {
  if (TraceParse) {
    newLine();
    printf("> Syntax Analysis ----------------------------------------------------------");
    printBars();
    printTree(abstractSyntaxTree);
    printf("\n> END OF FILE → (EOF)\n");
  } else {
    printf("\n> Syntax Analysis completed.\n");
  }
}

 /*  syntaxAnalysis() → Call yyparse() and build the AST ---> Traceable    */
void syntaxAnalysis(void) {
  yyparse();
  traceParser();
}

/*  yylex() → Call getToken()  */
static int yylex(void) {
  return getToken();
}

/*  yyerror() → Print Syntax error messages  */
static void yyerror(const char *msg) {
  printBars(); 
  printf("> Syntax Error\n     Line %d - %s.", yylineno, msg);
  printBars();
  newLine();
}