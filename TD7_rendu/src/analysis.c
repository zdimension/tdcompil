/*
 * analysis.c   -- AST analysis and typing
 *
 * Copyright © 2015-2021 Erick Gallesio - I3S-CNRS/Polytech Nice-Sophia <eg@unice.fr>
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
 *    Creation date: 14-Jan-2015 16:55 (eg)
 * Last file update: 14-Jan-2021 17:04 (eg)
 */

#include <string.h>
#include "toy.h"
#include "ast.h"
#include "utils.h"
#include "symbols.h"
#include "analysis.h"

/// \file
///
/// This module is in charge of analyzing the ASTs built by the
/// compiler. The main function of this module is analysis() which
/// take an AST as parameter and descends recursively it to fully
/// analyze this AST. Each node of the AST embeds a pointer on a
/// specialized function in charge of the node analysis.


static ast_node *current_function = NULL;
static int current_loop           = 0;

/// Test if a node is a number (that is an integer or a float)
static bool is_number(ast_node *n){
  return AST_TYPE(n) == int_type || AST_TYPE(n) == float_type;
}


/// The function compatible_types tests if e2 is compatible with e1.
/// Generally, e1 is on the left and e2 on the right (e.g "e1 = e2").
///
static bool compatible_types(ast_node *e1, ast_node *e2) {
  ast_node *t1 = AST_TYPE(e1), *t2 = AST_TYPE(e2);

  if (!t1 || !t2) return true; // to avoid error cascades => true
  if (t1 == t2)   return true; // same types => true

  // Testing the mix of integer and float computations
  if ((t1 == int_type   && t2 == float_type) || (t1 == float_type && t2 == int_type))
    return true; 

  return false;
}


// ======================================================================
//
//                              CONSTANTS
//
// ======================================================================
void analysis_constant(ast_node *node){ /* constants are typed when created */ }

// ======================================================================
//
//                              IDENTIFIERS
//
// ======================================================================
void analysis_identifier(ast_node *node){
  char *id           = IDENT_VAL(node);
  ast_node *var      = symbol_table_search(id);

  AST_TYPE(node) = int_type; // for now
  if (!var) {
    // var is undeclared. Declare it as an integer to avoid further errors
    symbol_table_declare_object(id, node);
    error_msg(node, "%s is not declared", id);
  }
  else if (AST_KIND(var) == k_function) {
    // var is declared but as a function.
    error_msg(node, "%s function used as variable", id);
  }
  else if (AST_KIND(var) == k_identifier) {
    // Set type of the node to the type of the identifier in the table
    AST_TYPE(node) = AST_TYPE(var);
  }
  else {
    die("fatal error when analyzing an identifier");
  }
}


// ======================================================================
//
//                              TYPES
//
// ======================================================================
void analysis_type(ast_node *node){
  if (TYPE_IS_STANDARD(node)) return;  // Standard type are correct
}


// ======================================================================
//
//                              VARIABLE DECLARATIONS
//
// ======================================================================
static int is_const(ast_node *node) {
  /// Determine if node denotes a constant expression
  if (AST_KIND(node) == k_constant)
    return 1;
  if (AST_KIND(node) == k_expression) {
    int c1, c2=1, c3=1;
    switch (EXPRESSION_ARITY(node)) {
        case 3: c3 = is_const(EXPRESSION_OP3(node)); // no break !!!
        case 2: c2 = is_const(EXPRESSION_OP2(node)); // no break !!!
        case 1: c1 = is_const(EXPRESSION_OP1(node)); break;
        default: die("weird operator arity");
    }
    return c1 && c2 && c3;
  }
  // If we are here, node doesn't denote a constant
  return 0;
}


