//
// Created by Tom on 05/11/2021.
//

#include <stdio.h>
#include <stdlib.h>
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
        if (ptr->type->type == T_CONST)
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
        if (ptr->type->type == T_CONST)
            continue;
        instr("# allocate variable %s (position %d, size %d)", ptr->header.name, ptr->position,
              type_size_cells(ptr->type));
        if (type_size_cells(ptr->type) == 1)
        {
            for (int i = 0; i < type_size_bits(ptr->type); i++)
            {
                instr("FROM @%d", ++label);
                instr("'_,'_,'_,'_ '0,'_,'_,'_ R,S,S,S @%d", label + 1);
            }
            instr("FROM @%d", ++label);
            instr("'_,'_,'_,'_ '/,'_,'_,'_ R,S,S,S @%d", label + 1);
        }
        else if (ptr->type->type == T_ARRAY)
        {
            char* str = ptr->initial;
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
        }
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

void emit_functions_epilogues()
{
    for (func_list* ptr = funcs_head; ptr; ptr = (func_list*) ptr->header.next)
    {
        if (!ptr->callsites)
            continue;

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
}

/**
 * push(*pop());
 */
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

void eval(ast_node* n, stack_frame* frame);

/**
 * Moves the heap pointer to the specified variable
 * @throws exit If the given node is not an lvalue (variable or dereference)
 */
void nav_to_var(ast_node* op, stack_frame* frame)
{
    ast_node* pos = AST_INFERRED_POS(op);

    if (!pos)
    {
        error_msg(op, "Expected lvalue for assignment\n");
        exit(1);
    }

    if (AST_KIND(pos) == k_number)
    {
        int vid = NUMBER_VALUE(pos);
        instr("# navigating to %d", vid);
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
        eval(pos, frame);
        deref();
    }
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
    value &= (1 << size) - 1;
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

call_site_list* add_call_site(call_site_list** list)
{
    call_site_list* newNode = malloc(sizeof(call_site_list));
    newNode->next = *list;
    if (*list)
        newNode->id = (*list)->id + 1;
    else
        newNode->id = 0;
    *list = newNode;
    return newNode;
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
        case k_ident:
        {
            PROD1S("load", VAR_NAME(n));
            if (clean_stack)
                USELESS();
            var_list* ptr = get_var_id(n, frame, F_DEFAULT);
            if (ptr->type->type == T_ARRAY) // array
            {
                push_number(var_position(ptr), POINTER_BITS);// todo: check unused
            }
            else
            {
                nav_to_var(n, frame);
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
                    eval(op[0], frame);
                    if (clean_stack)
                    {
                        pop(1);
                        USELESS();
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
                    for (int i = 0; i < 8 - 1; i++)// TODO
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
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                    instr("FROM @%d", label); // equal
                    for (int i = 0; i < 8 - 1; i++)// TODO
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
                        USELESS();
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
                        USELESS();
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
                    nav_to_var(op[0], frame);
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
                        instr("'/|'0|'1,'/,'_,'_ L,S,S,S");
                        instr("'[,'/,'_,'_ S,S,S,S @%d", label + 1);
                    }
                    return;
                }
                case DEC:
                {
                    PROD0("dec");
                    nav_to_var(op[0], frame);
                    instr("FROM @%d", ++label);
                    instr("'/|'[,'[,'_,'_ R,R,S,S @%d", ++label);
                    instr("'/|'[,'/,'_,'_ R,R,S,S @%d", label);
                    if (clean_stack) // statement
                    {
                        instr("FROM @%d", label);
                        int left_to_end = ++label;
                        int end = label + 1;
                        instr("'1,'_,'_,'_ '0,'_,'_,'_ L,S,S,S @%d", left_to_end);
                        instr("'0,'_,'_,'_ '1,'_,'_,'_ R,S,S,S");
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
                        instr("'/|'0|'1,'/,'_,'_ L,S,S,S");
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
                    instr("FROM @%d", ++label);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d_break", frame->loop->address);
                    return;
                case KCONTINUE:
                    PROD0("continue");
                    instr("FROM @%d", ++label);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d_continue", frame->loop->address);
                    return;
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
                case '=':
                {
                    PROD0("store");
                    PROD0("evaluating right-hand value");
                    eval(op[1], frame);
                    instr("FROM @%d", ++label);
                    instr("'0|'1|'/,'/,'_,'_ L,S,S,S");
                    instr("'[,'/,'_,'_ S,S,S,S @%d", label + 1);
                    nav_to_var(op[0], frame);
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
                        instr("'0|'1,'/,'_,'_ '0,'/,'_,'_ R,S,S,S");
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
                        instr("'0|'1,'/,'_,'_ '0,'/,'_,'_ R,S,S,S");
                        instr("FROM @%d", label);
                        instr("'0|'1,'/,'_,'_ L,S,S,S");
                        instr("'/|'[,'/,'_,'_ S,S,S,S @%d", label + 1);
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
                case KPROC:
                    PROD0("proc");
                    return;
                case KTYPE:
                    PROD0("type");
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
                    PROD0("call");
                    func_list* fct = FIND_SYM(func_list, funcs_head, op[0]);
                    linked_list* args = ((ast_linked_list*) op[1])->list;
                    int argcount = 0;
                    for (linked_list* arg = args; arg; arg = arg->next, argcount++)
                    {
                        eval(arg->value, frame);
                    }
                    int start_left = ++label;
                    int right_heap = ++label;
                    PROD0("go to left of args on stack");
                    instr("FROM @%d", start_left);
                    instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                    instr("'[,'[,'_,'_ S,S,S,S @%d", right_heap);
                    for (int i = 1; i < argcount; i++)
                    {
                        instr("FROM @%d", label);
                        instr("'[,'0|'1,'_,'_ S,L,S,S");
                        instr("'[,'/,'_,'_ S,L,S,S @%d", ++label);
                    }
                    instr("FROM @%d", label);
                    instr("'[,'0|'1,'_,'_ S,L,S,S");
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d", right_heap);

                    PROD0("go to right of heap");
                    instr("FROM @%d", right_heap);
                    instr("'/|'0|'1|'[,'/,'_,'_ R,S,S,S");
                    instr("'/|'0|'1|'[,'[,'_,'_ R,S,S,S");
                    call_site_list* call = add_call_site(&fct->callsites);
                    instr("'_,'/|'[,'_,'_ '[,'/|'[,'_,'_ R,S,S,S @F%dC%d", fct->header.id, call->id);
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
                case '{':
                {
                    PROD0("; compound expression statement");
                    exec(op[0], frame);
                    PROD0("; compound expression result");
                    exec(op[1], frame);
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

void emit_functions()
{
    for (func_list* ptr = funcs_head; ptr; ptr = (func_list*) ptr->header.next)
    {
        if (!ptr->callsites)
            continue;
        instr("# %s %s (F%d)", ptr->return_type == VOID_TYPE ? "procedure" : "function", ptr->header.name, ptr->header.id);
        instr("FROM @F%d", ptr->header.id);
        instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
        BLOCK("function code",
              {
                  exec(SC_CODE(ptr->code), &ptr->frame);
              });
        instr("FROM @%d", ++label);
        instr("'[,'/|'[,'_,'_ S,S,S,S @F%dret", ptr->header.id);
    }
}

void emit_main(ast_node* n)
{
    instr("NEW \"generated\" 4");
    instr("START @INIT");
    instr("END @END \"END\"");
    instr("END @DEFAULT \"default option\"");

    instr("FROM @INIT");
    instr("'_,'_,'_,'_ '[,'[,'_,'_ S,S,S,S @1");

    allocate_scope(&global_frame);

    BLOCK("program",
          {
              eval(n, &global_frame);
          });
    instr("FROM @%d", ++label);
    instr("'[,'/,'_,'_ S,S,S,S @END");
    instr("'[,'[,'_,'_ S,S,S,S @END");
}