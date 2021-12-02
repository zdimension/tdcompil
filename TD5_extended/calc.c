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
#include <string.h>

#include "calc.h"
#include "syntax.h"

#define initialize_header(p, kind) { \
  AST_INFO(p).line = yylloc.first_line;\
  AST_INFO(p).col = yylloc.first_column;\
  AST_INFO(p).code = strdup(yylloc.code);\
  AST_KIND(p)   = kind;              \
  AST_CLEAN_STACK(p) = false;        \
  AST_INFERRED(p) = NULL;            \
  AST_DATA(p) = NULL;               \
}

extern int yylineno;                    // line number defined by lex

static ast_node* allocate_node(int size);

ast_node* clean_stack(ast_node* p)
{
    AST_CLEAN_STACK(p) = true;
    return p;
}


// ----------------------------------------------------------------------
//		Idents stuff
// ----------------------------------------------------------------------
ast_node* make_ident(const char* str)
{
    ast_node* p = allocate_node(sizeof(ast_ident));

    initialize_header(p, k_ident);
    VAR_NAME(p) = str;
    return p;
}


// ----------------------------------------------------------------------
//		Numbers stuff
// ----------------------------------------------------------------------

ast_node* make_number(turing_int f)
{
    return make_number_sized(f, 0);
}

ast_node* make_number_sized(turing_int f, int s)
{
    ast_node* p = allocate_node(sizeof(ast_number));

    initialize_header(p, k_number);
    NUMBER_VALUE(p) = f;
    NUMBER_SIZE(p) = s;
    return p;
}


// ----------------------------------------------------------------------
//		Operators stuff
// ----------------------------------------------------------------------
ast_node* make_node(int operator, int arity, ...)
{
    ast_node* p;
    va_list ap;

    p = allocate_node(sizeof(ast_operator) + arity * sizeof(ast_node*));
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

ast_node* make_scope(ast_node* code)
{
    ast_node* p = allocate_node(sizeof(ast_scope));
    initialize_header(p, k_scope);
    SC_CODE(p) = code;
    SC_SCOPE(p) = NULL;
    AST_CLEAN_STACK(p) = true;
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
        case k_operator:
            for (int i = 0; i < OPER_ARITY(p); i++)
                free(OPER_OPERANDS(p)[i]);
            break;
    }
    free(p);
}

void display_code(ast_node* node)
{
    bool has_node = node && node->info.code;
    int left = (has_node ? node->info.col : yylloc.first_column);
    char* ptr = (has_node ? node->info.code : yylloc.code);
    for (; *ptr == ' ' || *ptr == '\t'; ptr++, left--);
    fprintf(stderr, "\t%s\n\t%*s^\n", ptr, left - 1, "");
}

int line_offset = 0;

void error_msg(ast_node* node, const char* format, ...)
{
    va_list ap;

    fprintf(stderr, "*** Error on line %d: ", (node ? node->info.line : yylineno) + line_offset);
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    display_code(node);
}

void info_msg(ast_node* node, const char* format, ...)
{
    va_list ap;

    fprintf(stderr, "*** Info on line %d: ", (node ? node->info.line : yylineno) + line_offset);
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    display_code(node);
}

ast_node* make_node_from_list(linked_list* value)
{
    ast_linked_list* ptr = malloc(sizeof(*ptr));
    initialize_header(ptr, k_list);
    ptr->list = value;
    return (ast_node*) ptr;
}

ast_node* make_empty_list()
{
    return make_node_from_list(NULL);
}

ast_node* make_list(ast_node* value)
{
    ast_node* ptr = make_node_from_list(NULL);
    prepend_list(ptr, value);
    return (ast_node*) ptr;
}

linked_list* make_list_item(ast_node* value)
{
    linked_list* ptr = malloc(sizeof(*ptr));
    ptr->value = value;
    ptr->next = NULL;
    return ptr;
}

ast_node* prepend_list(ast_node* node, ast_node* value)
{
    ast_linked_list* list = (ast_linked_list*) node;
    linked_list* ptr = make_list_item(value);
    ptr->next = list->list;
    list->list = ptr;
    return (ast_node*) list;
}

ast_node* set_inferred_type(ast_node* n, struct type_list_s const* type)
{
    AST_INFERRED(n) = type;
    return n;
}

ast_node* ast_copy_internal(ast_node* n)
{
    switch (n->kind)
    {
        case k_ident:
            return make_ident(VAR_NAME(n));
        case k_number:
            return make_number(NUMBER_VALUE(n));
        case k_operator:
        {
            size_t ops_size = OPER_ARITY(n) * sizeof(ast_node*);
            ast_node* p = allocate_node(sizeof(ast_operator) + ops_size);
            initialize_header(p, k_operator);
            OPER_OPERATOR(p) = OPER_OPERATOR(n);
            OPER_ARITY(p) = OPER_ARITY(n);
            for (int i = 0; i < OPER_ARITY(p); i++)
                OPER_OPERANDS(p)[i] = ast_copy(OPER_OPERANDS(n)[i]);
            return p;
        }
        case k_list:
        {
            linked_list* list = NULL;
            linked_list** end = &list;
            for (linked_list* ptr = AST_LIST_HEAD(n); ptr; ptr = ptr->next)
            {
                linked_list* new_item = make_list_item(ast_copy(ptr->value));
                *end = new_item;
                end = &new_item->next;
            }
            return make_node_from_list(list);
        }
        case k_scope:
            return make_scope(ast_copy(SC_CODE(n)));
    }
}

ast_node* ast_copy(ast_node* n)
{
    if (!n)
        return NULL;
    return set_inferred_type(ast_copy_internal(n), AST_INFERRED(n));
}