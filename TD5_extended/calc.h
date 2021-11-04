/*
 * calc.h 	-- Calc types
 *
 * Copyright © 2015 Erick Gallesio - I3S-CNRS/Polytech Nice-Sophia <eg@unice.fr>
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 21-Oct-2015 11:20 (eg)
 * Last file update:  4-Nov-2015 16:46 (eg)
 */

#ifndef _CALC_H_
#define _CALC_H_

#include <stdbool.h>

#define ENUM_ITEM(X) X,
#define ENUM_NAME(X) #X,

#define ENUM_DEFINE(name, items) enum name\
{\
    items(ENUM_ITEM)\
};\
static const char* name##_NAMES[] =\
{\
    items(ENUM_NAME)\
};


/// The various kind of node we can have: ident, number, operator
#define KINDS(F) \
    F(k_ident) \
    F(k_number) \
    F(k_operator) \
    F(k_list)
ENUM_DEFINE(node_kind, KINDS)


// ----------------------------------------------------------------------
//		AST node type definition
// ----------------------------------------------------------------------

typedef struct
{
    int lineno;                   // source line number of the node
    enum node_kind kind;        // kind of node
    bool clean_stack;
} ast_node;

#define AST_LINENO(p)        (((ast_node *)(p))->lineno)
#define AST_KIND(p)        (((ast_node *)(p))->kind)
#define AST_CLEAN_STACK(p)        (((ast_node *) (p))->clean_stack)

// ----------------------------------------------------------------------
//		Idents stuff
// ----------------------------------------------------------------------
typedef struct
{
    ast_node header;        // AST header
    char* name;            // name of the ident
} ast_ident;

#define VAR_NAME(p)        (((ast_ident *) (p))->name)

ast_node* make_ident(char* str);    // make a ident node


// ----------------------------------------------------------------------
//		Numbers stuff
// ----------------------------------------------------------------------
typedef struct
{
    ast_node header;              // AST header
    int value;            // value of the number
} ast_number;

#define NUMBER_VALUE(p)    (((ast_number *) (p))->value)

ast_node* make_number(int f);    // make a number node


// ----------------------------------------------------------------------
//		Operators stuff
// ----------------------------------------------------------------------
typedef struct
{
    ast_node header;        // AST header
    int operator;            // lex token
    int arity;            // arity of the operator
    ast_node* operands[0];    // array of operands
} ast_operator;

#define OPER_OPERATOR(p)    (((ast_operator *) (p))->operator)
#define OPER_ARITY(p)        (((ast_operator *) (p))->arity)
#define OPER_OPERANDS(p)    (((ast_operator *) (p))->operands)

ast_node* make_node(int operator, int arity, ...); // make an operator node



// Lists
typedef struct linked_list_s
{
    ast_node* value;
    struct linked_list_s* next;
} linked_list;


typedef struct
{
    ast_node header;
    linked_list* list;
}ast_linked_list;

ast_node* make_list(ast_node* value);
ast_node* prepend_list(ast_node* list, ast_node* value);

// ----------------------------------------------------------------------
//		Utilities
// ----------------------------------------------------------------------
void error_msg(ast_node* node, const char* format, ...);        // Display an error message
void info_msg(ast_node* node, const char* format, ...);        // Display an info message
void free_node(ast_node* p);                    // Freeing a node

extern bool optimize;


#endif /* _CALC_H_ */
