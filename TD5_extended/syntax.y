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
%locations

%initial-action
{
	yylloc = (YYLTYPE){1, 1, 1, 1, malloc(1024)};
	yylloc.code[0] = 0;
};

%union {
	struct { int value; int size; } number; // number value
    char *var;                   // ident name
    int chr;
    ast_node *node;              // node pointer
};

//                      Tokens
%token  <number>         NUMBER
%token  <var>           IDENT STRING
%token                  KWHILE KIF KPRINT KELSE KREAD KFOR KDO KVAR KFUNC KRETURN KBREAK KCONTINUE KCONST KTYPE KTYPEOF
%token					KSIZEOF KSTRUCT KBITSOF KNEW KASSERT KLOOP KMATCH KIS RANGE IRANGE KGLOBAL KIMPL KIN KFOREACH
%token					KINTERFACE KSELF
%token '+' '-' '*' '/' GE LE EQ NE '>' '<' REF DEREF APL AMN AML ADV INC DEC AND OR SHL SHR ARROW STRUCTLIT
%token UMINUS VDECL SCOPE TUPLEASSIGN
//                       Precedence rules
%left '+'

%precedence DEREF
%nonassoc THEN
%nonassoc KELSE
%nonassoc KIS

//                      Non terminal types
%type   <node>		stmt expr stmt_list var expr_opt basic_expr postfix_expr unary_expr mult_expr add_expr rel_expr eq_expr assign_expr shift_expr is_expr
%type	<node> 		l_and_expr l_or_expr stmt_list_opt expr_discard expr_discard_opt scalar_var_init type_arg_list tuple_assign_left tuple_assign_right
%type 	<node>		param_list param_list_ne arg_list arg_list_ne var_decl var_decl_list type_spec_opt type_spec type_decl type_decl_list type_params type_param_list
%type	<node>		struct_field struct_field_list var_typed stmt_braced expr_discard_or_inline_decl_opt expr_or_inline_decl pattern pattern_list pattern_branch pattern_basic
%type	<node>		struct_field_init struct_field_init_list func_list func func_signature interf_func_list interf_func
%type   <chr>		aug_assign unary_op

%%

session
    : stmts                   	{ fprintf(stderr, "Bye.\n");  exit(0); }
        ;

stmts
    : stmts stmt            	{ produce_code($2); free_node($2); }
        | /* empty */
        ;

stmt
    : ';'                               													{ $$ = make_node(';', 0); }
    | expr_discard ';'																		{ $$ = $1; }
    | KVAR var_decl_list ';'																{ $$ = $2; }
    | KTYPE type_decl_list ';'																{ $$ = $2; }
    | '(' tuple_assign_left ')' '=' '(' tuple_assign_right ')' ';'							{ $$ = make_node(TUPLEASSIGN, 2, $2, $6); }
    | KCONST var '=' expr ';'																{ $$ = make_node(KCONST, 2, $2, $4); }
    | KPRINT expr ';'                  														{ $$ = make_node(KPRINT, 1, $2); }
    | KREAD expr ';'                  														{ $$ = make_node(KREAD, 1, $2); }
    | KRETURN expr_opt ';'                  												{ $$ = make_node(KRETURN, 1, $2); }
    | KASSERT expr ';'                  													{ $$ = make_node(KASSERT, 1, $2); }
    | KBREAK ';'																			{ $$ = make_node(KBREAK, 0); }
    | KBREAK expr ';'																		{ $$ = make_node(KBREAK, 1, $2); }
    | KCONTINUE ';'																			{ $$ = make_node(KCONTINUE, 0); }
    | KWHILE '(' expr ')' stmt         														{ $$ = make_node(KFOR, 4, NULL, $3, NULL, $5); }
    | KIF '(' expr_or_inline_decl ')' stmt    %prec THEN    								{ $$ = make_scope(make_node(KIF, 3, $3, $5, NULL)); }
    | KIF '(' expr_or_inline_decl ')' stmt KELSE stmt      									{ $$ = make_scope(make_node(KIF, 3, $3, $5, $7)); }
    | KFOR '(' expr_discard_or_inline_decl_opt ';' expr_opt ';' expr_discard_opt ')' stmt 	{ $$ = make_scope(make_node(KFOR, 4, $3, $5, $7, $9)); }
    | KFOR '(' KVAR var KIN expr ')' stmt 													{ $$ = make_scope(make_node(KFOREACH, 3, $4, $6, $8)); }
    | KDO stmt KWHILE '(' expr ')' ';' 														{ $$ = make_node(KDO, 2, $2, $5); }
    | func																					{ $$ = $1; }
    | KIMPL var '{' func_list '}'															{ $$ = make_node(KIMPL, 2, $2, $4); }
    | KIMPL var KFOR var '{' func_list '}'													{ $$ = make_node(KIMPL, 3, $4, $6, $2); }
    | stmt_braced		                													{ $$ = $1; }
    ;

