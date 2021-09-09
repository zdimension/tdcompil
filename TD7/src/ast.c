/*
 * ast.c        -- Abstract Syntax Tree
 *
 * Copyright © 2014-2018 Erick Gallesio - I3S-CNRS/Polytech'Nice <eg@unice.fr>
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
 *    Creation date: 14-Dec-2014 08:33 (eg)
 * Last file update:  7-Dec-2018 11:15 (eg)
 */

#include <stdarg.h>
#include <string.h>

#include "toy.h"
#include "ast.h"
#include "analysis.h"
#include "prodcode.h"
#include "utils.h"

/// \file
///
/// The module ast is in charge of allocating and freeing all the
/// elements of the Abstact Syntax Tree (AST). For each element xxx
/// which can be stored in the AST, we need to declare two functions:
///   - ast_node *make_xxx(....) in charge of the new node allocation.
///     Parameters depend of the node which need to be created
///     (e.g. and if-then-else node takes 3 ast as parameters, whereas
///     an integer constants takes just one int
///   - void free_xxx(ast_node *node) in charge of freeing the given
///     node


#define DEF_AST0(_var, _name, _type)                                    \
  struct s_##_name *_var= must_malloc(sizeof (struct s_##_name));       \
  p->header.kind         = k_##_name;                                   \
  p->header.type         = _type;                                       \
  p->header.cast         = NULL;                                        \
  p->header.line         = yylineno;                                    \
  p->header.produce_code = produce_code_##_name;                        \
  p->header.analysis     = analysis_##_name;                            \
  p->header.free         = free_##_name;                                \


#ifdef DEBUG
  #define DEF_AST(_var, _name, _type)           \
    DEF_AST0(_var, _name, _type)                \
    p->header.info    = #_name;                 \
    p->header.c_line  = __LINE__;
#else
  #define DEF_AST(_var, _name, _type) \
    DEF_AST0(_var, _name, _type)
#endif


// ======================================================================
//
//                              CONSTANTS
//
// ======================================================================
static void free_constant(ast_node *node) {
  if (AST_TYPE(node) == string_type)
    free(CONSTANT_STRING_VALUE(node));
  free(node);
}

ast_node *make_integer_constant(int value) {
  DEF_AST(p, constant, int_type);
  p->value.ivalue = value;
  return (ast_node *) p;
}

ast_node *make_float_constant(float value) {
  DEF_AST(p, constant, float_type);
  p->value.fvalue = value;
  return (ast_node *) p;
}

ast_node *make_boolean_constant(int value) {
  DEF_AST(p, constant, bool_type);
  p->value.ivalue = value;
  return (ast_node *) p;
}

ast_node *make_string_constant(char* value) {
  DEF_AST(p, constant, string_type);
  p->value.svalue = value;
  return (ast_node *) p;
}

// ======================================================================
//
//                              IDENTIFIERS
//
// ======================================================================
static void free_identifier(ast_node *node) {
  free(IDENT_VAL(node)); // NO
  free(node);
}

ast_node *make_identifier(char *id) {
  DEF_AST(p, identifier, NULL);
  p->value = id;
  return (ast_node *) p;
}


// ======================================================================
//
//                              TYPES
//
// ======================================================================
static void free_type(ast_node *node) {/* never free types */ }


ast_node *make_type(char *name, char *default_init) {
  DEF_AST(p, type, (ast_node *) p);
  p->name          = name;
  p->default_init  = default_init;
  p->is_standard   = false;
  return (ast_node *) p;
}


static ast_node *make_standard_type(char *name, char *default_init) {
  ast_node * n = make_type(name, default_init); 
  TYPE_IS_STANDARD(n) = true;
  return n;
}


/* Exported symbols representing standard types */
ast_node *int_type, *float_type, *bool_type, *string_type, *void_type;


// ======================================================================
//
//                              VARIABLE DECLARATIONS
//
// ======================================================================
static void free_var_decl(ast_node *node) {
  list_destroy(VARDECL_VARS(node));
  free(node);
}

static void free_one_var_decl(void *p) {
  // p is a two cell array where p[0] is the var and p[1] its init. expression
  ast_node **table = p;

  // Free the expression (not the var which must stay in symbol table
  free_node(table[1]);
}

