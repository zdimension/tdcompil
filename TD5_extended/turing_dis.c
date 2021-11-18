#include <stdio.h>
#include <stdlib.h>
#include "turing.h"

const int TAB_WIDTH = 4;
int indent = 0;
bool expect_sub = false;

#define DEBUG

#ifdef DEBUG
#define AFTER_PRINT fflush(stdout)
#else
#define AFTER_PRINT (void)0
#endif

#define tab() printf("%*s", TAB_WIDTH * indent, "")
//#define code(fmt, ...) printf("%*s" fmt, TAB_WIDTH * indent, "", ##__VA_ARGS__)

#define code_n(fmt, ...) do { printf(fmt, ##__VA_ARGS__); AFTER_PRINT; } while (0)
#define sc() code_n(";\n")

#define write_bin(sym) do { if (!AST_CLEAN_STACK(n)) { code_n("("); } write_inline(op[0]); code_n(" %s ", sym); write_inline(op[1]); if (!AST_CLEAN_STACK(n)) { code_n(")"); } } while (0)
#define write_un(sym) do { if (!AST_CLEAN_STACK(n)) { code_n("("); } code_n("%s", sym); write_inline(op[0]); if (!AST_CLEAN_STACK(n)) { code_n(")"); } } while (0)
#define un_stmt(sym) do { code_n("%s(", sym); write_inline(op[0]); code_n(")"); } while (0)

#define sci() do { if (AST_CLEAN_STACK(n)) sc(); } while (0)

void write_inline(ast_node* n)
{
    if (!n)
    {
        return;
    }

    int ind = indent;
    indent = 0;
    bool cs = AST_CLEAN_STACK(n);
    AST_CLEAN_STACK(n) = false;
    write_code(n);
    AST_CLEAN_STACK(n) = cs;
    indent = ind;
}

