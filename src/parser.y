/*-------------------------------------------------------------------------------------------------/
 *  YACC-Bison Specifications & Parser for a C- Compiler
 *  File: parser.y
 *---------------------------------*/

%debug

%{

#include "globals.h"

bool firstExecution = true;

void yyerror(const char* s);

%}

%define parse.error verbose

%start program

%token INT VOID
%token IF ELSE WHILE RETURN
%left SUM SUB
%left MUL DIV
%token GET MORE LESS EQUALMORE EQUALLESS EQUAL DIFER
%token COMMA SEMI
%token OPARENTHESIS CPARENTHESIS OBRACKETS CBRACKETS OKEYS CKEYS

%union {
    int num;
    char* id;
}

%token <num> NUM
%token <id> ID

%%

program:
    declaration_list
;

declaration_list:
    declaration_list declaration | declaration
;

declaration:
    variable_declaration | function_declaration | error SEMI { yyerror("Recovered from lexical error in an declatarion"); yyerrok; }
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
  | error SEMI { yyerror("Recovered from lexical error in an statement"); yyerrok; }
;

expression_stmt:
    expression SEMI
  | SEMI
  | error SEMI { yyerror("Recovered from lexical error in an expression"); yyerrok; }
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
  | RETURN error SEMI { yyerror("Recovered from lexical error in an return statement"); yyerrok; }
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
  | error CPARENTHESIS { yyerror("Recovered from lexical error in an arguments call"); yyerrok; }
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

