/*
 * prodcode.c   -- Code Production
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
 *    Creation date: 21-Dec-2014 20:14 (eg)
 * Last file update: 14-Jan-2021 20:14 (eg)
 */

#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "toy.h"
#include "ast.h"
#include "prodcode.h"

FILE *fout;             ///< The file where code is emitted


// ======================================================================
//      Static variables / functions
// ======================================================================
static int margin = 0;                  ///< used for indenting code

/// Increments or decrements the current margin and indents
static void indent(int val) {
  margin += val;
  if (margin>0)
    fprintf(fout, "%*s", 2*margin, " ");
}


/// Produce code for the given code
static void code(ast_node *p) {
  int m;

  if (p) {
    m = margin;
    p->produce_code(p);
    margin = m;
  }
}


/// Write code on the output file. The printf format conventions are accepted
static void emit(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vfprintf(fout, fmt, ap);
  va_end(ap);
}


/// Produce code of expression eventually with a cast
inline static void code_expr_cast(ast_node *node) {
  if (AST_CAST(node)) {
    emit("(%s)(", TYPE_NAME(AST_CAST(node))); code(node); emit(")");
  } else
    code(node);
}


/// Produce the C name of a function
inline static void emit_function_name(ast_node *fct) {
  emit(IDENT_VAL(FUNCTION_NAME(fct)));
}


// ======================================================================
//
//                              CONSTANTS
//
// ======================================================================
void produce_code_constant(ast_node *node){
  if (AST_TYPE(node) == string_type)
    emit("%s", CONSTANT_STRING_VALUE(node));
  else if (AST_TYPE(node) == float_type)
    emit("((float) %g)", CONSTANT_FLOAT_VALUE(node));
  else
    emit("%d", CONSTANT_INT_VALUE(node));
}


// ======================================================================
//
//                              IDENTIFIERS
//
// ======================================================================
void produce_code_identifier(ast_node *node){ emit("%s", IDENT_VAL(node)); }


// ======================================================================
//
//                              TYPES
//
// ======================================================================
void produce_code_type(ast_node *node){ emit("%s", TYPE_NAME(node)); }


// ======================================================================
//
//                              VARIABLE DECLARATIONS
//
// ======================================================================
static void produce_declarations(ast_node *node, int emit_init) {
  /// This function is used for producing variable declaration
  struct s_var_decl *n = (struct s_var_decl *) node;
  int len = list_size(n->vars);

  if (n->is_static)
  {
      emit("static ");
  }

  code(AST_TYPE(node)); emit(" ");
  FORLIST(p, n->vars) {
    ast_node *var = ((ast_node **)list_item_data(p))[0];
    ast_node *val = ((ast_node **)list_item_data(p))[1];

    code(var);
    if (emit_init) {      // Initialization code must be issued
      emit(" = ");
      if (val) code_expr_cast(val); else emit(TYPE_DEFAULT_INIT(AST_TYPE(node)));
    }
    if (--len) emit(", ");
  }
  emit(";\n");
}

void produce_code_var_decl(ast_node *node) {
  produce_declarations(node, 1);  // Emit declarations and initialize the variables
}


// ======================================================================
//
//                              EXPRESSIONS
//
// ======================================================================
void produce_code_expression(ast_node *node) {
  char *name = EXPRESSION_OPERATOR(node);

  switch (EXPRESSION_ARITY(node)) {
    case 1:
      if (EXPRESSION_KIND(node) == preincr) {
        emit(name); code(EXPRESSION_OP1(node));
      } else if (EXPRESSION_KIND(node) == postincr) {
        code(EXPRESSION_OP1(node)); emit(name);
      } else {
        switch(name[0]) {
          case '-':
          case '!':
            emit("%s", name);
            // NO BREAK HERE !!!!
          case '(':
            emit("("); code(EXPRESSION_OP1(node)); emit(")");
            break;
        }
      }
      break;
    case 2:
      if (EXPRESSION_KIND(node)==comp &&
          AST_TYPE(EXPRESSION_OP1(node))==string_type) {
        // op2 is also a string (analysis has verified that point)
        // ⟹ We are comparing strings, generate a strcmp
        emit("(_toy_strcmp("); code(EXPRESSION_OP1(node));
        emit(", ");      code(EXPRESSION_OP2(node));
        emit(") %s 0)", name);
      } else if (strcmp(name, "**") == 0)  {
        // power operator
        emit("_toy_powint("); code(EXPRESSION_OP1(node));
        emit(", ");  code(EXPRESSION_OP2(node));
        emit(")");
      } else {
        // Simple binary operator on numbers
        code(EXPRESSION_OP1(node));
        emit(" %s ", name);
        code_expr_cast(EXPRESSION_OP2(node));
      }
      break;
    case 3: /* the only ternary operator is expr ? expr : expr */
      code(EXPRESSION_OP1(node)); emit (" ? "); code(EXPRESSION_OP2(node));
      emit(" : "); code(EXPRESSION_OP3(node));
      break;
  }
}


// ======================================================================
//
//                              STRING ACCESS
//
// ======================================================================
void produce_code_string_access(ast_node *node) {
    if (AST_KIND(STRING_ACCESS_INDEX(node)) == k_slice) {
        emit("_toy_str_slice(");
        code(STRING_ACCESS_STR(node));
        emit(", ");
        ast_node* slice_a = SLICE_A(STRING_ACCESS_INDEX(node));
        if (slice_a)
            code(slice_a);
        else
            emit("0");
        emit(", ");
        ast_node* slice_b = SLICE_B(STRING_ACCESS_INDEX(node));
        if (slice_b)
            code(slice_b);
        else
        {
            emit("strlen(");
            code(STRING_ACCESS_STR(node));
            emit(")");
        }
        emit(")");
    }
    else
    {
        emit("_toy_str_element(");
        code(STRING_ACCESS_STR(node));
        emit(", ");
        code(STRING_ACCESS_INDEX(node));
        emit(")");
    }
}

