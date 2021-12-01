/*
 * ast.h        -- Abstrat Syntax Tree
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
 *    Creation date: 14-Dec-2014 08:28 (eg)
 * Last file update: 16-Jan-2021 13:15 (eg)
 */

#ifndef _AST_H_
#define _AST_H_

#include <list.h>               // for the List type


/// The various kind of node we can have: constant, identifier, ...
enum node_kind {k_constant, k_identifier, k_type, k_var_decl, k_expression,
                k_call, k_function, k_parameter_list,
                k_if_statement, k_while_statement, k_for_statement,
                k_block_statement, k_expr_statement, k_print_statement,
                k_return_statement, k_break_statement, k_string_access};

typedef struct ast_node ast_node;

struct ast_node {
  enum node_kind kind;                  ///< kind of node
  ast_node *type;                       ///< node type (NULL for statements)
  ast_node *cast;                       ///< type in which node must be casted
  int line;                             ///< line # of the element
  void (*produce_code)(ast_node *this); ///< method for producing code
  void (*analysis)(ast_node *this);     ///< method for node analysis
  void (*free)(ast_node *this);         ///< method for freeing a node
#ifdef DEBUG
  char *info;                           ///< Used for message in debug mode
  int  c_line;                          ///< Source line where the object was created
#endif
};

#define AST_KIND(p)             (((ast_node *)(p))->kind)
#define AST_TYPE(p)             (((ast_node *)(p))->type)
#define AST_CAST(p)             (((ast_node *)(p))->cast)
#define AST_LINE(p)             (((ast_node *)(p))->line)
#define AST_CODE(p)             (((ast_node *)(p))->produce_code)
#define AST_ANALYSIS(p)         (((ast_node *)(p))->analysis)
#define AST_FREE(p)             (((ast_node *)(p))->free)


// ======================================================================
//
//                              CONSTANTS
//
// ======================================================================
struct s_constant {
  ast_node header;      ///< AST header
  union {
    int ivalue;
    float fvalue;
    char *svalue;
  } value;              ///< Value of the constant (can be int, float, bool or string)
};

#define CONSTANT_INT_VALUE(p)    (((struct s_constant *) p)->value.ivalue)
#define CONSTANT_FLOAT_VALUE(p)  (((struct s_constant *) p)->value.fvalue)
#define CONSTANT_STRING_VALUE(p) (((struct s_constant *) p)->value.svalue)

ast_node *make_integer_constant(int value);
ast_node *make_float_constant(float value);
ast_node *make_boolean_constant(int value);
ast_node *make_string_constant(char *value);

// ======================================================================
//
//                              IDENTIFIERS
//
// ======================================================================
struct s_identifier {
  ast_node header;      ///< AST header
  char *value;          ///< value of the identifier (a string)
};

#define IDENT_VAL(p)            (((struct s_identifier *) p)->value)

ast_node *make_identifier(char *id);

// ======================================================================
//
//                              TYPES
//
// ======================================================================
struct s_type {
  ast_node header;      ///< AST header
  char *name;           ///< external name of the type (e.g. "int", "string", ...)
  char *default_init;   ///< Default init value as a string (e.g. "0", "NULL", ...)
  bool is_standard;     ///< type is a predefined one (int, bool, ...)
};

#define TYPE_NAME(p)            (((struct s_type*) (p))->name)
#define TYPE_DEFAULT_INIT(p)    (((struct s_type*) (p))->default_init)
#define TYPE_IS_STANDARD(p)     (((struct s_type*) (p))->is_standard)

ast_node *make_type(char *name, char *default_init);

extern ast_node *int_type,      ///< AST node representing the "int" standard type
                *float_type,    ///< AST node representing the "float" standard type
                *bool_type,     ///< AST node representing the "bool" standard type
                *string_type,   ///< AST node representing the "string" standard type
                *void_type;     ///< AST node representing the "void" standard type


// ======================================================================
//
//                              VARIABLE DECLARATIONS
//
// ======================================================================
struct s_var_decl {
  ast_node header;              ///< AST header
  List     vars;                ///< List of declared variables
};

#define VARDECL_VARS(p)         (((struct s_var_decl*) (p))->vars)

ast_node *make_var_decl(ast_node *var, ast_node *type, ast_node *expr);
void add_variable_to_decl(ast_node *decl, ast_node *var, ast_node *expr);


// ======================================================================
//
//                              EXPRESSIONS
//
// ======================================================================
enum expr_kind {assign, uarith, barith, ulogic, blogic, comp, ternary, parenthesis,
                preincr, postincr};

struct s_expression {
  ast_node header;              ///< AST header
  char *operator;               ///< operator as a string (e.g. "+", "<=")
  char *cast;                   ///< type name in which the expression must be casted
  enum expr_kind ekind;         ///< kind of expression (for analysis)
  short arity;                  ///< operator arity
  ast_node *op1, *op2, *op3;    ///< operands (NULL if unused)
};