func_signature
	: KFUNC var '(' param_list ')' ':' type_spec											{ $$ = make_node(KFUNC, 4, $2, $4, NULL, $7); }
    ;

func
	: func_signature stmt_braced															{ $$ = $1; OPER_OPERANDS($$)[2] = $2; }
    ;

func_list
	: func							{ $$ = make_list($1); }
	| func func_list				{ $$ = prepend_list($2, $1); }
	;

tuple_assign_left
	: var ',' var					{ $$ = prepend_list(make_list($3), $1); }
	| var ',' tuple_assign_left		{ $$ = prepend_list($3, $1); }

tuple_assign_right
	: expr ',' expr					{ $$ = prepend_list(make_list($3), $1); }
	| expr ',' tuple_assign_right	{ $$ = prepend_list($3, $1); }

expr_or_inline_decl
	: expr							{ $$ = $1; }
	| KVAR scalar_var_init			{ $$ = $2; }
	;

expr_discard_or_inline_decl_opt
	: 								{ $$ = NULL; }
	| expr_discard					{ $$ = $1; }
    | KVAR var_decl_list			{ $$ = $2; AST_CLEAN_STACK($2) = true; }
	;

stmt_braced
	: '{' stmt_list_opt '}'                											{ $$ = make_scope($2); }
	;

scalar_var_init
	: var type_spec_opt '=' expr													{ $$ = make_node(';', 2, make_node(KVAR, 2, $1, $2 ? $2 : make_node(KTYPEOF, 1, $4)), make_node('=', 2, $1, $4)); }
    ;

var_decl
    : var type_spec_opt																{ $$ = make_node(KVAR, 2, $1, $2); }
    | scalar_var_init																{ $$ = $1; AST_CLEAN_STACK(OPER_OPERANDS($1)[1]) = true; }
    | var '[' expr ']'  															{ $$ = make_node(KVAR, 3, $1, $3, NULL); }
    | var '[' expr ']' '=' STRING 													{ $$ = make_node(KVAR, 3, $1, $3, make_ident($6)); }
    | var '[' ']' '=' STRING														{ $$ = make_node(KVAR, 3, $1, make_number(strlen($5)), make_ident($5)); }
    ;

type_decl
    : var type_params '=' type_spec													{ $$ = make_node(KTYPE, 3, $1, $4, $2); }
    ;

type_params
	: 								{ $$ = NULL; }
	| '<' type_param_list '>'		{ $$ = $2; }
	;

type_param_list
	: var							{ $$ = make_list($1); }
	| var ',' type_param_list		{ $$ = prepend_list($3, $1); }
	;

type_spec_opt
	:								{ $$ = NULL; }
	| ':' type_spec					{ $$ = $2; }
	;

type_spec
	: var									{ $$ = $1; }
	| var '<' type_arg_list '>'				{ $$ = make_node(KNEW, 2, $1, $3); }
	| KSELF									{ $$ = make_node(KSELF, 0); }
	| type_spec '*'							{ $$ = make_node('*', 1, $1); }
	| type_spec KCONST						{ $$ = make_node(KCONST, 1, $1); }
	| type_spec KGLOBAL						{ $$ = make_node(KGLOBAL, 1, $1); }
	| type_spec '[' expr ']'				{ $$ = make_node('[', 2, $1, $3); }
	| KTYPEOF '(' expr ')'					{ $$ = make_node(KTYPEOF, 1, $3); }
	| KSTRUCT '{' struct_field_list '}'		{ $$ = make_node(KSTRUCT, 1, $3); }
	| KINTERFACE '{' interf_func_list '}'	{ $$ = make_node(KINTERFACE, 1, $3); }
	;

type_arg_list
	: type_spec								{ $$ = make_list($1); }
	| type_spec ',' type_arg_list			{ $$ = prepend_list($3, $1); }
	;

var_typed
	: var ':' type_spec					{ $$ = make_node(':', 2, $1, $3); }
    ;

struct_field
	: var_typed ';'						{ $$ = $1; }
	;

struct_field_list
	: struct_field						{ $$ = make_list($1); }
	| struct_field struct_field_list 	{ $$ = prepend_list($2, $1); }
	;

interf_func
	: func_signature ';'				{ $$ = $1; }
	;

interf_func_list
	: interf_func						{ $$ = make_list($1); }
	| interf_func interf_func_list 		{ $$ = prepend_list($2, $1); }
	;

var_decl_list
	: var_decl						{ $$ = $1; }
	| var_decl ',' var_decl_list	{ $$ = make_node(';', 2, $1, $3); }

type_decl_list
	: type_decl						{ $$ = $1; }
	| type_decl ',' type_decl_list	{ $$ = make_node(';', 2, $1, $3); }

param_list
	: 								{ $$ = NULL; }
	| param_list_ne					{ $$ = $1; }
	;

param_list_ne
	: var_typed						{ $$ = make_list($1); }
	| var_typed ',' param_list_ne	{ $$ = prepend_list($3, $1); }
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