void produce_code_slice(ast_node* node) {
    ;
}

// ======================================================================
//
//                              FUNCTIONS
//
// ======================================================================
void produce_code_function(ast_node *node) {
  struct s_function *n = (struct s_function *) node;
  int comma = 0;

  // Emit function type and name
  code(AST_TYPE(node)); emit(" "); emit_function_name(node);

  // Produce parameters
  emit("(");
  if (list_size(n->parameters) == 0)
    emit("void");
  else {
    FORLIST(p, n->parameters) {
      ast_node *v = list_item_data(p);
      if (comma++) emit(", ");
      code(AST_TYPE(v)); emit(" "); code(v);
    }
  }
  emit(")");

  // Body or ';'
  if (n->body) { emit(" "); code(n->body); } else emit (";\n");
}


void produce_code_call(ast_node *node) {
  struct s_call *n = (struct s_call *) node;
  int comma = 0;

  if (!AST_TYPE(node)) return; // previous error detected

  code(n->callee); emit("(");
  // produce the parameters
  FORLIST(p, n->parameters) {
    if (comma++) emit(", ");
    code_expr_cast(list_item_data(p));
  }
  emit(")");
}

// ======================================================================
//
//                              STATEMENTS
//
// ======================================================================
void produce_code_if_statement(ast_node *node) {
  struct s_if_statement *n = (struct s_if_statement *) node;

  emit("if ("); code(n->cond); emit(")\n");
  indent(+1); code(n->then_stat);
  if (n->else_stat) {
    indent(-1); emit("else\n");
    indent(+1); code(n->else_stat);
  }
}


void produce_code_while_statement(ast_node *node) {
  struct s_while_statement *n = (struct s_while_statement *) node;

  emit("{ L%04d:\n", n->label);
  indent(+1); emit("while ("); code(n->cond); emit(")\n");
  indent(+1); code(n->body_stat);
  indent(-2); emit("}\n");
}


void produce_code_for_statement(ast_node *node) {
  struct s_for_statement *n = (struct s_for_statement *) node;

  emit("{\n");
  // Produce for1  (the code before the while)
  indent(+1); code(n->for1); emit(";\n");

  // produce for2 (the condition of the while
  indent(0);
  emit("while ("); if (n->for2) code(n->for2); else emit("1"); emit("){\n");

  // Produce the body of the for
  indent(+1); code(n->body_stat); emit("\n");

  // produce the for (incrementation code at end of body
  indent(0); emit("L%04d:\n", n->label);
  indent(0); if (n->for3) code(n->for3);  emit(";\n");

  // close the loop and the for block
  indent(-1); emit("}\n");
  indent(-1); emit("}\n");
}


void produce_code_block_statement(ast_node *node) {
  struct s_block_statement *n = (struct s_block_statement *)node;
  int sz = list_size(n->statements);

  emit("{\n"); indent(+1);
  FORLIST(p, n->statements) {
    code(list_item_data(p));
    if (--sz) indent(0);
  }
  indent(-1); emit("}\n");
}


void produce_code_expr_statement(ast_node *node) {
  struct s_expr_statement *n = (struct s_expr_statement *)node;

  if (n->stat) code(n->stat);
  emit(";\n");
}


void produce_code_print_statement(ast_node *node) {
  struct s_print_statement *n = (struct s_print_statement *) node;
  bool first_parameter = true;

  // Note: all the print must be in the same block (think to 'if (C) print(a, b)'
  emit("{\n"); indent(+1);
  FORLIST(p, n->parameters) {
    ast_node *item = list_item_data(p);

    if (! first_parameter) indent(0);
    if (AST_TYPE(item) == int_type) {                           // int
      emit("_toy_print_int(");
    } else if (AST_TYPE(item) == float_type) {                  // float
      emit("_toy_print_float(");
    } else if (AST_TYPE(item) == bool_type) {                   // bool
      emit("_toy_print_bool(");
    } else {                                                    //string
      emit("_toy_print_string(");
    }
    code(item);
    emit(");\n");
    first_parameter = false;
  }
  indent(0); emit("fflush(stdout);\n");
  indent(-1); emit("}\n");
}


void produce_code_return_statement(ast_node *node) {
  struct s_return_statement *n = (struct s_return_statement *) node;
  emit("return");
  if (n->expr) {
    emit(" "); code_expr_cast(n->expr);
  }
  emit(";\n");
}

void produce_code_break_statement(ast_node *node) {
  emit("break;");
}

void produce_code_c_code_literal(ast_node *node) {
  struct s_c_code_literal *n = (struct s_c_code_literal *) node;
  emit("{\n");
  emit("%s", n->code);
  emit("}\n");
}

// ======================================================================
//
// produce_code
//
// ======================================================================

void produce_code(ast_node * node) {
  static bool initialized = false;

  if (!initialized) {
    time_t now = time(NULL);

    emit("// Code generated by toy compiler v%s on %s", TOY_VERSION, ctime(&now));
    emit("#include <toy-runtime.h>\n\n");
    initialized = true;
  }
  code(node);
}
