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
#include <string.h>

#include "calc.h"
#include "code.h"

// Yacc prototypes
int  yylex(void);
void yyerror(const char *s);

#define  YYERROR_VERBOSE 1      // produce verbose syntax error messages
%}

%union {
    int value;                 // number value
    char *var;                   // ident name
    int chr;
    ast_node *node;              // node pointer
};

//                      Tokens
%token  <value>         NUMBER
%token  <var>           IDENT STRING
%token                  KWHILE KIF KPRINT KELSE KREAD KFOR KDO KVAR KFUNC KRETURN KPROC KBREAK KCONTINUE KCONST KTYPE KTYPEOF
%token '+' '-' '*' '/' GE LE EQ NE '>' '<' REF DEREF APL AMN AML ADV INC DEC AND OR
%token UMINUS VDECL
//                       Precedence rules
%left '+'

%precedence DEREF
%nonassoc THEN
%nonassoc KELSE

//                      Non terminal types
%type   <node>		stmt expr stmt_list var expr_opt basic_expr postfix_expr unary_expr mult_expr add_expr rel_expr eq_expr assign_expr
%type	<node> 		l_and_expr l_or_expr stmt_list_opt expr_discard expr_discard_opt
%type 	<node>		param_list param_list_ne arg_list arg_list_ne var_decl var_decl_list type_spec_opt type_spec type_decl type_decl_list
%type   <chr>		aug_assign func_type unary_op

%%

session
    : stmts                   	{ fprintf(stderr, "Bye.\n");  exit(0); }
        ;

stmts
    : stmts stmt            	{ produce_code($2); free_node($2); }
        | /* empty */
        ;

stmt
    : ';'                               											{ $$ = make_node(';', 0); }
    | expr_discard ';'																{ $$ = $1; }
    | KVAR var_decl_list ';'														{ $$ = $2; }
    | KTYPE type_decl_list ';'														{ $$ = $2; }
    | KCONST var '=' expr ';'														{ $$ = make_node(KCONST, 2, $2, $4); }
    | KPRINT expr ';'                  												{ $$ = make_node(KPRINT, 1, $2); }
    | KREAD expr ';'                  												{ $$ = make_node(KREAD, 1, $2); }
    | KRETURN expr_opt ';'                  										{ $$ = make_node(KRETURN, 1, $2); }
    | KBREAK ';'																	{ $$ = make_node(KBREAK, 0); }
    | KCONTINUE ';'																	{ $$ = make_node(KCONTINUE, 0); }
    | KWHILE '(' expr ')' stmt         												{ $$ = make_node(KFOR, 4, NULL, $3, NULL, $5); }
    | KIF '(' expr ')' stmt    %prec THEN    										{ $$ = make_node(KIF, 3, $3, $5, NULL); }
    | KIF '(' expr ')' stmt KELSE stmt      										{ $$ = make_node(KIF, 3, $3, $5, $7); }
    | KFOR '(' expr_discard_opt ';' expr_opt ';' expr_discard_opt ')' stmt 			{ $$ = make_node(KFOR, 4, $3, $5, $7, $9); }
    | KDO stmt KWHILE '(' expr ')' ';' 												{ $$ = make_node(KDO, 2, $2, $5); }
    | func_type var '(' param_list ')' stmt											{ $$ = make_node($1, 3, $2, $4, $6); }
    | '{' stmt_list_opt '}'                											{ $$ = $2; }
    ;

var_decl
    : var type_spec_opt																{ $$ = make_node(KVAR, 2, $1, $2); }
    | var '=' expr																	{ ast_node* assign = make_node('=', 2, $1, $3); AST_CLEAN_STACK(assign) = true; $$ = make_node(';', 2, make_node(KVAR, 2, $1, make_node(KTYPEOF, 1, $3)), assign); }
    | var ':' type_spec '=' expr													{ ast_node* assign = make_node('=', 2, $1, $5); AST_CLEAN_STACK(assign) = true; $$ = make_node(';', 2, make_node(KVAR, 2, $1, $3), assign); }
    | var '[' expr ']'  															{ $$ = make_node(KVAR, 3, $1, $3, NULL); }
    | var '[' expr ']' '=' STRING 													{ $$ = make_node(KVAR, 3, $1, $3, make_ident($6)); }
    | var '[' ']' '=' STRING														{ $$ = make_node(KVAR, 3, $1, make_number(strlen($5)), make_ident($5)); }
    ;

type_decl
    : var '=' type_spec																{ $$ = make_node(KTYPE, 2, $1, $3); }
    ;

type_spec_opt
	:								{ $$ = NULL; }
	| ':' type_spec					{ $$ = $2; }
	;

type_spec
	: var							{ $$ = $1; }
	| type_spec '*'					{ $$ = make_node('*', 1, $1); }
	| type_spec KCONST				{ $$ = make_node(KCONST, 1, $1); }
	| type_spec '[' expr ']'		{ $$ = make_node('[', 2, $1, $3); }
	| KTYPEOF '(' expr ')'			{ $$ = make_node(KTYPEOF, 1, $3); }
	;

var_decl_list
	: var_decl						{ $$ = $1; }
	| var_decl ',' var_decl_list	{ $$ = make_node(';', 2, $1, $3); }

type_decl_list
	: type_decl						{ $$ = $1; }
	| type_decl ',' type_decl_list	{ $$ = make_node(';', 2, $1, $3); }

func_type
	: KFUNC							{ $$ = KFUNC; }
	| KPROC							{ $$ = KPROC; }
	;