#define EXPRESSION_OPERATOR(p)   (((struct s_expression *) p)->operator)
#define EXPRESSION_KIND(p)       (((struct s_expression *) p)->ekind)
#define EXPRESSION_ARITY(p)      (((struct s_expression *) p)->arity)
#define EXPRESSION_OP1(p)        (((struct s_expression *) p)->op1)
#define EXPRESSION_OP2(p)        (((struct s_expression *) p)->op2)
#define EXPRESSION_OP3(p)        (((struct s_expression *) p)->op3)

ast_node *make_expression(char *operator, enum expr_kind ekind, int arity, ...);

// ======================================================================
//
//                              STRING ACCESS
//
// ======================================================================
struct s_string_access {
  ast_node header;              ///< AST header
  ast_node *str;                ///< String accessed
  ast_node *index;              ///< Index in the string
};

#define STRING_ACCESS_STR(p)    (((struct s_string_access *) p)->str)
#define STRING_ACCESS_INDEX(p)  (((struct s_string_access *) p)->index)
  
ast_node *make_string_access(ast_node* name, ast_node* index);  


// ======================================================================
//
//                              FUNCTIONS
//
// ======================================================================

/* ---- FUNCTION ------------------------------------------------------*/
struct s_function {
  ast_node header;              ///< AST header
  ast_node *name;               ///< Name of the function
  List parameters;              ///< Formal Parameters list of the function
  ast_node *body;               ///< AST of the body function
};

#define FUNCTION_NAME(p)        (((struct s_function *) p)->name)
#define FUNCTION_PARAMETERS(p)  (((struct s_function *) p)->parameters)
#define FUNCTION_BODY(p)        (((struct s_function *) p)->body)


ast_node *make_function(ast_node *name, ast_node *ret_type, List parameters);
void add_parameter_to_function(List lst, ast_node *name, ast_node *type);
void add_body_to_function(ast_node *func, List body);


/* ----CALL -----------------------------------------------------------*/
struct s_call {
  ast_node header;              ///< AST header
  ast_node *callee;             ///< Callee function
  List parameters;              ///< Actual parameter list of the function
};

#define CALL_CALLEE(p)          (((struct s_call *) p)->callee)
#define CALL_PARAMETERS(p)      (((struct s_call *) p)->parameters)


ast_node *make_call(ast_node *callee, List parameters);


// ======================================================================
//
//                              STATEMENTS
//
// ======================================================================

/* ---- IF -------------------------------------------------- */
struct s_if_statement {
  ast_node header;              ///< AST header
  ast_node *cond;               ///< AST of the IF condition
  ast_node *then_stat;          ///< AST of the then part of IF
  ast_node *else_stat;          ///< AST of the else part of IF (maybe NULL)
};

ast_node *make_if_statement(ast_node *cond, ast_node *tpart, ast_node *epart);


/* ---- WHILE ------------------------------------------------------------ */
struct s_while_statement {
  ast_node header;              ///< AST header
  ast_node *cond;               ///< while condition AST
  ast_node *body_stat;          ///< while body AST
  int label;                    ///< label for continue
};

ast_node *make_while_statement(ast_node *cond, ast_node *body);

/* ---- FOR ------------------------------------------------------------ */
struct s_for_statement {
  ast_node header;              ///< AST header
  ast_node *for1,*for2,*for3;   ///< components of the "condition"
  ast_node *body_stat;          ///< for body AST
  int label;                    ///< label for continue  
};

ast_node *make_for_statement(ast_node *for1, ast_node *for2, ast_node *for3,
                             ast_node *body);

/* ---- BLOCK ------------------------------------------------------------ */
struct s_block_statement {
  ast_node header;              ///< AST header
  List statements;              ///< statements list of the block
};

ast_node *make_block_statement(List statements);

/* ---- EXPR. STAT. ------------------------------------------------------ */
struct s_expr_statement {
  ast_node header;              ///< AST header
  ast_node *stat;               ///< Expr. transformed in a statement (with a ';')
};

ast_node *make_expr_statement(ast_node *stat);

/* ---- PRINT ------------------------------------------------------------ */
struct s_print_statement {
  ast_node header;              ///< AST header
  List parameters;              ///< effective parameters of a print statement
};

ast_node *make_print_statement(List parameters);

/* ---- RETURN ------------------------------------------------------------ */
struct s_return_statement {
  ast_node header;              ///< AST header
  ast_node *expr;               ///< the returned expression AST
};

ast_node *make_return_statement(ast_node *expr);

/* ---- BREAK ------------------------------------------------------------ */
struct s_break_statement {
  ast_node header;              ///< AST header
};

ast_node *make_break_statement(void);


// ======================================================================
// Misc:
//      - Freeing an ast
//      - the initialization function for the ast module
// ======================================================================
void free_node(ast_node *node); ///< Freeing an AST
void init_ast(void);            ///< AST module initialization function

#endif /* _AST_H_ */
