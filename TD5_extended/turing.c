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

typedef struct call_site_list_s
{
    int id;
    int return_address, argalloc_address;
    struct call_site_list_s* next;
} call_site_list;

typedef struct linked_list_header_s
{
    const char* name;
    int id;
    void* owner;
    struct linked_list_header_s* next;
} linked_list_header;

typedef enum
{
    V_VAR,
    V_ARRAY,
    V_CONST
} var_type;

typedef struct
{
    linked_list_header header;
    var_type type;
    union
    {
        struct // var / array
        {
            int position;
            int size;
            struct // array
            {
                char* initial;
            };
        };
        struct // const
        {
            int value;
        };
    };
} var_list;

typedef struct stack_frame_s
{
    struct func_list_s* function;
    struct
    {
        var_list* head, * tail;
    } vars;
    struct stack_frame_s* parent;
} stack_frame;
stack_frame global_frame = {.function = NULL, .vars = {NULL, NULL}, .parent = NULL};

typedef struct func_list_s
{
    linked_list_header header;
    bool is_void;
    linked_list* arglist;
    ast_node* code;
    call_site_list* callsites;
    stack_frame frame;
} func_list;
func_list * funcs_head = NULL, * funcs_tail = NULL;

typedef struct
{
    int address;
} loop_info;

int label = 0;

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

void nav_to_var(ast_node* op, stack_frame* frame, loop_info* loop);

void deref();

void exec(ast_node* n, stack_frame* frame, loop_info* loop);

typedef enum
{
    F_DEFAULT = 0,
    F_NULLABLE = 1,
    F_RECURSE = 2
} find_flags;

linked_list_header* find_symbol(linked_list_header* list, ast_node* node, find_flags flags)
{
    const char* name = VAR_NAME(node);
    for (linked_list_header* ptr = list; ptr; ptr = ptr->next)
    {
        if (!strcmp(ptr->name, name))
        {
            return ptr;
        }
    }
    if (list && (flags & F_RECURSE) && ((stack_frame*)list->owner)->parent)
        return find_symbol(&((stack_frame*)list->owner)->parent->vars.head->header, node, flags);
    if (flags & F_NULLABLE)
        return NULL;
    error_msg(NULL, "SYMBOL NOT FOUND: %s\n", name);
    exit(1);
}

#define FIND_SYM(type, list, name, flags) ((type*)find_symbol(&((list)->header), name, flags))

