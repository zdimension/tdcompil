%{
	#include <stdio.h>
	#include <ctype.h>
	#include <unistd.h>
	#include <stdarg.h>
	
	void yyerror(const char* msg);
	int yylex(void);
	void prompt(void);
	char *make_string(const char *fmt, ...);
	
	#define YYERROR_VERBOSE 1
	extern int yylineno;
%}

%union {
    double val;
    char* xml;
}
%type <xml> expr
%token <val> NUMBER
%left '+' '-'
%left '*' '/'
%right UMINUS



%%
lines: lines expr '\n' 			{ printf("==> %s\n", $2); prompt(); }
	 | lines '\n'
	 | error '\n' 				{ yyerrok; }
	 | /* empty */
	 ;
expr: expr '+' expr 			{ $$ = make_string("<add op='+'>%s%s</add>", $1, $3); free($1); free($3); }
	| expr '-' expr 			{ $$ = make_string("<add op='-'>%s%s</add>", $1, $3); free($1); free($3); }
	| expr '*' expr 			{ $$ = make_string("<mul op='*'>%s%s</mul>", $1, $3); free($1); free($3); }
	| expr '/' expr 			{ $$ = make_string("<mul op='/'>%s%s</mul>", $1, $3); free($1); free($3); }
	| '(' expr ')' 				{ $$ = make_string("%s", $2); free($2); }
	| '-' expr %prec UMINUS 	{ $$ = make_string("<neg>%s</neg>", $2); free($2); }
	| NUMBER					{ $$ = make_string("<number>%f</number>", $1); }
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

char *make_string(const char *fmt, ...) {
	int size = 0;
	char *p = NULL;
	va_list ap;
	va_start(ap, fmt);
	size = vsnprintf(p, size, fmt, ap);
	va_end(ap);
	if (size < 0)
		return NULL;
	size++;
	p = malloc(size);
	if (p == NULL)
		return NULL;
	va_start(ap, fmt);
	size = vsnprintf(p, size, fmt, ap);
	va_end(ap);
	if (size < 0) {
		free(p);
		return NULL;
	}
	return p;
}