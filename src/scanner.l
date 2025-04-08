/*-------------------------------------------------------------------------------------------------/
 *  Lex Specifications & Scanner for a C- Compiler
 *  File: scanner.l
 *---------------------------------*/

%option noyywrap yylineno

%{

#include "globals.h"

#include "parser.tab.h"

%}

digit       	[0-9]
number        {digit}+

letter      	[a-zA-Z]
id            {letter}({letter}|{digit})*

whitespace  	[ \t\r]+
newline       \n

%x COMMENT

%%

"void"		      return VOID;
"int"           return INT;

"if"            return IF;
"else"          return ELSE;
"while"         return WHILE;
"return"        return RETURN;

"+"             return SUM;
"-"             return SUB;
"*"             return MUL;
"/"             return DIV;

"="             return GET;
">"	            return MORE;
"<"             return LESS;
">="	          return EQUALMORE;
"<="	          return EQUALLESS;
"=="            return EQUAL;
"!="            return DIFER;

","             return COMMA;
";"             return SEMI;

"("             return OPARENTHESIS;
")"             return CPARENTHESIS;
"["             return OBRACKETS;
"]"             return CBRACKETS;
"{"             return OKEYS;
"}"             return CKEYS;

"/*"            { BEGIN(COMMENT); }

<COMMENT>{
  "*/"        { BEGIN(INITIAL); }
  {newline}   { /* skip but count newline */ }
  .           { /* skip content */ }
}

{whitespace}    { /* skip whitespace */ }
{newline}       { /* skip but count newline */ }

{digit}+        { yylval.num = atoi(yytext); return NUM; }
{letter}+ 		  { yylval.id = strdup(yytext); return ID; }

.               {
                  printf("> Error [ Lexical ]:\n    Line (%d) - Unidentified token [ %s ]\n", yylineno, yytext);
                  return ERROR;
                }

<<EOF>>         return 0;

%%