void analysis_var_decl(ast_node *node) {
  analysis(AST_TYPE(node));

  // Verify that we don't declare void vars
  if (AST_TYPE(node) == void_type)
    error_msg(node, "void is an improper type here");

  // Enter all the vars in the symbol table
  bool global_decl = (!current_function);

  FORLIST(p, VARDECL_VARS(node)) {
    // p denote a couple with a variable identifier and initial value
    ast_node *var = ((ast_node **)list_item_data(p))[0];
    ast_node *val = ((ast_node **)list_item_data(p))[1];

    symbol_table_declare_object(IDENT_VAL(var), var);

    // type the expression and verify that the type is conform to declaration
    if (val) {
      analysis(val);
      if (!compatible_types(node, val))
        error_msg(node, "initializer type incompatible with '%s'", IDENT_VAL(var));
      if (global_decl && !is_const(val))
        error_msg(node, "initializer for '%s' is not constant", IDENT_VAL(var));
    }
  }
}


// ======================================================================
//
//                              EXPRESSIONS
//
// ======================================================================
void analysis_expression(ast_node *node) {
  char     *name = EXPRESSION_OPERATOR(node);
  ast_node  *op1 = EXPRESSION_OP1(node);
  ast_node  *op2 = EXPRESSION_OP2(node);  /* may be NULL */
  ast_node  *op3 = EXPRESSION_OP3(node);  /* may be NULL */

  switch (EXPRESSION_ARITY(node)) {
    case 1: analysis(op1);
            switch(EXPRESSION_KIND(node)) {
              case preincr:
              case postincr:
              case uarith:
                if (AST_TYPE(op1) != int_type)
                  error_msg(node, "operand of unary %s must be int", name);
                AST_TYPE(node) = int_type;
                break;
              case ulogic:
                if (AST_TYPE(op1) != bool_type)
                  error_msg(node, "operand of unary %s must be bool", name);
                AST_TYPE(node) = bool_type;
                break;
              case parenthesis:
                AST_TYPE(node) = AST_TYPE(op1);
                break;
              default: die("unhandled unary expression");
            }
            break;

    case 2: analysis(op1); analysis(op2);
            if (!AST_TYPE(op1) || !AST_TYPE(op2)) //
              // if one of the types is NULL, we consider that this is compatible
              // It arises generally when a variable is auto-declared
              break;
            if (! compatible_types(op1, op2))
              error_msg(node, "operands of %s have incompatible types",
                              !strcmp(name, "=")? "assignment" : name);

            switch(EXPRESSION_KIND(node)) {
              case assign:
                AST_TYPE(node)= AST_TYPE(op2);
                break;
              case barith:
                if (!is_number(op1) || !is_number(op2))
                  error_msg(node, "operands of %s must be numbers", name);
                AST_TYPE(node) = (AST_TYPE(op1)== float_type ||
                                  AST_TYPE(op2) == float_type)? float_type: int_type;
                break;
              case blogic:
                if (AST_TYPE(op1) != bool_type || AST_TYPE(op2) != bool_type)
                  error_msg(node, "operands of %s must be bool", name);
                AST_TYPE(node) = bool_type;
                break;
              case comp:
                AST_TYPE(node) = bool_type;
                break;
              default: die("unhandled binary expression");
            }
            break;
    case 3: // the only ternary operator is expr ? expr : expr
            analysis(op1);analysis(op2); analysis(op3);
            if (AST_TYPE(op1) != bool_type)
              error_msg(node, "test is not boolean in a '?:' expression");
            else {
              if (AST_TYPE(op2) != AST_TYPE(op3))
                // FIXME: c'est bien plus difficle que ça....
                error_msg(node, "branches of '?:' are of different types");
            }
            AST_TYPE(node) = AST_TYPE(op2);
            break;
  }
}

// ======================================================================
//
//                              STRING ACCESS
//
// ======================================================================
void analysis_string_access(ast_node *node) {
    analysis(STRING_ACCESS_STR(node));
    analysis(STRING_ACCESS_INDEX(node));

    if (AST_TYPE(STRING_ACCESS_STR(node)) != string_type)
        error_msg(STRING_ACCESS_STR(node), "string access must be applied to a string");

    if (AST_KIND(STRING_ACCESS_INDEX(node)) != k_slice && AST_TYPE(STRING_ACCESS_INDEX(node)) != int_type)
        error_msg(STRING_ACCESS_INDEX(node), "index of string access must be an integer or a slice");
}

