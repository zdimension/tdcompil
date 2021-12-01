/*
 * analysis.h   -- AST analysis and typing
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
 * Last file update: 14-Jan-2021 16:10 (eg)
 */

#ifndef _ANALYSIS_H_
#define _ANALYSIS_H_

// ======================================================================
//
//                              CONSTANTS
//
// ======================================================================
void analysis_constant(ast_node *node);

// ======================================================================
//
//                              IDENTIFIERS
//
// ======================================================================
void analysis_identifier(ast_node *node);
void analysis_prefixed_identifier(ast_node *node);

// ======================================================================
//
//                              SIMPLE TYPES
//
// ======================================================================
void analysis_type(ast_node *node);


// ======================================================================
//
//                              VARIABLE DECLARATIONS
//
// ======================================================================
void analysis_var_decl(ast_node *node);


// ======================================================================
//
//                              EXPRESSIONS
//
// ======================================================================
void analysis_expression(ast_node *node);

// ======================================================================
//
//                              STRING_ACCESS
//
// ======================================================================
void analysis_string_access(ast_node *node);

// ======================================================================
//
//                              FUNCTIONS
//
// ======================================================================
void analysis_function(ast_node *node);
void analysis_parameter_list(ast_node *node);
void analysis_call(ast_node *node);

// ======================================================================
//
//                              STATEMENTS
//
// ======================================================================
void analysis_if_statement(ast_node *node);
void analysis_while_statement(ast_node *node);
void analysis_for_statement(ast_node *node);
void analysis_block_statement(ast_node *node);
void analysis_expr_statement(ast_node *node);
void analysis_statement_list(ast_node *node);
void analysis_print_statement(ast_node *node);
void analysis_return_statement(ast_node *node);
void analysis_break_statement(ast_node *node);


void analysis_c_code_literal(ast_node *node);

// ======================================================================
void analysis(ast_node *node);

#endif /* _ANALYSIS_H_ */