var_list* get_var_id(ast_node* name, stack_frame* frame, find_flags flags)
{
    return FIND_SYM(var_list, frame->vars.head, name, flags);
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

void eval(ast_node* n, stack_frame* frame)
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

#define RETURN(x) do{*n = x; return true;}while(0)

bool static_fold(ast_node** n, stack_frame* frame)
{
    if (!*n)
        return false;

    switch (AST_KIND(*n))
    {
        case k_number:
        {
            RETURN(*n);
        }
        case k_ident:
        {
            if (frame)
            {
                var_list* ptr = get_var_id(*n, frame, F_NULLABLE | F_RECURSE);
                if (ptr)
                {
                    if (ptr->type == V_CONST) // allow higher-level constant resolution
                    {
                        RETURN(make_number(ptr->value));
                    }
                    if (ptr->header.owner == frame) // only local mutables can be referenced
                    {
                        if (ptr->type == V_ARRAY) // array
                        {
                            RETURN(make_number(ptr->position));
                        }
                    }
                }
            }
            return false;
        }
        case k_operator:
        {
            ast_node** op = OPER_OPERANDS(*n);
            int arity = OPER_ARITY(*n);
            struct
            {
                bool is_num;
                int value;
            } * o = malloc(arity * sizeof(*o));
            for (int i = 0; i < arity; i++)
            {
                if (!op[i])
                    continue;
                static_fold(&op[i], frame);
                if ((o[i].is_num = AST_KIND(op[i]) == k_number))
                    o[i].value = NUMBER_VALUE(op[i]);
            }

            switch (OPER_OPERATOR(*n))
            {
                /* Expressions */
                case UMINUS:
                {
                    if (o[0].is_num)
                    {
                        RETURN(make_number(-o[0].value));
                    }
                    return false;
                }
                case '~':
                {
                    if (o[0].is_num)
                    {
                        RETURN(make_number(~o[0].value));
                    }
                    return false;
                }
                case REF:
                {
                    if (frame)
                    {
                        int ptr;
                        if (AST_KIND(op[0]) == k_ident)
                        {
                            var_list* var = get_var_id(op[0], frame, F_DEFAULT);
                            if (var->type == V_CONST)
                            {
                                error_msg(*n, "Cannot take address of constant '%s'\n", var->header.name);
                                exit(1);
                            }
                            ptr = var->position;
                        }
                        else if (AST_KIND(op[0]) == k_number)
                        {
                            ptr = NUMBER_VALUE(op[0]);
                        }else if (AST_KIND(op[0]) == k_operator && OPER_OPERATOR(op[0]) == DEREF){ast_node* inner = OPER_OPERANDS(op[0])[0];static_fold(&inner, frame);RETURN(inner);}

                        if (op[1] == NULL)
                        {
                            RETURN(make_number(ptr));
                        }
                        else if (o[1].is_num)
                        {
                            RETURN(make_number(ptr + o[1].value));
                        }
                    }
                    return false;
                }
                case '+':
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value + o[1].value));
                    }
                    else if (o[0].is_num && o[0].value == 0)
                    {
                        RETURN(op[1]);
                    }
                    else if (o[1].is_num && o[1].value == 0)
                    {
                        RETURN(op[0]);
                    }
                    return false;
                }
                case '-':
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value - o[1].value));
                    }
                    else if (o[0].is_num && o[0].value == 0)
                    {
                        RETURN(make_node(UMINUS, 1, op[1]));
                    }
                    else if (o[1].is_num && o[1].value == 0)
                    {
                        RETURN(op[0]);
                    }
                    return false;
                }
                case '*':
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value * o[1].value));
                    }
                    else if (o[0].is_num)
                    {
                        if (o[0].value == 1)
                        {
                            RETURN(op[1]);
                        }
                        else if (o[0].value == 0)
                        {
                            RETURN(make_number(0));
                        }
                    }
                    else if (o[1].is_num)
                    {
                        if (o[1].value == 1)
                        {
                            RETURN(op[0]);
                        }
                        else if (o[1].value == 0)
                        {
                            RETURN(make_number(0));
                        }
                    }
                    return false;
                }
                case '/':
                    // todo: implement division
                    return false;
                case '<':
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value < o[1].value));
                    }
                    return false;
                }
                case '>':
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value > o[1].value));
                    }
                    return false;
                }
                case GE:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value >= o[1].value));
                    }
                    return false;
                }
                case LE:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value <= o[1].value));
                    }
                    return false;
                }
                case NE:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value != o[1].value));
                    }
                    return false;
                }
                case EQ:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value == o[1].value));
                    }
                    return false;
                }
                case AND:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value && o[1].value));
                    }
                    return false;
                }
                case OR:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value || o[1].value));
                    }
                    return false;
                }
                case '=':
                {
                    if (AST_KIND(op[1]) == k_ident && !strcmp(VAR_NAME(op[0]), VAR_NAME(op[1])))
                    {
                        info_msg(*n, "Assignment has no effect\n");
                        RETURN(op[0]);
                    }
                    return false;
                }
                case KIF:
                {
                    if (o[0].is_num)
                    {
                        info_msg(*n, "Condition is always %s\n", o[0].value ? "true" : "false");
                        RETURN(o[0].value ? op[1] : op[2]);
                    }
                    return false;
                }
            }
        }
    }

    return false;
}

#undef RETURN

int static_eval(ast_node* n, stack_frame* frame)
{
    if (!static_fold(&n, frame) || AST_KIND(n) != k_number)
    {
        error_msg(n, "Value must be compile-time constant\n");
        exit(1);
    }

    return NUMBER_VALUE(n);
}

#define USELESS() do{info_msg(n, "Line has no effect\n");return;}while(0)

