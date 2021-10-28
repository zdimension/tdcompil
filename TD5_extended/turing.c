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

#define INT_WIDTH 32

// ----------------------------------------------------------------------
//		Utilities
// ---------------------------------------------------------------------
#define LABEL(n)     printf("#L%03d:\n", n);            // output a label

#define PROD0(op)     printf("#\t%s\n", op)
#define PROD1F(op, v)     printf("#\t%s\t%g\n", op, v)    // v is a float
#define PROD1S(op, v)     printf("#\t%s\t%s\n", op, v)    // v is a string
#define PROD1L(op, v)     printf("#\t%s\tL%03d\n", op, v) // v is a label

#define instr(format, ...) printf (format "\t\t# line %d\n", ## __VA_ARGS__, __LINE__)

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
        return;

    instr("# KIND = %d", AST_KIND(n));

    switch (AST_KIND(n))
    {
        case k_number:
        {
            instr("FROM @%d", ++(*label));
            PROD1F("push", NUMBER_VALUE(n));
            int value = NUMBER_VALUE(n);
            instr("'/|'[,'[,'_ S,R,S @%d", ++*label);
            instr("'/|'[,'/,'_ S,R,S @%d", *label);
            instr("FROM @%d", *label);
            for (int i = 0; i < INT_WIDTH; i++, value /= 2)
            {
                instr("'/|'[,'_,'_ '/|'[,'%d,'_ S,R,S @%d", value & 1, ++*label);
                instr("FROM @%d", *label);
            }
            instr("'/|'[,'_,'_ '/|'[,'/,'_ S,S,S @%d", *label + 1);
            return;
        }
        case k_ident:
        {
            instr("FROM @%d", ++(*label));
            instr("'[,'/|'[,'_ S,S,S @%d", ++*label);
            PROD1S("load", VAR_NAME(n));
            nav_to_var(label, n);
            instr("FROM @%d", *label);
            instr("'/|'[,'/,'_ R,R,S @%d", ++*label);
            instr("'/|'[,'[,'_ R,R,S @%d", *label);
            instr("FROM @%d", *label);
            instr("'0|'1,'_,'_ '0|'1,'0|'1,'_ R,R,S");
            instr("'/,'_,'_ '/,'/,'_ L,S,S @%d", ++*label);
            instr("FROM @%d", *label);
            instr("'/|'0|'1,'/,'_ L,S,S");
            instr("'[,'/,'_ S,S,S @%d", *label + 1);
            return;
        }
        case k_operator:
        {
            ast_node** op = OPER_OPERANDS(n);
            int arity = OPER_ARITY(n);

            switch (OPER_OPERATOR(n))
            {
                /* Expressions */
                case UMINUS:
                {
                    instr("FROM @%d", ++(*label));
                    instr("'[,'/|'[,'_ S,S,S @%d", *label + 1);
                    eval(op[0], label);
                    PROD0("negate");
                    instr("FROM @%d", ++*label);
                    instr("'[,'/,'_ '[,'/,'_ S,L,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_ S,L,S");
                    instr("'[,'/|'[,'_ S,R,S @%d", ++*label);
                    int no_carry = *label;
                    int end = ++*label;
                    int carry = ++*label;
                    instr("FROM @%d", no_carry);
                    instr("'[,'0,'_ '[,'0,'_ S,R,S");
                    instr("'[,'1,'_ '[,'1,'_ S,R,S @%d", carry);
                    instr("'[,'/,'_ S,S,S @%d", end);
                    instr("FROM @%d", carry);
                    instr("'[,'0,'_ '[,'1,'_ S,R,S");
                    instr("'[,'1,'_ '[,'0,'_ S,R,S");
                    instr("'[,'/,'_ S,S,S @%d", end);
                    instr("FROM @%d", end);
                    instr("'[,'/,'_ S,S,S @%d", *label + 1);
                    return;
                }
                case '+':
                {
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("add");
                    instr("FROM @%d", ++*label);
                    instr("'[,'/,'_ '[,'_,'_ S,L,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_ '[,'_,'0|'1 S,L,L");
                    instr("'[,'/,'_ S,L,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_ S,L,S");
                    instr("'[,'/|'[,'_ S,R,R @%d", ++*label);
                    int no_carry = *label;
                    int end = ++*label;
                    int carry = ++*label;
                    instr("FROM @%d", no_carry);
                    instr("'[,'0,'0 '[,'0,'_ S,R,R");
                    instr("'[,'0|'1,'1|'0 '[,'1,'_ S,R,R");
                    instr("'[,'1,'1 '[,'0,'_ S,R,R @%d", carry);
                    instr("'[,'/,'_ S,S,S @%d", end);
                    instr("FROM @%d", carry);
                    instr("'[,'0,'0 '[,'1,'_ S,R,R @%d", no_carry);
                    instr("'[,'0|'1,'1|'0 '[,'0,'_ S,R,R @%d", carry);
                    instr("'[,'1,'1 '[,'1,'_ S,R,R @%d", carry);
                    instr("'[,'/,'_ S,S,S @%d", end);
                    instr("FROM @%d", end);
                    instr("'[,'/,'_ S,S,S @%d", *label + 1);
                    return;
                }
                case '-':
                {
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("sub");
                    instr("FROM @%d", ++*label);
                    instr("'[,'/,'_ '[,'_,'_ S,L,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_ '[,'_,'0|'1 S,L,L");
                    instr("'[,'/,'_ S,L,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_ S,L,S");
                    instr("'[,'/|'[,'_ S,R,R @%d", ++*label);
                    int no_carry = *label;
                    int end = ++*label;
                    int carry = ++*label;
                    instr("FROM @%d", no_carry);
                    instr("'[,'0|'1,'0 '[,'0|'1,'_ S,R,R");
                    instr("'[,'0,'1 '[,'1,'_ S,R,R @%d", carry);
                    instr("'[,'1,'1 '[,'0,'_ S,R,R");
                    instr("'[,'/,'_ S,S,S @%d", end);
                    instr("FROM @%d", carry);
                    instr("'[,'0|'1,'0|'1 '[,'1,'_ S,R,R");
                    instr("'[,'1,'0 '[,'0,'_ S,R,R @%d", no_carry);
                    instr("'[,'0,'1 '[,'0,'_ S,R,R");
                    instr("'[,'/,'_ S,S,S @%d", end);
                    instr("FROM @%d", end);
                    instr("'[,'/,'_ S,S,S @%d", *label + 1);
                    return;
                }
                case '*':

                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("mul");
                    return;
                case '/':
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("div");
                    return;
                case '<':
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("cmplt");
                    return;
                case '>':
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("cmpgt");
                    return;
                case GE:
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("cmpge");
                    return;
                case LE:
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("cmple");
                    return;
                case NE:
                {
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("cmpne");
                    instr("FROM @%d", ++*label);
                    instr("'[,'/,'_ '[,'_,'_ S,L,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_ '[,'_,'0|'1 S,L,L");
                    instr("'[,'/,'_ S,L,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_ S,L,S");
                    instr("'[,'/|'[,'_ S,R,R @%d", ++*label);
                    instr("FROM @%d", *label);
                    int not_equal = ++*label;
                    int end = ++*label;
                    instr("'[,'0|'1,'0|'1 '[,'0|'1,'_ S,R,R");
                    instr("'[,'0|'1,'1|'0 '[,'0|'1,'_ S,R,R @%d", not_equal);
                    instr("'[,'/,'_ S,L,S @%d", ++*label);
                    instr("FROM @%d", *label); // equal
                    instr("'[,'0|'1,'_ '[,'0,'_ S,L,S");
                    instr("'[,'/|'[,'_ S,S,S @%d", end);
                    instr("FROM @%d", not_equal);
                    instr("'[,'0|'1,'0 '[,'0|'1,'_ S,S,R");
                    instr("'[,'0|'1,'1 '[,'0|'1,'_ S,S,R");
                    instr("'[,'0|'1,'_ S,R,S");
                    instr("'[,'/,'_ S,L,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    for (int i = 0; i < INT_WIDTH - 1; i++)
                    {
                        instr("'[,'0|'1,'_ '[,'0,'_ S,L,S @%d", ++*label);
                        instr("FROM @%d", *label);
                    }
                    instr("'[,'0|'1,'_ '[,'1,'_ S,L,S @%d", *label + 1);
                    instr("FROM @%d", end);
                    instr("'[,'/|'[,'_ S,S,S @%d", *label + 1);
                    return;
                }
                case EQ:
                {
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("cmpeq");
                    instr("FROM @%d", ++*label);
                    instr("'[,'/,'_ '[,'_,'_ S,L,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_ '[,'_,'0|'1 S,L,L");
                    instr("'[,'/,'_ S,L,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_ S,L,S");
                    instr("'[,'/|'[,'_ S,R,R @%d", ++*label);
                    instr("FROM @%d", *label);
                    int not_equal = ++*label;
                    int end = ++*label;
                    instr("'[,'0|'1,'0|'1 '[,'0|'1,'_ S,R,R");
                    instr("'[,'0|'1,'1|'0 '[,'0|'1,'_ S,R,R @%d", not_equal);
                    instr("'[,'/,'_ S,L,S @%d", ++*label);
                    instr("FROM @%d", *label); // equal
                    for (int i = 0; i < INT_WIDTH - 1; i++)
                    {
                        instr("'[,'0|'1,'_ '[,'0,'_ S,L,S @%d", ++*label);
                        instr("FROM @%d", *label);
                    }
                    instr("'[,'0|'1,'_ '[,'1,'_ S,L,S @%d", end);
                    instr("FROM @%d", not_equal);
                    instr("'[,'0|'1,'0 '[,'0|'1,'_ S,S,R");
                    instr("'[,'0|'1,'1 '[,'0|'1,'_ S,S,R");
                    instr("'[,'0|'1,'_ S,R,S");
                    instr("'[,'/,'_ S,L,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0|'1,'_ '[,'0,'_ S,L,S");
                    instr("'[,'/|'[,'_ S,S,S @%d", *label + 1);
                    instr("FROM @%d", end);
                    instr("'[,'/|'[,'_ S,S,S @%d", *label + 1);
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
                case KWHILE:
                {
                    int start = ++*label;
                    instr("FROM @%d", start);
                    instr("'[,'/|'[,'_ S,S,S @%d", *label + 1);
                    eval(op[0], label);
                    instr("FROM @%d", ++*label);
                    instr("'[,'/,'_ '[,'_,'_ S,L,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0,'_ '[,'_,'_ S,L,S");
                    int one_found = ++*label;
                    int end = ++*label;
                    instr("'[,'1,'_ '[,'_,'_ S,L,S @%d", one_found);
                    instr("'[,'/|'[,'_ S,S,S @%d", end);
                    instr("FROM @%d", one_found);
                    instr("'[,'0|'1,'_ '[,'_,'_ S,L,S");
                    int is_nonzero = *label + 1;
                    instr("'[,'/|'[,'_ S,S,S @%d", is_nonzero);

                    eval(op[1], label);

                    instr("FROM @%d", ++*label); // after is_non_zero
                    instr("'[,'/|'[,'_ S,S,S @%d", start);

                    instr("FROM @%d", end);
                    instr("'[,'/|'[,'_ S,S,S @%d", *label + 1);
                    return;
                }
                case KIF:
                {
                    eval(op[0], label);
                    instr("FROM @%d", ++*label);
                    instr("'[,'/,'_ '[,'_,'_ S,L,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'[,'0,'_ '[,'_,'_ S,L,S");
                    int one_found = ++*label;
                    int is_zero;
                    int end = ++*label;
                    if (op[2] == NULL)
                        is_zero = end;
                    else
                        is_zero = ++*label;
                    instr("'[,'1,'_ '[,'_,'_ S,L,S @%d", one_found);
                    instr("'[,'/|'[,'_ S,S,S @%d", is_zero);
                    instr("FROM @%d", one_found);
                    instr("'[,'0|'1,'_ '[,'_,'_ S,L,S");
                    int is_nonzero = *label + 1;
                    instr("'[,'/|'[,'_ S,S,S @%d", is_nonzero);

                    eval(op[1], label);

                    instr("FROM @%d", ++*label); // after is_non_zero
                    instr("'[,'/|'[,'_ S,S,S @%d", end);

                    if (op[2])
                    {
                        instr("FROM @%d", is_zero);
                        int zero_id = *label + 1;
                        instr("'[,'/|'[,'_ S,S,S @%d", zero_id);
                        // else

                        eval(op[2], label);

                        instr("FROM @%d", ++*label);
                        instr("'[,'/|'[,'_ S,S,S @%d", *label + 1);
                    }
                    instr("FROM @%d", end);
                    instr("'[,'/|'[,'_ S,S,S @%d", *label + 1);
                    return;
                }
                case KREAD:
                    PROD1S("read", VAR_NAME(op[0]));
                    return;
                case '=':
                {
                    eval(op[1], label);
                    instr("FROM @%d", ++*label);
                    instr("'0|'1|'/,'/,'_ L,S,S");
                    instr("'[,'/,'_ S,S,S @%d", ++*label);
                    nav_to_var(label, op[0]);
                    instr("FROM @%d", *label);
                    instr("'/|'[,'/,'_ S,L,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'/,'0|'1,'_ S,L,S");
                    instr("'[,'0|'1,'_ S,L,S");
                    instr("'/,'/|'[,'_ R,R,S @%d", ++*label);
                    instr("'[,'/|'[,'_ R,R,S @%d", *label);
                    instr("FROM @%d", *label);
                    instr("'0|'1,'0,'_ '0,'_,'_ R,R,S");
                    instr("'0|'1,'1,'_ '1,'_,'_ R,R,S");
                    instr("'/,'/,'_ '/,'_,'_ L,L,S @%d", ++*label);
                    instr("FROM @%d", *label);
                    instr("'/|'0|'1|'[,'_,'_ L,L,S");
                    instr("'/|'0|'1|'[,'/,'_ S,S,S @%d", *label + 2);
                    instr("'/|'0|'1|'[,'[,'_ S,S,S @%d", *label + 1);
                    instr("FROM @%d", ++*label);
                    instr("'/|'0|'1,'[,'_ L,S,S");
                    instr("'[,'[,'_ S,S,S @%d", *label + 1);
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
        instr("FROM @%d", *label);
        instr("'/|'[,'/,'_ R,S,S @%d", ++*label);
        instr("'/|'[,'[,'_ R,S,S @%d", *label);
        instr("FROM @%d", *label);
        instr("'0|'1,'/,'_ R,S,S");
        instr("'0|'1,'[,'_ R,S,S");
        instr("'/,'/|'[,'_ S,S,S @%d", ++*label);
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

    instr("NEW \"generated\" 3");
    instr("START @INIT");
    instr("END @END \"END\"");
    instr("END @DEFAULT \"default option\"");
    instr("FROM @INIT");
    instr("'_,'_,'_ '[,'[,'_ R,S,S @0");

    for (struct var_list* ptr = head; ptr; ptr = ptr->next)
    {
        for (int i = 0; i < INT_WIDTH; i++)
        {
            instr("FROM @%d", label);
            instr("'_,'[,'_ '0,'[,'_ R,S,S @%d", ++label);
        }
        instr("FROM @%d", label);
        instr("'_,'[,'_ '/,'[,'_ R,S,S @%d", ++label);
    }
    instr("FROM @%d", label);
    instr("'_|'/|'0,'[,'_ L,S,S");
    instr("'[,'[,'_ S,S,S @%d", label + 1);

    eval(n, &label);

    instr("FROM @%d", ++label);
    instr("'[,'/,'_ S,S,S @END");
    instr("'[,'[,'_ S,S,S @END");
}
