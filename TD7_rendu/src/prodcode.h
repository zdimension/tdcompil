/*
 * prodcode.h   -- Code Production
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
 * Last file update: 14-Jan-2021 16:08 (eg)
 */

#ifndef _PRODCODE_H_
#define _PRODCODE_H_

extern FILE *fout;

void produce_code(ast_node *node);

/// Functions declared below are used by the AST. They should not be
/// used directly

// ======================================================================
//
//                              CONSTANTS
//
// ======================================================================
void produce_code_constant(ast_node *node);


// ======================================================================
//
//                              IDENTIFIERS
//
// ======================================================================
void produce_code_identifier(ast_node *node);
void produce_code_prefixed_identifier(ast_node *node);

// ======================================================================
//
//                              SIMPLE TYPES
//
// ======================================================================
void produce_code_type(ast_node *node);

// ======================================================================
//
//                              VARIABLE DECLARATIONS
//
// ======================================================================
void produce_code_var_decl(ast_node *node);

// ======================================================================
//
//                              EXPRESSIONS
//
// ======================================================================
void produce_code_expression(ast_node *node);

// ======================================================================
//
//                              STRING ACCESS
//
// ======================================================================
void produce_code_string_access(ast_node *node);
void produce_code_slice(ast_node* node);

// ======================================================================
//
//                              FUNCTIONS
//
// ======================================================================
void produce_code_function(ast_node *node);
void produce_code_parameter_list(ast_node *node);
void produce_code_call(ast_node *node);

// ======================================================================
//
//                              STATEMENTS
//
// ======================================================================
void produce_code_if_statement(ast_node *node);
void produce_code_while_statement(ast_node *node);
void produce_code_for_statement(ast_node *node);
void produce_code_block_statement(ast_node *node);
void produce_code_expr_statement(ast_node *node);
void produce_code_print_statement(ast_node *node);
void produce_code_return_statement(ast_node *node);
void produce_code_break_statement(ast_node *node);


void produce_code_c_code_literal(ast_node *node);

#endif /* _PRODCODE_H_ */