void exec(ast_node* n, stack_frame* frame, loop_info* loop)
{
    if (!n)
    {
        PROD0("empty node");
        return;
    }

    instr("# KIND = %d (%s)", AST_KIND(n), node_kind_NAMES[AST_KIND(n)]);

    bool clean_stack = AST_CLEAN_STACK(n);

    if (optimize)
    {
        if (static_fold(&n, frame))
        {
            PROD0("statically evaluated");
        }
    }

    switch (AST_KIND(n))
    {
        case k_number:
        {
            PROD1F("push", NUMBER_VALUE(n));
            if (clean_stack)
                USELESS();
            push_number(NUMBER_VALUE(n));
            return;
        }
        case k_ident:
        {
            PROD1S("load", VAR_NAME(n));
            if (clean_stack)
                USELESS();
            var_list* ptr = get_var_id(n, frame, F_DEFAULT);
            if (ptr->type == V_ARRAY) // array
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
                case REF:
                {
                    PROD0("ref");
                    if (clean_stack)
                    {
                        USELESS();
                    }
                    int ptr = get_var_id(op[0], frame, F_DEFAULT)->position;
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
                        instr("'/|'0|'1,'/,'_,'_ L,S,S,S");
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
                        error_msg(n, "Break statement only permitted in loop\n");
                        exit(1);
                    }
                    instr("FROM @%d", ++label);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d_break", loop->address);
                    return;
                case KCONTINUE:
                    PROD0("continue");
                    if (!loop)
                    {
                        error_msg(n, "Continue statement only permitted in loop\n");
                        exit(1);
                    }
                    instr("FROM @%d", ++label);
                    instr("'[,'/|'[,'_,'_ S,S,S,S @%d_continue", loop->address);
                    return;
                case KDO:
                {
                    PROD0("do");
                    int start = label + 1;
                    loop_info loop_info = {.address = start};
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
                    loop_info loop_info = {.address = start};
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
                case KRETURN:
                {
                    PROD0("return");
                    if (!frame->function)
                    {
                        error_msg(n, "Can only return from a function or procedure\n");
                        exit(1);
                    }
                    if (frame->function->is_void && op[0])
                    {
                        error_msg(n, "Cannot return value from procedure\n");
                        exit(1);
                    }
                    if (!frame->function->is_void && !op[0])
                    {
                        error_msg(n, "Function must return a value\n");
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
                    func_list* fct = FIND_SYM(func_list, funcs_head, op[0], F_DEFAULT);
                    if (fct->is_void && !clean_stack)
                    {
                        error_msg(n, "Procedure call does not have a value\n");
                        exit(1);
                    }
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
//                    instr("FROM @%d", label);
//                    instr("'_,'0|'1,'_,'_ S,L,S,S");
//                    instr("'_,'/|'[,'_,'_ S,R,S,S @%d", ++label);
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
                    error_msg(n, "Houston, we have a problem: unattended token %d\n",
                              OPER_OPERATOR(n));
                    exit(1);
            }
        }
        default:
            return;
    }
}

void nav_to_var(ast_node* op, stack_frame* frame, loop_info* loop)
{
    if (AST_KIND(op) == k_ident)
    {
        instr("# navigating to %s", VAR_NAME(op));
        var_list* var = get_var_id(op, frame, F_DEFAULT);
        if (var->type == V_CONST)
        {
            error_msg(op,
                      "Cannot navigate to constant; this usually indicates that an internal error occurred during static analysis\n");
            exit(1);
        }
        int vid = var->position;
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
        error_msg(op, "Expected lvalue for assignment\n");
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


linked_list_header* add_symbol(linked_list_header** head, linked_list_header** tail, size_t size)
{
    linked_list_header* newNode = malloc(size);
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

#define ADD_SYM(type, head, tail) ((type*) add_symbol((linked_list_header**)(head), (linked_list_header**)(tail), sizeof(type)))


/**
 *
 * @param name
 * @param size
 * @param vars_head
 * @param vars_tail
 * @return NULL if the variable already exists
 */
var_list* check_add_var(const char* name, int size, stack_frame* frame)
{
    bool found = false;
    int i = 0;
    if (frame->vars.head)
    {
        for (var_list* ptr = frame->vars.head; ptr; ptr = (var_list*) ptr->header.next)
        {
            if (!strcmp(ptr->header.name, name))
            {
                found = true;
                break;
            }
            if (ptr->type != V_CONST)
                i += ptr->size;
        }
    }
    if (!found)
    {
        var_list* newNode = ADD_SYM(var_list, &frame->vars.head, &frame->vars.tail);
        newNode->header.name = name;
        newNode->header.owner = frame;
        newNode->position = i;
        newNode->type = V_VAR;
        newNode->size = size;
        newNode->initial = NULL;
        return newNode;
    }
    return NULL;
}


void traverse_vars(ast_node* n, stack_frame* frame)
{
    if (!n)
        return;

    if (AST_KIND(n) == k_operator)
    {
        if (OPER_ARITY(n) > 0 && OPER_OPERANDS(n)[0] && AST_KIND(OPER_OPERANDS(n)[0]) == k_ident)
        {
            if (OPER_OPERATOR(n) == '=')
            {
                check_add_var(VAR_NAME(OPER_OPERANDS(n)[0]), 1, frame);
            }
            else if (OPER_OPERATOR(n) == KCONST)
            {
                var_list* var = check_add_var(VAR_NAME(OPER_OPERANDS(n)[0]), 1, frame);
                if (!var)
                {
                    error_msg(n, "Cannot redeclare constant\n");
                    exit(1);
                }
                var->type = V_CONST;
                var->value = static_eval(OPER_OPERANDS(n)[1], frame);
            }
        }
        if (OPER_OPERATOR(n) == KVAR)
        {
            switch (OPER_ARITY(n))
            {
                case 1:
                    check_add_var(VAR_NAME(OPER_OPERANDS(n)[0]), 1, frame);
                    break;
                case 3:
                {
                    var_list* var = check_add_var(VAR_NAME(OPER_OPERANDS(n)[0]),
                                                         static_eval(OPER_OPERANDS(n)[1], frame), frame);
                    if (!var)
                    {
                        error_msg(n, "Cannot redeclare array\n");
                        exit(1);
                    }
                    var->type = V_ARRAY;
                    ast_node* initial = OPER_OPERANDS(n)[2];
                    if (initial)
                    {
                        var->initial = VAR_NAME(initial);
                    }
                    break;
                }
            }
        }

        if (OPER_OPERATOR(n) != KFUNC && OPER_OPERATOR(n) != KPROC)
        {
            for (int i = 0; i < OPER_ARITY(n); i++)
                traverse_vars(OPER_OPERANDS(n)[i], frame);
        }
    }
}

void traverse_funcs(ast_node* n, stack_frame* frame)
{
    if (!n)
        return;

    if (AST_KIND(n) == k_operator)
    {
        if (OPER_OPERATOR(n) == KFUNC || OPER_OPERATOR(n) == KPROC)
        {
            func_list* newNode = ADD_SYM(func_list, &funcs_head, &funcs_tail);
            newNode->header.name = VAR_NAME(OPER_OPERANDS(n)[0]);
            newNode->header.owner = NULL;
            newNode->is_void = OPER_OPERATOR(n) == KPROC;
            newNode->arglist = ((ast_linked_list*) OPER_OPERANDS(n)[1])->list;
            newNode->code = OPER_OPERANDS(n)[2];
            newNode->callsites = NULL;
            newNode->frame = (stack_frame) {.function = newNode, .vars = {NULL, NULL}, .parent = frame};

            for (linked_list* ptr = newNode->arglist; ptr; ptr = ptr->next)
            {
                check_add_var(VAR_NAME(ptr->value), 1, &newNode->frame);
            }

            traverse_vars(newNode->code, &newNode->frame);
            traverse_funcs(newNode->code, &newNode->frame);
        }
        else
        {
            for (int i = 0; i < OPER_ARITY(n); i++)
                traverse_funcs(OPER_OPERANDS(n)[i], frame);
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

    traverse_vars(n, &global_frame);
    traverse_funcs(n, &global_frame);

    {
        printf("# Memory map\n");
        printf("# POSITION  SIZE  NAME\n");
        for (var_list* ptr = global_frame.vars.head; ptr; ptr = (var_list*) ptr->header.next)
        {
            if (ptr->type == V_CONST)
                continue;
            printf("# %8d  %4d  %s\n", ptr->position, ptr->size, ptr->header.name);
        }
    }

    instr("FROM @INIT");
    instr("'_,'_,'_,'_ '[,'[,'_,'_ R,S,S,S @0");

    for (var_list* ptr = global_frame.vars.head; ptr; ptr = (var_list*) ptr->header.next)
    {
        if (ptr->type == V_CONST)
            continue;
        instr("# allocate variable %s (position %d, size %d)", ptr->header.name, ptr->position, ptr->size);
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

    BLOCK("program",
          {
              eval(n, &global_frame);
          });
    instr("FROM @%d", ++label);
    instr("'[,'/,'_,'_ S,S,S,S @END");
    instr("'[,'[,'_,'_ S,S,S,S @END");

    for (func_list* ptr = funcs_head; ptr; ptr = (func_list*) ptr->header.next)
    {
        if (!ptr->callsites)
            continue;
        instr("# %s %s (F%d)", ptr->is_void ? "procedure" : "function", ptr->header.name, ptr->header.id);
        instr("FROM @F%d", ptr->header.id);
        instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
        BLOCK("function code",
              {
                  exec(ptr->code, &ptr->frame, NULL);
              });
        instr("FROM @%d", ++label);
        instr("'[,'/|'[,'_,'_ S,S,S,S @F%dret", ptr->header.id);
    }

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
