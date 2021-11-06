/*
 * Lexical Grammar of calc
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 21-Oct-2015 13:20 (eg)
 * Last file update:  4-Nov-2015 17:09 (eg)
 */

%{
#include <stdlib.h>
#include <string.h>

#include "calc.h"
#include "syntax.h"

#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line; \
    yylloc.first_column = yylloc.last_column; \
    for(char* ptr = yytext; *ptr; ptr++) { \
        if(*ptr  == '\n') { \
            yylloc.last_line++; \
            yylloc.last_column = 1; \
        } \
        else { \
        	yylloc.last_column++; \
        } \
    }\
    strcat(yylloc.code, yytext);
%}

%option yylineno


%%

	/* ---- Numbers ---- */

[0-9]+u32 			{ yylval.number.value = atol(yytext); yylval.number.size = 32; return NUMBER; }
[0-9]+u16 			{ yylval.number.value = atol(yytext); yylval.number.size = 16; return NUMBER; }
[0-9]+u8 			{ yylval.number.value = atol(yytext); yylval.number.size = 8; return NUMBER; }
[0-9]+ 				{ yylval.number.value = atol(yytext); yylval.number.size = 0; return NUMBER; }

\'\\a\'	{ yylval.number.value = '\a'; yylval.number.size = 0; return NUMBER; }
\'\\b\'	{ yylval.number.value = '\b'; yylval.number.size = 0; return NUMBER; }
\'\\e\'	{ yylval.number.value = '\e'; yylval.number.size = 0; return NUMBER; }
\'\\f\'	{ yylval.number.value = '\f'; yylval.number.size = 0; return NUMBER; }
\'\\n\'	{ yylval.number.value = '\n'; yylval.number.size = 0; return NUMBER; }
\'\\r\'	{ yylval.number.value = '\r'; yylval.number.size = 0; return NUMBER; }
\'\\t\'	{ yylval.number.value = '\t'; yylval.number.size = 0; return NUMBER; }
\'\\v\'	{ yylval.number.value = '\v'; yylval.number.size = 0; return NUMBER; }
\'\\'\'	{ yylval.number.value = '\''; yylval.number.size = 0; return NUMBER; }
\'\\\\\'	{ yylval.number.value = '\\'; yylval.number.size = 0; return NUMBER; }
\'[^\']\' { yylval.number.value = yytext[1]; yylval.number.size = 0; return NUMBER; }
\"[^\"]*\" { yylval.var = strndup(yytext + 1, strlen(yytext) - 2); return STRING; }


	/* ---- Single char tokens ---- */

[-()\[\]<>=+*/;{}.&~,_:]			{ return *yytext; }

	/* ---- "complex" tokens ---- */
">="				{ return GE; }
"<="            		{ return LE; }
"=="            		{ return EQ; }
"!="            		{ return NE; }
"+=" { return APL; }
"-=" { return AMN; }
"*=" { return AML; }
"/=" { return ADV; }
"++" { return INC; }
"--" { return DEC; }
"&&" { return AND; }
"||" { return OR; }
"<<" { return SHL; }
">>" { return SHR; }

"print"         		{ return KPRINT; }
"while" { return KWHILE; }
"if" { return KIF; }
"else" { return KELSE; }
"read" { return KREAD; }
"for" { return KFOR; }
"do" { return KDO; }
"var" { return KVAR; }
"return" { return KRETURN; }
"func" { return KFUNC; }
"proc" { return KPROC; }
"break" { return KBREAK; }
"continue" { return KCONTINUE; }
"const" { return KCONST; }
"type" { return KTYPE; }
"typeof" { return KTYPEOF; }
"sizeof" { return KSIZEOF; }
"struct" { return KSTRUCT; }

	/* ---- Idents ---- */
[a-zA-Z_][a-zA-Z0-9_]*			{ yylval.var = strdup(yytext); return IDENT; }


	/* ---- Spaces ----*/
[ \t\r]+        		{ ; }
\n        		{ yylloc.code[0] = 0 ; }

	/* ---- Comments ---- */
#.*$				{ ; }


	/* ---- Other characters ---- */
.				{ error_msg(NULL, "unknown character '%c'\n", *yytext); }

%%