void analysis_slice(ast_node* node) {
    if (SLICE_A(node))
    {
        analysis(SLICE_A(node));
        if (AST_TYPE(SLICE_A(node)) != int_type)
            error_msg(SLICE_A(node), "first index of slice must be an integer");
    }

    if (SLICE_B(node))
    {
        analysis(SLICE_B(node));
        if (AST_TYPE(SLICE_B(node)) != int_type)
            error_msg(SLICE_B(node), "second index of slice must be an integer");
    }
}


// ======================================================================
//
//                              FUNCTIONS
//
// ======================================================================
static int parameter_cmp(List formal, List effective, char *id, ast_node *node) {
  List_item  p1 = list_head(formal);
  List_item  p2 = list_head(effective);
  int       err = 0;

  for (int idx=1; p1 && p2; p1 = list_item_next(p1), p2 = list_item_next(p2), idx++){
    if (!compatible_types(list_item_data(p1), list_item_data(p2))) {
      error_msg(node, "type of parameter #%d is not compatible with "
                      "previous definition of '%s'", idx, id);
      err += 1;
    }
  }

  if (p1 && !p2) {
    error_msg(node, "call or definition of '%s' has not enough parameters", id);
    err++;
  }
  if (!p1 && p2) {
    error_msg(node, "call or definition of '%s' has too much parameters", id);
    err++;
  }
  return !err;
}


static void analysis_formal(List formal) {
  int pos = 0, len  = list_size(formal);
  char *names[len];

  FORLIST(p, formal) {
    ast_node *param= list_item_data(p);

    // verify formal parameter type
    if (AST_TYPE(param) == void_type)
      error_msg(param, "type of parameter cannot be void");
    else
      analysis(AST_TYPE(param));

    // Verify that this parameter was not previously used
    for (int i = 0; i < pos; i ++) {
      if (strcmp(IDENT_VAL(param), names[i]) == 0) {
        error_msg(param, "name '%s' was already used for paramater #%d",
                         IDENT_VAL(param), i+1);
        break;
      }
    }
    // Retain the current parameter name for further controls
    names[pos++] = IDENT_VAL(param);
  }
}

// ----------------------------------------------------------------------
// Function / Method Declaration
// ----------------------------------------------------------------------
static void analysis_function_body(ast_node *node) {
  struct s_function *n = (struct s_function *) node;

  if (n->body->kind != k_block_statement)
      return;

  // Declare the parameters in a new scope
  enter_scope();
  FORLIST(p, n->parameters) {
    ast_node *param= list_item_data(p);

    symbol_table_declare_object(IDENT_VAL(param), param);
  }

  // Analyze the statements of the function (don't use analysis_block_statement
  // which enter a new scope)
  struct s_block_statement *body = (struct s_block_statement *) n->body;

  current_function = node;
  FORLIST(p, body->statements) analysis(list_item_data(p));
  current_function = NULL;

  leave_scope();
}


void analysis_function(ast_node *node) {
  struct s_function *n = (struct s_function *) node;
  char *id             = IDENT_VAL(n->name);

  analysis(AST_TYPE(node));                     // Verify the return type
  analysis_formal(FUNCTION_PARAMETERS(node));   // Verify the formal parameter list

  // Declare  the function
  ast_node *proto = symbol_table_search_prototype(id);
  if (proto) {
    if (parameter_cmp(FUNCTION_PARAMETERS(proto),FUNCTION_PARAMETERS(node),id,node))
      symbol_table_replace_prototype(id, node);
  } else
    symbol_table_declare_object(id, node);

  // Body analysis
  if (n->body) analysis_function_body(node);
}


