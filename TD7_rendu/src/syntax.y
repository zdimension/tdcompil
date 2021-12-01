/*
 * syntax.y     -- Bison source of Toy syntaxic analyzer
 *
 * Copyright © 2014-2021 Erick Gallesio - I3S-CNRS/Polytech'Nice <eg@unice.fr>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 13-Dec-2014 13:19 (eg)
 * Last file update: 14-Jan-2021 20:19 (eg)
 */


// ======================================================================
//              C code
// ======================================================================
%{
#include <list.h>
#include "toy.h"
#include "ast.h"
#include "syntax.h"
#include "prodcode.h"
#include "analysis.h"
#include "symbols.h"
#include "utils.h"


#define FREE_NODE       (((void (*)(void *)) free_node))  // to symplify writing

extern int yylex(void);         ///< the tokenizer build by Flex/Lex


#define YYERROR_VERBOSE 1
static void yyerror(const char *s) {
    error_msg(NULL, (char *) s);
}


static void analysis_and_code(ast_node *node) {
  analysis(node);
  if (!error_detected) produce_code(node);
  free_node(node);
  symbol_table_free_unused_tables();
}

static int exit_compiler(int errors) {
  if (errors)
    fprintf(stderr, "*** %d error%s detected\n", errors, (errors > 1) ? "s": "");

  return errors; // return code = number of seen errors
}

%}

// ======================================================================
//              Yacc/Bison declarations
// ======================================================================

%union {
    int int_value;              // constant integer or boolean value
    float float_value;          // constant float value
    char *string_value;         // constant string value
    char *ident;                // identifier
    List lst;                   // used for lists of parameters (formal or effective)
    ast_node *ast;              // node pointer
};

%token <int_value> INTEGER BOOLEAN
%token <float_value>  FLOAT
%token <string_value> STRING CCODE
%token <ident> IDENTIFIER
%token KWHILE KIF KPRINT KRETURN KBREAK KFOR
%token TINT TFLOAT TBOOL TSTRING TVOID


%type   <ast>   declarations declaration
%type   <ast>   stmt expr var_decl type var call
%type   <ast>   func_decl prototype expr_opt init_var
%type   <ast>   for1 for2 for3
%type   <ast>   c_code func_body
%type   <lst>   fparam_list eparam_list stmt_list



%nonassoc IFX
%nonassoc KELSE

%right '='
%right '?' ':'
%left KOR
%left KAND
%left '>' '<' GE LE
%left EQ NE
%left '+' '-'
%left '*' '/' '%'
%left KNOT UMINUS
%left PP MM
%right POW
%left '['
%%

// ======================================================================
//              Grammar
// ======================================================================
program:        declarations            { return exit_compiler(error_detected); }
        ;


declarations: declarations declaration  { analysis_and_code($2);}
        |     declaration               { analysis_and_code($1);}
        ;

declaration:    var_decl ';'            { }
        |       func_decl               { }
        ;

stmt:           ';'                     { $$ = make_expr_statement(NULL); }
        |       expr ';'                { $$ = make_expr_statement($1); }
        |       KPRINT '(' eparam_list ')' ';'
                                        { $$ = make_print_statement($3); }
        |       '{' stmt_list '}'       { $$ = make_block_statement($2); }
        |       KWHILE '(' expr ')' stmt
                                        { $$ = make_while_statement($3, $5); }
        |       KFOR '(' for1 ';' for2 ';' for3 ')' stmt
                                        { $$ = make_for_statement($3, $5, $7, $9); }
        |       KRETURN expr_opt ';'    { $$ = make_return_statement($2); }
        |       KBREAK ';'              { $$ = make_break_statement(); }
        |       KIF '(' expr ')' stmt %prec IFX
                                        { $$ = make_if_statement($3, $5, NULL); }
        |       KIF '(' expr ')' stmt KELSE stmt
                                        { $$ = make_if_statement($3, $5, $7); }
        |       var_decl ';'            { $$ = $1;}
        |       prototype ';'           { $$ = $1;}
        |       error ';'               { yyerrok; $$ = NULL; }
        ;


stmt_list:      stmt_list stmt          { list_append($1, $2, FREE_NODE); $$ = $1; }
        |       /* empty */             { $$ = list_create();  }
        ;


var_decl:       var_decl ',' var init_var
                                        { add_variable_to_decl($1, $3, $4); $$ = $1; }
        |       type var init_var       { $$ = make_var_decl($2, $1, $3); }
        ;


init_var:       '=' expr                { $$ = $2; }
        |       /* empty */             { $$ = NULL; }
        ;

