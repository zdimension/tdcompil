/*
 * code2.c 	-- Production of code for a stack machine
 *
 * Copyright © 2015 Erick Gallesio - I3S-CNRS/Polytech Nice-Sophia <eg@unice.fr>
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 21-Oct-2015 15:31 (eg)
 * Last file update:  4-Nov-2015 17:06 (eg)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "calc.h"
#include "syntax.h"

// ----------------------------------------------------------------------
//		Utilities
// ---------------------------------------------------------------------
#define LABEL(n)     printf("L%03d:\n", n);            // output a label

#define PROD0(op)     printf("\t%s\n", op)
#define PROD1D(op, v)     printf("\t%s\t%g\n", op, v)    // v is a float
#define PROD1S(op, v)     printf("\t%s\t%s\n", op, v)    // v is a string
#define PROD1L(op, v)     printf("\t%s\tL%03d\n", op, v) // v is a label

void eval(ast_node* n, int* label)
{
    if (!n)
        return;

    switch (AST_KIND(n))
    {
        case k_number:
            PROD1D("push", NUMBER_VALUE(n));
            return;
        case k_ident:
            PROD1S("load", VAR_NAME(n));
            return;
        case k_operator:
        {
            ast_node** op = OPER_OPERANDS(n);
            int arity = OPER_ARITY(n);

            switch (OPER_OPERATOR(n))
            {
                /* Expressions */
                case UMINUS:
                    eval(op[0], label);
                    PROD0("negate");
                    return;
                case '+':
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("add");
                    return;
                case '-':
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("sub");
                    return;
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
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("cmpne");
                    return;
                case EQ:
                    eval(op[0], label);
                    eval(op[1], label);
                    PROD0("cmpeq");
                    return;

                    /* Statements */
                case ';':
                    if (arity == 0)
                        return;
                    else
                    {
                        eval(op[0], label);
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
                    int end = ++*label;
                    LABEL(start);

                    eval(op[0], label);
                    PROD1L("jumpz", end);

                    eval(op[1], label);
                    PROD1L("jump", start);
                    LABEL(end);
                    return;
                }
                case KIF:
                {
                    int else_ = ++*label;
                    int end = ++*label;

                    eval(op[0], label);
                    PROD1L("jumpz", else_);

                    eval(op[1], label);
                    if (op[2] != NULL)
                        PROD1L("jump", end);

                    LABEL(else_);
                    eval(op[2], label);

                    LABEL(end);
                    return;
                }
                case KREAD:
                    PROD1S("read", VAR_NAME(op[0]));
                    return;
                case '=':
                    eval(op[1], label);
                    PROD1S("store", VAR_NAME(op[0]));
                    return;
                default:
                    error_msg("Houston, we have a problem: unattended token %d\n",
                              OPER_OPERATOR(n));
                    return;
            }
        }
        default:
            return;
    }
}


// ----------------------------------------------------------------------
//		Code production
//			This version produces code for a stack machine
// ---------------------------------------------------------------------
void produce_code(ast_node* n)
{
    int label = -1;
    eval(n, &label);
}
