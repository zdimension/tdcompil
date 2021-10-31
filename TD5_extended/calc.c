/*
 * calc.c 	-- Calculator functions
 *
 * Copyright © 2015 Erick Gallesio - I3S-CNRS/Polytech Nice-Sophia <eg@unice.fr>
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 21-Oct-2015 12:02 (eg)
 * Last file update:  4-Nov-2015 16:49 (eg)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "calc.h"

#define initialize_header(p, kind) { \
  AST_LINENO(p) = yylineno;          \
  AST_KIND(p)   = kind;              \
  AST_CLEAN_STACK(p) = false;        \
}

extern int yylineno;                    // line number defined by lex

static ast_node* allocate_node(int size);


// ----------------------------------------------------------------------
//		Idents stuff
// ----------------------------------------------------------------------
ast_node* make_ident(char* str)
{
    ast_node* p = allocate_node(sizeof(struct ast_ident));

    initialize_header(p, k_ident);
    VAR_NAME(p) = str;
    return p;
}


// ----------------------------------------------------------------------
//		Numbers stuff
// ----------------------------------------------------------------------

ast_node* make_number(int f)
{
    ast_node* p = allocate_node(sizeof(struct ast_number));

    initialize_header(p, k_number);
    NUMBER_VALUE(p) = f;
    return p;
}


// ----------------------------------------------------------------------
//		Operators stuff
// ----------------------------------------------------------------------
ast_node* make_node(int operator, int arity, ...)
{
    ast_node* p;
    va_list ap;

    p = allocate_node(sizeof(struct ast_operator) + arity * sizeof(ast_node*));
    initialize_header(p, k_operator);
    OPER_OPERATOR(p) = operator;
    OPER_ARITY(p) = arity;

    // Fill in the operands array from the vararg list
    va_start(ap, arity);
    for (int i = 0; i < arity; i++)
        OPER_OPERANDS(p)[i] = va_arg(ap, ast_node *);
    va_end(ap);

    return p;
}


// ----------------------------------------------------------------------
//	Utilities
// ----------------------------------------------------------------------
static ast_node* allocate_node(int size)
{
    ast_node* p = malloc(size);

    if (!p)
    {
        error_msg(NULL, "out of memory\n");
        exit(1);
    }
    return p;
}


void free_node(ast_node* p)
{
    if (!p)
        return;

    switch (AST_KIND(p))
    {
        case k_number:    /* Nothing to do */ break;
        case k_ident:
            free(VAR_NAME(p));
            break;
        case k_operator:
            for (int i = 0; i < OPER_ARITY(p); i++)
                free(OPER_OPERANDS(p)[i]);
            break;
    }
    free(p);
}


void error_msg(struct ast_node* node, const char* format, ...)
{
    va_list ap;

    fprintf(stderr, "*** Error on line %d: ", node ? node->lineno : yylineno);
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
}

void info_msg(struct ast_node* node, const char* format, ...)
{
    va_list ap;

    fprintf(stderr, "*** Info on line %d: ", node ? node->lineno : yylineno);
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
}


struct ast_node* make_list(ast_node* value)
{
    struct ast_linked_list* ptr = malloc(sizeof(*ptr));
    initialize_header(ptr, k_list);
    ptr->list = NULL;
    prepend_list((struct ast_node*) ptr, value);
    return (struct ast_node*) ptr;
}

struct ast_node* prepend_list(struct ast_node* node, ast_node* value)
{
    struct ast_linked_list* list = (struct ast_linked_list*) node;
    struct linked_list* ptr = malloc(sizeof(*ptr));
    ptr->value = value;
    ptr->next = list->list;
    list->list = ptr;
    return (struct ast_node*) list;
}