pattern_basic
	: l_and_expr 											{ $$ = $1; }
	| pattern_basic RANGE l_and_expr 						{ $$ = make_node(RANGE, 2, $1, $3); }
	| pattern_basic IRANGE l_and_expr 						{ $$ = make_node(RANGE, 2, $1, $3); AST_DATA($$) = (void*) 1; }
	| '_'													{ $$ = NULL; }
	;

pattern
	: pattern_basic					{ $$ = $1; }
	| pattern '|' pattern_basic		{ $$ = make_node('|', 2, $1, $3); }
	;

pattern_branch
	: pattern ARROW expr			{ $$ = make_node(ARROW, 2, $1, $3); }
	;

pattern_list
	: pattern_branch					{ $$ = make_list($1); }
	| pattern_branch ',' pattern_list	{ $$ = prepend_list($3, $1); }
	;

struct_field_init
	: var ':' pattern						{ $$ = make_node('=', 2, $1, $3); }
	;

struct_field_init_list
	: struct_field_init								{ $$ = make_list($1); }
	| struct_field_init ',' struct_field_init_list 	{ $$ = prepend_list($3, $1); }
	;

basic_expr
	: NUMBER																{ $$ = make_number_sized($1.value, $1.size); }
	| KSIZEOF '(' type_spec ')' 											{ $$ = make_node(KSIZEOF, 1, $3); }
	| KBITSOF '(' type_spec ')' 											{ $$ = make_node(KBITSOF, 1, $3); }
	| KNEW '(' type_spec ')' 												{ $$ = make_node(KNEW, 1, $3); }
	| var																	{ $$ = $1; }
	| '(' expr ')'															{ $$ = $2; }
	| '{' stmt_list expr '}'												{ $$ = make_scope(make_node('{', 2, $2, $3)); }
    | KIF '(' expr_or_inline_decl ')' '{' expr '}' KELSE '{' expr '}'      	{ $$ = make_scope(make_node(KIF, 3, $3, $6, $10)); }
    | KLOOP '{' stmt_list '}'												{ $$ = make_node(KLOOP, 1, $3); }
    | KMATCH '(' expr_or_inline_decl ')' '{' pattern_list '}'				{ $$ = make_node(KMATCH, 2, $3, $6); }
    | var '{' struct_field_init_list '}'									{ $$ = make_node(STRUCTLIT, 2, $1, $3); }
    | var '{' arg_list '}'													{ $$ = make_node(STRUCTLIT, 3, $1, $3, NULL); }
	;

postfix_expr
    : basic_expr							{ $$ = $1; }
    | postfix_expr INC              		{ $$ = make_node(INC, 2, $1, NULL); }
    | postfix_expr DEC              		{ $$ = make_node(DEC, 2, $1, NULL); }
    | postfix_expr '[' expr ']'   			{ $$ = make_node(DEREF, 1, make_node('+', 2, $1, $3)); }
    | postfix_expr '.' var					{ $$ = make_node('.', 2, $1, $3); }
    | postfix_expr '(' arg_list ')'			{ $$ = make_node('(', 2, $1, $3); }
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
    | mult_expr '/' unary_expr		{ $$ = make_node('/', 2, $1, $3); }
    ;

add_expr
	: mult_expr						{ $$ = $1; }
    | add_expr '+' mult_expr		{ $$ = make_node('+', 2, $1, $3); }
    | add_expr '-' mult_expr		{ $$ = make_node('-', 2, $1, $3); }
    ;

shift_expr
	: add_expr						{ $$ = $1; }
	| shift_expr SHL add_expr		{ $$ = make_node(SHL, 2, $1, $3); }
	| shift_expr SHR add_expr		{ $$ = make_node(SHR, 2, $1, $3); }
	;

rel_expr
    : shift_expr					{ $$ = $1; }
    | rel_expr '<' shift_expr		{ $$ = make_node('<', 2, $1, $3); }
    | rel_expr '>' shift_expr		{ $$ = make_node('>', 2, $1, $3); }
    | rel_expr LE shift_expr		{ $$ = make_node(LE, 2, $1, $3); }
    | rel_expr GE shift_expr		{ $$ = make_node(GE, 2, $1, $3); }
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

is_expr
	: l_or_expr						{ $$ = $1; }
	| is_expr KIS pattern			{ $$ = make_node(KIS, 2, $1, $3); }
	;

assign_expr
    : is_expr								{ $$ = $1; }
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
bool write_c = false;
int  main(int argc, char* argv[]) {
  extern FILE *yyin;
  yyin = stdin;
  if (argc >= 2) {
    yyin = fopen(argv[1], "r");
    if (!yyin) {
      fprintf(stderr, "%s: cannot open input file '%s'\n", *argv, argv[1]);
      exit(1);
    }
    for (int i = 2; i < argc; i++) {
      if (!strcmp(argv[i], "-c")) {
		printf("# Write C code\n");
	    write_c = true;
	  }
    }
  }
  return yyparse();
}