ast_node *make_var_decl(ast_node *var, ast_node *type, ast_node* expr) {
  DEF_AST(p, var_decl, type);

  /* add the given variable to a newly created list */
  p->vars = list_create();
  add_variable_to_decl((ast_node *) p, var, expr);

  return (ast_node *) p;
}

void add_variable_to_decl(ast_node *decl, ast_node *var, ast_node *expr) {
  ast_node **table = must_malloc(2 * sizeof(ast_node *));

  AST_TYPE(var) = AST_TYPE(decl);
  // Build a couple for the var and its initial value (maybe NULL)
  table[0] = var;
  table[1] = expr;
  list_append(VARDECL_VARS(decl), table, free_one_var_decl);
}



// ======================================================================
//
//                              EXPRESSIONS
//
// ======================================================================
static void free_expression(ast_node *node) {
  free_node(EXPRESSION_OP1(node));
  free_node(EXPRESSION_OP2(node));
  free_node(EXPRESSION_OP3(node));
  free(node);
}

ast_node *make_expression(char *operator, enum expr_kind ekind, int arity, ...)
{
  va_list ap;
  va_start(ap, arity);

  DEF_AST(p, expression, NULL);
  p->operator = operator;
  p->cast     = NULL;                   // NULL => no cast needed
  p->ekind    = ekind;
  p->arity    = arity;

  // Fill in the operands
  EXPRESSION_OP1(p) = ((arity >= 1) ? va_arg(ap, ast_node *): NULL);
  EXPRESSION_OP2(p) = ((arity >= 2) ? va_arg(ap, ast_node *): NULL);
  EXPRESSION_OP3(p) = ((arity >= 3) ? va_arg(ap, ast_node *): NULL);
  va_end(ap);

  return (ast_node *) p;
}

// ======================================================================
//
//                              FUNCTIONS
//
// ======================================================================

/* ---- FUNCTION ------------------------------------------------------*/
static void free_function(ast_node *node){
  // Keep the function name and its parameter list, to permit controls
  // when the function is called later. Since functions cannot be
  // nested, they will never be freed. Nevertheless, code of the
  // function can be freed after generation was done
  free_node(((struct s_function *) node)->body);
}

ast_node *make_function(ast_node *name, ast_node *ret_type, List parameters) {
  DEF_AST(p, function, ret_type);
  p->name       = name;
  p->parameters = parameters;
  p->body       = NULL; // will be filled later if not a prototype

  return (ast_node *) p;
}

void add_parameter_to_function(List lst, ast_node *name, ast_node *type) {
  AST_TYPE(name) = type;
  list_append(lst, name, free);
}

void add_body_to_function(ast_node *func, List body) {
  struct s_function *f = (struct s_function *) func;
  f->body = make_block_statement(body);
}

/* ----CALL -----------------------------------------------------------*/
static void free_call(ast_node *node) {
  // Don't free the name which is in symbol table, but free parameters
  list_destroy(CALL_PARAMETERS(node));
  free(node);
}

ast_node * make_call(ast_node *callee, List parameters)
{
  DEF_AST(p, call, NULL);

  p->callee     = callee;
  p->parameters = parameters;

  return (ast_node *) p;
}


// ======================================================================
//
//                              STATEMENTS
//
// ======================================================================

/* ---- IF ------------------------------------------------------------ */
static void free_if_statement(ast_node *node){
  struct s_if_statement *n = (struct s_if_statement *) node;
  free_node(n-> cond); free_node(n->then_stat); free_node(n->else_stat);
  free(node);
}

ast_node *make_if_statement(ast_node *cond, ast_node *tpart, ast_node *epart) {
  DEF_AST(p, if_statement, NULL);
  p->cond      = cond;
  p->then_stat = tpart;
  p->else_stat = epart;
  return (ast_node *) p;
}

/* ---- WHILE ------------------------------------------------------------ */
static void free_while_statement(ast_node *node){
  struct s_while_statement *n = (struct s_while_statement *)node;
  free_node(n->cond); free_node(n->body_stat);
  free(node);
}

ast_node *make_while_statement(ast_node *cond, ast_node *body) {
  DEF_AST(p, while_statement, NULL);
  p->cond      = cond;
  p->body_stat = body;
  return (ast_node *) p;
}

