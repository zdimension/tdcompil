%{
	#include <stdio.h>
	#include <ctype.h>
	#include <unistd.h>
	
	void yyerror(const char* msg);
	int yylex(void);
	void prompt(void);
	
	#define YYERROR_VERBOSE 1
	extern int yylineno;
%}

%union {
    double val;
}
%type <val> expr
%token <val> NUMBER
%left '+' '-'
%left '*' '/'
%right UMINUS

%%
lines: lines expr '\n' 			{ printf("==> %f\n", $2); prompt(); }
	 | lines '\n'
	 | error '\n' 				{ yyerrok; }
	 | /* empty */
	 ;
expr: expr '+' expr 			{ $$ = $1 + $3; }
	| expr '-' expr 			{ $$ = $1 - $3; }
	| expr '*' expr 			{ $$ = $1 * $3; }
	| expr '/' expr 			{ if ($3) $$ = $1 / $3; else fprintf(stderr, "division by 0\n"); }
	| '(' expr ')' 				{ $$ = $2; }
	| '-' expr %prec UMINUS 	{ $$ = -$2; }
	| NUMBER				
	;
%%

void yyerror(const char* msg) {
	fprintf(stderr, "line %d, unexpected token ", yylineno);
	fprintf(stderr, yychar == '\n' ? "NEWLINE" : (yychar < 256) ? "'%c'": "[%d]", yychar);
	fprintf(stderr, ": %s\n", msg);
	prompt();
}

void prompt(void) {
	printf("[%d] ", yylineno);
	fflush(stdout);
}

int main() {
	prompt();
	return yyparse();
}
