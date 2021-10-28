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
instr("FROM @%d", ++*label);\
instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++*label);\
instr("FROM @%d", *label);\
instr("'[,'0|'1,'_,'_ '[,'_,'0|'1,'_ S,L,L,S");\
instr("'[,'/,'_,'_ S,S,R,S @%d", ++*label);\
instr("FROM @%d", *label);\
instr("'[,'/,'0|'1,'_ S,S,R,S");\
instr("'[,'/,'_,'_ S,L,L,S @%d", ++*label);\
instr("FROM @%d", *label);\
int left_then_one = ++*label;\
int left_then_zero = ++*label;\
int one = ++*label;\
int zero = ++*label;\
int end = *label + 1;\
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

struct var_list
{
    const char* name;
    struct var_list* next;
};

struct var_list* head = NULL, * tail = NULL;

void nav_to_var(int* label, struct ast_node* op);

int get_var_id(const char* name)
{
    int i = 0;
    for (struct var_list* ptr = head; ptr; ptr = ptr->next, i++)
    {
        if (!strcmp(ptr->name, name))
        {
            return i;
        }
    }
    fprintf(stderr, "VAR NOT FOUND: %s\n", name);
    abort();
}

void eval(ast_node* n, int* label)
{
    if (!n)
    {
        fprintf(stderr, "Null node eval\n");
        abort();
    }

    instr("# KIND = %d", AST_KIND(n));

    switch (AST_KIND(n))
    {
        case k_number:
        {
            instr("FROM @%d", ++(*label));
            PROD1F("push", NUMBER_VALUE(n));
            int value = NUMBER_VALUE(n);
            instr("'/|'[,'[,'_,'_ S,R,S,S @%d", ++*label);
            instr("'/|'[,'/,'_,'_ S,R,S,S @%d", *label);
            instr("FROM @%d", *label);
            for (int i = 0; i < INT_WIDTH; i++, value /= 2)
            {
                instr("'/|'[,'_,'_,'_ '/|'[,'%d,'_,'_ S,R,S,S @%d", value & 1, ++*label);
                instr("FROM @%d", *label);
            }
            instr("'/|'[,'_,'_,'_ '/|'[,'/,'_,'_ S,S,S,S @%d", *label + 1);
            return;
        }
        case k_ident:
        {
            instr("FROM @%d", ++(*label));
            instr("'[,'/|'[,'_,'_ S,S,S,S @%d", *label + 1);
            PROD1S("load", VAR_NAME(n));
            nav_to_var(label, n);
            instr("FROM @%d", ++*label);
            instr("'/|'[,'/,'_,'_ R,R,S,S @%d", ++*label);
            instr("'/|'[,'[,'_,'_ R,R,S,S @%d", *label);
            instr("FROM @%d", *label);
            instr("'0|'1,'_,'_,'_ '0|'1,'0|'1,'_,'_ R,R,S,S");
            instr("'/,'_,'_,'_ '/,'/,'_,'_ L,S,S,S @%d", ++*label);
            instr("FROM @%d", *label);
            instr("'/|'0|'1,'/,'_,'_ L,S,S,S");
            instr("'[,'/,'_,'_ S,S,S,S @%d", *label + 1);
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
                    instr("FROM @%d", ++(*label));
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", *label + 1);
                    eval(op[0], label);
                    PROD0("negate");
                    instr("FROM @%d", ++*label);
                    instr("'[,'/,'_,'_ '[,'/,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", ++*label);
                    int no_carry = *label;
                    int end = ++*label;
                    int carry = ++*label;
                    instr("FROM @%d", no_carry);
                    instr("'[,'0,'_,'_ '[,'0,'_,'_ S,R,S,S");
                    instr("'[,'1,'_,'_ '[,'1,'_,'_ S,R,S,S @%d", carry);
                    instr("'[,'/,'_,'_ S,S,S,S @%d", end);
                    instr("FROM @%d", carry);
                    instr("'[,'0,'_,'_ '[,'1,'_,'_ S,R,S,S");
                    instr("'[,'1,'_,'_ '[,'0,'_,'_ S,R,S,S");
                    instr("'[,'/,'_,'_ S,S,S,S @%d", end);
                    instr("FROM @%d", end);
                    instr("'[,'/,'_,'_ S,S,S,S @%d", *label + 1);
                    return;
                }
                case '+':
                {
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("add");
                    instr("FROM @%d", ++*label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_,'_ '[,'_,'0|'1,'_ S,L,L,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,R,S @%d", ++*label);
                    int no_carry = *label;
                    int end = ++*label;
                    int carry = ++*label;
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
                    instr("'[,'/,'_,'_ S,S,S,S @%d", *label + 1);
                    return;
                }
                case '-':
                {
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("sub");
                    instr("FROM @%d", ++*label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_,'_ '[,'_,'0|'1,'_ S,L,L,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,R,S @%d", ++*label);
                    int no_carry = *label;
                    int end = ++*label;
                    int carry = ++*label;
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
                    instr("'[,'/,'_,'_ S,S,S,S @%d", *label + 1);
                    return;
                }
                case '*':
                {
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("mul");
                    instr("FROM @%d", ++*label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_,'_ '[,'_,'0|'1,'_ S,L,L,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_,'_ '[,'0,'_,'0|'1 S,L,S,L");
                    instr("'[,'/|'[,'_,'_ S,R,R,R @%d", ++*label);
                    int loop = *label;
                    int no_carry = ++*label;
                    int carry = ++*label;
                    int next = ++*label;
                    int end = *label + 1;
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
                    instr("'[,'/,'_,'_ '[,'/,'_,'_ S,S,R,L @%d", end);
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
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("div");
                    return;
                case '<':
                {
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("cmplt");
                    COMPARISON({
                                   instr("'[,'0,'1,'_ '[,'_,'_,'_ S,L,L,S @%d", left_then_one);
                                   instr("'[,'1,'0,'_ '[,'_,'_,'_ S,L,L,S @%d", left_then_zero);
                                   instr("'[,'/|'[,'_,'_ '[,'/|'[,'_,'_ S,R,S,S @%d", zero);
                               });
                    return;
                }
                case '>':
                {
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("cmpgt");
                    COMPARISON({
                                   instr("'[,'1,'0,'_ '[,'_,'_,'_ S,L,L,S @%d", left_then_one);
                                   instr("'[,'0,'1,'_ '[,'_,'_,'_ S,L,L,S @%d", left_then_zero);
                                   instr("'[,'/|'[,'_,'_ '[,'/|'[,'_,'_ S,R,S,S @%d", zero);
                               });
                    return;
                }
                case GE:
                {
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("cmpge");
                    COMPARISON({
                                   instr("'[,'1,'0,'_ '[,'_,'_,'_ S,L,L,S @%d", left_then_one);
                                   instr("'[,'0,'1,'_ '[,'_,'_,'_ S,L,L,S @%d", left_then_zero);
                                   instr("'[,'/|'[,'_,'_ '[,'/|'[,'_,'_ S,R,S,S @%d", one);
                               });
                    return;
                }
                case LE:
                {
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("cmple");
                    COMPARISON({
                                   instr("'[,'0,'1,'_ '[,'_,'_,'_ S,L,L,S @%d", left_then_one);
                                   instr("'[,'1,'0,'_ '[,'_,'_,'_ S,L,L,S @%d", left_then_zero);
                                   instr("'[,'/|'[,'_,'_ '[,'/|'[,'_,'_ S,R,S,S @%d", one);
                               });
                    return;
                }
                case NE:
                {
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("cmpne");
                    instr("FROM @%d", ++*label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_,'_ '[,'_,'0|'1,'_ S,L,L,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,R,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    int not_equal = ++*label;
                    int end = ++*label;
                    instr("'[,'0|'1,'0|'1,'_ '[,'0|'1,'_,'_ S,R,R,S");
                    instr("'[,'0|'1,'1|'0,'_ '[,'0|'1,'_,'_ S,R,R,S @%d", not_equal);
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label); // equal
                    instr("'[,'0|'1,'_,'_ '[,'0,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", end);
                    instr("FROM @%d", not_equal);
                    instr("'[,'0|'1,'0,'_ '[,'0|'1,'_,'_ S,R,R,S");
                    instr("'[,'0|'1,'1,'_ '[,'0|'1,'_,'_ S,R,R,S");
                    instr("'[,'/,'0|'1,'_ '[,'/,'_,'_ S,S,R,S");
                    instr("'[,'0|'1,'_,'_ S,R,S,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    for (int i = 0; i < INT_WIDTH - 1; i++)
                    {
                        instr("'[,'0|'1,'_,'_ '[,'0,'_,'_ S,L,S,S @%d", ++*label);
                        instr("FROM @%d", *label);
                    }
                    instr("'[,'0|'1,'_,'_ '[,'1,'_,'_ S,R,S,S @%d", end);
                    instr("FROM @%d", end);
                    instr("'[,'0|'1,'_,'_ S,R,S,S");
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", *label + 1);
                    return;
                }
                case EQ:
                {
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("cmpeq");
                    instr("FROM @%d", ++*label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_,'_ '[,'_,'0|'1,'_ S,L,L,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,R,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    int not_equal = ++*label;
                    int end = ++*label;
                    instr("'[,'0|'1,'0|'1,'_ '[,'0|'1,'_,'_ S,R,R,S");
                    instr("'[,'0|'1,'1|'0,'_ '[,'0|'1,'_,'_ S,R,R,S @%d", not_equal);
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label); // equal
                    for (int i = 0; i < INT_WIDTH - 1; i++)
                    {
                        instr("'[,'0|'1,'_,'_ '[,'0,'_,'_ S,L,S,S @%d", ++*label);
                        instr("FROM @%d", *label);
                    }
                    instr("'[,'0|'1,'_,'_ '[,'1,'_,'_ S,R,S,S @%d", end);
                    instr("FROM @%d", not_equal);
                    instr("'[,'0|'1,'0,'_ '[,'0|'1,'_,'_ S,R,R,S");
                    instr("'[,'0|'1,'1,'_ '[,'0|'1,'_,'_ S,R,R,S");
                    instr("'[,'/,'0|'1,'_ '[,'/,'_,'_ S,S,R,S");
                    instr("'[,'0|'1,'_,'_ S,R,S,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_,'_ '[,'0,'_,'_ S,L,L,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", end);
                    instr("FROM @%d", end);
                    instr("'[,'0|'1,'_,'_ S,R,S,S");
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", *label + 1);
                    return;
                }
                case INC:
                {
                    PROD0("inc");
                    nav_to_var(label, op[0]);
                    instr("FROM @%d", ++*label);
                    instr("'/|'[,'[,'_,'_ R,R,S,S @%d", ++*label);
                    instr("'/|'[,'/,'_,'_ R,R,S,S @%d", *label);
                    if (clean_stack) // statement
                    {
                        instr("FROM @%d", *label);
                        int left_to_end = ++*label;
                        int end = *label + 1;
                        instr("'0,'_,'_,'_ '1,'_,'_,'_ L,S,S,S @%d", left_to_end);
                        instr("'1,'_,'_,'_ '0,'_,'_,'_ R,S,S,S");
                        instr("FROM @%d", left_to_end);
                        instr("'0|'1|'/,'_,'_,'_ L,S,S,S");
                        instr("'[,'_,'_,'_ S,L,S,S @%d", end);
                    }
                    else // expression
                    {
                        instr("FROM @%d", *label);
                        int end = ++*label;
                        int left = ++*label;
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
                        instr("'[,'/,'_,'_ S,S,S,S @%d", *label + 1);
                    }
                    return;
                }
                case DEC:
                {
                    PROD0("dec");
                    nav_to_var(label, op[0]);
                    instr("FROM @%d", ++*label);
                    instr("'/|'[,'[,'_,'_ R,R,S,S @%d", ++*label);
                    instr("'/|'[,'/,'_,'_ R,R,S,S @%d", *label);
                    if (clean_stack) // statement
                    {
                        instr("FROM @%d", *label);
                        int left_to_end = ++*label;
                        int end = *label + 1;
                        instr("'1,'_,'_,'_ '0,'_,'_,'_ L,S,S,S @%d", left_to_end);
                        instr("'0,'_,'_,'_ '1,'_,'_,'_ R,L,S,S");
                        instr("FROM @%d", left_to_end);
                        instr("'0|'1|'/,'_,'_,'_ L,S,S,S");
                        instr("'[,'_,'_,'_ S,L,S,S @%d", end);
                    }
                    else // expression
                    {
                        instr("FROM @%d", *label);
                        int end = ++*label;
                        int left = ++*label;
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
                        instr("'[,'/,'_,'_ S,S,S,S @%d", *label + 1);
                    }
                    return;
                }

                    /* Statements */
                case ';':
                    if (arity == 0)
                        return;
                    else
                    {
                        instr("# ; first");
                        eval(op[0], label);
                        instr("# ; second");
                        eval(op[1], label);
                        return;
                    }
                case KPRINT:
                    eval(op[0], label);
                    PROD0("print");
                    return;
                case KDO:
                {
                    int start = *label + 1;
                    eval(op[0], label);
                    eval(op[1], label);
                    instr("FROM @%d", ++*label);
                    instr("'[,'/|'[,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    int one_found = ++*label;
                    int end = *label + 1;
                    instr("'[,'1,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", one_found);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", end);
                    instr("FROM @%d", one_found);
                    instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", start);
                    return;
                }
                case KWHILE:
                {
                    int start = ++*label;
                    instr("FROM @%d", start);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", *label + 1);
                    eval(op[0], label);
                    instr("FROM @%d", ++*label);
                    instr("'[,'/|'[,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    int one_found = ++*label;
                    int end = ++*label;
                    instr("'[,'1,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", one_found);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", end);
                    instr("FROM @%d", one_found);
                    instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    int is_nonzero = *label + 1;
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", is_nonzero);

                    eval(op[1], label);

                    instr("FROM @%d", ++*label); // after is_non_zero
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", start);

                    instr("FROM @%d", end);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", *label + 1);
                    return;
                }
                case KIF:
                {
                    eval(op[0], label);
                    instr("FROM @%d", ++*label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    int one_found = ++*label;
                    int is_zero;
                    int end = ++*label;
                    if (op[2] == NULL)
                        is_zero = end;
                    else
                        is_zero = ++*label;
                    instr("'[,'1,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", one_found);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", is_zero);
                    instr("FROM @%d", one_found);
                    instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    int is_nonzero = *label + 1;
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", is_nonzero);

                    eval(op[1], label);

                    instr("FROM @%d", ++*label); // after is_non_zero
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", end);

                    if (op[2])
                    {
                        instr("FROM @%d", is_zero);
                        int zero_id = *label + 1;
                        instr("'[,'/|'[,'_,'_ S,S,S,S @%d", zero_id);
                        // else

                        eval(op[2], label);

                        instr("FROM @%d", ++*label);
                        instr("'[,'/|'[,'_,'_ S,S,S,S @%d", *label + 1);
                    }
                    instr("FROM @%d", end);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", *label + 1);
                    return;
                }
                case KREAD:
                    PROD1S("read", VAR_NAME(op[0]));
                    return;
                case '=':
                {
                    eval(op[1], label);
                    instr("FROM @%d", ++*label);
                    instr("'0|'1|'/,'/,'_,'_ L,S,S,S");
                    instr("'[,'/,'_,'_ S,S,S,S @%d", *label + 1);
                    nav_to_var(label, op[0]);
                    instr("FROM @%d", ++*label);
                    instr("'/|'[,'/,'_,'_ S,L,S,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'/,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'/,'/|'[,'_,'_ R,R,S,S @%d", ++*label);
                    instr("'[,'/|'[,'_,'_ R,R,S,S @%d", *label);
                    if (clean_stack) // statement, clear from stack
                    {
                        instr("FROM @%d", *label);
                        instr("'0|'1,'0,'_,'_ '0,'_,'_,'_ R,R,S,S");
                        instr("'0|'1,'1,'_,'_ '1,'_,'_,'_ R,R,S,S");
                        instr("'/,'/,'_,'_ '/,'_,'_,'_ L,L,S,S @%d", ++*label);
                        instr("FROM @%d", *label);
                        instr("'/|'0|'1|'[,'_,'_,'_ L,L,S,S");
                        instr("'/|'0|'1|'[,'/,'_,'_ S,S,S,S @%d", *label + 2);
                        instr("'/|'0|'1|'[,'[,'_,'_ S,S,S,S @%d", ++*label);
                        instr("FROM @%d", *label);
                        instr("'/|'0|'1,'[,'_,'_ L,S,S,S");
                        instr("'[,'[,'_,'_ S,S,S,S @%d", *label + 1);
                    }
                    else // expression, keep in stack
                    {
                        instr("FROM @%d", *label);
                        instr("'0|'1,'0,'_,'_ '0,'0,'_,'_ R,R,S,S");
                        instr("'0|'1,'1,'_,'_ '1,'1,'_,'_ R,R,S,S");
                        instr("'/,'/,'_,'_ L,S,S,S @%d", ++*label);
                        instr("FROM @%d", *label);
                        instr("'0|'1,'/,'_,'_ L,S,S,S");
                        instr("'/|'[,'/,'_,'_ S,S,S,S @%d", *label + 1);
                    }
                    PROD1S("store", VAR_NAME(op[0]));
                    return;
                }
                default:
                    error_msg("Houston, we have a problem: unattended token %d",
                              OPER_OPERATOR(n));
                    return;
            }
        }
        default:
            return;
    }
}

void nav_to_var(int* label, struct ast_node* op)
{
    int vid = get_var_id(VAR_NAME(op));
    while (vid-- > 0)
    {
        instr("FROM @%d", ++*label);
        instr("'/|'[,'/,'_,'_ R,S,S,S @%d", ++*label);
        instr("'/|'[,'[,'_,'_ R,S,S,S @%d", *label);
        instr("FROM @%d", *label);
        instr("'0|'1,'/,'_,'_ R,S,S,S");
        instr("'0|'1,'[,'_,'_ R,S,S,S");
        instr("'/,'/|'[,'_,'_ S,S,S,S @%d", *label + 1);
    }
}


void traverse_vars(ast_node* n)
{
    if (!n)
        return;

    if (AST_KIND(n) == k_operator)
    {
        if (OPER_OPERATOR(n) == '=')
        {
            const char* name = VAR_NAME(OPER_OPERANDS(n)[0]);
            bool found = false;
            for (struct var_list* ptr = head; ptr; ptr = ptr->next)
            {
                if (!strcmp(ptr->name, name))
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                struct var_list* newNode = malloc(sizeof(struct var_list));
                newNode->name = name;
                newNode->next = NULL;
                if (head == NULL)
                {
                    head = newNode;
                    tail = newNode;
                }
                else
                {
                    (tail)->next = newNode;
                    tail = newNode;
                }
            }
        }

        for (int i = 0; i < OPER_ARITY(n); i++)
            traverse_vars(OPER_OPERANDS(n)[i]);
    }
}


// ----------------------------------------------------------------------
//		Code production
//			This version produces code for a stack machine
// ---------------------------------------------------------------------
void produce_code(ast_node* n)
{
    int label = 0;

    traverse_vars(n);

    {
        int i = 0;
        printf("# Memory map\n");
        for (struct var_list* ptr = head; ptr; ptr = ptr->next, i++)
        {
            printf("# %4d %s\n", i, ptr->name);
        }
    }

    instr("NEW \"generated\" 4");
    instr("START @INIT");
    instr("END @END \"END\"");
    instr("END @DEFAULT \"default option\"");
    instr("FROM @INIT");
    instr("'_,'_,'_,'_ '[,'[,'_,'_ R,S,S,S @0");

    for (struct var_list* ptr = head; ptr; ptr = ptr->next)
    {
        for (int i = 0; i < INT_WIDTH; i++)
        {
            instr("FROM @%d", label);
            instr("'_,'[,'_,'_ '0,'[,'_,'_ R,S,S,S @%d", ++label);
        }
        instr("FROM @%d", label);
        instr("'_,'[,'_,'_ '/,'[,'_,'_ R,S,S,S @%d", ++label);
    }
    instr("FROM @%d", label);
    instr("'_|'/|'0,'[,'_,'_ L,S,S,S");
    instr("'[,'[,'_,'_ S,S,S,S @%d", label + 1);

    eval(n, &label);

    instr("FROM @%d", ++label);
    instr("'[,'/,'_,'_ S,S,S,S @END");
    instr("'[,'[,'_,'_ S,S,S,S @END");
}
