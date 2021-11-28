//
// Created by Tom on 05/11/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "turing.h"

#define LABEL(n)     printf("#L%03d:\n", n);            // output a label

#define PROD0(op)     printf("#\t%s\n", op)
#define PROD1F(op, v)     printf("#\t%s\t%d\n", op, v)    // v is a float
#define PROD1S(op, v)     printf("#\t%s\t%s\n", op, v)    // v is a string
#define PROD1L(op, v)     printf("#\t%s\tL%03d\n", op, v) // v is a label

#define instr(format, ...) printf (format "\t\t# line %d\n", ## __VA_ARGS__, __LINE__)

#define BLOCK(name, code) { \
PROD0("------ BEGIN: " name); \
code;                       \
PROD0("------ END: " name); \
}

#define COMPARISON(code) { \
instr("FROM @%d", ++label);\
instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);\
instr("FROM @%d", label);\
instr("'[,'0|'1,'_,'_ '[,'_,'0|'1,'_ S,L,L,S");\
instr("'[,'/,'_,'_ S,S,R,S @%d", ++label);\
instr("FROM @%d", label);\
instr("'[,'/,'0|'1,'_ S,S,R,S");\
instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,L,S @%d", ++label);\
instr("FROM @%d", label);\
int left_then_one = ++label;\
int left_then_zero = ++label;\
int one = ++label;\
int zero = ++label;\
int end = ++label;\
instr("'[,'0|'1,'0|'1,'_ '[,'_,'_,'_ S,L,L,S");\
code \
instr("FROM @%d", zero);\
instr("'[,'_,'_,'_ '[,'0,'_,'_ S,R,S,S @%d", end);   \
instr("FROM @%d", left_then_zero);\
instr("'[,'0|'1,'0,'_ '[,'_,'_,'_ S,L,L,S");\
instr("'[,'0|'1,'1,'_ '[,'_,'_,'_ S,L,L,S");\
instr("'[,'/|'[,'_,'_ S,R,S,S @%d", zero);\
instr("FROM @%d", left_then_one);\
instr("'[,'0|'1,'0,'_ '[,'_,'_,'_ S,L,L,S");\
instr("'[,'0|'1,'1,'_ '[,'_,'_,'_ S,L,L,S");\
instr("'[,'/|'[,'_,'_ S,R,S,S @%d", one);\
instr("FROM @%d", one);\
instr("'[,'_,'_,'_ '[,'1,'_,'_ S,R,S,S @%d", end);   \
instr("FROM @%d", end);    \
instr("'[,'_,'_,'_ '[,'/,'_,'_ S,S,S,S @%d", label + 1); \
}

void allocate_var(var_list* ptr)
{
    if (ptr->type->is_const)
        return;
    instr("# allocate variable %s (position %d, size %d)", ptr->header.name, ptr->position,
          type_size_cells(ptr->type));
    switch (ptr->type->type)
    {
        case T_SCALAR:
        case T_POINTER:
        {
            for (int i = 0; i < type_size_bits(ptr->type); i++)
            {
                instr("FROM @%d", ++label);
                instr("'_,'_,'_,'_ '0,'_,'_,'_ R,S,S,S @%d", label + 1);
            }
            instr("FROM @%d", ++label);
            instr("'_,'_,'_,'_ '/,'_,'_,'_ R,S,S,S @%d", label + 1);
            break;
        }
        case T_ARRAY:
        {
            const char* str = ptr->initial;
            int cell_bits = type_size_bits(ptr->type->array_target);
            for (int j = 0; j < ptr->type->array_count; j++, (str && *str && str++))
            {
                if (str && *str)
                {
                    char value = *str;
                    for (int i = 0; i < cell_bits; i++, value >>= 1)
                    {
                        instr("FROM @%d", ++label);
                        instr("'_,'_,'_,'_ '%d,'_,'_,'_ R,S,S,S @%d", value & 1, label + 1);
                    }
                }
                else
                {
                    for (int i = 0; i < cell_bits; i++)
                    {
                        instr("FROM @%d", ++label);
                        instr("'_,'_,'_,'_ '0,'_,'_,'_ R,S,S,S @%d", label + 1);
                    }
                }
                instr("FROM @%d", ++label);
                instr("'_,'_,'_,'_ '/,'_,'_,'_ R,S,S,S @%d", label + 1);
            }
            break;
        }
        case T_COMPOSITE:
        {
            PROD0("composite type, allocating fields:");
            for (var_list* member = ptr->type->composite_members.head; member; member = (var_list*) member->header.next)
            {
                allocate_var(member);
            }
            break;
        }
        default:
        {
            error_msg(NULL, "Invalid type in allocate_var\n");
            exit(1);
        }
    }
}


/**
 * Allocates blank cells for the variables in the scope\n
 * This will take in account array initializers (strings)
 */
void allocate_scope(stack_frame* frame)
{
    if (!frame->size)
        return;

    printf("# Memory map\n");
    printf("# POSITION  SIZE  NAME        TYPE\n");
    for (var_list* ptr = frame->vars.head; ptr; ptr = (var_list*) ptr->header.next)
    {
        if (ptr->type->is_const)
            continue;
        printf("# %8d  %4d  %-10s  %-10s\n", ptr->position, type_size_cells(ptr->type), ptr->header.name,
               type_display(ptr->type));
    }

    instr("FROM @%d", ++label);
    instr("'/|'0|'1|'[,'[,'_,'_ R,S,S,S");
    instr("'/|'0|'1|'[,'/,'_,'_ R,S,S,S");
    instr("'_,'/|'[,'_,'_ S,R,S,S @%d", label + 1);
    for (var_list* ptr = frame->vars.head; ptr; ptr = (var_list*) ptr->header.next)
    {
        allocate_var(ptr);
    }
    instr("FROM @%d", ++label);
    instr("'_|'/|'0|'1,'_,'_,'_ L,S,S,S");
    instr("'[,'_,'_,'_ S,L,S,S @%d", label + 1);
}

/**
 * Frees the cells used by the variables in the scope
 */
void free_scope(stack_frame* frame)
{
    if (frame->size)
    {
        instr("FROM @%d", ++label);
        instr("'/|'0|'1|'[,'[,'_,'_ R,R,S,S");
        instr("'/|'0|'1|'[,'/,'_,'_ R,R,S,S");
        instr("'/|'0|'1|'[,'_,'_,'_ R,S,S,S");
        instr("'_,'_,'_,'_ L,S,S,S @%d", label + 1);
        for (int i = 0; i < frame->size - 1; i++)
        {
            instr("FROM @%d", ++label);
            instr("'/|'0|'1,'_,'_,'_ '_,'_,'_,'_ L,S,S,S @%d", ++label);
            instr("FROM @%d", label);
            instr("'0|'1,'_,'_,'_ '_,'_,'_,'_ L,S,S,S");
            instr("'/,'_,'_,'_ '_,'_,'_,'_ L,S,S,S @%d", label + 1);
        }
        instr("FROM @%d", ++label);
        instr("'/|'0|'1,'_,'_,'_ '_,'_,'_,'_ L,S,S,S @%d", ++label);
        instr("FROM @%d", label);
        instr("'0|'1,'_,'_,'_ '_,'_,'_,'_ L,S,S,S");
        instr("'/,'_,'_,'_ L,S,S,S @%d", label + 1);
        instr("'[,'_,'_,'_ S,L,S,S @%d", label + 2);
        instr("FROM @%d", ++label);
        instr("'/|'0|'1,'_,'_,'_ L,S,S,S");
        instr("'[,'_,'_,'_ S,L,S,S @%d", label + 1);
    }
}

/**
 * push(*pop());
 */
void deref()
{
    instr("# deref()");
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

void eval(ast_node* n, stack_frame* frame);

void nav_back_local()
{
    PROD0("nav back local");
    instr("FROM @%d", ++label);
    instr("'0|'1|'/,'/,'_,'_ L,S,S,S");
    instr("'0|'1|'/,'[,'_,'_ L,S,S,S");
    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
}

void nav_back_global()
{
    PROD0("nav back global");
    instr("FROM @%d", ++label);
    instr("'0|'1|'/|'[,'/,'_,'_ R,R,S,S");
    instr("'0|'1|'/|'[,'[,'_,'_ R,R,S,S");
    instr("'0|'1|'/|'[,'_,'_,'_ R,S,S,S");
    instr("'#,'_,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", label + 1);
}

void setup_global_nav()
{
    PROD0("setup nav global");
    instr("FROM @%d", ++label);
    instr("'[,'/,'_,'_ '#,'/,'_,'_ L,S,S,S @%d", label + 1);
    instr("FROM @%d", ++label);
    instr("'/|'0|'1|'[,'/,'_,'_ L,S,S,S");
    instr("'_,'/,'_,'_ R,S,S,S @%d", label + 1);
}

/**
 * Moves the heap pointer to the specified variable
 * @throws exit If the given node is not an lvalue (variable or dereference)
 */
void (* nav_to_var(ast_node* op, stack_frame* frame))()
{
    if (AST_KIND(op) != k_operator || OPER_OPERATOR(op) != DEREF)
    {
        error_msg(op, "Expected lvalue for assignment\n");
        exit(1);
    }

    ast_node* pos = OPER_OPERANDS(op)[0];

    if (AST_KIND(pos) == k_number)
    {
        int vid = NUMBER_VALUE(pos);
        instr("# navigating to %d", vid);
        if (pos->inferred_type->pointer_is_global && frame->function)
            setup_global_nav();
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
    else
    {
        PROD0("navigating to dynamic pointer");
        eval(pos, frame);
        if (pos->inferred_type->pointer_is_global && frame->function)
            setup_global_nav();
        deref();
    }

    return pos->inferred_type->pointer_is_global && frame->function ? &nav_back_global : &nav_back_local;
}

/**
 * Removes n values from the top of the stack
 */
void pop(int n)
{
    if (!n)
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

/**
 * Pushes a number to the stack
 */
void push_number(int value, int size)
{
    value &= (1L << size) - 1;
    instr("FROM @%d", ++(label));
    instr("'/|'[,'[,'_,'_ S,R,S,S @%d", ++label);
    instr("'/|'[,'/,'_,'_ S,R,S,S @%d", label);
    instr("FROM @%d", label);
    for (int i = 0; i < size; i++, value /= 2)
    {
        instr("'/|'[,'_,'_,'_ '/|'[,'%d,'_,'_ S,R,S,S @%d", value & 1, ++label);
        instr("FROM @%d", label);
    }
    instr("'/|'[,'_,'_,'_ '/|'[,'/,'_,'_ S,S,S,S @%d", label + 1);
}

void exec(ast_node* n, stack_frame* frame);

void eval(ast_node* n, stack_frame* frame)
{
    if (!n)
    {
        fprintf(stderr, "Null node eval\n");
        abort();
    }

    exec(n, frame);
}

#define USELESS() do{info_msg(n, "Line has no effect\n");return;}while(0)

void write_char(int bit, int state, int end_state, int number)
{
    instr("# write_char %d", number);
    instr("FROM @%d", state);

    if (bit > 255)
    {
        instr("'[,'0|'1,'_,'_ S,R,S,S");
        instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
        instr("FROM @%d", label);
        instr("'[,'_|'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
        instr("'[,'/|'[,'_,'_ '[,'/|'[,'_,'%c S,S,S,R @%d", (isalnum(number)) ? number : '?', ++label);
        instr("FROM @%d", label);
        instr("'[,'/|'[,'_,'_ '[,'/|'[,'_,'. S,S,S,R @%d", end_state);
    }
    else
    {
        int bit_0 = ++label;
        int bit_1 = ++label;

        instr("'[,'0,'_,'_ S,R,S,S @%d", bit_0);
        instr("'[,'1,'_,'_ S,R,S,S @%d", bit_1);

        write_char(bit << 1, bit_0, end_state, number);
        write_char(bit << 1, bit_1, end_state, number | bit);
    }
}

void write_string(int bit, int state, int start_state, int end_state, int number)
{
    instr("# write_string %d", number);
    instr("FROM @%d", state);

    if (bit > 255)
    {
        if (number == 0)
        {
            instr("'1|'0|'/,'_,'_,'_ '1|'0|'/,'_,'_,'. L,S,S,R @%d", end_state);
        }
        else
        {
            instr("'1|'0,'_,'_,'_ R,S,S,S");
            instr("'/,'_,'_,'_ '/,'_,'_,'%c R,S,S,R @%d", (isalnum(number)) ? number : '?', start_state);
        }

    }
    else
    {
        int bit_0 = ++label;
        int bit_1 = ++label;

        instr("'0,'_,'_,'_ R,S,S,S @%d", bit_0);
        instr("'1,'_,'_,'_ R,S,S,S @%d", bit_1);

        write_string(bit << 1, bit_0, start_state, end_state, number);
        write_string(bit << 1, bit_1, start_state, end_state, number | bit);
    }
}

/**
 * Emits the code for the specified node.
 */
void exec(ast_node* n, stack_frame* frame)
{
    if (!n)
    {
        PROD0("empty node");
        return;
    }

    PROD0("** begin");
    //write_code(n);
    PROD0("** end");

    instr("# KIND = %d (%s)", AST_KIND(n), node_kind_NAMES[AST_KIND(n)]);

    bool clean_stack = AST_CLEAN_STACK(n); // whether this node is expected to "leave" something on the stack or not

    switch (AST_KIND(n))
    {
        case k_number:
        {
            PROD1F("push", NUMBER_VALUE(n));
            if (clean_stack)
                USELESS();
            push_number(NUMBER_VALUE(n), type_size_bits(AST_INFERRED(n)));
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
                    PROD0("negate");
                    eval(op[0], frame);
                    if (clean_stack)
                    {
                        pop(1);
                        USELESS();
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
                case '~':
                {
                    PROD0("complement");
                    eval(op[0], frame);
                    if (clean_stack)
                    {
                        pop(1);
                        USELESS();
                    }
                    instr("FROM @%d", ++label);
                    instr("'[,'/,'_,'_ '[,'/,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ '[,'1|'0,'_,'_ S,R,S,S");
                    instr("'[,'/,'_,'_ S,S,S,S @%d", label + 1);
                    return;
                }
                case DEREF:
                {
                    PROD0("deref");
                    if (clean_stack)
                        USELESS();
                    void (*back)() = nav_to_var(n, frame);
                    instr("FROM @%d", ++label);
                    instr("'/|'[,'/,'_,'_ R,R,S,S @%d", ++label);
                    instr("'/|'[,'[,'_,'_ R,R,S,S @%d", label);
                    int size = type_size_cells(n->inferred_type);
                    for (int i = 1; i < size; i++)
                    {
                        instr("FROM @%d", label);
                        instr("'0|'1,'_,'_,'_ '0|'1,'0|'1,'_,'_ R,R,S,S");
                        instr("'/,'_,'_,'_ '/,'/,'_,'_ R,R,S,S @%d", ++label);
                    }
                    instr("FROM @%d", label);
                    instr("'0|'1,'_,'_,'_ '0|'1,'0|'1,'_,'_ R,R,S,S");
                    instr("'/,'_,'_,'_ '/,'/,'_,'_ L,S,S,S @%d", label + 1);
                    for (int i = 1; i < size; i++)
                    {
                        instr("FROM @%d", ++label);
                        instr("'0|'1,'/,'_,'_ L,S,S,S");
                        instr("'/,'/,'_,'_ L,S,S,S @%d", label + 1);
                    }
                    back();
                    return;
                }
                case SHL:
                {
                    PROD0("shl");
                    eval(op[0], frame);
                    eval(op[1], frame);
                    if (clean_stack)
                    {
                        pop(2);
                        USELESS();
                    }
                    instr("FROM @%d", ++label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ '[,'_,'0|'1,'_ S,L,L,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ '[,'_,'_,'0|'1 S,L,S,L");
                    instr("'[,'/|'[,'_,'_ S,R,R,S @%d", ++label);
                    int no_carry = label;
                    int carry = ++label;
                    int copy = ++label;
                    instr("FROM @%d", no_carry);
                    instr("'[,'_,'1,'_ '[,'0,'0,'_ S,R,S,S");
                    instr("'[,'_,'0,'_ '[,'_,'1,'_ S,S,R,S @%d", carry);
                    instr("FROM @%d", carry);
                    instr("'[,'_,'1,'_ '[,'0,'0,'_ S,R,L,S @%d", ++label);
                    instr("'[,'_,'0,'_ '[,'_,'1,'_ S,S,R,S");
                    instr("'[,'_,'_,'_ S,S,L,R @%d", copy);
                    instr("FROM @%d", label);
                    instr("'[,'_,'0|'1,'_ S,S,L,S");
                    instr("'[,'_,'_,'_ S,S,R,S @%d", no_carry);
                    instr("FROM @%d", copy);
                    instr("'[,'_,'1,'0|'1 '[,'0|'1,'_,'_ S,R,L,R");
                    instr("'[,'_,'_,'0|'1 '[,'0|'1,'_,'_ S,R,S,R");
                    instr("'[,'/,'1,'0|'1 '[,'/,'_,'_ S,S,L,R");
                    instr("'[,'/,'_,'0|'1 '[,'/,'_,'_ S,S,S,R");
                    instr("'[,'/,'_,'_ S,S,S,S @%d", label + 1);
                    return;
                }
                case SHR:
                {
                    PROD0("shr");
                    eval(op[0], frame);
                    eval(op[1], frame);
                    if (clean_stack)
                    {
                        pop(2);
                        USELESS();
                    }
                    instr("FROM @%d", ++label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ '[,'_,'0|'1,'_ S,L,L,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ '[,'_,'_,'0|'1 S,L,S,L");
                    instr("'[,'/|'[,'_,'_ S,R,R,R @%d", ++label);
                    int no_carry = label;
                    int carry = ++label;
                    int copy = ++label;
                    instr("FROM @%d", no_carry);
                    instr("'[,'_,'1,'0|'1 '[,'_,'0,'_ S,S,S,R");
                    instr("'[,'_,'0,'0|'1 '[,'_,'1,'0|'1 S,S,R,S @%d", carry);
                    instr("FROM @%d", carry);
                    instr("'[,'_,'1,'0|'1 '[,'_,'0,'_ S,S,L,S @%d", ++label);
                    instr("'[,'_,'0,'0|'1 '[,'_,'1,'0|'1 S,S,R,S");
                    instr("'[,'_,'_,'0|'1 '[,'0|'1,'_,'_ S,R,L,R @%d", copy);
                    instr("FROM @%d", label);
                    instr("'[,'_,'0|'1,'_ S,S,L,S");
                    instr("'[,'_,'_,'_ S,S,R,R @%d", no_carry);
                    instr("FROM @%d", copy);
                    instr("'[,'_,'1,'0|'1 '[,'0|'1,'_,'_ S,R,L,R");
                    instr("'[,'_,'_,'0|'1 '[,'0|'1,'_,'_ S,R,S,R");
                    instr("'[,'_,'1,'_ '[,'0,'_,'_ S,R,L,S");
                    instr("'[,'_,'_,'_ '[,'0,'_,'_ S,R,S,S");
                    instr("'[,'/,'1,'_ '[,'/,'_,'_ S,S,L,S @%d", label + 1);
                    instr("'[,'/,'_,'_ S,S,S,S @%d", label + 1);
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
                        USELESS();
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
                        USELESS();
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
                        USELESS();
                    }
                    instr("FROM @%d", ++label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,R @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ '[,'_,'0|'1,'_ S,L,L,S");
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ '[,'0,'_,'0|'1 S,L,S,R");
                    instr("'[,'/|'[,'_,'_ S,R,R,L @%d", ++label);
                    int loop = label;
                    int no_carry = ++label;
                    int carry = ++label;
                    int next = ++label;
                    int end = label + 1;
                    instr("FROM @%d", loop);
                    instr("'[,'0,'1|'0,'0 '[,'0,'1|'0,'# S,R,S,L");
                    instr("'[,'1,'1|'0,'0 '[,'1,'1|'0,'# S,R,S,L");
                    instr("'[,'0,'1|'0,'1 '[,'0,'1|'0,'# S,S,S,S @%d", no_carry);
                    instr("'[,'1,'1|'0,'1 '[,'1,'1|'0,'# S,S,S,S @%d", no_carry);
                    instr("'[,'0,'1|'0,'# S,R,S,L");
                    instr("'[,'1,'1|'0,'# S,R,S,L");
                    instr("'[,'/,'0|'1,'_ '[,'/,'_,'_ S,S,S,R");
                    instr("'[,'/,'0|'1,'# '[,'/,'_,'_ S,S,R,R");
                    instr("'[,'/,'_,'# '[,'/,'_,'_ S,S,R,R");
                    instr("'[,'/,'_,'_ S,S,S,L");
                    instr("'[,'/,'_,'. S,S,L,R @%d", end);
                    instr("FROM @%d", no_carry);
                    instr("'[,'0,'0,'# '[,'0,'0,'# S,R,R,S");
                    instr("'[,'0|'1,'1|'0,'# '[,'1,'1|'0,'# S,R,R,S");
                    instr("'[,'1,'1,'# '[,'0,'1,'# S,R,R,S @%d", carry);
                    instr("'[,'/,'0|'1|'_,'# S,L,L,R @%d", next);
                    instr("FROM @%d", carry);
                    instr("'[,'0,'0,'# '[,'1,'0,'# S,R,R,S @%d", no_carry);
                    instr("'[,'0|'1,'1|'0,'# '[,'0,'1|'0,'# S,R,R,S");
                    instr("'[,'1,'1,'# '[,'1,'1,'# S,R,R,S");
                    instr("'[,'/,'0|'1,'# S,L,L,R @%d", next);
                    instr("FROM @%d", next);
                    instr("'[,'0|'1,'0,'# S,L,L,R");
                    instr("'[,'0|'1,'1,'# S,L,L,R");
                    instr("'[,'0|'1,'0,'_ S,L,L,S");
                    instr("'[,'0|'1,'1,'_ S,L,L,S");
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'0|'1,'_,'# S,L,S,R");
                    instr("'[,'/|'[,'_,'_ S,R,R,L @%d", loop);
                    return;
                }
                case '/':
                    PROD0("div");
                    eval(op[0], frame);
                    eval(op[1], frame);
                    if (clean_stack)
                    {
                        pop(2);
                        USELESS();
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
                        USELESS();
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
                        USELESS();
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
                        USELESS();
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
                        USELESS();
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
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label); // equal
                    instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'_,'_,'_ '[,'0,'_,'_ S,R,S,S @%d", end);
                    instr("FROM @%d", not_equal);
                    instr("'[,'0|'1,'0,'_ '[,'0|'1,'_,'_ S,R,R,S");
                    instr("'[,'0|'1,'1,'_ '[,'0|'1,'_,'_ S,R,R,S");
                    instr("'[,'/,'0|'1,'_ '[,'/,'_,'_ S,S,R,S");
                    instr("'[,'0|'1,'_,'_ S,R,S,S");
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'_,'_,'_ '[,'1,'_,'_ S,R,S,S @%d", end);
                    instr("FROM @%d", end);
                    instr("'[,'_,'_,'_ '[,'/,'_,'_ S,S,S,S @%d", label + 1);
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
                        USELESS();
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
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label); // equal
                    instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'_,'_,'_ '[,'1,'_,'_ S,R,S,S @%d", end);
                    instr("FROM @%d", not_equal);
                    instr("'[,'0|'1,'0,'_ '[,'0|'1,'_,'_ S,R,R,S");
                    instr("'[,'0|'1,'1,'_ '[,'0|'1,'_,'_ S,R,R,S");
                    instr("'[,'/,'0|'1,'_ '[,'/,'_,'_ S,S,R,S");
                    instr("'[,'0|'1,'_,'_ S,R,S,S");
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    instr("'[,'_,'_,'_ '[,'0,'_,'_ S,R,S,S @%d", end);
                    instr("FROM @%d", end);
                    instr("'[,'_,'_,'_ '[,'/,'_,'_ S,S,S,S @%d", label + 1);
                    return;
                }
                case AND:
                {
                    PROD0("and");
                    eval(op[0], frame);

                    instr("FROM @%d", ++label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    int b_nonzero = ++label;
                    int write_zero = ++label;
                    int a_nonzero = ++label;
                    int write_one = ++label;
                    int end = ++label;
                    int end2 = ++label;
                    instr("'[,'0,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'1,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", b_nonzero);
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", clean_stack ? end2 : write_zero);
                    instr("FROM @%d", b_nonzero);
                    instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
                    eval(op[1], frame);
                    instr("FROM @%d", ++label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
                    int next = label + 1;
                    instr("FROM @%d", label);
                    instr("'[,'0,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'1,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", a_nonzero);
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", clean_stack ? next : write_zero);
                    instr("FROM @%d", write_zero);
                    instr("'[,'_,'_,'_ '[,'0,'_,'_ S,R,S,S @%d", end);
                    instr("FROM @%d", a_nonzero);
                    instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", clean_stack ? next : write_one);
                    instr("FROM @%d", write_one);
                    instr("'[,'_,'_,'_ '[,'1,'_,'_ S,R,S,S @%d", end);
                    instr("FROM @%d", end);
                    instr("'[,'_,'_,'_ '[,'/,'_,'_ S,S,S,S @%d", label + 1);
                    instr("FROM @%d", end2);
                    instr("'[,'_,'_,'_ S,L,S,S @%d", label + 1);

                    return;
                }
                case OR:
                {
                    PROD0("or");
                    eval(op[0], frame);
                    instr("FROM @%d", ++label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label);
                    int check_a = ++label;
                    int b_nonzero = ++label;
                    int a_nonzero = ++label;
                    int write_zero = ++label;
                    int scroll_left = ++label;
                    int write_one = ++label;
                    int end = ++label;
                    int end2 = ++label;
                    instr("'[,'0,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'1,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", b_nonzero);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
                    eval(op[1], frame);
                    instr("FROM @%d", ++label);
                    instr("'[,'/,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", check_a);
                    int next = label + 1;
                    instr("FROM @%d", b_nonzero);
                    instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", clean_stack ? next : write_one);
                    instr("FROM @%d", scroll_left);
                    instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", clean_stack ? next : write_one);
                    instr("FROM @%d", check_a);
                    instr("'[,'0,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'1,'_,'_ '[,'_,'_,'_ S,L,S,S @%d", a_nonzero);
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", clean_stack ? next : write_zero);
                    instr("FROM @%d", a_nonzero);
                    instr("'[,'0|'1,'_,'_ '[,'_,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", clean_stack ? next : write_one);
                    instr("FROM @%d", write_one);
                    instr("'[,'_,'_,'_ '[,'1,'_,'_ S,R,S,S @%d", end);
                    instr("FROM @%d", write_zero);
                    instr("'[,'_,'_,'_ '[,'0,'_,'_ S,R,S,S @%d", end);
                    instr("FROM @%d", end);
                    instr("'[,'_,'_,'_ '[,'/,'_,'_ S,S,S,S @%d", label + 1);

                    return;
                }
                case INC:
                {
                    PROD0("inc");
                    void (* back)() = nav_to_var(op[0], frame);
                    instr("FROM @%d", ++label);
                    instr("'/|'[,'[,'_,'_ R,R,S,S @%d", ++label);
                    instr("'/|'[,'/,'_,'_ R,R,S,S @%d", label);
                    if (clean_stack) // statement
                    {
                        instr("FROM @%d", label);
                        instr("'0,'_,'_,'_ '1,'_,'_,'_ L,L,S,S @%d", label + 1);
                        instr("'1,'_,'_,'_ '0,'_,'_,'_ R,S,S,S");
                        instr("'/,'_,'_,'_ S,L,S,S @%d", label + 1);
                        back();
                    }
                    else // expression
                    {
                        instr("FROM @%d", label);
                        int end = ++label;
                        if (arity == 1) // ++x
                        {
                            instr("'0,'_,'_,'_ '1,'1,'_,'_ R,R,S,S @%d", end);
                            instr("'1,'_,'_,'_ '0,'0,'_,'_ R,R,S,S");
                            instr("'/,'_,'_,'_ S,S,S,S @%d", end);
                        }
                        else // x++
                        {
                            instr("'0,'_,'_,'_ '1,'0,'_,'_ R,R,S,S @%d", end);
                            instr("'1,'_,'_,'_ '0,'1,'_,'_ R,R,S,S");
                            instr("'/,'_,'_,'_ S,S,S,S @%d", end);
                        }
                        instr("FROM @%d", end);
                        instr("'0|'1,'_,'_,'_ '0|'1,'0|'1,'_,'_ R,R,S,S");
                        instr("'/,'_,'_,'_ '/,'/,'_,'_ L,S,S,S @%d", label + 1);

                        back();
                    }
                    return;
                }
                case DEC:
                {
                    PROD0("dec");
                    void (* back)() = nav_to_var(op[0], frame);
                    instr("FROM @%d", ++label);
                    instr("'/|'[,'[,'_,'_ R,R,S,S @%d", ++label);
                    instr("'/|'[,'/,'_,'_ R,R,S,S @%d", label);
                    if (clean_stack) // statement
                    {
                        instr("FROM @%d", label);
                        instr("'1,'_,'_,'_ '0,'_,'_,'_ L,L,S,S @%d", label + 1);
                        instr("'0,'_,'_,'_ '1,'_,'_,'_ R,S,S,S");
                        instr("'/,'_,'_,'_ S,L,S,S @%d", label + 1);
                        back();
                    }
                    else // expression
                    {
                        instr("FROM @%d", label);
                        int end = ++label;
                        if (arity == 1) // --x
                        {
                            instr("'1,'_,'_,'_ '0,'0,'_,'_ R,R,S,S @%d", end);
                            instr("'0,'_,'_,'_ '1,'1,'_,'_ R,R,S,S");
                            instr("'/,'_,'_,'_ S,S,S,S @%d", end);
                        }
                        else // x--
                        {
                            instr("'1,'_,'_,'_ '0,'1,'_,'_ R,R,S,S @%d", end);
                            instr("'0,'_,'_,'_ '1,'0,'_,'_ R,R,S,S");
                            instr("'/,'_,'_,'_ S,S,S,S @%d", end);
                        }
                        instr("FROM @%d", end);
                        instr("'0|'1,'_,'_,'_ '0|'1,'0|'1,'_,'_ R,R,S,S");
                        instr("'/,'_,'_,'_ '/,'/,'_,'_ L,S,S,S @%d", label + 1);

                        back();
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
                        exec(op[0], frame);
                        PROD0("; second");
                        exec(op[1], frame);
                        return;
                    }
                case KPRINT:
                    PROD0("print");
                    eval(op[0], frame);
                    return;
                case KBREAK:
                    PROD0("break");
                    if (arity == 1 && !frame->loop->node->clean_stack)
                    {
                        eval(op[0], frame);
                    }
                    instr("FROM @%d", ++label);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d_break", frame->loop->address);
                    return;
                case KCONTINUE:
                    PROD0("continue");
                    instr("FROM @%d", ++label);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d_continue", frame->loop->address);
                    return;
                case KLOOP:
                {
                    PROD0("loop");
                    int start = label + 1;
                    SC_SCOPE(op[0])->loop->address = start;
                    exec(op[0], frame);
                    instr("FROM @%d", ++label);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", start);
                    instr("FROM @%d_break", start);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
                    return;
                }
                case KDO:
                {
                    PROD0("do");
                    int start = label + 1;
                    SC_SCOPE(op[0])->loop->address = start;
                    exec(op[0], frame);
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
                    exec(op[0], frame);
                    int start = ++label;
                    SC_SCOPE(op[3])->loop->address = start;
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
                    exec(op[3], frame);
                    instr("FROM @%d_continue", start);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
                    if (op[2])
                    {
                        PROD0("evaluating 'for' increment");
                        exec(op[2], frame);
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
                    exec(op[1], frame);

                    instr("FROM @%d", ++label); // after is_non_zero
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", end);

                    if (op[2])
                    {
                        PROD0("executing negative branch");
                        instr("FROM @%d", is_zero);
                        int zero_id = label + 1;
                        instr("'[,'/|'[,'_,'_ S,S,S,S @%d", zero_id);

                        exec(op[2], frame);

                        instr("FROM @%d", ++label);
                        instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
                    }
                    instr("FROM @%d", end);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
                    return;
                }
                case TUPLEASSIGN:
                {
                    PROD0("tuple assign");
                    linked_list* left = ((ast_linked_list*) op[0])->list;
                    linked_list* right = ((ast_linked_list*) op[1])->list;

                    PROD0("evaluating right-hand values");
                    int size = 0;
                    for (; right; right = right->next)
                    {
                        eval(right->value, frame);
                        size += type_size_cells(right->value->inferred_type);
                    }

                    PROD0("navigating to left of stack head");
                    instr("FROM @%d", ++label);
                    instr("'[,'/,'_,'_ S,L,S,S @%d", label + 1);
                    for (int i = 1; i < size; i++)
                    {
                        instr("FROM @%d", ++label);
                        instr("'[,'0|'1,'_,'_ S,L,S,S");
                        instr("'[,'/|'[,'_,'_ S,L,S,S @%d", label + 1);
                    }
                    instr("FROM @%d", ++label);
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);

                    PROD0("assigning values");
                    for (; left; left = left->next)
                    {
                        void (*back)() = nav_to_var(left->value, frame);
                        instr("FROM @%d", ++label);
                        instr("'/|'[,'/,'_,'_ R,R,S,S @%d", label + 1);
                        instr("'/|'[,'[,'_,'_ R,R,S,S @%d", label + 1);
                        int csize = type_size_cells(left->value->inferred_type);
                        for (int i = 1; i < csize; i++)
                        {
                            instr("FROM @%d", ++label);
                            instr("'0|'1,'_,'_,'_ S,R,S,S");
                            instr("'0|'1,'0,'_,'_ '0,'_,'_,'_ R,R,S,S");
                            instr("'0|'1,'1,'_,'_ '1,'_,'_,'_ R,R,S,S");
                            instr("'0|'1,'/,'_,'_ '0,'/,'_,'_ R,S,S,S");
                            instr("'/,'/,'_,'_ '/,'_,'_,'_ R,R,S,S @%d", label + 1);
                        }
                        instr("FROM @%d", ++label);
                        instr("'0|'1,'_,'_,'_ S,R,S,S");
                        instr("'0|'1,'0,'_,'_ '0,'_,'_,'_ R,R,S,S");
                        instr("'0|'1,'1,'_,'_ '1,'_,'_,'_ R,R,S,S");
                        instr("'0|'1,'/,'_,'_ '0,'/,'_,'_ R,S,S,S");
                        instr("'/,'/,'_,'_ '/,'_,'_,'_ L,L,S,S @%d", ++label);
                        instr("FROM @%d", label);
                        instr("'/|'0|'1|'[,'_,'_,'_ L,L,S,S");
                        instr("'/|'0|'1|'[,'/,'_,'_ S,S,S,S @%d", label + 1);
                        instr("'/|'0|'1|'[,'[,'_,'_ S,S,S,S @%d", label + 1);

                        back();
                    }
                    return;
                }
                case '=':
                {
                    PROD0("store");
                    PROD0("evaluating right-hand value");
                    int size = type_size_cells(op[1]->inferred_type);
                    eval(op[1], frame);
                    instr("FROM @%d", ++label);
                    instr("'0|'1|'/,'/,'_,'_ L,S,S,S");
                    instr("'[,'/,'_,'_ S,S,S,S @%d", label + 1);
                    void (*back)() = nav_to_var(op[0], frame);
                    PROD0("navigating to left of stack head");
                    instr("FROM @%d", ++label);
                    instr("'/|'[,'/,'_,'_ S,L,S,S @%d", ++label);
                    for (int i = 1; i < size; i++)
                    {
                        instr("FROM @%d", label);
                        instr("'/,'0|'1,'_,'_ S,L,S,S");
                        instr("'[,'0|'1,'_,'_ S,L,S,S");
                        instr("'/,'/|'[,'_,'_ S,L,S,S @%d", ++label);
                        instr("'[,'/|'[,'_,'_ S,L,S,S @%d", label);
                    }
                    instr("FROM @%d", label);
                    instr("'/,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'/,'/|'[,'_,'_ R,R,S,S @%d", ++label);
                    instr("'[,'/|'[,'_,'_ R,R,S,S @%d", label);
                    if (clean_stack) // statement, clear from stack
                    {
                        for (int i = 1; i < size; i++)
                        {
                            instr("FROM @%d", label);
                            instr("'0|'1,'0,'_,'_ '0,'_,'_,'_ R,R,S,S");
                            instr("'0|'1,'1,'_,'_ '1,'_,'_,'_ R,R,S,S");
                            instr("'0|'1,'/,'_,'_ '0,'/,'_,'_ R,S,S,S");
                            instr("'/,'/,'_,'_ '/,'_,'_,'_ R,R,S,S @%d", ++label);
                        }
                        instr("FROM @%d", label);
                        instr("'0|'1,'0,'_,'_ '0,'_,'_,'_ R,R,S,S");
                        instr("'0|'1,'1,'_,'_ '1,'_,'_,'_ R,R,S,S");
                        instr("'0|'1,'/,'_,'_ '0,'/,'_,'_ R,S,S,S");
                        instr("'/,'/,'_,'_ '/,'_,'_,'_ L,L,S,S @%d", ++label);
                        instr("FROM @%d", label);
                        instr("'/|'0|'1|'[,'_,'_,'_ L,L,S,S");
                        instr("'/|'0|'1|'[,'/,'_,'_ S,S,S,S @%d", label + 1);
                        instr("'/|'0|'1|'[,'[,'_,'_ S,S,S,S @%d", label + 1);

                        back();
                    }
                    else // expression, keep in stack
                    {
                        for (int i = 1; i < size; i++)
                        {
                            instr("FROM @%d", label);
                            instr("'0|'1,'0,'_,'_ '0,'0,'_,'_ R,R,S,S");
                            instr("'0|'1,'1,'_,'_ '1,'1,'_,'_ R,R,S,S");
                            instr("'0|'1,'/,'_,'_ '0,'/,'_,'_ R,S,S,S");
                            instr("'/,'/,'_,'_ R,R,S,S @%d", ++label);
                        }
                        instr("FROM @%d", label);
                        instr("'0|'1,'0,'_,'_ '0,'0,'_,'_ R,R,S,S");
                        instr("'0|'1,'1,'_,'_ '1,'1,'_,'_ R,R,S,S");
                        instr("'0|'1,'/,'_,'_ '0,'/,'_,'_ R,S,S,S");
                        instr("'/,'/,'_,'_ L,S,S,S @%d", label + 1);

                        back();
                    }
                    return;
                }
                case KVAR:
                    PROD0("var");
                    return;
                case KCONST:
                    PROD0("const");
                    return;
                case KFUNC:
                    PROD0("func");
                    return;
                case KTYPE:
                    PROD0("type");
                    return;
                case KASSERT:
                    PROD0("assert");
                    return;
                case KIMPL:
                    PROD0("impl");
                    return;
                case KRETURN:
                {
                    PROD0("return");
                    eval(op[0], frame);
                    instr("FROM @%d", ++label);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @F%dret", frame->function->header.id);
                    return;
                }
                case '(':
                {
                    func_data* call_site = AST_DATA(n);
                    func_list* fct = call_site->function;

                    instr("# call %s", fct->header.name);
                    linked_list* args = AST_LIST_HEAD(op[2]);

                    if (fct->kind == F_BUILTIN)
                    {
                        if (!strcmp(fct->header.name, "putc"))
                        {
                            eval(args->value, frame);
                            instr("FROM @%d", ++label);
                            instr("'[,'/,'_,'_ S,L,S,L @%d", ++label);
                            instr("FROM @%d", label);
                            instr("'[,'0|'1,'_,'. S,L,S,S");
                            instr("'[,'/|'[,'_,'. '[,'/|'[,'_,'_ S,R,S,S @%d", label + 2);
                            int end_state = ++label;
                            write_char(1, ++label, end_state, 0);
                            instr("FROM @%d", end_state);
                            instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
                        }
                        else if (!strcmp(fct->header.name, "puts"))
                        {
                            void (*back)() = nav_to_var(make_node(DEREF, 1, args->value), frame);
                            instr("FROM @%d", ++label);
                            instr("'[,'/|'[,'_,'_ S,R,S,L @%d", ++label);
                            instr("'/,'/|'[,'_,'_ S,R,S,L @%d", label);
                            instr("FROM @%d", label);
                            instr("'/|'[,'_,'_,'. '/|'[,'_,'_,'_ R,S,S,S @%d", label + 2);
                            int end_state = ++label;
                            int start_state = ++label;
                            write_string(1, start_state, start_state, end_state, 0);
                            instr("FROM @%d", end_state);
                            instr("'1|'0|'/,'_,'_,'_ L,L,S,S @%d", label + 1);
                            back();
                        }
                        else
                        {
                            error_msg(n, "Builtin function '%s' not implemented\n", fct->header.name);
                            exit(1);
                        }
                        return;
                    }

                    int right_heap;
                    if (args)
                    {
                        int argcount = 0;
                        int argcells = 0;
                        for (linked_list* arg = args; arg; argcount++, argcells += type_size_cells(
                                arg->value->inferred_type), arg = arg->next)
                        {
                            eval(arg->value, frame);
                        }
                        int start_left = ++label;
                        right_heap = ++label;
                        PROD0("go to left of args on stack");
                        instr("FROM @%d", start_left);
                        instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                        instr("'[,'[,'_,'_ S,S,S,S @%d", right_heap);
                        for (int i = 1; i < argcells; i++)
                        {
                            instr("FROM @%d", label);
                            instr("'[,'0|'1,'_,'_ S,L,S,S");
                            instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                        }
                        instr("FROM @%d", label);
                        instr("'[,'0|'1,'_,'_ S,L,S,S");
                        instr("'[,'/|'[,'_,'_ S,S,S,S @%d", right_heap);
                    }
                    else
                    {
                        right_heap = ++label;
                    }
                    PROD0("go to right of heap");
                    instr("FROM @%d", right_heap);
                    instr("'/|'0|'1|'[,'/,'_,'_ R,S,S,S");
                    instr("'/|'0|'1|'[,'[,'_,'_ R,S,S,S");
                    call_site_list* call = call_site->site;
                    instr("'_,'/|'[,'_,'_ '[,'/|'[,'_,'_ R,S,S,S @F%dC%d", fct->header.id, call->id);
                    call->argalloc_address = label + 1;
                    if (args)
                    {
                        PROD0("start copying args from stack to heap");
                        instr("FROM @%d", ++label);
                        instr("'_,'/|'[,'_,'_ '[,'/|'[,'_,'_ R,R,S,S @%d", ++label);
                        instr("FROM @%d", label);
                        instr("'_,'0|'1,'_,'_ '0|'1,'_,'_,'_ R,R,S,S");
                        instr("'_,'/,'_,'_ '/,'_,'_,'_ R,R,S,S");
                        instr("'_,'_,'_,'_ L,L,S,S @%d", label + 1);
                    }
                    instr("FROM @%d", ++label);
                    instr("'/|'0|'1,'_,'_,'_ L,L,S,S");
                    instr("'/|'0|'1,'/,'_,'_ L,S,S,S");
                    instr("'/|'0|'1,'[,'_,'_ L,S,S,S");
                    instr("'[,'/|'[,'_,'_ S,S,S,S @F%d", fct->header.id);
                    instr("'_,'/|'[,'_,'_ '[,'/|'[,'_,'_ S,S,S,S @F%d", fct->header.id);
                    call->return_address = label + 1;
                    if (clean_stack && fct->return_type->type != T_VOID)
                    {
                        pop(type_size_cells(fct->return_type));
                    }

                    return;
                }
                case '{':
                {
                    PROD0("; compound expression statement");
                    exec(op[0], frame);
                    PROD0("; compound expression result");
                    exec(op[1], frame);
                    return;
                }
                case KNEW:
                {
                    PROD0("new");
                    if (clean_stack)
                        return;

                    instr("FROM @%d", ++label);
                    instr("'[,'/|'[,'_,'_ S,R,S,S @%d", label + 1);
                    for (int i = 0; i < POINTER_BITS; i++)
                    {
                        instr("FROM @%d", ++label);
                        instr("'[,'_,'_,'_ '[,'0,'_,'_ S,R,S,S @%d", label + 1);
                    }
                    instr("FROM @%d", ++label);
                    instr("'[,'_,'_,'_ '[,'/,'_,'_ S,L,S,S");
                    instr("'[,'0,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ R,R,S,S @%d", ++label);

                    int loop = label;
                    int empty_found = ++label;

                    instr("FROM @%d", loop);
                    instr("'_,'0|'1,'_,'_ S,R,S,S @%d", empty_found);
                    instr("'0|'1,'0,'_,'_ R,S,S,S");
                    instr("'0|'1,'1,'_,'_ R,S,S,S");
                    instr("'/,'0|'1,'_,'_ S,S,S,S @%d", ++label);
                    instr("FROM @%d", label); // inc
                    instr("'/,'0,'_,'_ '/,'1,'_,'_ R,S,S,S @%d", loop);
                    instr("'/,'1,'_,'_ '/,'0,'_,'_ S,R,S,S @%d", ++label);
                    instr("FROM @%d", label); // inc carry
                    instr("'/,'0,'_,'_ '/,'1,'_,'_ S,L,S,S @%d", ++label);
                    instr("'/,'1,'_,'_ '/,'0,'_,'_ S,R,S,S");
                    instr("FROM @%d", label); // back left
                    instr("'/,'0|'1,'_,'_ S,L,S,S");
                    instr("'/,'/|'[,'_,'_ R,R,S,S @%d", loop);

                    instr("FROM @%d", empty_found);
                    instr("'_,'0|'1,'_,'_ S,R,S,S");
                    instr("'_,'/,'_,'_ S,R,S,S @%d", label + 1);
                    var_list var = {0};
                    var.header.name = "<dynamic alloc>";
                    var.type = decode_spec(op[0], frame);
                    allocate_var(&var);
                    instr("FROM @%d", ++label);
                    instr("'_,'_,'_,'_ L,L,S,S");
                    instr("'0|'1|'/,'/,'_,'_ L,S,S,S");
                    instr("'[,'/,'_,'_ S,S,S,S @%d", label + 1);
                    return;
                }
                default:
                    error_msg(n, "Houston, we have a problem: unattended token %d\n",
                              OPER_OPERATOR(n));
                    exit(1);
            }
        }
        case k_scope:
        {
            allocate_scope(SC_SCOPE(n));
            PROD0("; entering scope");
            exec(SC_CODE(n), SC_SCOPE(n));
            PROD0("; exiting scope");
            free_scope(SC_SCOPE(n));
            return;
        }
        case k_list:
        {
            error_msg(n, "INTERNAL: Invalid exec parameter\n");
            exit(1);
        }
    }
}

void emit_function(func_list* ptr)
{
    if (ptr->kind == F_BUILTIN)
    {
        return;
    }

    if (ptr->kind == F_GENERIC)
    {
        for (func_list* inst = ptr->instances; ptr; ptr = (func_list*)ptr->header.next)
        {
            emit_function(inst);
        }
        return;
    }

    if (!ptr->callsites)
        return;

    instr("# %s %s (F%d)", ptr->return_type == VOID_TYPE ? "procedure" : "function", ptr->header.name,
          ptr->header.id);
    instr("FROM @F%d", ptr->header.id);
    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
    BLOCK("function code",
          {
              exec(SC_CODE(ptr->code), &ptr->frame);
          });
    instr("FROM @%d", ++label);
    instr("'[,'/|'[,'_,'_ S,S,S,S @F%dret", ptr->header.id);

    for (call_site_list* call = ptr->callsites; call; call = call->next)
    {
        BLOCK("call site ID writing",
              {
                  instr("FROM @F%dC%d", ptr->header.id, call->id); // site id alloc
                  int i;
                  // todo: O(n) -> O(1) ?
                  for (i = 0; i < ptr->callsites->id; i++)
                  {
                      instr("'_,'/|'[,'_,'_ '%d,'/|'[,'_,'_ R,S,S,S @%d", call->id == i ? 1 : 0, ++label);
                      instr("FROM @%d", label);
                  }
                  instr("'_,'/|'[,'_,'_ '%d,'/|'[,'_,'_ R,S,S,S @%d", call->id == i ? 1 : 0,
                        call->argalloc_address);
              });
        PROD0("begin call site ID check");
        BLOCK("call site ID check",
              {
                  instr("FROM @F%dC%dcheck", ptr->header.id, call->id);
                  instr("'1,'_,'_,'_ '_,'_,'_,'_ L,S,S,S @%d", ++label); // ok
                  if (call->next)
                      instr("'0,'_,'_,'_ '_,'_,'_,'_ L,S,S,S @F%dC%dcheck", ptr->header.id, call->id - 1);
                  instr("FROM @%d", label);
                  instr("'0,'_,'_,'_ '_,'_,'_,'_ L,S,S,S");
                  instr("'[,'_,'_,'_ '_,'_,'_,'_ L,S,S,S @%d", ++label);
                  instr("FROM @%d", label);
                  instr("'/|'0|'1,'_,'_,'_ L,S,S,S");
                  instr("'[,'_,'_,'_ S,L,S,S @%d", call->return_address);
              });
    }

    BLOCK("function epilogue",
          {
              instr("FROM @F%dret", ptr->header.id); // clean heap
              instr("'[,'/|'[,'_,'_ '[,'/|'[,'_,'_ R,R,S,S");
              instr("'0|'1|'/,'_,'_,'_ '_,'_,'_,'_ R,S,S,S");
              instr("'_,'_,'_,'_ L,S,S,S");
              instr("'[,'_,'_,'_ '_,'_,'_,'_ L,S,S,S @F%dC%dcheck", ptr->header.id, ptr->callsites->id);
          });
}

void emit_functions()
{
    for (func_list* ptr = funcs_head; ptr; ptr = (func_list*) ptr->header.next)
    {
        emit_function(ptr);
    }
}

void emit_main(ast_node* n)
{
    instr("NEW \"generated\" 4");
    instr("START @INIT");
    instr("END @END \"END\"");
    instr("END @DEFAULT \"default option\"");

    instr("FROM @INIT");
    instr("'_,'_,'_,'_ '[,'[,'_,'. S,S,S,R @1");

    allocate_scope(&global_frame);

    BLOCK("program",
          {
              eval(n, &global_frame);
          });
    instr("FROM @%d", ++label);
    instr("'[,'/,'_,'_ S,S,S,S @END");
    instr("'[,'[,'_,'_ S,S,S,S @END");
}