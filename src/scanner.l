/*-------------------------------------------------------------------------------------------------/
 *  Lex Specifications & Scanner for a C- Compiler
 *  File: scanner.l
 *---------------------------------*/

%option noyywrap yylineno

%{

#include "globals.h"

%}

digit       	[0-9]
number        {digit}+

letter      	[a-zA-Z]
id            {letter}({letter}|{digit})*

whitespace  	[ \t\r]+
newline       \n

%x COMMENT

%%

"int"           { printf("TOKEN: INT\n"); }
"void"		      { printf("TOKEN: VOID\n"); }

"if"            { printf("TOKEN: IF\n"); }
"else"          { printf("TOKEN: ELSE\n"); }
"while"         { printf("TOKEN: WHILE\n"); }
"return"        { printf("TOKEN: RETURN\n"); }

"+"             { printf("TOKEN: SUM (+)\n"); }
"-"             { printf("TOKEN: SUB (-)\n"); }
"*"             { printf("TOKEN: MUL (*)\n"); }
"/"             { printf("TOKEN: DIV (/)\n"); }

"="             { printf("TOKEN: GET (=)\n"); }
">"	            { printf("TOKEN: MORE (>)\n"); }
"<"             { printf("TOKEN: LESS (>)\n"); }
">="	          { printf("TOKEN: EQUALMORE (>=)\n"); }
"<="	          { printf("TOKEN: EQUALLESS (<=)\n"); }
"=="            { printf("TOKEN: EQUAL (==)\n"); }
"!="            { printf("TOKEN: DIFER (!=)\n"); }

","             { printf("TOKEN: COMMA (,)\n"); }
";"             { printf("TOKEN: SEMICOLON (;)\n"); }

"("             { printf("TOKEN: OPARENTHESIS\n"); }
")"             { printf("TOKEN: CPARENTHESIS\n"); }
"["             { printf("TOKEN: OBRACKETS\n"); }
"]"             { printf("TOKEN: CBRACKETS\n"); }
"{"             { printf("TOKEN: OKEYS\n"); }
"}"             { printf("TOKEN: CKEYS\n"); }

"/*"            { BEGIN(COMMENT); }

<COMMENT>{
  "*/"        { BEGIN(INITIAL); }
  {newline}   { /* skip but count newline */ }
  .           { /* skip content */ }
}

{whitespace}    { /* skip whitespace */ }
{newline}       { /* skip but count newline */ }

{digit}+        { printf("TOKEN: NUMBER (%s)\n", yytext); }
{letter}+ 		  { printf("TOKEN: ID (%s)\n", yytext); }

.               { printf("> Error [ Lexical ]:\n    Line (%d) - Unidentified token [ %s ]\n", yylineno, yytext); }
<<EOF>>         return 0;

%%