void write_code(ast_node* n)
{
    if (!n)
    {
        return;
    }

    bool sub = expect_sub;
    expect_sub = false;

    if (AST_KIND(n) == k_scope)
    {
        if (sub)
        {
            indent--;
            tab();
        }

        code_n("{\n");
        indent++;
        tab();
        write_code(SC_CODE(n));
        indent--;
        tab();
        code_n("}");
        if (AST_CLEAN_STACK(n))
        {
            code_n("\n");
        }
        if (sub)
        {
            indent++;
        }
        return;
    }

    if (sub)
    {
        tab();
    }

    switch (AST_KIND(n))
    {
        case k_ident:
            code_n("%s", VAR_NAME(n));
            return;
        case k_number:
            code_n("%d", NUMBER_VALUE(n));// todo: type
            return;
        case k_operator:
        {
            ast_node** op = OPER_OPERANDS(n);
            int arity = OPER_ARITY(n);
            char str[] = {OPER_OPERATOR(n), 0};

            switch (OPER_OPERATOR(n))
            {
                case UMINUS:
                    write_un("-");
                    sci();
                    return;
                case '~':
                    write_un("~");
                    sci();
                    return;
                case DEREF:
                    write_un("*");
                    sci();
                    return;
                case ';':
                    if (arity)
                    {
                        //tab();
                        write_code(op[0]);
                        if (op[1])
                        {
                            if (op[0])
                                tab();
                            write_code(op[1]);
                        }
                    }
                    return;
                case KIF:
                    code_n("if (");
                    write_inline(op[0]);
                    code_n(")\n");
                    expect_sub = true;
                    indent++;
                    write_code(op[1]);
                    indent--;
                    if (op[2])
                    {
                        tab();
                        code_n("else\n");
                        expect_sub = true;
                        indent++;
                        write_code(op[2]);
                        indent--;
                    }
                    return;
                case '+':
                case '-':
                case '*':
                case '/':
                case '<':
                case '>':
                case '=':
                    write_bin(str);
                    sci();
                    return;
                case SHL:
                    write_bin("<<");
                    sci();
                    return;
                case SHR:
                    write_bin(">>");
                    sci();
                    return;
                case GE:
                    write_bin(">=");
                    sci();
                    return;
                case LE:
                    write_bin("<=");
                    sci();
                    return;
                case EQ:
                    write_bin("==");
                    sci();
                    return;
                case NE:
                    write_bin("!=");
                    sci();
                    return;
                case OR:
                    write_bin("||");
                    sci();
                    return;
                case AND:
                    write_bin("&&");
                    sci();
                    return;
                case INC:
                    un_stmt("++");
                    sci();
                    return;
                case DEC:
                    un_stmt("--");
                    sci();
                    return;
                case KNEW:
                    un_stmt("new");
                    sci();
                    return;
                case KPRINT:
                    code_n("print ");
                    write_inline(op[0]);
                    sc();
                    return;
                case KRETURN:
                    code_n("return");
                    if (op[0])
                    {
                        code_n(" ");
                        write_inline(op[0]);
                    }
                    sc();
                    return;
                case KBREAK:
                    code_n("break");
                    if (arity == 1)
                    {
                        code_n(" ");
                        write_inline(op[0]);
                    }
                    sc();
                    return;
                case KVAR:
                    code_n("var ");
                    write_inline(op[0]);
                    switch (arity)
                    {
                        case 2:
                        {
                            code_n(": %s", type_display(AST_INFERRED(op[0])));
                            break;
                        }
                        case 3:
                        {
                            code_n("[%d]", NUMBER_VALUE(op[1]));
                            if (op[2])
                            {
                                code_n(" = \"%s\"", VAR_NAME(op[2]));
                            }
                            break;
                        }
                    }
                    if (!AST_CLEAN_STACK(n))
                        sc();
                    return;
                case KFOR:
                    code_n("for (");
                    write_inline(op[0]);
                    code_n("; ");
                    write_inline(op[1]);
                    code_n("; ");
                    write_inline(op[2]);
                    code_n(")\n");
                    expect_sub = true;
                    indent++;
                    write_code(op[3]);
                    indent--;
                    return;
                case KLOOP:
                    code_n("loop\n");
                    indent++;
                    if (AST_CLEAN_STACK(n))
                    {
                        expect_sub = true;
                    }
                    else
                    {
                        tab();
                    }
                    write_code(op[0]);
                    if (AST_CLEAN_STACK(n))
                    {
                        code_n("\n");
                    }
                    indent--;
                    return;
                case KTYPE:
                    code_n("type ");
                    write_inline(op[0]);
                    code_n(" = %s", type_display_full(AST_INFERRED(op[0]), true, false));
                    sc();
                    return;
                case ':':
                    write_inline(op[0]);
                    code_n(": %s", type_display(AST_INFERRED(op[0])));
                    return;
                case KFUNC:
                    code_n("func ");
                    write_inline(op[0]);
                    code_n("(");
                    for (linked_list* lst = AST_LIST_HEAD(op[1]); lst; lst = lst->next)
                    {
                        write_inline(lst->value);
                        if (lst->next)
                            code_n(", ");
                    }
                    code_n("): %s\n", type_display(AST_INFERRED(op[0])));
                    expect_sub = true;
                    indent++;
                    write_code(op[2]);
                    indent--;
                    return;
                case TUPLEASSIGN:
                    code_n("(");
                    for (linked_list* lst = AST_LIST_HEAD(op[0]); lst; lst = lst->next)
                    {
                        write_inline(lst->value);
                        if (lst->next)
                            code_n(", ");
                    }
                    code_n(") = (");
                    for (linked_list* lst = AST_LIST_HEAD(op[1]); lst; lst = lst->next)
                    {
                        write_inline(lst->value);
                        if (lst->next)
                            code_n(", ");
                    }
                    code_n(")");
                    sci();
                    return;
                case '(':
                    write_inline(op[0]);
                    code_n("(");
                    for (linked_list* lst = AST_LIST_HEAD(op[1]); lst; lst = lst->next)
                    {
                        write_inline(lst->value);
                        if (lst->next)
                            code_n(", ");
                    }
                    code_n(")");
                    sci();
                    return;
            }
            return;
        }
        case k_list:
            break;
    }
}