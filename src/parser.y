/*-------------------------------------------------------------------------------------------------/
 *  YACC-Bison Specifications & Parser for a C- Compiler
 *  File: parser.y
 *---------------------------------*/

%debug

%{

#include "globals.h"
#include "utils.h"

bool firstExecution = true;

void yyerror(const char* s);

%}

%code requires {
  #include "globals.h"
}

%define parse.error verbose

%union {
    int num;
    char* id;
    TreeNode *tree;
}

%token INT VOID
%token IF ELSE WHILE RETURN
%left SUM SUB
%left MUL DIV
%token GET MORE LESS EQUALMORE EQUALLESS EQUAL DIFER
%token COMMA SEMI
%token OPARENTHESIS CPARENTHESIS OBRACKETS CBRACKETS OKEYS CKEYS

%token <num> NUM
%token <id> ID

%type <tree> declaration_list declaration
%type <tree> variable_declaration function_declaration
%type <tree> statement expression

%start program

%%

program:
    declaration_list 
    ;

declaration_list:
    declaration_list declaration | declaration
    ;

declaration:
    variable_declaration | function_declaration | error SEMI { yyerror("Recovered from error in an declaration"); yyerrok; }
    ;

variable_declaration:
    type ID SEMI
  | type ID OBRACKETS NUM CBRACKETS SEMI
  ;

type:
    INT
  | VOID
  ;

function_declaration:
    type ID OPARENTHESIS function_params CPARENTHESIS compound_stmt
  ;

function_params:
    parameter_list
  | VOID
  ;

parameter_list:
    parameter_list COMMA parameter
  | parameter
  ;

parameter:
    type ID
  | type ID OBRACKETS CBRACKETS
  ;

compound_stmt:
    OKEYS local_declarations statement_list CKEYS
  ;

local_declarations:
    local_declarations variable_declaration
  | /* Empty */
  ;

statement_list:
    statement_list statement
  | /* Empty */
  ;

statement:
    expression_stmt
  | compound_stmt
  | selection_stmt
  | iteration_stmt
  | return_stmt
  | error SEMI { yyerror("Recovered from error in an statement"); yyerrok; }
  ;

expression_stmt:
    expression SEMI
  | SEMI
  | error SEMI { yyerror("Recovered from error in an expression"); yyerrok; }
  ;

selection_stmt:
    IF OPARENTHESIS expression CPARENTHESIS statement
  | IF OPARENTHESIS expression CPARENTHESIS statement ELSE statement
  ;

iteration_stmt:
    WHILE OPARENTHESIS expression CPARENTHESIS statement
  ;

return_stmt:
    RETURN SEMI
  | RETURN expression SEMI
  | RETURN error SEMI { yyerror("Recovered from error in an return statement"); yyerrok; }
  ;

expression:
    variable GET expression
  | simple_expression
  ;

variable:
    ID
  | ID OBRACKETS expression CBRACKETS
  ;

simple_expression:
    add_expression relational add_expression
  | add_expression
  ;

relational:
    MORE | LESS | EQUALMORE | EQUALLESS | EQUAL | DIFER
  ;

add_expression:
    add_expression sum_sub term
  | term
  ;

sum_sub:
    SUM | SUB
  ;

term:
    term mul_div factor
  | factor
  ;

mul_div:
    MUL | DIV
  ;

factor:
    OPARENTHESIS expression CPARENTHESIS
  | variable
  | call
  | NUM
  ;

call:
    ID OPARENTHESIS args CPARENTHESIS
  ;

args:
    argument_list
  | /* Empty */
  | error CPARENTHESIS { yyerror("Recovered from error in an arguments call"); yyerrok; }
  ;

argument_list:
    argument_list COMMA expression
  | expression
  ;

%%

void yyerror(const char *msg) {
  if (firstExecution == true) {
    firstExecution = false;
    printf("> Error [ Syntax ]:\n");
  }
  printf("    Line (%d) - %s.\n", yylineno, msg);
}

