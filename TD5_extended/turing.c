/*
 * turing.c 	-- Production of code for a stack machine
 *
 * Copyright © 2021 Tom Niget - Tom Niget/Polytech Nice-Sophia <tom.niget@etu.unice.fr>
 *
 *           Author: Tom Niget [tom.niget@etu.unice.fr]
 *    Creation date: 27-Oct-2021 22:27 (zdimension)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "calc.h"
#include "syntax.h"

#define INT_WIDTH 8

// ----------------------------------------------------------------------
//		Utilities
// ---------------------------------------------------------------------
#define LABEL(n)     printf("#L%03d:\n", n);            // output a label

#define PROD0(op)     printf("#\t%s\n", op)
#define PROD1F(op, v)     printf("#\t%s\t%g\n", op, v)    // v is a float
#define PROD1S(op, v)     printf("#\t%s\t%s\n", op, v)    // v is a string
#define PROD1L(op, v)     printf("#\t%s\tL%03d\n", op, v) // v is a label

#define instr(format, ...) printf (format "\t\t# line %d\n", ## __VA_ARGS__, __LINE__)

#define COMPARISON(code) { \
instr("FROM @%d", ++label);\
instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);\
instr("FROM @%d", label);\
instr("'[,'0|'1,'_,'_ '[,'_,'0|'1,'_ S,L,L,S");\
instr("'[,'/,'_,'_ S,S,R,S @%d", ++label);\
instr("FROM @%d", label);\
instr("'[,'/,'0|'1,'_ S,S,R,S");\
instr("'[,'/,'_,'_ S,L,L,S @%d", ++label);\
instr("FROM @%d", label);\
int left_then_one = ++label;\
int left_then_zero = ++label;\
int one = ++label;\
int zero = ++label;\
int end = label + 1;\
instr("'[,'0|'1,'0|'1,'_ '[,'_,'_,'_ S,L,L,S");\
code \
instr("FROM @%d", zero);\
instr("'[,'_,'_,'_ '[,'0,'_,'_ S,R,S,S");\
instr("'[,'/,'_,'_ S,S,S,S @%d", end);\
instr("FROM @%d", left_then_zero);\
instr("'[,'0|'1,'0,'_ '[,'_,'_,'_ S,L,L,S");\
instr("'[,'0|'1,'1,'_ '[,'_,'_,'_ S,L,L,S");\
instr("'[,'/|'[,'_,'_ S,R,S,S @%d", zero);\
instr("FROM @%d", left_then_one);\
instr("'[,'0|'1,'0,'_ '[,'_,'_,'_ S,L,L,S");\
instr("'[,'0|'1,'1,'_ '[,'_,'_,'_ S,L,L,S");\
instr("'[,'/|'[,'_,'_ S,R,S,S @%d", one);\
instr("FROM @%d", one);\
instr("'[,'_,'_,'_ '[,'1,'_,'_ S,R,S,S @%d", zero);\
}

struct call_site_list
{
    int id;
    int return_address, argalloc_address;
    struct call_site_list* next;
};

struct linked_list_header
{
    const char* name;
    int id;
    struct linked_list_header* next;
};

struct stack_frame
{
    struct func_list* function;
    struct var_list* vars;
};

struct var_list
{
    struct linked_list_header header;
    int position;
    int size;
    char* initial;
} * globals_head = NULL, * globals_tail = NULL;

struct loop_info
{
    int address;
};

int label = 0;

struct func_list
{
    struct linked_list_header header;
    bool is_void;
    struct linked_list* arglist;
    struct ast_node* code;
    struct call_site_list* callsites;
    struct var_list* locals_head, * locals_tail;
} * funcs_head = NULL, * funcs_tail = NULL;

struct call_site_list* add_call_site(struct call_site_list** list)
{
    struct call_site_list* newNode = malloc(sizeof(struct call_site_list));
    newNode->next = *list;
    if (*list)
        newNode->id = (*list)->id + 1;
    else
        newNode->id = 0;
    *list = newNode;
    return newNode;
}

void nav_to_var(struct ast_node* op, struct stack_frame* frame, struct loop_info* loop);

void deref();

void exec(ast_node* n, struct stack_frame* frame, struct loop_info* loop);

struct linked_list_header* find_symbol(struct linked_list_header* list, const char* name)
{
    for (struct linked_list_header* ptr = list; ptr; ptr = ptr->next)
    {
        if (!strcmp(ptr->name, name))
        {
            return ptr;
        }
    }
    fprintf(stderr, "SYMBOL NOT FOUND: %s\n", name);
    abort();
}

#define FIND_SYM(type, list, name) ((type*)find_symbol(&((list)->header), name))

struct var_list* get_var_id(const char* name, struct var_list* frame)
{
    return FIND_SYM(struct var_list, frame, name);
}

void push_number(int value)
{
    instr("FROM @%d", ++(label));
    instr("'/|'[,'[,'_,'_ S,R,S,S @%d", ++label);
    instr("'/|'[,'/,'_,'_ S,R,S,S @%d", label);
    instr("FROM @%d", label);
    for (int i = 0; i < INT_WIDTH; i++, value /= 2)
    {
        instr("'/|'[,'_,'_,'_ '/|'[,'%d,'_,'_ S,R,S,S @%d", value & 1, ++label);
        instr("FROM @%d", label);
    }
    instr("'/|'[,'_,'_,'_ '/|'[,'/,'_,'_ S,S,S,S @%d", label + 1);
}

void eval(ast_node* n, struct stack_frame* frame)
{
    if (!n)
    {
        fprintf(stderr, "Null node eval\n");
        abort();
    }

    exec(n, frame, NULL);
}

void pop(int n)
{
    if (n--)
        return;

    instr("FROM @%d", ++label);
    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);

    while (--n)
    {
        instr("FROM @%d", label);
        instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
        instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
    }

    instr("FROM @%d", label);
    instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
}

void exec(ast_node* n, struct stack_frame* frame, struct loop_info* loop)
{
    if (!n)
    {
        PROD0("empty node");
        return;
    }

    instr("# KIND = %d (%s)", AST_KIND(n), node_kind_NAMES[AST_KIND(n)]);

    switch (AST_KIND(n))
    {
        case k_number:
        {
            PROD1F("push", NUMBER_VALUE(n));
            push_number(NUMBER_VALUE(n));
            return;
        }
        case k_ident:
        {
            struct var_list* ptr = get_var_id(VAR_NAME(n), frame->vars);
            PROD1S("load", VAR_NAME(n));
            if (ptr->size != 1) // array
            {
                push_number(ptr->position);
            }
            else
            {
                nav_to_var(n, frame, NULL);
                instr("FROM @%d", ++label);
                instr("'/|'[,'/,'_,'_ R,R,S,S @%d", ++label);
                instr("'/|'[,'[,'_,'_ R,R,S,S @%d", label);
                instr("FROM @%d", label);
                instr("'0|'1,'_,'_,'_ '0|'1,'0|'1,'_,'_ R,R,S,S");
                instr("'/,'_,'_,'_ '/,'/,'_,'_ L,S,S,S @%d", ++label);
                instr("FROM @%d", label);
                instr("'/|'0|'1,'/,'_,'_ L,S,S,S");
                instr("'[,'/,'_,'_ S,S,S,S @%d", label + 1);
            }
            return;
        }
        case k_operator:
        {
            ast_node** op = OPER_OPERANDS(n);
            int arity = OPER_ARITY(n);
            bool clean_stack = OPER_CLEAN_STACK(n);

            switch (OPER_OPERATOR(n))
            {
                /* Expressions */
                case UMINUS:
                {
                    PROD0("negate");
                    eval(op[0], frame);
                    if (clean_stack)
                    {
                        pop(1);
                        return;
                    }
                    instr("FROM @%d", ++label);
                    instr("'[,'/,'_,'_ '[,'/,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", ++label);
                    int no_carry = label;
                    int end = ++label;
                    int carry = ++label;
                    instr("FROM @%d", no_carry);
                    instr("'[,'0,'_,'_ '[,'0,'_,'_ S,R,S,S");
                    instr("'[,'1,'_,'_ '[,'1,'_,'_ S,R,S,S @%d", carry);
                    instr("'[,'/,'_,'_ S,S,S,S @%d", end);
                    instr("FROM @%d", carry);
                    instr("'[,'0,'_,'_ '[,'1,'_,'_ S,R,S,S");
                    instr("'[,'1,'_,'_ '[,'0,'_,'_ S,R,S,S");
                    instr("'[,'/,'_,'_ S,S,S,S @%d", end);
                    instr("FROM @%d", end);
                    instr("'[,'/,'_,'_ S,S,S,S @%d", label + 1);
                    return;
                }
                case DEREF:
                {
                    PROD0("deref");
                    eval(op[0], frame);
                    if (clean_stack)
                    {
                        pop(1);
                        return;
                    }
                    deref();
                    instr("FROM @%d", ++label);
                    instr("'/|'[,'/,'_,'_ R,R,S,S @%d", ++label);
                    instr("'/|'[,'[,'_,'_ R,R,S,S @%d", label);
                    instr("FROM @%d", label);
                    instr("'0|'1,'0,'_,'_ '0|'1,'0|'1,'_,'_ R,R,S,S");
                    instr("'0|'1,'1,'_,'_ '0|'1,'0|'1,'_,'_ R,R,S,S");
                    instr("'0|'1,'_,'_,'_ '0|'1,'0|'1,'_,'_ R,R,S,S");
                    instr("'/,'_,'_,'_ '/,'/,'_,'_ L,S,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'0|'1|'/,'/,'_,'_ L,S,S,S");
                    instr("'[,'/,'_,'_ S,S,S,S @%d", label + 1);
                    return;
                }
                case REF:
                {
                    PROD0("ref");
                    if (clean_stack)
                    {
                        return;
                    }
                    int ptr = get_var_id(VAR_NAME(op[0]), frame->vars)->position;
                    if (op[1] != NULL)
                    {
                        if (AST_KIND(op[1]) == k_number)
                        {
                            push_number(ptr + (int) NUMBER_VALUE(op[1]));
                        }
                        else
                        {
                            push_number(ptr);
                            eval(op[1], frame);
                            goto ADD;
                        }
                    }
                    else
                    {
                        push_number(ptr);
                    }
                    return;
                }
                case '+':
                {
                    PROD0("add");
                    eval(op[0], frame);
                    eval(op[1], frame);
                    if (clean_stack)
                    {
                        pop(2);
                        return;
                    }
                    ADD:
                    instr("FROM @%d", ++label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ '[,'_,'0|'1,'_ S,L,L,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,R,S @%d", ++label);
                    int no_carry = label;
                    int end = ++label;
                    int carry = ++label;
                    instr("FROM @%d", no_carry);
                    instr("'[,'0,'0,'_ '[,'0,'_,'_ S,R,R,S");
                    instr("'[,'0|'1,'1|'0,'_ '[,'1,'_,'_ S,R,R,S");
                    instr("'[,'1,'1,'_ '[,'0,'_,'_ S,R,R,S @%d", carry);
                    instr("'[,'/,'_,'_ S,S,S,S @%d", end);
                    instr("FROM @%d", carry);
                    instr("'[,'0,'0,'_ '[,'1,'_,'_ S,R,R,S @%d", no_carry);
                    instr("'[,'0|'1,'1|'0,'_ '[,'0,'_,'_ S,R,R,S");
                    instr("'[,'1,'1,'_ '[,'1,'_,'_ S,R,R,S");
                    instr("'[,'/,'_,'_ S,S,S,S @%d", end);
                    instr("FROM @%d", end);
                    instr("'[,'/,'_,'_ S,S,S,S @%d", label + 1);
                    return;
                }
                case '-':
                {
                    PROD0("sub");
                    eval(op[0], frame);
                    eval(op[1], frame);
                    if (clean_stack)
                    {
                        pop(2);
                        return;
                    }
                    instr("FROM @%d", ++label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ '[,'_,'0|'1,'_ S,L,L,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,R,S @%d", ++label);
                    int no_carry = label;
                    int end = ++label;
                    int carry = ++label;
                    instr("FROM @%d", no_carry);
                    instr("'[,'0|'1,'0,'_ '[,'0|'1,'_,'_ S,R,R,S");
                    instr("'[,'0,'1,'_ '[,'1,'_,'_ S,R,R,S @%d", carry);
                    instr("'[,'1,'1,'_ '[,'0,'_,'_ S,R,R,S");
                    instr("'[,'/,'_,'_ S,S,S,S @%d", end);
                    instr("FROM @%d", carry);
                    instr("'[,'0|'1,'0|'1,'_ '[,'1,'_,'_ S,R,R,S");
                    instr("'[,'1,'0,'_ '[,'0,'_,'_ S,R,R,S @%d", no_carry);
                    instr("'[,'0,'1,'_ '[,'0,'_,'_ S,R,R,S");
                    instr("'[,'/,'_,'_ S,S,S,S @%d", end);
                    instr("FROM @%d", end);
                    instr("'[,'/,'_,'_ S,S,S,S @%d", label + 1);
                    return;
                }
                case '*':
                {
                    PROD0("mul");
                    eval(op[0], frame);
                    eval(op[1], frame);
                    if (clean_stack)
                    {
                        pop(2);
                        return;
                    }
                    instr("FROM @%d", ++label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ '[,'_,'0|'1,'_ S,L,L,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ '[,'0,'_,'0|'1 S,L,S,L");
                    instr("'[,'/|'[,'_,'_ S,R,R,R @%d", ++label);
                    int loop = label;
                    int no_carry = ++label;
                    int carry = ++label;
                    int next = ++label;
                    int end = label + 1;
                    instr("FROM @%d", loop);
                    instr("'[,'0,'1|'0,'0 '[,'0,'1|'0,'# S,R,S,R");
                    instr("'[,'1,'1|'0,'0 '[,'1,'1|'0,'# S,R,S,R");
                    instr("'[,'0,'1|'0,'1 '[,'0,'1|'0,'# S,S,S,S @%d", no_carry);
                    instr("'[,'1,'1|'0,'1 '[,'1,'1|'0,'# S,S,S,S @%d", no_carry);
                    instr("'[,'0,'1|'0,'# S,R,S,R");
                    instr("'[,'1,'1|'0,'# S,R,S,R");
                    instr("'[,'/,'0|'1,'_ '[,'/,'_,'_ S,S,R,L");
                    instr("'[,'/,'0|'1,'# '[,'/,'_,'_ S,S,R,L");
                    instr("'[,'/,'_,'# '[,'/,'_,'_ S,S,R,L");
                    instr("'[,'/,'_,'_ S,S,R,L @%d", end);
                    instr("FROM @%d", no_carry);
                    instr("'[,'0,'0,'# '[,'0,'0,'# S,R,R,S");
                    instr("'[,'0|'1,'1|'0,'# '[,'1,'1|'0,'# S,R,R,S");
                    instr("'[,'1,'1,'# '[,'0,'1,'# S,R,R,S @%d", carry);
                    instr("'[,'/,'0|'1|'_,'# S,L,L,L @%d", next);
                    instr("FROM @%d", carry);
                    instr("'[,'0,'0,'# '[,'1,'0,'# S,R,R,S @%d", no_carry);
                    instr("'[,'0|'1,'1|'0,'# '[,'0,'1|'0,'# S,R,R,S");
                    instr("'[,'1,'1,'# '[,'1,'1,'# S,R,R,S");
                    instr("'[,'/,'0|'1,'# S,L,L,L @%d", next);
                    instr("FROM @%d", next);
                    instr("'[,'0|'1,'0,'# S,L,L,L");
                    instr("'[,'0|'1,'1,'# S,L,L,L");
                    instr("'[,'0|'1,'0,'_ S,L,L,S");
                    instr("'[,'0|'1,'1,'_ S,L,L,S");
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'0|'1,'_,'# S,L,S,L");
                    instr("'[,'/|'[,'_,'_ S,R,R,R @%d", loop);
                    return;
                }
                case '/':
                    PROD0("div");
                    eval(op[0], frame);
                    eval(op[1], frame);
                    if (clean_stack)
                    {
                        pop(2);
                        return;
                    }
                    return;
                case '<':
                {
                    PROD0("cmplt");
                    eval(op[0], frame);
                    eval(op[1], frame);
                    if (clean_stack)
                    {
                        pop(2);
                        return;
                    }
                    COMPARISON({
                                   instr("'[,'0,'1,'_ '[,'_,'_,'_ S,L,L,S @%d", left_then_one);
                                   instr("'[,'1,'0,'_ '[,'_,'_,'_ S,L,L,S @%d", left_then_zero);
                                   instr("'[,'/|'[,'_,'_ '[,'/|'[,'_,'_ S,R,S,S @%d", zero);
                               });
                    return;
                }
                case '>':
                {
                    PROD0("cmpgt");
                    eval(op[0], frame);
                    eval(op[1], frame);
                    if (clean_stack)
                    {
                        pop(2);
                        return;
                    }
                    COMPARISON({
                                   instr("'[,'1,'0,'_ '[,'_,'_,'_ S,L,L,S @%d", left_then_one);
                                   instr("'[,'0,'1,'_ '[,'_,'_,'_ S,L,L,S @%d", left_then_zero);
                                   instr("'[,'/|'[,'_,'_ '[,'/|'[,'_,'_ S,R,S,S @%d", zero);
                               });
                    return;
                }
                case GE:
                {
                    PROD0("cmpge");
                    eval(op[0], frame);
                    eval(op[1], frame);
                    if (clean_stack)
                    {
                        pop(2);
                        return;
                    }
                    COMPARISON({
                                   instr("'[,'1,'0,'_ '[,'_,'_,'_ S,L,L,S @%d", left_then_one);
                                   instr("'[,'0,'1,'_ '[,'_,'_,'_ S,L,L,S @%d", left_then_zero);
                                   instr("'[,'/|'[,'_,'_ '[,'/|'[,'_,'_ S,R,S,S @%d", one);
                               });
                    return;
                }
                case LE:
                {
                    PROD0("cmple");
                    eval(op[0], frame);
                    eval(op[1], frame);
                    if (clean_stack)
                    {
                        pop(2);
                        return;
                    }
                    COMPARISON({
                                   instr("'[,'0,'1,'_ '[,'_,'_,'_ S,L,L,S @%d", left_then_one);
                                   instr("'[,'1,'0,'_ '[,'_,'_,'_ S,L,L,S @%d", left_then_zero);
                                   instr("'[,'/|'[,'_,'_ '[,'/|'[,'_,'_ S,R,S,S @%d", one);
                               });
                    return;
                }
                case NE:
                {
                    PROD0("cmpne");
                    eval(op[0], frame);
                    eval(op[1], frame);
                    if (clean_stack)
                    {
                        pop(2);
                        return;
                    }
                    instr("FROM @%d", ++label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ '[,'_,'0|'1,'_ S,L,L,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,R,S @%d", ++label);
                    instr("FROM @%d", label);
                    int not_equal = ++label;
                    int end = ++label;
                    instr("'[,'0|'1,'0|'1,'_ '[,'0|'1,'_,'_ S,R,R,S");
                    instr("'[,'0|'1,'1|'0,'_ '[,'0|'1,'_,'_ S,R,R,S @%d", not_equal);
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label); // equal
                    instr("'[,'0|'1,'_,'_ '[,'0,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", end);
                    instr("FROM @%d", not_equal);
                    instr("'[,'0|'1,'0,'_ '[,'0|'1,'_,'_ S,R,R,S");
                    instr("'[,'0|'1,'1,'_ '[,'0|'1,'_,'_ S,R,R,S");
                    instr("'[,'/,'0|'1,'_ '[,'/,'_,'_ S,S,R,S");
                    instr("'[,'0|'1,'_,'_ S,R,S,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    for (int i = 0; i < INT_WIDTH - 1; i++)
                    {
                        instr("'[,'0|'1,'_,'_ '[,'0,'_,'_ S,L,S,S @%d", ++label);
                        instr("FROM @%d", label);
                    }
                    instr("'[,'0|'1,'_,'_ '[,'1,'_,'_ S,R,S,S @%d", end);
                    instr("FROM @%d", end);
                    instr("'[,'0|'1,'_,'_ S,R,S,S");
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
                    return;
                }
                case EQ:
                {
                    PROD0("cmpeq");
                    eval(op[0], frame);
                    eval(op[1], frame);
                    if (clean_stack)
                    {
                        pop(2);
                        return;
                    }
                    instr("FROM @%d", ++label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ '[,'_,'0|'1,'_ S,L,L,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,R,S @%d", ++label);
                    instr("FROM @%d", label);
                    int not_equal = ++label;
                    int end = ++label;
                    instr("'[,'0|'1,'0|'1,'_ '[,'0|'1,'_,'_ S,R,R,S");
                    instr("'[,'0|'1,'1|'0,'_ '[,'0|'1,'_,'_ S,R,R,S @%d", not_equal);
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label); // equal
                    for (int i = 0; i < INT_WIDTH - 1; i++)
                    {
                        instr("'[,'0|'1,'_,'_ '[,'0,'_,'_ S,L,S,S @%d", ++label);
                        instr("FROM @%d", label);
                    }
                    instr("'[,'0|'1,'_,'_ '[,'1,'_,'_ S,R,S,S @%d", end);
                    instr("FROM @%d", not_equal);
                    instr("'[,'0|'1,'0,'_ '[,'0|'1,'_,'_ S,R,R,S");
                    instr("'[,'0|'1,'1,'_ '[,'0|'1,'_,'_ S,R,R,S");
                    instr("'[,'/,'0|'1,'_ '[,'/,'_,'_ S,S,R,S");
                    instr("'[,'0|'1,'_,'_ S,R,S,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ '[,'0,'_,'_ S,L,L,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", end);
                    instr("FROM @%d", end);
                    instr("'[,'0|'1,'_,'_ S,R,S,S");
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
                    return;
                }
                case AND:
                {
                    PROD0("and");
                    eval(op[0], frame);
                    eval(op[1], frame);
                    if (clean_stack)
                    {
                        pop(2);
                        return;
                    }
                    instr("FROM @%d", ++label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    int b_nonzero = ++label;
                    int check_a = ++label;
                    int b_zero = ++label;
                    int scroll_right = ++label;
                    int a_nonzero = ++label;
                    int write_one = ++label;
                    int end = label + 1;
                    instr("'[,'0,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'1,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", b_nonzero);
                    instr("'[,'/|'[,'_,'_ S,L,S,S @%d", b_zero);
                    instr("FROM @%d", b_nonzero);
                    instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", check_a);
                    instr("FROM @%d", check_a);
                    instr("'[,'0,'_,'_ '[,'0,'_,'_ S,L,S,S");
                    instr("'[,'1,'_,'_ '[,'0,'_,'_ S,L,S,S @%d", a_nonzero);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", b_zero);
                    instr("FROM @%d", b_zero);
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", scroll_right);
                    instr("FROM @%d", scroll_right);
                    instr("'[,'0|'1,'_,'_ '[,'0,'_,'_ S,R,S,S");
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", end);
                    instr("FROM @%d", a_nonzero);
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", write_one);
                    instr("FROM @%d", write_one);
                    instr("'[,'0|'1,'_,'_ '[,'1,'_,'_ S,R,S,S @%d", scroll_right);
                    return;
                }
                case OR:
                {
                    PROD0("or");
                    eval(op[0], frame);
                    eval(op[1], frame);
                    if (clean_stack)
                    {
                        pop(2);
                        return;
                    }
                    instr("FROM @%d", ++label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    int check_a = ++label;
                    int b_nonzero = ++label;
                    int a_nonzero = ++label;
                    int scroll_right = ++label;
                    int scroll_left = ++label;
                    int write_one = ++label;
                    instr("'[,'0,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'1,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", b_nonzero);
                    instr("'[,'/|'[,'_,'_ S,L,S,S @%d", check_a);
                    instr("FROM @%d", b_nonzero);
                    instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", scroll_left);
                    instr("FROM @%d", scroll_left);
                    instr("'[,'0|'1,'_,'_ '[,'0,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", write_one);
                    instr("FROM @%d", check_a);
                    instr("'[,'0,'_,'_ '[,'0,'_,'_ S,L,S,S");
                    instr("'[,'1,'_,'_ '[,'0,'_,'_ S,L,S,S @%d", a_nonzero);
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", scroll_right);
                    instr("FROM @%d", a_nonzero);
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", write_one);
                    instr("FROM @%d", write_one);
                    instr("'[,'0|'1,'_,'_ '[,'1,'_,'_ S,R,S,S @%d", scroll_right);
                    int end = label + 1;
                    instr("FROM @%d", scroll_right);
                    instr("'[,'0|'1,'_,'_ '[,'0,'_,'_ S,R,S,S");
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", end);
                    return;
                }
                case INC:
                {
                    PROD0("inc");
                    nav_to_var(op[0], frame, NULL);
                    instr("FROM @%d", ++label);
                    instr("'/|'[,'[,'_,'_ R,R,S,S @%d", ++label);
                    instr("'/|'[,'/,'_,'_ R,R,S,S @%d", label);
                    if (clean_stack) // statement
                    {
                        instr("FROM @%d", label);
                        int left_to_end = ++label;
                        int end = label + 1;
                        instr("'0,'_,'_,'_ '1,'_,'_,'_ L,S,S,S @%d", left_to_end);
                        instr("'1,'_,'_,'_ '0,'_,'_,'_ R,S,S,S");
                        instr("FROM @%d", left_to_end);
                        instr("'0|'1|'/,'_,'_,'_ L,S,S,S");
                        instr("'[,'_,'_,'_ S,L,S,S @%d", end);
                    }
                    else // expression
                    {
                        instr("FROM @%d", label);
                        int end = ++label;
                        int left = ++label;
                        if (arity == 1) // ++x
                        {
                            instr("'0,'_,'_,'_ '1,'1,'_,'_ R,R,S,S @%d", end);
                            instr("'1,'_,'_,'_ '0,'0,'_,'_ R,R,S,S");
                        }
                        else // x++
                        {
                            instr("'0,'_,'_,'_ '1,'0,'_,'_ R,R,S,S @%d", end);
                            instr("'1,'_,'_,'_ '0,'1,'_,'_ R,R,S,S");
                        }
                        instr("FROM @%d", end);
                        instr("'0|'1,'_,'_,'_ '0|'1,'0|'1,'_,'_ R,R,S,S");
                        instr("'/,'_,'_,'_ '/,'/,'_,'_ L,S,S,S @%d", left);
                        instr("FROM @%d", left);
                        instr("'0|'1,'/,'_,'_ L,S,S,S");
                        instr("'[,'/,'_,'_ S,S,S,S @%d", label + 1);
                    }
                    return;
                }
                case DEC:
                {
                    PROD0("dec");
                    nav_to_var(op[0], frame, NULL);
                    instr("FROM @%d", ++label);
                    instr("'/|'[,'[,'_,'_ R,R,S,S @%d", ++label);
                    instr("'/|'[,'/,'_,'_ R,R,S,S @%d", label);
                    if (clean_stack) // statement
                    {
                        instr("FROM @%d", label);
                        int left_to_end = ++label;
                        int end = label + 1;
                        instr("'1,'_,'_,'_ '0,'_,'_,'_ L,S,S,S @%d", left_to_end);
                        instr("'0,'_,'_,'_ '1,'_,'_,'_ R,L,S,S");
                        instr("FROM @%d", left_to_end);
                        instr("'0|'1|'/,'_,'_,'_ L,S,S,S");
                        instr("'[,'_,'_,'_ S,L,S,S @%d", end);
                    }
                    else // expression
                    {
                        instr("FROM @%d", label);
                        int end = ++label;
                        int left = ++label;
                        if (arity == 1) // --x
                        {
                            instr("'1,'_,'_,'_ '0,'0,'_,'_ R,R,S,S @%d", end);
                            instr("'0,'_,'_,'_ '1,'1,'_,'_ R,R,S,S");
                        }
                        else // x--
                        {
                            instr("'1,'_,'_,'_ '0,'1,'_,'_ R,R,S,S @%d", end);
                            instr("'0,'_,'_,'_ '1,'0,'_,'_ R,R,S,S");
                        }
                        instr("FROM @%d", end);
                        instr("'0|'1,'_,'_,'_ '0|'1,'0|'1,'_,'_ R,R,S,S");
                        instr("'/,'_,'_,'_ '/,'/,'_,'_ L,S,S,S @%d", left);
                        instr("FROM @%d", left);
                        instr("'0|'1,'/,'_,'_ L,S,S,S");
                        instr("'[,'/,'_,'_ S,S,S,S @%d", label + 1);
                    }
                    return;
                }

                    /* Statements */
                case ';':
                    if (arity == 0)
                    {
                        PROD0("; empty");
                        return;
                    }
                    else
                    {
                        PROD0("; first");
                        exec(op[0], frame, loop);
                        PROD0("; second");
                        exec(op[1], frame, loop);
                        return;
                    }
                case KPRINT:
                    PROD0("print");
                    eval(op[0], frame);
                    return;
                case KBREAK:
                    PROD0("break");
                    if (!loop)
                    {
                        error_msg("Break statement only permitted in loop\n");
                        exit(1);
                    }
                    instr("FROM @%d", ++label);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d_break", loop->address);
                    return;
                case KCONTINUE:
                    PROD0("continue");
                    if (!loop)
                    {
                        error_msg("Continue statement only permitted in loop\n");
                        exit(1);
                    }
                    instr("FROM @%d", ++label);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d_continue", loop->address);
                    return;
                case KDO:
                {
                    PROD0("do");
                    int start = label + 1;
                    struct loop_info loop_info = {.address = start};
                    exec(op[0], frame, &loop_info);
                    instr("FROM @%d_continue", start);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
                    eval(op[1], frame);
                    instr("FROM @%d", ++label);
                    instr("'[,'/|'[,'_,'_ '[,'_,'_,'_ S,L,S,S @%d_break", start);
                    instr("FROM @%d_break", start);
                    instr("'[,'0,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    int one_found = ++label;
                    int end = label + 1;
                    instr("'[,'1,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", one_found);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", end);
                    instr("FROM @%d", one_found);
                    instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", start);
                    return;
                }
                case KFOR:
                {
                    PROD0("for");
                    exec(op[0], frame, NULL);
                    int start = ++label;
                    struct loop_info loop_info = {.address = start};
                    instr("FROM @%d", start);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
                    if (op[1])
                    {
                        PROD0("evaluating 'for' condition");
                        eval(op[1], frame);
                        instr("FROM @%d", ++label);
                        instr("'[,'/|'[,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
                        instr("FROM @%d", label);
                        instr("'[,'0,'_,'_ '[,'_,'_,'_ S,L,S,S");
                        int one_found = ++label;
                        instr("'[,'1,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", one_found);
                        instr("'[,'/|'[,'_,'_ S,S,S,S @%d_break", start);
                        instr("FROM @%d", one_found);
                        instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
                        int is_nonzero = label + 1;
                        instr("'[,'/|'[,'_,'_ S,S,S,S @%d", is_nonzero);
                    }
                    PROD0("running 'for' code");
                    exec(op[3], frame, &loop_info);
                    instr("FROM @%d_continue", start);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
                    if (op[2])
                    {
                        PROD0("evaluating 'for' increment");
                        exec(op[2], frame, NULL);
                    }

                    instr("FROM @%d", ++label); // after is_non_zero
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", start);

                    instr("FROM @%d_break", start);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
                    return;
                }
                case KIF:
                {
                    PROD0("if");
                    PROD0("evaluating condition");
                    eval(op[0], frame);
                    PROD0("checking condition");
                    instr("FROM @%d", ++label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    int one_found = ++label;
                    int is_zero;
                    int end = ++label;
                    if (op[2] == NULL)
                        is_zero = end;
                    else
                        is_zero = ++label;
                    instr("'[,'1,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", one_found);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", is_zero);
                    instr("FROM @%d", one_found);
                    instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    int is_nonzero = label + 1;
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", is_nonzero);

                    PROD0("executing positive branch");
                    exec(op[1], frame, loop);

                    instr("FROM @%d", ++label); // after is_non_zero
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", end);

                    if (op[2])
                    {
                        PROD0("executing negative branch");
                        instr("FROM @%d", is_zero);
                        int zero_id = label + 1;
                        instr("'[,'/|'[,'_,'_ S,S,S,S @%d", zero_id);

                        exec(op[2], frame, loop);

                        instr("FROM @%d", ++label);
                        instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
                    }
                    instr("FROM @%d", end);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
                    return;
                }
                case '=':
                {
                    PROD0("store");
                    PROD0("evaluating right-hand value");
                    eval(op[1], frame);
                    instr("FROM @%d", ++label);
                    instr("'0|'1|'/,'/,'_,'_ L,S,S,S");
                    instr("'[,'/,'_,'_ S,S,S,S @%d", label + 1);
                    nav_to_var(op[0], frame, NULL);
                    PROD0("navigating to left of stack head");
                    instr("FROM @%d", ++label);
                    instr("'/|'[,'/,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'/,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'/,'/|'[,'_,'_ R,R,S,S @%d", ++label);
                    instr("'[,'/|'[,'_,'_ R,R,S,S @%d", label);
                    if (clean_stack) // statement, clear from stack
                    {
                        instr("FROM @%d", label);
                        instr("'0|'1,'0,'_,'_ '0,'_,'_,'_ R,R,S,S");
                        instr("'0|'1,'1,'_,'_ '1,'_,'_,'_ R,R,S,S");
                        instr("'/,'/,'_,'_ '/,'_,'_,'_ L,L,S,S @%d", ++label);
                        instr("FROM @%d", label);
                        instr("'/|'0|'1|'[,'_,'_,'_ L,L,S,S");
                        instr("'/|'0|'1|'[,'/,'_,'_ S,S,S,S @%d", label + 1);
                        instr("'/|'0|'1|'[,'[,'_,'_ S,S,S,S @%d", ++label);
                        instr("FROM @%d", label);
                        instr("'/|'0|'1,'[,'_,'_ L,S,S,S");
                        instr("'/|'0|'1,'/,'_,'_ L,S,S,S");
                        instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
                    }
                    else // expression, keep in stack
                    {
                        instr("FROM @%d", label);
                        instr("'0|'1,'0,'_,'_ '0,'0,'_,'_ R,R,S,S");
                        instr("'0|'1,'1,'_,'_ '1,'1,'_,'_ R,R,S,S");
                        instr("'/,'/,'_,'_ L,S,S,S @%d", ++label);
                        instr("FROM @%d", label);
                        instr("'0|'1,'/,'_,'_ L,S,S,S");
                        instr("'/|'[,'/,'_,'_ S,S,S,S @%d", label + 1);
                    }
                    return;
                }
                case KDIM:
                    PROD0("dim");
                    return;
                case KFUNC:
                    PROD0("func");
                    return;
                case KPROC:
                    PROD0("proc");
                    return;
                case KRETURN:
                {
                    PROD0("return");
                    if (!frame->function)
                    {
                        error_msg("Can only return from a function or procedure\n");
                        exit(1);
                    }
                    if (frame->function->is_void && op[0])
                    {
                        error_msg("Cannot return value from procedure\n");
                        exit(1);
                    }
                    if (!frame->function->is_void && !op[0])
                    {
                        error_msg("Function must return a value\n");
                        exit(1);
                    }
                    eval(op[0], frame);
                    instr("FROM @%d", ++label);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @F%dret", frame->function->header.id);
                    return;
                }
                case '(':
                {
                    PROD0("call");
                    struct func_list* fct = FIND_SYM(struct func_list, funcs_head, VAR_NAME(op[0]));
                    if (fct->is_void && !clean_stack)
                    {
                        error_msg("Procedure call does not have a value\n");
                        exit(1);
                    }
                    struct linked_list* args = (struct linked_list*) op[1];
                    int argcount = 0;
                    for (struct linked_list* arg = args; arg; arg = arg->next, argcount++)
                    {
                        eval(arg->value, frame);
                    }
                    {
                        PROD0("go to left of args on stack");
                        instr("FROM @%d", ++label);
                        instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                        for (int i = 1; i < argcount; i++)
                        {
                            instr("FROM @%d", label);
                            instr("'[,'0|'1,'_,'_ S,L,S,S");
                            instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                        }
                        instr("FROM @%d", label);
                        instr("'[,'0|'1,'_,'_ S,L,S,S");
                        instr("'[,'/|'[,'_,'_ S,S,S,S @%d", ++label);
                    }
                    PROD0("go to right of heap");
                    instr("FROM @%d", label);
                    instr("'/|'0|'1|'[,'/,'_,'_ R,S,S,S");
                    instr("'/|'0|'1|'[,'[,'_,'_ R,S,S,S");
                    struct call_site_list* call = add_call_site(&fct->callsites);
                    instr("'_,'/|'[,'_,'_ '[,'/|'[,'_,'_ R,S,S,S @C%d", call->id);
                    call->argalloc_address = ++label;
                    PROD0("start copying args from stack to heap");
                    instr("FROM @%d", label);
                    instr("'_,'/|'[,'_,'_ '[,'/|'[,'_,'_ R,R,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'_,'0|'1,'_,'_ '0|'1,'_,'_,'_ R,R,S,S");
                    instr("'_,'/,'_,'_ '/,'_,'_,'_ R,R,S,S");
                    instr("'_,'_,'_,'_ L,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'/|'0|'1,'_,'_,'_ L,L,S,S");
                    instr("'/|'0|'1,'/,'_,'_ L,S,S,S");
                    instr("'/|'0|'1,'[,'_,'_ L,S,S,S");
                    instr("'[,'/|'[,'_,'_ S,S,S,S @F%d", fct->header.id);
                    call->return_address = label + 1;

                    return;
                }
                default:
                    error_msg("Houston, we have a problem: unattended token %d\n",
                              OPER_OPERATOR(n));
                    exit(1);
            }
        }
        default:
            return;
    }
}

void nav_to_var(struct ast_node* op, struct stack_frame* frame, struct loop_info* loop)
{
    if (AST_KIND(op) == k_ident)
    {
        instr("# navigating to %s", VAR_NAME(op));
        int vid = get_var_id(VAR_NAME(op), frame->vars)->position;
        while (vid-- > 0)
        {
            instr("FROM @%d", ++label);
            instr("'/|'[,'/,'_,'_ R,S,S,S @%d", ++label);
            instr("'/|'[,'[,'_,'_ R,S,S,S @%d", label);
            instr("FROM @%d", label);
            instr("'0|'1,'/,'_,'_ R,S,S,S");
            instr("'0|'1,'[,'_,'_ R,S,S,S");
            instr("'/,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
        }
    }
    else if (AST_KIND(op) == k_operator && OPER_OPERATOR(op) == DEREF)
    {
        eval(OPER_OPERANDS(op)[0], frame);
        deref(label);
    }
    else
    {
        error_msg("Expected lvalue for assignment\n");
        exit(1);
    }
}

void deref()
{
    instr("FROM @%d", ++label);
    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
    instr("FROM @%d", label);
    instr("'[,'0|'1,'_,'_ '[,'_,'0|'1,'_ S,L,L,S");
    instr("'[,'/|'[,'_,'_ S,S,R,S @%d", ++label);
    int start = label;
    instr("FROM @%d", start);
    int dec0 = ++label;
    int dec = ++label;
    int dec2 = ++label;
    int end = ++label;
    instr("'/|'[,'[,'1,'_ '/|'[,'[,'0,'_ S,S,L,S @%d", dec0);
    instr("'/|'[,'[,'0,'_ '/|'[,'[,'1,'_ S,S,R,S");
    instr("'/|'[,'[,'_,'_ S,S,L,S @%d", end);
    instr("'/|'[,'/,'1,'_ '/|'[,'/,'0,'_ S,S,L,S @%d", dec0);
    instr("'/|'[,'/,'0,'_ '/|'[,'/,'1,'_ S,S,R,S");
    instr("'/|'[,'/,'_,'_ S,S,L,S @%d", end);
    instr("FROM @%d", dec0);
    instr("'[,'/,'0|'1,'_ S,S,L,S");
    instr("'[,'[,'0|'1,'_ S,S,L,S");
    instr("'/,'/,'0|'1,'_ S,S,L,S");
    instr("'/,'[,'0|'1,'_ S,S,L,S");
    instr("'/|'[,'/,'_,'_ R,S,S,S @%d", dec);
    instr("'/|'[,'[,'_,'_ R,S,S,S @%d", dec);
    instr("FROM @%d", dec);
    instr("'0,'[,'0|'1,'_ R,S,L,S");
    instr("'1,'[,'0|'1,'_ R,S,L,S");
    instr("'0,'/|'[,'_,'_ R,S,S,S");
    instr("'1,'/|'[,'_,'_ R,S,S,S");
    instr("'/,'[,'0|'1,'_ S,S,L,S @%d", dec2);
    instr("'/,'/,'0|'1,'_ S,S,L,S @%d", dec2);
    instr("'/,'/|'[,'_,'_ S,S,S,S @%d", dec2);
    instr("'[,'[,'0|'1,'_ S,S,L,S @%d", dec2);
    instr("'[,'/,'0|'1,'_ S,S,L,S @%d", dec2);
    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", dec2);
    instr("FROM @%d", dec2);
    instr("'/,'[,'0|'1,'_ S,S,L,S");
    instr("'/,'/,'0|'1,'_ S,S,L,S");
    instr("'/,'/|'[,'_,'_ S,S,R,S @%d", start);
    instr("'[,'[,'0|'1,'_ S,S,L,S");
    instr("'[,'/,'0|'1,'_ S,S,L,S");
    instr("'[,'/|'[,'_,'_ S,S,R,S @%d", start);
    instr("FROM @%d", end);
    instr("'/|'[,'/,'1,'_ '/|'[,'/,'_,'_ S,S,L,S");
    instr("'/|'[,'/,'_,'_ S,S,S,S @%d", label + 1);
    instr("'/|'[,'[,'1,'_ '/|'[,'[,'_,'_ S,S,L,S");
    instr("'/|'[,'[,'_,'_ S,S,S,S @%d", label + 1);
}


struct linked_list_header* add_symbol(struct linked_list_header** head, struct linked_list_header** tail, size_t size)
{
    struct linked_list_header* newNode = malloc(size);
    newNode->next = NULL;
    if (*head == NULL)
    {
        newNode->id = 0;
        *head = newNode;
        *tail = newNode;
    }
    else
    {
        newNode->id = (*tail)->id + 1;
        (*tail)->next = newNode;
        *tail = newNode;
    }
    return newNode;
}

#define ADD_SYM(type, head, tail) ((type*) add_symbol((struct linked_list_header**)(head), (struct linked_list_header**)(tail), sizeof(type)))


/**
 *
 * @param name
 * @param size
 * @param vars_head
 * @param vars_tail
 * @return NULL if the variable already exists
 */
struct var_list* check_add_var(const char* name, int size, struct var_list** vars_head, struct var_list** vars_tail)
{
    bool found = false;
    int i = 0;
    if (vars_head)
    {
        for (struct var_list* ptr = *vars_head; ptr; ptr = (struct var_list*) ptr->header.next)
        {
            if (!strcmp(ptr->header.name, name))
            {
                found = true;
                break;
            }
            i += ptr->size;
        }
    }
    if (!found)
    {
        struct var_list* newNode = ADD_SYM(struct var_list, vars_head, vars_tail);
        newNode->header.name = name;
        newNode->position = i;
        newNode->size = size;
        newNode->initial = NULL;
        return newNode;
    }
    return NULL;
}


void traverse_vars(ast_node* n, struct var_list** vars_head, struct var_list** vars_tail)
{
    if (!n)
        return;

    if (AST_KIND(n) == k_operator)
    {
        if ((OPER_OPERATOR(n) == '=' || OPER_OPERATOR(n) == KDIM) && AST_KIND(OPER_OPERANDS(n)[0]) == k_ident)
        {
            const char* name = VAR_NAME(OPER_OPERANDS(n)[0]);
            if (OPER_OPERATOR(n) == '=')
            {
                check_add_var(name, 1, vars_head, vars_tail);
            }
            else
            {
                struct var_list* var = check_add_var(name, NUMBER_VALUE(OPER_OPERANDS(n)[1]), vars_head, vars_tail);
                if (!var)
                {
                    error_msg("Cannot redeclare array\n");
                    exit(1);
                }
                struct ast_node* initial = OPER_OPERANDS(n)[2];
                if (initial)
                {
                    var->initial = VAR_NAME(initial);
                }
            }
        }

        if (OPER_OPERATOR(n) != KFUNC && OPER_OPERATOR(n) != KPROC)
        {
            for (int i = 0; i < OPER_ARITY(n); i++)
                traverse_vars(OPER_OPERANDS(n)[i], vars_head, vars_tail);
        }
    }
}

void traverse_funcs(ast_node* n)
{
    if (!n)
        return;

    if (AST_KIND(n) == k_operator)
    {
        if (OPER_OPERATOR(n) == KFUNC || OPER_OPERATOR(n) == KPROC)
        {
            struct func_list* newNode = ADD_SYM(struct func_list, &funcs_head, &funcs_tail);
            newNode->header.name = VAR_NAME(OPER_OPERANDS(n)[0]);
            newNode->is_void = OPER_OPERATOR(n) == KPROC;
            newNode->arglist = (struct linked_list*) OPER_OPERANDS(n)[1];
            newNode->code = OPER_OPERANDS(n)[2];
            newNode->callsites = NULL;
            newNode->locals_head = NULL;
            newNode->locals_tail = NULL;

            for (struct linked_list* ptr = newNode->arglist; ptr; ptr = ptr->next)
            {
                check_add_var(VAR_NAME(ptr->value), 1, &newNode->locals_head, &newNode->locals_tail);
            }

            traverse_vars(newNode->code, &newNode->locals_head, &newNode->locals_tail);
            traverse_funcs(newNode->code);
        }
        else
        {
            for (int i = 0; i < OPER_ARITY(n); i++)
                traverse_funcs(OPER_OPERANDS(n)[i]);
        }
    }
}


// ----------------------------------------------------------------------
//		Code production
//			This version produces code for a stack machine
// ---------------------------------------------------------------------
void produce_code(ast_node* n)
{


    instr("NEW \"generated\" 4");
    instr("START @INIT");
    instr("END @END \"END\"");
    instr("END @DEFAULT \"default option\"");

    traverse_vars(n, &globals_head, &globals_tail);
    traverse_funcs(n);

    {
        printf("# Memory map\n");
        printf("# POSITION  SIZE  NAME\n");
        for (struct var_list* ptr = globals_head; ptr; ptr = (struct var_list*) ptr->header.next)
        {
            printf("# %8d  %4d  %s\n", ptr->position, ptr->size, ptr->header.name);
        }
    }

    instr("FROM @INIT");
    instr("'_,'_,'_,'_ '[,'[,'_,'_ R,S,S,S @0");

    for (struct var_list* ptr = globals_head; ptr; ptr = (struct var_list*) ptr->header.next)
    {
        char* str = ptr->initial;
        for (int j = 0; j < ptr->size; j++, (str && *str && str++))
        {
            if (str && *str)
            {
                char value = *str;
                for (int i = 0; i < INT_WIDTH; i++, value >>= 1)
                {
                    instr("FROM @%d", label);
                    instr("'_,'[,'_,'_ '%d,'[,'_,'_ R,S,S,S @%d", value & 1, ++label);
                }
            }
            else
            {
                for (int i = 0; i < INT_WIDTH; i++)
                {
                    instr("FROM @%d", label);
                    instr("'_,'[,'_,'_ '0,'[,'_,'_ R,S,S,S @%d", ++label);
                }
            }
            instr("FROM @%d", label);
            instr("'_,'[,'_,'_ '/,'[,'_,'_ R,S,S,S @%d", ++label);
        }
    }
    instr("FROM @%d", label);
    instr("'_|'/|'0|'1,'[,'_,'_ L,S,S,S");
    instr("'[,'[,'_,'_ S,S,S,S @%d", label + 1);

    struct stack_frame global_frame = {.vars = globals_head, .function = NULL};

    eval(n, &global_frame);

    instr("FROM @%d", ++label);
    instr("'[,'/,'_,'_ S,S,S,S @END");
    instr("'[,'[,'_,'_ S,S,S,S @END");

    for (struct func_list* ptr = funcs_head; ptr; ptr = (struct func_list*) ptr->header.next)
    {
        if (!ptr->callsites)
            continue;
        instr("FROM @F%d", ptr->header.id);
        instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
        struct stack_frame local_frame = {.vars = ptr->locals_head, .function = ptr};
        exec(ptr->code, &local_frame, NULL);
        instr("FROM @%d", ++label);
        instr("'[,'/|'[,'_,'_ S,S,S,S @F%dret", ptr->header.id);
    }

    for (struct func_list* ptr = funcs_head; ptr; ptr = (struct func_list*) ptr->header.next)
    {
        if (!ptr->callsites)
            continue;

        for (struct call_site_list* call = ptr->callsites; call; call = call->next)
        {
            instr("FROM @C%d", call->id); // site id alloc
            int i;
            // todo: O(n) -> O(1) ?
            for (i = 0; i < ptr->callsites->id; i++)
            {
                instr("'_,'/|'[,'_,'_ '%d,'/|'[,'_,'_ R,S,S,S @%d", call->id == i ? 1 : 0, ++label);
                instr("FROM @%d", label);
            }
            instr("'_,'/|'[,'_,'_ '%d,'/|'[,'_,'_ R,S,S,S @%d", call->id == i ? 1 : 0, call->argalloc_address);
            instr("FROM @C%dcheck", call->id);
            instr("'1,'_,'_,'_ '_,'_,'_,'_ L,S,S,S @%d", ++label); // ok
            if (call->next)
                instr("'0,'_,'_,'_ '_,'_,'_,'_ L,S,S,S @C%dcheck", call->id - 1);
            instr("FROM @%d", label);
            instr("'0,'_,'_,'_ '_,'_,'_,'_ L,S,S,S");
            instr("'[,'_,'_,'_ '_,'_,'_,'_ L,S,S,S @%d", ++label);
            instr("FROM @%d", label);
            instr("'/|'0|'1,'_,'_,'_ L,S,S,S");
            instr("'[,'_,'_,'_ S,L,S,S @%d", call->return_address);
        }

        instr("FROM @F%dret", ptr->header.id); // clean heap
        instr("'[,'/|'[,'_,'_ '[,'/|'[,'_,'_ R,R,S,S");
        instr("'0|'1|'/,'_,'_,'_ '_,'_,'_,'_ R,S,S,S");
        instr("'_,'_,'_,'_ L,S,S,S");
        instr("'[,'_,'_,'_ '_,'_,'_,'_ L,S,S,S @C%dcheck", ptr->callsites->id);
    }
}
