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
[0-9]+\.?[0-9]*(e[+-]?[0-9]+)?	{ yylval.value = atof(yytext); return NUMBER; }
\.[0-9]+(e[+-]?[0-9]+)?		{ yylval.value = atof(yytext); return NUMBER; }

	/* ---- Single char tokens ---- */

[-()\[\]<>=+*/;{}.&]			{ return *yytext; }

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

"print"         		{ return KPRINT; }
"while" { return KWHILE; }
"if" { return KIF; }
"else" { return KELSE; }
"read" { return KREAD; }
"for" { return KFOR; }
"do" { return KDO; }
"dim" { return KDIM; }

	/* ---- Idents ---- */
[a-zA-Z]+			{ yylval.var = strdup(yytext); return IDENT; }


	/* ---- Spaces ----*/
[ \t\r\n]+        		{ ; }

	/* ---- Comments ---- */
#.*$				{ ; }


	/* ---- Other characters ---- */
.				{ error_msg("unknown character '%c'\n", *yytext); }

%%