// ----------------------------------------------------------------------
// Function call
// ----------------------------------------------------------------------
void analysis_call(ast_node *node) {
  struct s_call *n = (struct s_call *) node;

  // Simple function call: Search function name
  char *id = IDENT_VAL(n->callee);
  struct ast_node *func = symbol_table_search(id);

  if (!func || (func && AST_KIND(func) != k_function))
    error_msg(node, "function %s is undefined", id);
  // if !func, we could declare it. Refrain to do so: which type?, which params?
  else
    // func is declared as a function. Set type of node to its return type
    AST_TYPE(n) = AST_TYPE(func);

  // Type the parameter list
  FORLIST(p, n->parameters) analysis(list_item_data(p));

  if (func)
    // Compare formal parameter list with effective list
    parameter_cmp(FUNCTION_PARAMETERS(func), CALL_PARAMETERS(n), id, node);
}


// ======================================================================
//
//                              STATEMENTS
//
// ======================================================================
void analysis_if_statement(ast_node *node) {
  struct s_if_statement *n = (struct s_if_statement *) node;

  // Analyze condition
  analysis(n->cond);
  if (AST_TYPE(n->cond) != bool_type)
    error_msg(node, "condition must be boolean");

  // Analyze if and then parts
  analysis(n->then_stat);
  analysis(n->else_stat);
}


void analysis_while_statement(ast_node *node) {
  struct s_while_statement *n = (struct s_while_statement *) node;
  int old_loop = current_loop;

  // Analyze condition
  analysis(n->cond);
  if (AST_TYPE(n->cond) != bool_type)
    error_msg(node, "condition must be boolean");

  // Analyze loop body
  current_loop = n->label;
  analysis(n->body_stat);
  current_loop = old_loop;
}


void analysis_for_statement(ast_node *node) {
  struct s_for_statement *n = (struct s_for_statement *) node;
  int old_loop = current_loop;

  enter_scope();
  // Analyze the three components of the "condition"
  analysis(n->for1); analysis(n->for2); analysis(n->for3);
  if (n->for2 && (AST_TYPE(n->for2) != bool_type))
    error_msg(n->for2, "condition must be boolean");

  // Retain that we are in a loop (for breaks) and analyze loop body
  current_loop = n->label;
  analysis(n->body_stat);
  current_loop = old_loop;
  leave_scope();
}


void analysis_block_statement(ast_node *node) {
  enter_scope();
  list_for_each(((struct s_block_statement *) node)->statements,
                (list_iterator) analysis);
  leave_scope();
}


void analysis_expr_statement(ast_node *node) {
  analysis(((struct s_expr_statement *)node)->stat);
}

void analysis_print_statement(ast_node *node) {
  struct s_print_statement *n = (struct s_print_statement *) node;
  // Type the parameter list
  FORLIST(p, n->parameters) analysis(list_item_data(p));
}

void analysis_return_statement(ast_node *node) {
  struct s_return_statement *n = (struct s_return_statement *) node;
  analysis(n->expr);

  if (n->expr) {
    // Return with an expression
    if (AST_TYPE(current_function) == void_type)
      error_msg(node, "current function is void; expression unexpected here");
    else
      if (!compatible_types(current_function, n->expr))
        error_msg(node, "type of expression is incompatible with function type");
  } else {
    // No expression in the return
    if (AST_TYPE(current_function) != void_type)
      error_msg(node, "expression expected after return");
  }
}

void analysis_break_statement(ast_node *node) {
  if (!current_loop)
    error_msg(node, "break used outside a loop");
}

void analysis_c_code_literal(ast_node* node) {
  ;
}

// ======================================================================

/// The main analysis function. This function will launch the analysis
/// on the abstract tree given as parameter. The analysis will travel
/// recursively the tree, by launching an analysis on the encountered
/// nodes.
void analysis(ast_node *node) {
  if (!node) return;
  if (!AST_ANALYSIS(node)) {
    fprintf(stderr, "No analysis function on node %p (%d) \n", node, AST_KIND(node));
    exit(1);
  }
  AST_ANALYSIS(node)(node);
}
