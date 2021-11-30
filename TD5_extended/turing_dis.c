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

const char* stringify_operator_or_null(enum yytokentype op)
{
    if (op < YYerror)
    {
        char* res = malloc(2);
        res[0] = (char) op;
        res[1] = 0;
        return res;
    }

    switch (op)
    {
        case GE:
            return ">=";
        case LE:
            return "<=";
        case EQ:
            return "==";
        case NE:
            return "!=";
        case REF:
            return "&";
        case DEREF:
            return "*";
        case APL:
            return "+=";
        case AMN:
            return "-=";
        case AML:
            return "*=";
        case ADV:
            return "/=";
        case INC:
            return "++";
        case DEC:
            return "--";
        case AND:
            return "&&";
        case OR:
            return "||";
        case SHL:
            return "<<";
        case SHR:
            return ">>";
        case ARROW:
            return "=>";
        case UMINUS:
            return "-";
        case KNEW:
            return "new";
        case KIS:
            return "is";
        default:
            return NULL;
    }
}

const char* stringify_operator(enum yytokentype op)
{
    const char* res = stringify_operator_or_null(op);
    if (!res)
    {
        error_msg(NULL, "stringify: unknown %d\n", op);
        exit(1);
    }
    return res;
}

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
            code_n("`%s`", VAR_NAME(n));
            return;
        case k_number:
            if (n->inferred_type && n->inferred_type->type == T_SCALAR)
                code_n("%du%d", NUMBER_VALUE(n), n->inferred_type->scalar_bits);
            else
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
                case '~':
                case DEREF:
                    write_un(stringify_operator(OPER_OPERATOR(n)));
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
                case GE:
                case LE:
                case EQ:
                case NE:
                case AND:
                case OR:
                case SHL:
                case SHR:
                case KIS:
                    write_bin(stringify_operator(OPER_OPERATOR(n)));
                    sci();
                    return;
                case '.':
                    write_inline(op[0]);
                    code_n(".");
                    write_inline(op[1]);
                    sci();
                    return;
                case INC:
                case DEC:
                case KNEW:
                    un_stmt(stringify_operator(OPER_OPERATOR(n)));
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
                            code_n(": %s", type_display(AST_DATA(op[0])));
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
                case KIMPL:
                    if (arity == 3)
                    {
                        code_n("impl %s for %s\n", VAR_NAME(op[2]), VAR_NAME(op[0]));
                    }
                    else
                    {
                        code_n("impl %s\n", VAR_NAME(op[0]));
                    }
                    tab();
                    code_n("{\n");
                    indent++;
                    for (linked_list* lst = AST_LIST_HEAD(op[1]); lst; lst = lst->next)
                    {
                        tab();
                        write_code(lst->value);
                    }
                    indent--;
                    tab();
                    code_n("}\n");
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
                    code_n("): %s\n", type_display(AST_DATA(op[0])));
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
                {
                    write_inline(op[0]);
                    code_n("(");
                    linked_list* lst = AST_LIST_HEAD(op[2]);
                    if (lst)
                    {
                        if (AST_KIND(op[0]) == k_operator && OPER_OPERATOR(op[0]) == '.')
                        {
                            lst = lst->next;
                        }
                        for (; lst; lst = lst->next)
                        {
                            write_inline(lst->value);
                            if (lst->next)
                                code_n(", ");
                        }
                    }
                    code_n(")");
                    sci();
                    return;
                }
                case '{':
                    code_n("{");
                    write_inline(op[0]);
                    write_inline(op[1]);
                    code_n("}");
                    sci();
                    return;
                case CAST:
                    code_n("cast!(");
                    write_inline(op[0]);
                    code_n(")(");
                    write_inline(op[1]);
                    code_n(")");
                    sci();
                    return;
                case KSCALAROF:
                    code_n("scalarof(");
                    write_inline(op[0]);
                    code_n(")");
                    sci();
                    return;
                case GENINST:
                {
                    write_inline(op[0]);
                    code_n("!(");
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
                default:
                    error_msg(n, "unhandled kind %d\n", OPER_OPERATOR(n));
                    exit(1);
            }
            return;
        }
        case k_list:
            break;
    }
}