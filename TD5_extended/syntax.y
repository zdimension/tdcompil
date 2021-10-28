//
// syntax.y             -- The calc syntax rules
//
//           Author: Erick Gallesio [eg@unice.fr]
//    Creation date: 21-Oct-2015 11:17 (eg)
// Last file update: 17-Nov-2017 10:20 (eg)

%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "calc.h"
#include "code.h"

// Yacc prototypes
int  yylex(void);
void yyerror(const char *s);

#define  YYERROR_VERBOSE 1      // produce verbose syntax error messages
%}

%union {
    float value;                 // number value
    char *var;                   // ident name
    ast_node *node;              // node pointer
};

//                      Tokens
%token  <value>         NUMBER
%token  <var>           IDENT
%token                  KWHILE KIF KPRINT KELSE KREAD KFOR KDO

//                       Precedence rules

%right '='
%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS



%nonassoc "then"
%nonassoc KELSE

//                      Non terminal types
%type   <node>          stmt expr stmt_list var expr_opt expr_all

%%

session:
        stmts                   { fprintf(stderr, "Bye.\n");  exit(0); }
        ;

stmts:
          stmts stmt            { produce_code($2); free_node($2); }
        | /* empty */
        ;

stmt:
         ';'                               { $$ = make_node(';', 0); }
        | var '=' expr ';'        { $$ = make_node('=', 3, $1, $3, NULL); }
        | expr_all ';'                     { $$ = $1; }
        | KPRINT expr ';'                  { $$ = make_node(KPRINT, 1, $2); }
        | KREAD expr ';'                  { $$ = make_node(KREAD, 1, $2); }
        | KWHILE '(' expr ')' stmt         { $$ = make_node(KWHILE, 2, $3, $5); }
        | KIF '(' expr ')' stmt    %prec "then"    { $$ = make_node(KIF, 3, $3, $5, NULL); }
        | KIF '(' expr ')' stmt KELSE stmt      { $$ = make_node(KIF, 3, $3, $5, $7); }
        | KFOR '(' expr_opt ';' expr_opt ';' expr_opt ')' stmt { $$ = make_node(';', 2, $3, make_node(KWHILE, 2, $5, make_node(';', 2, $9, $7))); }
        | KDO stmt KWHILE '(' expr ')' ';' { $$ = make_node(KDO, 2, $2, $5); }
        | '{' stmt_list '}'                { $$ = $2; }
        ;

expr_opt:
          expr          { $$ = $1; }
        |               { $$ = NULL; }
        ;


stmt_list:
          stmt                  { $$ = $1; }
        | stmt_list stmt        { $$ = make_node(';', 2, $1, $2); }
        ;


expr:
          expr_all                  { $$ = $1; }
        | var '=' expr          { $$ = make_node('=', 2, $1, $3); }
        ;

expr_all:
          NUMBER                { $$ = make_number($1); }
        | var                   { $$ = $1; }
        | '-' expr %prec UMINUS { $$ = make_node(UMINUS, 1, $2); }
        | expr '+' expr         { $$ = make_node('+', 2, $1, $3); }
        | expr '-' expr         { $$ = make_node('-', 2, $1, $3); }
        | expr '*' expr         { $$ = make_node('*', 2, $1, $3); }
        | expr '/' expr         { $$ = make_node('/', 2, $1, $3); }
        | expr '<' expr         { $$ = make_node('<', 2, $1, $3); }
        | expr '>' expr         { $$ = make_node('>', 2, $1, $3); }
        | expr GE expr          { $$ = make_node(GE, 2, $1, $3); }
        | expr LE expr          { $$ = make_node(LE, 2, $1, $3); }
        | expr NE expr          { $$ = make_node(NE, 2, $1, $3); }
        | expr EQ expr          { $$ = make_node(EQ, 2, $1, $3); }
        | '(' expr ')'          { $$ = $2; }
        ;

var:       IDENT                { $$ = make_ident($1); }
        ;


%%
void yyerror(const char *s)     { error_msg("%s\n",s); exit(1); }
int  yywrap(void)               { return 1;          } // to avoid linking with -ldl

int  main(int argc, char* argv[]) {
  extern FILE *yyin;
  yyin = stdin;
  if (argc == 2) {
    yyin = fopen(argv[1], "r");
    if (!yyin) {
      fprintf(stderr, "%s: cannot open input file '%s'\n", *argv, argv[1]);
      exit(1);
    }
  }
  return yyparse();
}
