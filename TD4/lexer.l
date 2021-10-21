%{
	#include <stdio.h>
	#include "y.tab.h"
%}

%option noyywrap
%option yylineno

%%
[ \t] 		{ }
([0-9]*([0-9]\.?|\.[0-9])[0-9]*([Ee][-+]?[0-9]+)?) 		{ yylval.val = atof(yytext); return NUMBER; }
.|\n 		{ return *yytext; }
%%
