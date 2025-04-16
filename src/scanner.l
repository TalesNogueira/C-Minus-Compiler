/*-------------------------------------------------------------------------------------------------/
 *  Lex Specifications & Scanner for a C- Compiler
 *  File: scanner.l
 *---------------------------------*/

%option yylineno

%{

#include "globals.h"
#include "utils.h"
#include "parser.tab.h"

/*  firstExecution → Variable to track the first scanner run (used in TraceScan)  */
static bool firstExecution = true;

/*  currentToken → Stores the numeric value of the current token read (return from yylex())   */
static int currentToken = 0;

/*  getToken() → Call yylex(), trace the TOKEN and then return it on  */
int getToken(void);

/*  lexicalError() → Print a lexical error message  */
static void lexicalError(const char* msg);

%}

digit       	[0-9]
number        {digit}+

letter      	[a-zA-Z]
id            {letter}({letter}|{digit})*

whitespace  	[ \t\r]+
newline       \n

%x COMMENT

%%

"void"        return VOID;
"int"         return INT;

"if"          return IF;
"else"        return ELSE;
"while"       return WHILE;
"return"      return RETURN;

"+"           return SUM;
"-"           return SUB;
"*"           return MUL;
"/"           return DIV;

"="           return GET;
">"           return MORE;
"<"           return LESS;
">="          return EQUALMORE;
"<="          return EQUALLESS;
"=="          return EQUAL;
"!="          return DIFER;

","           return COMMA;
";"           return SEMI;

"("           return OPARENTHESIS;
")"           return CPARENTHESIS;
"["           return OBRACKETS;
"]"           return CBRACKETS;
"{"           return OKEYS;
"}"           return CKEYS;

"/*"          { BEGIN(COMMENT); }

<COMMENT>{
  "*/"        { BEGIN(INITIAL); }
  .           { /* skip content */ }
  {newline}   { /* skip but count newline */ }
}

{whitespace}  { /* skip whitespace */ }

{newline}     {
                if (TraceScan && !firstExecution) {
                  newLine();
                  printf("--- Line %d\n", yylineno);
                }
                /* skip but count newline */
              }

{digit}+      { yylval.num = atoi(yytext); return NUM; }
{letter}+     { yylval.id = strdup(yytext); return ID; }

.             { lexicalError(yytext); }

<<EOF>>       return 0;

%%

/*  traceScanner() → Check TraceScan and print out line jumps and TOKENS obtained during yylex()  */
static void traceScanner() {
  if (TraceScan) {
    if (firstExecution) {
      firstExecution = false;
      newLine();
      printf("> Tracing Scanner ---------------------------------------------\n");
      newLine();
      printf("--- Line %d\n", yylineno);
    }
    if (currentToken == 0) {
      printf("END OF FILE → (%s)\n", tokenToString(currentToken));
    } else {
      printf("%s → (%s)\n", yytext, tokenToString(currentToken));
    }
  }
}

/*  getToken() → Call yylex(), treat the TOKEN and then return ---> Traceable  */
int getToken(void)
{
  currentToken = yylex();
  traceScanner();
  return currentToken;
}

/*  lexicalError() → Print a lexical error message  */
static void lexicalError(const char* msg) {
    printf("> Lexical Error\n     Line %d - Unidentified token. [ %s ]\n", yylineno, msg);
}