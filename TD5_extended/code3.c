/*
 * code2.c 	-- Production of code for a stack machine
 *
 * Copyright © 2015 Erick Gallesio - I3S-CNRS/Polytech Nice-Sophia <eg@unice.fr>
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 22-Oct-2015 22:48 (eg)
 * Last file update:  4-Nov-2015 17:09 (eg)
 */

#include <stdio.h>
#include <stdlib.h>

#include "calc.h"
#include "syntax.h"

int box(int* label, const char* str, const char* color)
{
    printf("\tbox%d [label=\"%s\", fillcolor=\"%s\"];\n", *label, str, color);
    return (*label)++;
}

#define BOXF(fmt, args, color) (printf("\tbox%d [label=\"" fmt "\", fillcolor=\"" color "\"];\n", *label, args), (*label)++)

int link(int a, int b)
{
    if (b != -1)
        printf("\t\tbox%d -> box%d;\n", a, b);
    return a;
}

int linkl(int a, int b, const char* l)
{
    if (b != -1)
        printf("\t\tbox%d -> box%d [label=\"%s\"];\n", a, b, l);
    return a;
}

int link2(int x, int a, int b)
{
    link(x, b);
    link(x, a);
    return x;
}

#define KEYWORD(name) box(label, name, "turquoise4")

int eval(ast_node* n, int* label)
{
    if (!n)
        return -1;

    switch (AST_KIND(n))
    {
        case k_number:
            return BOXF("%d", NUMBER_VALUE(n), "tomato");
        case k_ident:
            return BOXF("%s", VAR_NAME(n), "peru");
        case k_scope:
            return link(KEYWORD("scope"), eval(SC_CODE(n), label));
        case k_list:
        {
            return -1;
        }
        case k_operator:
        {
            ast_node** op = OPER_OPERANDS(n);
            int arity = OPER_ARITY(n);

            int o = OPER_OPERATOR(n);

            switch(o)
            {
                case KTYPE:
                    return KEYWORD("type");
                case KIMPL:
                    return KEYWORD("impl");
            }

            int op0 = arity > 0 ? eval(op[0], label) : -1;
            int op1 = arity > 1 ? eval(op[1], label) : -1;
            int op2 = arity > 2 ? eval(op[2], label) : -1;
            int op3 = arity > 3 ? eval(op[3], label) : -1;
            const char* ops2[] = {">=", "<=", "==", "!="};
            switch (o)
            {
                /* Expressions */
                case UMINUS:
                    return link(KEYWORD("[-]"), op0);
                case STRUCTLIT:
                    return KEYWORD("structlit");
                case KRETURN:
                    return link(KEYWORD("return"), op0);
                case KTYPEOF:
                    return link(KEYWORD("typeof"), op0);
                case '(':
                {
                    int func = KEYWORD("call");
                    linkl(func, op0, "name");
                    int block = KEYWORD("args");
                    linkl(func, block, "args");
                    for (linked_list* ptr = ((ast_linked_list*)op[1])->list; ptr; ptr = ptr->next)
                    {
                        link(block, eval(ptr->value, label));
                    }
                    return func;
                }
                case KFUNC:
                {
                    int block = KEYWORD("func");
                    linkl(block, op0, "name");
                    //linkl(block, op1, "type_params");
                    linkl(block, op3, "type");
                    linkl(block, op2, "code");
                    return block;
                }
                case INC:
                    return link(KEYWORD(arity == 1 ? "pre++" : "post++"), op0);
                case DEC:
                    return link(KEYWORD(arity == 1 ? "pre--" : "post--"), op0);
                case DEREF:
                    return link(KEYWORD("[*]"), op0);
                case REF:
                    return arity == 1
                           ? link(KEYWORD("[&]"), op0)
                           : link2(KEYWORD("[&]"), op0, op1);
                case KVAR:
                    return link2(KEYWORD("var"), op0, op1);
                case GE:
                case LE:
                case EQ:
                case NE:
                    return link2(KEYWORD(ops2[o - GE]), op0, op1);
                case '+':
                case '-':
                case '*':
                case '/':
                case '<':
                case '>':
                case '=':
                case ';':
                case '.':
                    return link2(BOXF("[%c]", o, "turquoise4"), op0, op1);
                case KPRINT:
                    return link(KEYWORD("print"), op0);
                case KWHILE:
                {
                    int block = KEYWORD("while");
                    linkl(block, op0, "cond");
                    linkl(block, op1, "then");
                    return block;
                }
                case KIF:
                {
                    int block = KEYWORD("if");
                    linkl(block, op0, "cond");
                    linkl(block, op1, "then");
                    linkl(block, op2, "else");
                    return block;
                }
                case KREAD:
                    return link(KEYWORD("read"), op0);
                default:
                    error_msg(n, "Houston, we have a problem: unattended token %d\n",
                              OPER_OPERATOR(n));
                    exit(1);
            }
        }
        default:
            error_msg(n, "Houston, we have a problem: unattended kind %d\n",
                      AST_KIND(n));
            exit(1);
    }
}


// ----------------------------------------------------------------------
//		Code production
//			This version produces a dot file
// ---------------------------------------------------------------------
void produce_code(ast_node* n)
{
    printf("digraph E{\n");
    printf("\tnode [style=\"filled\"];\n");
    int id = 0;
    eval(n, &id);
    printf("}\n");
}