type:           TINT                    { $$ = int_type; }
        |       TFLOAT                  { $$ = float_type; }
        |       TBOOL                   { $$ = bool_type; }
        |       TSTRING                 { $$ = string_type; }
        |       TVOID                   { $$ = void_type; }
        ;

c_code
        : CCODE                         { $$ = make_c_code_literal($1); }
        ;

// ========== FUNCTIONS ==========
func_body
        : '{' stmt_list '}'             { $$ = make_block_statement($2); }
        | c_code                        { $$ = $1; }
        ;

func_decl:      prototype ';'           { $$ = $1; }
        |       prototype func_body
                                        { add_body_to_function($1, $2); $$ = $1; }
        ;


prototype:      type  var '(' fparam_list ')'
                                        { $$ = make_function($2, $1, $4); }
        ;

fparam_list:    fparam_list ',' type var{ add_parameter_to_function($1,$4,$3); $$=$1;}
        |       type var                { $$ = list_create();
                                          add_parameter_to_function($$, $2, $1);}
        |       /* empty */             { $$ = list_create(); }
        ;

eparam_list:    eparam_list ',' expr    { list_append($1, $3, FREE_NODE); $$ = $1; }
        |       expr                    { $$ = list_create();
                                          list_append($$, $1, FREE_NODE); }
        |       /* empty */             { $$ = list_create(); }
        ;


// ========== EXPRESSIONS ==========
expr:
                INTEGER               { $$ = make_integer_constant($1); }
        |       FLOAT                 { $$ = make_float_constant($1); }
        |       BOOLEAN               { $$ = make_boolean_constant($1); }
        |       STRING                { $$ = make_string_constant($1);}
        |       var                   { $$ = $1; }
        |       var '[' expr ']'      { $$ = make_string_access($1, $3); }
        |       call                  { $$ = $1; }
        |       var '=' expr          { $$ = make_expression("=",  assign, 2, $1,$3);}
        |       '-' expr %prec UMINUS { $$ = make_expression("-",  uarith, 1, $2); }
        |       '+' expr %prec UMINUS { $$ = $2; }
        |       expr '+' expr         { $$ = make_expression("+",  barith, 2, $1,$3);}
        |       expr '-' expr         { $$ = make_expression("-",  barith, 2, $1,$3);}
        |       expr '*' expr         { $$ = make_expression("*",  barith, 2, $1,$3);}
        |       expr '/' expr         { $$ = make_expression("/",  barith, 2, $1,$3);}
        |       expr '%' expr         { $$ = make_expression("%",  barith, 2, $1,$3);}
        |       expr POW expr         { $$ = make_expression("**", barith, 2, $1,$3);}
        |       expr '<' expr         { $$ = make_expression("<",  comp,   2, $1,$3);}
        |       expr '>' expr         { $$ = make_expression(">",  comp,   2, $1,$3);}
        |       expr EQ  expr         { $$ = make_expression("==", comp,   2, $1,$3);}
        |       expr GE  expr         { $$ = make_expression(">=", comp,   2, $1,$3);}
        |       expr LE  expr         { $$ = make_expression("<=", comp,   2, $1,$3);}
        |       expr NE  expr         { $$ = make_expression("!=", comp,   2, $1,$3);}
        |       expr KOR expr         { $$ = make_expression("||", blogic, 2, $1,$3);}
        |       expr KAND expr        { $$ = make_expression("&&", blogic, 2, $1,$3);}
        |       KNOT expr             { $$ = make_expression("!",  ulogic, 1, $2); }
        |       '('expr ')'           { $$ = make_expression("(", parenthesis,1,$2);}
        |       expr '?' expr ':' expr{ $$ = make_expression("?:",ternary,3,$1,$3,$5);}
        |       PP var                { $$ = make_expression("++", preincr,  1, $2); }
        |       MM var                { $$ = make_expression("--", preincr,  1, $2); }
        |       var PP                { $$ = make_expression("++", postincr, 1, $1); }
        |       var MM                { $$ = make_expression("--", postincr, 1, $1); }
        ;

// --- FOR
for1:           var_decl                { $$ = $1; }
        |       expr                    { $$ = $1; }
        |       /* empty */             { $$ = NULL; }
        ;

for2:           expr_opt                { $$ = $1; }
        ;

for3:           expr_opt                { $$ = $1; }
        ;


expr_opt:       expr                  { $$ = $1; }
        |       /* empty */           { $$ = NULL; }
        ;

var:            IDENTIFIER            { $$ = make_identifier($1); }

call:           var '(' eparam_list ')'
                                      { $$ = make_call($1, $3); }
        ;
%%
