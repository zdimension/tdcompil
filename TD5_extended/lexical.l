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

[0-9]*([0-9]\.|\.[0-9])[0-9]*([Ee][-+]?[0-9]+)? { yylval.number_float = (typeof(yylval.number_float)){.value = atof(yytext)}; return FLOAT; }
[0-9]+u[1-9][0-9]*	{ sscanf(yytext, "%du%d", &yylval.number.value, &yylval.number.size); return NUMBER; }
[0-9]+ 				{ yylval.number = (typeof(yylval.number)){.value = atol(yytext)}; return NUMBER; }

\'\\a\'		{ yylval.number = (typeof(yylval.number)){.value = '\a'}; return NUMBER; }
\'\\b\'		{ yylval.number = (typeof(yylval.number)){.value = '\b'}; return NUMBER; }
\'\\e\'		{ yylval.number = (typeof(yylval.number)){.value = '\e'}; return NUMBER; }
\'\\f\'		{ yylval.number = (typeof(yylval.number)){.value = '\f'}; return NUMBER; }
\'\\n\'		{ yylval.number = (typeof(yylval.number)){.value = '\n'}; return NUMBER; }
\'\\r\'		{ yylval.number = (typeof(yylval.number)){.value = '\r'}; return NUMBER; }
\'\\t\'		{ yylval.number = (typeof(yylval.number)){.value = '\t'}; return NUMBER; }
\'\\v\'		{ yylval.number = (typeof(yylval.number)){.value = '\v'}; return NUMBER; }
\'\\'\'		{ yylval.number = (typeof(yylval.number)){.value = '\''}; return NUMBER; }
\'\\\\\'	{ yylval.number = (typeof(yylval.number)){.value = '\\'}; return NUMBER; }
\'[^\']\' 	{ yylval.number = (typeof(yylval.number)){.value = yytext[1]}; return NUMBER; }
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
"bitsof" { return KBITSOF; }
"new" { return KNEW; }

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

