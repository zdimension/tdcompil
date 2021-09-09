/*
 * symbols.c    -- Symbol Table Management
 *
 * Copyright © 2015 Erick Gallesio - I3S-CNRS/Polytech Nice-Sophia <eg@unice.fr>
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
 *    Creation date: 12-Jan-2015 11:12 (eg)
 * Last file update: 14-Nov-2015 18:01 (eg)
 */


#include <hash.h>
#include "toy.h"
#include "ast.h"
#include "utils.h"
#include "symbols.h"


struct symbol_table {
  Hash_table table;                     ///< A hash table
  struct symbol_table *next;            ///< The embedding block
};

static Symbol_table current_table  = NULL; ///< Table of the current block
static Symbol_table tables_to_free = NULL; ///< Symbols table cannot be freed as soon
                                           ///< as we leave a scope (code generation
                                           ///< is done late, so we keep  a list
                                           ///< of tables to free).


/// \file
///
/// The module SYMBOLS is in charge of implementing the various symbol
/// tables needed by the compiler. A new table is created when we
/// enter a new scope (entering a function or a block). These tables
/// are linked so that we can search a symbol (denoting a variable, a
/// function, ...) from the inner scope to the global one.
///

//
// Create a new symbol table for current scope.
//
void enter_scope(void){
  Symbol_table tmp = must_malloc(sizeof(struct symbol_table));

  tmp->table    = hash_table_create();
  tmp->next     = current_table;
  current_table = tmp;
}


///
/// Mark the table for current scope for destruction.  In fact, table
/// destruction is postponed after code production, and is done by
/// @function(symbol_table_free_unused_tables).
///
void leave_scope(void) {
  Symbol_table tmp = current_table;

  current_table = tmp->next;

  // Push the current table to the list of symbol tables to free
  tmp->next = tables_to_free;
  tables_to_free = tmp;
}


//
// Declare an object (ident or function) in the current block.
//
void symbol_table_declare_object(char *id, ast_node *obj) {
  ast_node *old = hash_table_search(current_table->table, id);

  if (old)
    error_msg(obj, "%s is already declared", id);
  else
    hash_table_add(current_table->table, id, obj, NULL); // obj is a node which will
                                                         // be  freed by free_node
}


//
// Search a symbol starting from current scope.
//
ast_node *symbol_table_search(char *id) {
  Symbol_table t;

  for (t = current_table; t; t = t->next) {
    ast_node *res= hash_table_search(t->table, id);
    if (res) return res;
  }
  return NULL;
}


//
// Search a prototype in the current scope.
//
ast_node *symbol_table_search_prototype(char *id) {
  ast_node *old = hash_table_search(current_table->table, id);

  if (old && (AST_KIND(old)==k_function) && (FUNCTION_BODY(old)==NULL))
    return old;     // old is a prototype
  return NULL;
}


//
// Replace a prototype in current scope by a new function (or prototype).
//
void symbol_table_replace_prototype(char *id, ast_node *func) {
  hash_table_delete(current_table->table, id);
  hash_table_add(current_table->table, id, func, (void (*)(void*)) free_node);
}


//
// Free the old (no more used) tables.
//
void symbol_table_free_unused_tables(void) {
  Symbol_table p, next;

  for (p = tables_to_free; p ; p = next) {
    next = p->next;
    hash_table_destroy(p->table);
    free(p);
  }
  tables_to_free = NULL;
}


//
// Initialization of the symbol table module.
//
void init_symbols(void) {
  // Create the table of global variables and push it in the global list
  enter_scope();
}
