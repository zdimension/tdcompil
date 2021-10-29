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
#include <stdbool.h>

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
    int chr;
    ast_node *node;              // node pointer
};

//                      Tokens
%token  <value>         NUMBER
%token  <var>           IDENT
%token                  KWHILE KIF KPRINT KELSE KREAD KFOR KDO KDIM
%token '+' '-' '*' '/' GE LE EQ NE '>' '<' REF DEREF APL AMN AML ADV INC DEC AND OR
%token UMINUS
//                       Precedence rules
%left '+'

%precedence DEREF
%nonassoc THEN
%nonassoc KELSE

//                      Non terminal types
%type   <node>          stmt expr stmt_list var expr_opt ref_offset basic_expr postfix_expr unary_expr mult_expr add_expr rel_expr eq_expr assign_expr
%type	<node>		l_and_expr l_or_expr
%type   <chr>		aug_assign

%%

session
	: stmts                   { fprintf(stderr, "Bye.\n");  exit(0); }
        ;

stmts
	: stmts stmt            { produce_code($2); free_node($2); }
        | /* empty */
        ;

stmt
	: ';'                               			{ $$ = make_node(';', 0); }
        | expr ';'						{ if (AST_KIND($1) == k_operator) { OPER_CLEAN_STACK($1) = true; } $$ = $1; }
        | KDIM var '[' NUMBER ']' ';' 				{ $$ = make_node(KDIM, 2, $2, make_number($4)); }
//        | expr_assign ';'        				{ OPER_CLEAN_STACK($1) = true; $$ = $1; }
//        | expr_all ';'                    			{ $$ = $1; }
        | KPRINT expr ';'                  			{ $$ = make_node(KPRINT, 1, $2); }
        | KREAD expr ';'                  			{ $$ = make_node(KREAD, 1, $2); }
        | KWHILE '(' expr ')' stmt         			{ $$ = make_node(KWHILE, 2, $3, $5); }
        | KIF '(' expr ')' stmt    %prec THEN    		{ $$ = make_node(KIF, 3, $3, $5, NULL); }
        | KIF '(' expr ')' stmt KELSE stmt      		{ $$ = make_node(KIF, 3, $3, $5, $7); }
        | KFOR '(' expr_opt ';' expr_opt ';' expr_opt ')' stmt 	{ if ($3) { OPER_CLEAN_STACK($3) = true; } if ($7) { OPER_CLEAN_STACK($7) = true; } $$ = make_node(';', 2, $3, make_node(KWHILE, 2, $5, make_node(';', 2, $9, $7))); }
        | KDO stmt KWHILE '(' expr ')' ';' 			{ $$ = make_node(KDO, 2, $2, $5); }
        | '{' stmt_list '}'                			{ $$ = $2; }
        ;

expr_opt
	: expr          { $$ = $1; }
        |               { $$ = NULL; }
        ;


stmt_list
	: stmt                  { $$ = $1; }
        | stmt_list stmt        { $$ = make_node(';', 2, $1, $2); }
        ;

//
//
//expr_assign:
//          lvalue '=' expr          { $$ = make_node('=', 2, $1, $3); }
//        | lvalue aug_assign expr   { $$ = make_node('=', 2, $1, make_node($2, 2, $1, $3)); }
//        | lvalue INC               { $$ = make_node(INC, 2, $1, NULL); }
//        | lvalue DEC               { $$ = make_node(DEC, 2, $1, NULL); }
//        ;

aug_assign
	: APL 	{ $$ = '+'; }
        | AMN 	{ $$ = '-'; }
        | AML 	{ $$ = '*'; }
        | ADV	{ $$ = '/'; }
        ;

//expr_all:
//          NUMBER                		{ $$ = make_number($1); }
//        | lvalue                   		{ $$ = $1; }
//        | expr bin_op expr %prec BINOP			{ $$ = make_node($2, 2, $1, $3); }
//        | '(' expr ')'          		{ $$ = $2; }
//        ;

basic_expr
	: NUMBER			{ $$ = make_number($1); }
	| var				{ $$ = $1; }
	| '(' expr ')'			{ $$ = $2; }
	;

postfix_expr
	: basic_expr				{ $$ = $1; }
	| postfix_expr INC               	{ $$ = make_node(INC, 2, $1, NULL); }
        | postfix_expr DEC               	{ $$ = make_node(DEC, 2, $1, NULL); }
        | postfix_expr '[' expr ']'   	 	{ $$ = make_node(DEREF, 1, make_node('+', 2, $1, $3)); }
        ;

unary_expr
	: postfix_expr				{ $$ = $1; }
	| '-' postfix_expr 			{ $$ = make_node(UMINUS, 1, $2); }
	| '*' postfix_expr 			{ $$ = make_node(DEREF, 1, $2); }
	| INC postfix_expr               	{ $$ = make_node(INC, 1, $2); }
	| DEC postfix_expr               	{ $$ = make_node(DEC, 1, $2); }
	| '&' var ref_offset 			{ $$ = make_node(REF, 2, $2, $3); }
	;

mult_expr
	: unary_expr				{ $$ = $1; }
	| mult_expr '*' unary_expr		{ $$ = make_node('*', 2, $1, $3); }
	| mult_expr '/' unary_expr		{ $$ = make_node('*', 2, $1, $3); }
	;

add_expr:
	  mult_expr				{ $$ = $1; }
	| add_expr '+' mult_expr		{ $$ = make_node('+', 2, $1, $3); }
	| add_expr '-' mult_expr		{ $$ = make_node('-', 2, $1, $3); }
	;

rel_expr
	: add_expr				{ $$ = $1; }
	| rel_expr '<' add_expr			{ $$ = make_node('<', 2, $1, $3); }
	| rel_expr '>' add_expr			{ $$ = make_node('>', 2, $1, $3); }
	| rel_expr LE add_expr			{ $$ = make_node(LE, 2, $1, $3); }
	| rel_expr GE add_expr			{ $$ = make_node(GE, 2, $1, $3); }
	;

eq_expr
	: rel_expr				{ $$ = $1; }
	| eq_expr EQ rel_expr			{ $$ = make_node(EQ, 2, $1, $3); }
	| eq_expr NE rel_expr			{ $$ = make_node(NE, 2, $1, $3); }
	;

l_and_expr
	: eq_expr				{ $$ = $1; }
	| l_and_expr AND eq_expr		{ $$ = make_node(AND, 2, $1, $3); }

l_or_expr
	: l_and_expr				{ $$ = $1; }
	| l_or_expr OR l_and_expr		{ $$ = make_node(OR, 2, $1, $3); }
	;

assign_expr
	: l_or_expr				{ $$ = $1; }
	| unary_expr '=' assign_expr		{ $$ = make_node('=', 2, $1, $3); }
	| unary_expr aug_assign assign_expr   	{ $$ = make_node('=', 2, $1, make_node($2, 2, $1, $3)); }
	;

expr
	: assign_expr				{ $$ = $1; }
	;

ref_offset
	: '[' expr ']'				{ $$ = $2; }
        |					{ $$ = NULL; }
        ;

//lvalue
//	: var	   			{ $$ = $1; }
//        | '*' expr	%prec DEREF	{ $$ = make_node(DEREF, 1, $2); }
//        | lvalue '[' expr ']'    	{ $$ = make_node(DEREF, 1, make_node('+', 2, $1, $3)); }
//        ;


var
    	:  IDENT                { $$ = make_ident($1); }
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
