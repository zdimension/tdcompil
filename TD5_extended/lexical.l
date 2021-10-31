/*
 * Lexical Grammar of calc
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 21-Oct-2015 13:20 (eg)
 * Last file update:  4-Nov-2015 17:09 (eg)
 */

%{
#include <stdlib.h>
#include <strings.h>

#include "calc.h"
#include "syntax.h"

%}

%option yylineno

%%

	/* ---- Numbers ---- */

[0-9]+ 			{ yylval.value = atol(yytext); return NUMBER; }

\'\\a\'	{ yylval.value = '\a'; return NUMBER; }
\'\\b\'	{ yylval.value = '\b'; return NUMBER; }
\'\\e\'	{ yylval.value = '\e'; return NUMBER; }
\'\\f\'	{ yylval.value = '\f'; return NUMBER; }
\'\\n\'	{ yylval.value = '\n'; return NUMBER; }
\'\\r\'	{ yylval.value = '\r'; return NUMBER; }
\'\\t\'	{ yylval.value = '\t'; return NUMBER; }
\'\\v\'	{ yylval.value = '\v'; return NUMBER; }
\'\\'\'	{ yylval.value = '\''; return NUMBER; }
\'\\\\\'	{ yylval.value = '\\'; return NUMBER; }
\'[^\']\' { yylval.value = yytext[1]; return NUMBER; }
\"[^\"]*\" { yylval.var = strndup(yytext + 1, strlen(yytext) - 2); return STRING; }


	/* ---- Single char tokens ---- */

[-()\[\]<>=+*/;{}.&~,]			{ return *yytext; }

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

	/* ---- Idents ---- */
[a-zA-Z]+			{ yylval.var = strdup(yytext); return IDENT; }


	/* ---- Spaces ----*/
[ \t\r\n]+        		{ ; }

	/* ---- Comments ---- */
#.*$				{ ; }


	/* ---- Other characters ---- */
.				{ error_msg(NULL, "unknown character '%c'\n", *yytext); }

%%

