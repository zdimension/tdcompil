/*
 * symbols.h    -- Symbol Table Management
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
 *    Creation date: 12-Jan-2015 11:16 (eg)
 * Last file update:  3-Dec-2021 12:11 (eg)
 */

#ifndef _SYMBOLS_H_
#define _SYMBOLS_H_


typedef struct symbol_table *Symbol_table;   ///<  The Toy symbol table

/// Create a new symbol table for current scope.
void enter_scope(void);

/// Mark the table for current scope for destruction.
void leave_scope(void);

/// Declare an object (ident or function) in the current block.
void symbol_table_declare_object(char *id, ast_node *ident);

/// Search a symbol starting from current scope.
ast_node *symbol_table_search(char *id);

/// Search a prototype in the current scope.
ast_node *symbol_table_search_prototype(char *id);

/// Replace a prototype in current scope by a new function (or prototype).
void symbol_table_replace_prototype(char *id, ast_node *func);

/// Declare 'var' as a variable of type 'type'
void declare_simple_variable(ast_node *var, ast_node *type);

/// Free the old (no more used) tables.
void symbol_table_free_unused_tables(void);

/// Initialization of the symbol table module.
void init_symbols(void);

#endif /* _SYMBOLS_H_ */