/* ---- FOR ------------------------------------------------------------ */
static void free_for_statement(ast_node *node){
  struct s_for_statement *n = (struct s_for_statement *)node;
  free_node(n->for1); free_node(n->for2); free_node(n->for3);
  free_node(n->body_stat);
  free(node);
}

ast_node *make_for_statement(ast_node *for1, ast_node *for2, ast_node *for3,
                             ast_node *body) {
  DEF_AST(p, for_statement, NULL);
  p->for1      = for1;
  p->for2      = for2;
  p->for3      = for3;
  p->body_stat = body;
  return (ast_node *) p;
}

/* ---- SWITCH ----------------------------------------------------------- */
static void free_switch_statement(ast_node *node) {
  struct s_switch_statement *n = (struct s_switch_statement *)node;
  list_destroy(n->cond_list); free_node(n->defcond);
  free(node);
}

ast_node *make_switch_statement(List cond_list, ast_node *defcond) {
  DEF_AST(p, switch_statement, NULL);
  p->cond_list = cond_list;
  p->defcond   = defcond;
  return (ast_node *) p;
}

/* ---- TRY -------------------------------------------------------------- */
static void free_try_statement(ast_node *node) {
  struct s_try_statement *n = (struct s_try_statement *)node;
  free_node(n->try); free_node(n->catch); free_node(n->finally);
  free(node);
}

ast_node *make_try_statement(ast_node *try, ast_node *catch, ast_node *finally) {
  DEF_AST(p, try_statement, NULL);
  p->try     = try;
  p->catch   = catch;
  p->finally = finally;
  return (ast_node *) p;
}

/* ---- THROW ---------------------------------------------------------- */
static void free_throw_statement(ast_node *node){
  free(node);
}

ast_node *make_throw_statement(void)
{
  DEF_AST(p, throw_statement, NULL);
  return (ast_node *) p;
}

/* ---- BLOCK ------------------------------------------------------------ */
static void free_block_statement(ast_node *node){
  struct s_block_statement *n = (struct s_block_statement *)node;
  list_for_each(n->statements, (list_iterator) free_node);
  free(node);
}

ast_node *make_block_statement(List statements)
{
  DEF_AST(p, block_statement, NULL);
  p->statements = statements;
  return (ast_node *) p;
}

/* ---- EXPR. STAT. ------------------------------------------------------ */
static void free_expr_statement(ast_node *node){
  free_node(((struct s_expr_statement *)node)->stat); free(node);
}

ast_node *make_expr_statement(ast_node *stat)
{
  DEF_AST(p, expr_statement, NULL);
  p->stat        = stat;
  return (ast_node *) p;
}


/* ---- PRINT ------------------------------------------------------------ */
static void free_print_statement(ast_node *node){
  list_destroy(((struct s_print_statement *)node)->parameters);
  free(node);
}

ast_node *make_print_statement(List parameters)
{
  DEF_AST(p, print_statement, NULL);
  p->parameters = parameters;
  return (ast_node *) p;
}

/* ---- RETURN ---------------------------------------------------------- */
static void free_return_statement(ast_node *node){
  free_node(((struct s_return_statement *)node)->expr); free(node);
}

ast_node *make_return_statement(ast_node *expr)
{
  DEF_AST(p, return_statement, NULL);
  p->expr   = expr;
  return (ast_node *) p;
}

/* ---- BREAK ---------------------------------------------------------- */
static void free_break_statement(ast_node *node){
  free(node);
}

ast_node *make_break_statement(void)
{
  DEF_AST(p, break_statement, NULL);
  return (ast_node *) p;
}

// ======================================================================
// Free_node:
//      Call the function embedded in the node itself
// ======================================================================
void free_node(ast_node *node)
{
  if (!node) return;
#ifdef DEBUG
  //fprintf(stderr, "***Free ==> %p %s %d\n", node, node->info, node->c_line);
#endif
  AST_FREE(node)(node);
}

// ======================================================================
//      The initialization function for the ast module
// ======================================================================
void init_ast(void)
{
  // Initialization of standard types
  int_type    = make_standard_type("int",         "0");
  float_type  = make_standard_type("float",       "0.0");
  bool_type   = make_standard_type("char",        "0");
  string_type = make_standard_type("_toy_string", "NULL");
  void_type   = make_standard_type("void",        "");
}