param_list
	: 								{ $$ = NULL; }
	| param_list_ne					{ $$ = $1; }
	;

param_list_ne
	: var							{ $$ = make_list($1); }
	| var ',' param_list_ne			{ $$ = prepend_list($3, $1); }
	;

arg_list
	: 								{ $$ = NULL; }
	| arg_list_ne					{ $$ = $1; }
	;

arg_list_ne
	: expr							{ $$ = make_list($1); }
	| expr ',' arg_list_ne			{ $$ = prepend_list($3, $1); }
	;

expr_opt
    : expr          				{ $$ = $1; }
    |              					{ $$ = NULL; }
    ;

stmt_list_opt
	: 								{ $$ = NULL; }
	| stmt_list						{ $$ = $1; }
	;

stmt_list
    : stmt                  		{ $$ = $1; }
    | stmt_list stmt        		{ $$ = make_node(';', 2, $1, $2); }
    ;

aug_assign
    : APL 							{ $$ = '+'; }
    | AMN 							{ $$ = '-'; }
    | AML 							{ $$ = '*'; }
    | ADV							{ $$ = '/'; }
    ;

basic_expr
	: NUMBER												{ $$ = make_number($1); }
	| var													{ $$ = $1; }
	| var '(' arg_list ')'									{ $$ = make_node('(', 2, $1, $3); }
	| '(' expr ')'											{ $$ = $2; }
	| '{' stmt_list expr '}'								{ $$ = make_node('{', 2, $2, $3); }
    | KIF '(' expr ')' '{' expr '}' KELSE '{' expr '}'      { $$ = make_node(KIF, 3, $3, $6, $10); }
	;

postfix_expr
    : basic_expr					{ $$ = $1; }
    | postfix_expr INC              { $$ = make_node(INC, 2, $1, NULL); }
    | postfix_expr DEC              { $$ = make_node(DEC, 2, $1, NULL); }
    | postfix_expr '[' expr ']'   	{ $$ = make_node(DEREF, 1, make_node('+', 2, $1, $3)); }
    ;

unary_expr
    : postfix_expr					{ $$ = $1; }
    | unary_op unary_expr			{ $$ = make_node($1, 1, $2); }
    ;

unary_op
	: '-'		{ $$ = UMINUS; }
	| '~'		{ $$ = '~'; }
	| '*'		{ $$ = DEREF; }
	| INC		{ $$ = INC; }
	| DEC		{ $$ = DEC; }
	| '&'		{ $$ = REF; }
	;

mult_expr
    : unary_expr					{ $$ = $1; }
    | mult_expr '*' unary_expr		{ $$ = make_node('*', 2, $1, $3); }
    | mult_expr '/' unary_expr		{ $$ = make_node('*', 2, $1, $3); }
    ;

add_expr:
      mult_expr						{ $$ = $1; }
    | add_expr '+' mult_expr		{ $$ = make_node('+', 2, $1, $3); }
    | add_expr '-' mult_expr		{ $$ = make_node('-', 2, $1, $3); }
    ;

rel_expr
    : add_expr						{ $$ = $1; }
    | rel_expr '<' add_expr			{ $$ = make_node('<', 2, $1, $3); }
    | rel_expr '>' add_expr			{ $$ = make_node('>', 2, $1, $3); }
    | rel_expr LE add_expr			{ $$ = make_node(LE, 2, $1, $3); }
    | rel_expr GE add_expr			{ $$ = make_node(GE, 2, $1, $3); }
    ;

eq_expr
    : rel_expr						{ $$ = $1; }
    | eq_expr EQ rel_expr			{ $$ = make_node(EQ, 2, $1, $3); }
    | eq_expr NE rel_expr			{ $$ = make_node(NE, 2, $1, $3); }
    ;

l_and_expr
    : eq_expr						{ $$ = $1; }
    | l_and_expr AND eq_expr		{ $$ = make_node(AND, 2, $1, $3); }

l_or_expr
    : l_and_expr					{ $$ = $1; }
    | l_or_expr OR l_and_expr		{ $$ = make_node(OR, 2, $1, $3); }
    ;

assign_expr
    : l_or_expr								{ $$ = $1; }
    | unary_expr '=' assign_expr			{ $$ = make_node('=', 2, $1, $3); }
    | unary_expr aug_assign assign_expr   	{ $$ = make_node('=', 2, $1, make_node($2, 2, $1, $3)); }
    ;

expr
    : assign_expr				{ $$ = $1; }
    ;

expr_discard
	: expr						{ $$ = $1; AST_CLEAN_STACK($$) = true; }
	;

expr_discard_opt
	: expr_discard				{ $$ = $1; }
	|							{ $$ = NULL; }
	;

var
    :  IDENT                	{ $$ = make_ident($1); }
    ;

%%
void yyerror(const char *s)     { error_msg(NULL, "%s\n",s); exit(1); }
int  yywrap(void)               { return 1;          } // to avoid linking with -ldl
bool optimize = false;
int  main(int argc, char* argv[]) {
  extern FILE *yyin;
  yyin = stdin;
  if (argc >= 2) {
    yyin = fopen(argv[1], "r");
    if (!yyin) {
      fprintf(stderr, "%s: cannot open input file '%s'\n", *argv, argv[1]);
      exit(1);
    }
    if (argc >= 3 && !strcmp(argv[2], "-O")) {
      printf("# Optimizations enabled\n");
      optimize = true;
    }
  }
  return yyparse();
}
