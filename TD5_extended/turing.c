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
#include <assert.h>

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

typedef struct
{
    linked_list_header header;
    struct type_list_s const* type;
    union
    {
        struct // var / array
        {
            int position;
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
    bool is_root;
    int end;
    struct
    {
        var_list* head, * tail;
    } vars;
    struct stack_frame_s* parent;
} stack_frame;
stack_frame global_frame = {.function = NULL, .is_root = true, .end = 0, .vars = {NULL, NULL}, .parent = NULL};
int static_eval(ast_node* n, stack_frame* frame);
int frame_start(stack_frame* frame)
{
    if (frame->is_root)
        return 0;

    return frame->parent->end + frame_start(frame->parent);
}

int var_position(var_list* var)
{
    return frame_start((stack_frame*)var->header.owner) + var->position;
}

typedef struct func_list_s
{
    linked_list_header header;
    bool is_void;
    linked_list* arglist;
    ast_node* code;
    call_site_list* callsites;
    stack_frame frame;
} func_list;
func_list* funcs_head = NULL, * funcs_tail = NULL;

typedef struct
{
    int address;
} loop_info;

typedef enum
{
    T_VOID,
    T_SCALAR,
    T_ARRAY,
    T_POINTER,
    T_CONST,
    T_COMPOSITE
} type_type;

typedef struct type_list_s
{
    linked_list_header header;
    type_type type;
    union
    {
        int scalar_size; // always 1 for now
        struct
        {
            int array_size;
            struct type_list_s const* array_target;
        };
        struct type_list_s const* pointer_target;
        struct type_list_s const* const_target;
        struct
        {
            var_list* head, * tail;
        } composite_members;
    };
} type_list;
type_list* types_head = NULL, * types_tail = NULL;

type_list* VOID_TYPE;
type_list* check_add_type(const char* name);
bool type_same(type_list const* a, type_list const* b)
{
    if (a->type != b->type)
        return false;

    switch(a->type)
    {
        case T_SCALAR:
            return a->scalar_size == b->scalar_size;
        case T_ARRAY:
            return a->array_size == b->array_size && type_same(a->array_target, b->array_target);
        case T_POINTER:
            return type_same(a->pointer_target, b->pointer_target);
        case T_CONST:
            return type_same(a->const_target, b->const_target);
        case T_COMPOSITE:
            return false;
    }
}

void allocate_scope(stack_frame* frame);
#define ADD_SYM(type, head, tail) ((type*) add_symbol((linked_list_header**)(head), (linked_list_header**)(tail), sizeof(type)))


int type_size(type_list const* type)
{
    switch (type->type)
    {
        case T_SCALAR:
            return type->scalar_size;
        case T_ARRAY:
            return type->array_size;
        case T_POINTER:
            return 1;
        case T_CONST:
            return type_size(type->const_target);
        case T_COMPOSITE:
        {
            int sum = 0;
            for (var_list* ptr = type->composite_members.head; ptr; ptr = (var_list*) ptr->header.next)
            {
                sum += type_size(ptr->type);
            }
            return sum;
        }
    }
}

var_list* check_add_var(const char* name, stack_frame* frame, type_list const* type);

const char* type_display(type_list const* type);

const char* type_display_full(type_list const* type, bool inner, bool expand)
{
    if (!inner && type->header.name)
        return type->header.name;

    switch (type->type)
    {
        case T_VOID:
            return "void";
        case T_SCALAR:
            return "word";
        case T_ARRAY:
        {
            char* buf = malloc(256);
            sprintf(buf, "%s[%d]", type_display(type->array_target), type->array_size);
            return buf;
        }
        case T_POINTER:
        {
            char* buf = malloc(256);
            sprintf(buf, "%s*", type_display(type->pointer_target));
            return buf;
        }
        case T_CONST:
        {
            char* buf = malloc(256);
            sprintf(buf, "%s const", type_display(type->const_target));
            return buf;
        }
        case T_COMPOSITE:
        {
            if (!expand)
                return "<anonymous type>";
            char* buf = malloc(1024);
            int p = sprintf(buf, "struct { ");
            for(var_list* ptr = type->composite_members.head; ptr; ptr = (var_list*)ptr->header.next)
            {
                p += sprintf(buf + p, "%s: %s; ", ptr->header.name, type_display_full(ptr->type, false, true));
            }
            strcat(buf + p, "}");
            return buf;
        }
    }
}

const char* type_display(type_list const* type)
{
    return type_display_full(type, false, false);
}

#define WORD_TYPE types_head

int label = 0;

void free_scope(stack_frame* frame)
{
    if (frame->end)
    {
        instr("FROM @%d", ++label);
        instr("'/|'0|'1|'[,'[,'_,'_ R,R,S,S");
        instr("'/|'0|'1|'[,'/,'_,'_ R,R,S,S");
        instr("'/|'0|'1|'[,'_,'_,'_ R,S,S,S");
        instr("'_,'_,'_,'_ L,S,S,S @%d", label + 1);
        for (int i = 0; i < frame->end - 1; i++)
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

_Noreturn void missing_symbol(ast_node* node, const char* name)
{
    error_msg(node, "SYMBOL NOT FOUND: %s\n", name);
    exit(1);
}

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
    // todo: broken
    if (list && ((stack_frame*)list->owner)->parent &&
            ((flags & F_RECURSE) || !((stack_frame*)list->owner)->is_root))
        // either F_RECURSE forces recursing through all frames
        // or we simply allow looking up from parent scopes (but in the same frame)
        return find_symbol(&((stack_frame*) list->owner)->parent->vars.head->header, node, flags);
    if (flags & F_NULLABLE)
        return NULL;
    missing_symbol(NULL, name);
}

#define FIND_SYM(type, list, name, flags) ((type*)find_symbol(&((list)->header), name, flags))

var_list* get_var_id(ast_node* node, stack_frame* frame, find_flags flags)
{
    const char* name = VAR_NAME(node);
    for (linked_list_header* ptr = &frame->vars.head->header; ptr; ptr = ptr->next)
    {
        if (!strcmp(ptr->name, name))
        {
            return (var_list*)ptr;
        }
    }
    if (frame->parent &&
        ((flags & F_RECURSE) || !frame->is_root))
        // either F_RECURSE forces recursing through all frames
        // or we simply allow looking up from parent scopes (but in the same frame)
        return get_var_id(node, frame->parent, flags);
    if (flags & F_NULLABLE)
        return NULL;
    missing_symbol(node, name);
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



#define NEW_TYPE() (calloc(1, sizeof(type_list)))

type_list const* decay_array_ptr(type_list const* t)
{
    if (t->type != T_ARRAY)
        return t;

    type_list* ret = NEW_TYPE();
    ret->type = T_POINTER;
    ret->pointer_target = t->array_target;

    return ret;
}

type_list const* type_check(ast_node* n, stack_frame* frame);

type_list const* infer_type(ast_node* n)
{
    if (!n)
        return NULL;

    if (AST_INFERRED(n))
        return AST_INFERRED(n);

    error_msg(n, "Node without type\n");
    exit(1);
}

type_list const* type_check_internal(ast_node* n, stack_frame* frame);
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



type_list const* type_check_internal(ast_node* n, stack_frame* frame)
{
    switch (AST_KIND(n))
    {
        case k_number:
            return WORD_TYPE;
        case k_ident:
        {
            var_list* ptr = get_var_id(n, frame, F_NULLABLE);
            if (ptr)
            {
                return ptr->type;
            }
            ptr = get_var_id(n, frame, F_NULLABLE | F_RECURSE);
            if (ptr && ptr->type->type == T_CONST)
            {
                return ptr->type;
            }
            missing_symbol(n, VAR_NAME(n));
        }
        case k_operator:
        {
            ast_node** op = OPER_OPERANDS(n);
            switch (OPER_OPERATOR(n))
            {
                case KSIZEOF:
                    return WORD_TYPE;
                case '+':
                {
                    type_list const* left = decay_array_ptr(infer_type(op[0]));
                    type_list const* right = decay_array_ptr(infer_type(op[1]));
                    if (left->type == T_POINTER && right->type == T_POINTER)
                    {
                        error_msg(n, "Cannot add two pointers\n");
                        exit(1);
                    }
                    if (left->type == T_POINTER && right->type == T_SCALAR)
                        return left;
                    if (left->type == T_SCALAR && right->type == T_POINTER)
                        return right;
                    if (left->type == T_SCALAR && right->type == T_SCALAR)
                        return left;
                    error_msg(n, "Cannot add types %s and %s\n", type_display(left), type_display(right));
                    exit(1);
                }
                case '-':
                {
                    type_list const* left = decay_array_ptr(infer_type(op[0]));
                    type_list const* right = decay_array_ptr(infer_type(op[1]));
                    if (left->type == T_POINTER && right->type == T_SCALAR)
                        return left;
                    if (left->type == T_SCALAR && right->type == T_SCALAR)
                        return left;
                    error_msg(n, "Cannot subtract types %s and %s\n", type_display(left), type_display(right));
                    exit(1);
                }
                case UMINUS:
                case '~':
                {
                    type_list const* left = infer_type(op[0]);
                    if (left->type == T_SCALAR)
                        return left;
                    error_msg(n, "Cannot perform arithmetic operation '%c' on non-numeric type %s\n", OPER_OPERATOR(n),
                              type_display(left));
                    exit(1);
                }
                case INC:
                case DEC:
                {
                    type_list const* left = infer_type(op[0]);
                    if (left->type == T_SCALAR || left->type == T_POINTER)
                        return left;
                    error_msg(n, "Cannot perform arithmetic operation '%c' on non-numeric type %s\n", OPER_OPERATOR(n),
                              type_display(left));
                    exit(1);
                }
                case '*':
                case '/':
                {
                    type_list const* left = infer_type(op[0]);
                    type_list const* right = infer_type(op[1]);
                    if (left->type == T_SCALAR && right->type == T_SCALAR)
                        return left;
                    error_msg(n, "Cannot perform arithmetic operation '%c' on types %s and %s\n", OPER_OPERATOR(n),
                              type_display(left), type_display(right));
                    exit(1);
                }
                case '<':
                case '>':
                case GE:
                case LE:
                case NE:
                case EQ:
                {
                    type_list const* left = decay_array_ptr(infer_type(op[0]));
                    type_list const* right = decay_array_ptr(infer_type(op[1]));
                    if (!type_same(left, right))
                    {
                        error_msg(n, "Cannot perform arithmetic comparison '%c' on different types %s and %s",
                                  OPER_OPERATOR(n), type_display(left), type_display(right));
                        exit(1);
                    }
                    if (left->type == T_SCALAR || left->type == T_POINTER)
                        return left;
                    error_msg(n, "Cannot perform arithmetic comparison '%c' on non-numeric types %s and %s\n",
                              OPER_OPERATOR(n), type_display(left), type_display(right));
                    exit(1);
                }
                case AND:
                case OR:
                    // todo: check that both are bools
                    return WORD_TYPE; // future bool?
                case '=':
                {
                    type_list const* left = infer_type(op[0]);
                    type_list const* right = infer_type(op[1]);
                    if (!type_same(left, right))
                    {
                        error_msg(n, "Cannot assign rvalue of type '%s' to lvalue of type '%s'\n", type_display(right), type_display(left));
                        exit(1);
                    }
                    return left;
                }
                case REF:
                {
                    type_list* ret = NEW_TYPE();
                    ret->type = T_POINTER;
                    if (!(ret->pointer_target = infer_type(op[0])))
                    {
                        error_msg(n, "Cannot determine type of referenced value\n");
                        exit(1);
                    }
                    return ret;
                }
                case DEREF:
                {
                    type_list const* inner = infer_type(op[0]);
                    if (!inner)
                        break;
                    if (inner->type != T_POINTER)
                    {
                        info_msg(n, "Dereferencing a non-pointer value\n");
                        break;
                    }
                    return inner->pointer_target;
                }
                case '(':
                    return WORD_TYPE; // functions can only return numbers for now, todo
                case '{':
                    return infer_type(op[1]);
            }
            break;
        }
        case k_scope:
        {
            return NULL;
        }
        case k_list:
        {
            error_msg(n, "INTERNAL: Invalid type_check parameter\n");
            exit(1);
        }
    }

    return NULL;
}

type_list const* decode_spec(ast_node* spec, stack_frame* frame);

ast_node* set_inferred(ast_node* n, type_list const* type)
{
    AST_INFERRED(n) = type;
    return n;
}

#define RETURN(x, type) do{*n = x; AST_INFERRED(*n) = type; return;}while(0)
#define SET_TYPE(type) do {AST_INFERRED(*n) = type; return;}while(0)
void static_fold(ast_node** n, stack_frame* frame)
{
    if (!*n)
        return;

    switch (AST_KIND(*n))
    {
        case k_number:
        {
            if (!AST_INFERRED(*n))
                SET_TYPE(WORD_TYPE);
            return;
        }
        case k_ident:
        {
            if (frame)
            {
                var_list* ptr = get_var_id(*n, frame, F_NULLABLE);
                if (ptr)
                {
                    if (ptr->type->type == T_ARRAY) // array
                    {
                        RETURN(make_number(var_position(ptr)), decay_array_ptr(ptr->type));
                    }
                    SET_TYPE(ptr->type);
                }
                ptr = get_var_id(*n, frame, F_RECURSE);
                if (ptr)
                {
                    if (ptr->type->type == T_CONST &&
                        ptr->type->const_target == WORD_TYPE) // allow higher-level constant resolution
                    {
                        RETURN(make_number(ptr->value), ptr->type);
                    }
                }
            }
            break;
        }
        case k_operator:
        {
            ast_node** op = OPER_OPERANDS(*n);
            int arity = OPER_ARITY(*n);

            switch (OPER_OPERATOR(*n))
            {
                /* Expressions */
                case KSIZEOF:
                {
                    if (frame)
                    {
                        type_list const* type = decode_spec(op[0], frame);
                        RETURN(make_number(type_size(type)), WORD_TYPE);
                    }
                }
                case KCONST:
                {
                    var_list* var = check_add_var(VAR_NAME(op[0]), frame, NULL);
                    if (!var)
                    {
                        error_msg(*n, "Cannot redeclare constant '%s'\n", VAR_NAME(op[0]));
                        exit(1);
                    }
                    type_list* type = NEW_TYPE();
                    type->type = T_CONST;
                    type->const_target = WORD_TYPE;
                    var->type = type;
                    var->value = static_eval(op[1], frame);
                    SET_TYPE(VOID_TYPE);
                }
                case KTYPE:
                {
                    type_list* type = check_add_type(VAR_NAME(op[0]));
                    if (!type)
                    {
                        error_msg(*n, "Cannot redeclare type '%s'\n", VAR_NAME(op[0]));
                        exit(1);
                    }
                    ast_node* spec = op[1];
                    type_list const* nt = decode_spec(spec, frame);
                    linked_list_header h = type->header;
                    *type = *nt;
                    type->header = h;
                    SET_TYPE(VOID_TYPE);
                }
                case KVAR:
                {
                    switch (OPER_ARITY(*n))
                    {
                        case 2:
                        {
                            const type_list* type = op[1] ? decode_spec(op[1], frame) : WORD_TYPE;
                            var_list* var = check_add_var(VAR_NAME(op[0]), frame, type);
                            if (!var)
                            {
                                error_msg(*n, "Cannot redeclare variable '%s'\n", VAR_NAME(op[0]));
                                exit(1);
                            }
                            SET_TYPE(VOID_TYPE);
                        }
                        case 3:
                        {
                            type_list* type = NEW_TYPE();
                            type->type = T_ARRAY;
                            type->array_size = static_eval(op[1], frame);
                            type->array_target = WORD_TYPE;
                            var_list* var = check_add_var(VAR_NAME(op[0]), frame, type);
                            if (!var)
                            {
                                error_msg(*n, "Cannot redeclare array '%s'\n", VAR_NAME(op[0]));
                                exit(1);
                            }
                            ast_node* initial = op[2];
                            if (initial)
                            {
                                var->initial = VAR_NAME(initial);
                            }
                            SET_TYPE(VOID_TYPE);
                        }
                    }
                }
                case KFUNC:
                case KPROC:
                {
                    func_list* newNode = ADD_SYM(func_list, &funcs_head, &funcs_tail);
                    newNode->header.name = VAR_NAME(op[0]);
                    newNode->header.owner = NULL;
                    newNode->is_void = OPER_OPERATOR(n) == KPROC;
                    newNode->arglist = ((ast_linked_list*) op[1])->list;
                    newNode->callsites = NULL;
                    newNode->frame = (stack_frame) {.function = newNode, .is_root = true, .vars = {NULL,
                                                                                                   NULL}, .parent = frame};
                    newNode->code = make_scope(op[2]);
                    SC_SCOPE(newNode->code) = &newNode->frame;
                    for (linked_list* ptr = newNode->arglist; ptr; ptr = ptr->next)
                    {
                        var_list* var = check_add_var(VAR_NAME(ptr->value), &newNode->frame, WORD_TYPE);
                    }

                    static_fold(&newNode->code, &newNode->frame);
                    SET_TYPE(VOID_TYPE);
                }
                case ';':
                {
                    if (arity)
                    {
                        static_fold(&op[0], frame);
                        static_fold(&op[1], frame);
                    }
                    SET_TYPE(VOID_TYPE);
                }
                case '(':
                {
                    for (linked_list* list = ((ast_linked_list*)op[1])->list; list; list = list->next)
                    {
                        static_fold(&list->value, frame);
                    }
                    SET_TYPE(WORD_TYPE);
                }
            }

            struct
            {
                bool is_num;
                int value;
                type_list const* type;
            } * o = malloc(arity * sizeof(*o));
            for (int i = 0; i < arity; i++)
            {
                if (!op[i])
                    continue;
                static_fold(&op[i], frame);
                if ((o[i].is_num = AST_KIND(op[i]) == k_number))
                    o[i].value = NUMBER_VALUE(op[i]);
                if (!AST_INFERRED(op[i]))
                    return;
                o[i].type = decay_array_ptr(AST_INFERRED(op[i]));
            }

            type_list const* result;

            switch(OPER_OPERATOR(*n))
            {
                case '<':
                case '>':
                case GE:
                case LE:
                case NE:
                case EQ:
                {
                    type_list const* left = decay_array_ptr(infer_type(op[0]));
                    type_list const* right = decay_array_ptr(infer_type(op[1]));
                    if (!type_same(left, right))
                    {
                        error_msg(*n, "Cannot perform arithmetic comparison '%c' on different types %s and %s",
                                  OPER_OPERATOR(*n), type_display(left), type_display(right));
                        exit(1);
                    }
                    if (!(left->type == T_SCALAR || left->type == T_POINTER))
                    {
                        error_msg(*n, "Cannot perform arithmetic comparison '%c' on non-numeric types %s and %s\n",
                                  OPER_OPERATOR(*n), type_display(left), type_display(right));
                        exit(1);
                    }
                    result = left;
                    break;
                }

            }

            switch (OPER_OPERATOR(*n))
            {
                /* Expressions */
                case UMINUS:
                {
                    type_list const* left = o[0].type;
                    if (left->type != T_SCALAR)
                    {
                        error_msg(*n, "Cannot perform unary minus on non-integral type %s\n", type_display(left));
                        exit(1);
                    }
                    if (o[0].is_num)
                    {
                        RETURN(make_number(-o[0].value), left);
                    }
                    SET_TYPE(left);
                }
                case '~':
                {
                    type_list const* left = o[0].type;
                    if (left->type != T_SCALAR)
                    {
                        error_msg(*n, "Cannot perform binary complement on non-integral type %s\n", type_display(left));
                        exit(1);
                    }
                    if (o[0].is_num)
                    {
                        RETURN(make_number(~o[0].value), left);
                    }
                    SET_TYPE(left);
                }
                case REF:
                {
                    if (frame)
                    {
                        int ptr;
                        type_list* type;
                        if (AST_KIND(op[0]) == k_ident)
                        {
                            var_list* var = get_var_id(op[0], frame, F_DEFAULT);
                            if (var->type->type == T_CONST)
                            {
                                error_msg(*n, "Cannot take address of constant '%s'\n", var->header.name);
                                exit(1);
                            }
                            ptr = var_position(var);
                            type = NEW_TYPE();
                            type->type = T_POINTER;
                            type->pointer_target = var->type;
                        }
                        else if (AST_KIND(op[0]) == k_number)
                        {
                            ptr = NUMBER_VALUE(op[0]);
                        }
                        else if (AST_KIND(op[0]) == k_operator && OPER_OPERATOR(op[0]) == DEREF)
                        {
                            ast_node* inner = OPER_OPERANDS(op[0])[0];
                            static_fold(&inner, frame);
                            RETURN(inner, AST_INFERRED(inner));
                        }

                        if (op[1] && o[1].is_num)
                        {
                            ptr += o[1].value;
                        }

                        RETURN(make_number(ptr), type);
                    }
                    break;
                }
                case '+':
                {
                    type_list const* left = o[0].type;
                    type_list const* right = o[1].type;
                    type_list const* result;
                    if (left->type == T_POINTER && right->type == T_POINTER)
                    {
                        error_msg(*n, "Cannot add two pointers\n");
                        exit(1);
                    }
                    if (left->type == T_POINTER && right->type == T_SCALAR)
                        result = left;
                    else if (left->type == T_SCALAR && right->type == T_POINTER)
                        result = right;
                    else if (left->type == T_SCALAR && right->type == T_SCALAR)
                        result = left;
                    else
                    {
                        error_msg(*n, "Cannot add types %s and %s\n", type_display(left), type_display(right));
                        exit(1);
                    }
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value + o[1].value), result);
                    }
                    else if (o[0].is_num && o[0].value == 0)
                    {
                        RETURN(op[1], result);
                    }
                    else if (o[1].is_num && o[1].value == 0)
                    {
                        RETURN(op[0], result);
                    }
                    SET_TYPE(result);
                }
                case '-':
                {
                    type_list const* left = o[0].type;
                    type_list const* right = o[1].type;
                    type_list const* result;
                    if (left->type == T_POINTER && right->type == T_POINTER)
                        result = WORD_TYPE;
                    else if (left->type == T_POINTER && right->type == T_SCALAR)
                        result = left;
                    else if (left->type == T_SCALAR && right->type == T_SCALAR)
                        result = left;
                    else
                    {
                        error_msg(*n, "Cannot subtract types %s and %s\n", type_display(left), type_display(right));
                        exit(1);
                    }
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value - o[1].value), result);
                    }
                    else if (o[0].is_num && o[0].value == 0)
                    {
                        RETURN(make_node(UMINUS, 1, op[1]), result);
                    }
                    else if (o[1].is_num && o[1].value == 0)
                    {
                        RETURN(op[0], result);
                    }
                    SET_TYPE(result);
                }
                /*case '*':
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
                    return false;*/
                case '<':
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value < o[1].value), result);
                    }
                    SET_TYPE(result);
                }
                case '>':
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value > o[1].value), result);
                    }
                    SET_TYPE(result);
                }
                case GE:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value >= o[1].value), result);
                    }
                    SET_TYPE(result);
                }
                case LE:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value <= o[1].value), result);
                    }
                    SET_TYPE(result);
                }
                case NE:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value != o[1].value), result);
                    }
                    SET_TYPE(result);
                }
                case EQ:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value == o[1].value), result);
                    }
                    SET_TYPE(result);
                }
                /*case AND:
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
                }*/
                case INC:
                case DEC:
                {
                    type_list const* left = infer_type(op[0]);
                    if (left->type != T_SCALAR && left->type != T_POINTER)
                    {
                        error_msg(*n, "Cannot perform arithmetic operation '%s' on non-numeric type %s\n",
                                  OPER_OPERATOR(*n) == INC ? "++" : "--",
                                  type_display(left));
                        exit(1);
                    }
                    else
                        SET_TYPE(left);
                }
                case '=':
                {
                    type_list const* left = infer_type(op[0]);
                    type_list const* right = infer_type(op[1]);
                    if (!type_same(left, right))
                    {
                        error_msg(*n, "Cannot assign rvalue of type '%s' to lvalue of type '%s'\n", type_display(right), type_display(left));
                        exit(1);
                    }
                    if (AST_KIND(op[1]) == k_ident && !strcmp(VAR_NAME(op[0]), VAR_NAME(op[1])))
                    {
                        info_msg(*n, "Assignment has no effect\n");
                    }
                    SET_TYPE(left);
                }
                case DEREF:
                {
                    type_list const* inner = infer_type(op[0]);
                    if (!inner)
                        break;
                    if (inner->type != T_POINTER)
                    {
                        info_msg(*n, "Dereferencing a non-pointer value\n");
                        break;
                    }
                    SET_TYPE(inner->pointer_target);
                }
                case KFOR:
                case KIF:
                case KWHILE:
                case KDO:
                case KPRINT:
                case KREAD:
                case KBREAK:
                case KCONTINUE:
                case KRETURN:
                    SET_TYPE(VOID_TYPE);
                    /* case KIF:
                     {
                         if (o[0].is_num)
                         {
                             info_msg(*n, "Condition is always %s\n", o[0].value ? "true" : "false");
                             RETURN(o[0].value ? op[1] : op[2]);
                         }
                         return false;
                     }*/
                default:
                    break;
            }
            break;
        }
        case k_list:
        {
           SET_TYPE(VOID_TYPE);
        }
        case k_scope:
        {
            if (!SC_SCOPE(*n))
            {
                stack_frame* sc_frame = malloc(sizeof(*sc_frame));
                *sc_frame = (stack_frame) {.function = frame->function, .is_root = false, .vars = {.head = NULL, .tail = NULL}, .parent = frame};
                SC_SCOPE(*n) = sc_frame;
            }
            AST_INFERRED(*n) = VOID_TYPE;
            static_fold(&SC_CODE(*n), SC_SCOPE(*n));
            return;
        }
    }

    error_msg(*n, "Could not type check\n");
    exit(1);
}

#undef RETURN

int static_eval(ast_node* n, stack_frame* frame)
{
    if (AST_KIND(n) != k_number)
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

    //type_check(n, frame);

    instr("# KIND = %d (%s)", AST_KIND(n), node_kind_NAMES[AST_KIND(n)]);

    bool clean_stack = AST_CLEAN_STACK(n);

    //static_fold(&n, frame);

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
            if (ptr->type->type == T_ARRAY) // array
            {
                push_number(var_position(ptr));
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
                    int ptr = var_position(get_var_id(op[0], frame, F_DEFAULT));
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
                case KTYPE:
                    PROD0("type");
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
                case '{':
                {
                    PROD0("; compound expression statement");
                    exec(op[0], frame, loop);
                    PROD0("; compound expression result");
                    exec(op[1], frame, loop);
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
            exec(SC_CODE(n), SC_SCOPE(n), loop);
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

void nav_to_var(ast_node* op, stack_frame* frame, loop_info* loop)
{
    if (AST_KIND(op) == k_ident)
    {
        instr("# navigating to %s", VAR_NAME(op));
        var_list* var = get_var_id(op, frame, F_DEFAULT);
        if (var->type->type == T_CONST)
        {
            error_msg(op,
                      "Cannot navigate to constant; this usually indicates that an internal error occurred during static analysis\n");
            exit(1);
        }
        int vid = var_position(var);
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




type_list* check_add_type(const char* name)
{
    for (type_list* ptr = types_head; ptr; ptr = (type_list*) ptr->header.next)
    {
        if (!strcmp(ptr->header.name, name))
        {
            return NULL;
        }
    }
    type_list* newNode = ADD_SYM(type_list, &types_head, &types_tail);
    newNode->header.name = name;
    return newNode;
}

/**
 *
 * @param name
 * @param size
 * @param vars_head
 * @param vars_tail
 * @return NULL if the variable already exists
 */
var_list* check_add_var(const char* name, stack_frame* frame, type_list const* type)
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
            if (ptr->type->type != T_CONST)
                i += type_size(ptr->type);
        }
    }
    if (!found)
    {
        var_list* newNode = ADD_SYM(var_list, &frame->vars.head, &frame->vars.tail);
        newNode->header.name = name;
        newNode->header.owner = frame;
        newNode->position = i;
        newNode->initial = NULL;
        newNode->type = type;
        if (type)
            frame->end += type_size(type);
        return newNode;
    }
    return NULL;
}


type_list const* decode_spec(ast_node* spec, stack_frame* frame)
{
    if (AST_KIND(spec) == k_ident)
    {
        return FIND_SYM(type_list, types_head, spec, F_DEFAULT);
//        error_msg(spec, "ALIAS NOT IMPL\n");
//        exit(1);
    }
    else
    {
        switch(OPER_OPERATOR(spec))
        {
            case '*':
            {
                type_list* ret = NEW_TYPE();
                ret->type = T_POINTER;
                ret->pointer_target = decode_spec(OPER_OPERANDS(spec)[0], frame);
                return ret;
            }
            case KCONST:
            {
                type_list* ret = NEW_TYPE();
                ret->type = T_CONST;
                ret->const_target = decode_spec(OPER_OPERANDS(spec)[0], frame);
                return ret;
            }
            case '[':
            {
                type_list* ret = NEW_TYPE();
                ret->type = T_ARRAY;
                ret->array_target = decode_spec(OPER_OPERANDS(spec)[0], frame);
                ret->array_size = static_eval(OPER_OPERANDS(spec)[1], frame);
                return ret;
            }
            case KTYPEOF:
                static_fold(&OPER_OPERANDS(spec)[0], frame);
                return infer_type(OPER_OPERANDS(spec)[0]);
        }
    }
}


void traverse_vars(ast_node* n, stack_frame* frame)
{
    if (!n)
        return;

    if (AST_KIND(n) == k_operator)
    {
        ast_node** op = OPER_OPERANDS(n);

        if (OPER_OPERATOR(n) != KFUNC && OPER_OPERATOR(n) != KPROC)
        {
            for (int i = 0; i < OPER_ARITY(n); i++)
                traverse_vars(op[i], frame);
        }

        if (OPER_OPERATOR(n) == KCONST)
        {

            var_list* var = check_add_var(VAR_NAME(op[0]), frame, NULL);
            if (!var)
            {
                error_msg(n, "Cannot redeclare constant '%s'\n", VAR_NAME(op[0]));
                exit(1);
            }
            type_list* type = NEW_TYPE();
            type->type = T_CONST;
            type->const_target = WORD_TYPE;
            var->type = type;
            var->value = static_eval(op[1], frame);
        }
        else if (OPER_OPERATOR(n) == KTYPE)
        {
            type_list* type = check_add_type(VAR_NAME(op[0]));
            if (!type)
            {
                error_msg(n, "Cannot redeclare type '%s'\n", VAR_NAME(op[0]));
                exit(1);
            }
            ast_node* spec = op[1];
            type_list const* nt = decode_spec(spec, frame);
            linked_list_header h = type->header;
            *type = *nt;
            type->header = h;
        }
        else if (OPER_OPERATOR(n) == KVAR)
        {
            switch (OPER_ARITY(n))
            {
                case 2:
                {
                    const type_list* type = op[1] ? decode_spec(op[1], frame) : WORD_TYPE;
                    var_list* var = check_add_var(VAR_NAME(op[0]), frame, type);
                    if (!var)
                    {
                        error_msg(n, "Cannot redeclare variable '%s'\n", VAR_NAME(op[0]));
                        exit(1);
                    }
                    break;
                }
                case 3:
                {
                    type_list* type = NEW_TYPE();
                    type->type = T_ARRAY;
                    type->array_size = static_eval(op[1], frame);
                    type->array_target = WORD_TYPE;
                    var_list* var = check_add_var(VAR_NAME(op[0]), frame, type);
                    if (!var)
                    {
                        error_msg(n, "Cannot redeclare array '%s'\n", VAR_NAME(op[0]));
                        exit(1);
                    }
                    ast_node* initial = op[2];
                    if (initial)
                    {
                        var->initial = VAR_NAME(initial);
                    }
                    break;
                }
            }
        }
        else if (OPER_ARITY(n) > 0 && OPER_OPERANDS(n)[0] && AST_KIND(op[0]) == k_ident)
        {
            if (OPER_OPERATOR(n) == '=')
            {
                type_list const* value_type = decay_array_ptr(infer_type(op[1]));
                var_list* var = check_add_var(VAR_NAME(op[0]), frame, value_type);
                if (!var)
                {
                    //var = (var_list*) find_symbol(&frame->vars.head->header, op[0], F_DEFAULT);
                }
                else
                {
                }
            }
        }
    }
    else if (AST_KIND(n) == k_scope)
    {
        if (!SC_SCOPE(n))
        {
            stack_frame* sc_frame = malloc(sizeof(*sc_frame));
            *sc_frame = (stack_frame) {.function = frame->function, .is_root = false, .vars = {.head = NULL, .tail = NULL}, .parent = frame};
            SC_SCOPE(n) = sc_frame;
        }
        traverse_vars(SC_CODE(n), SC_SCOPE(n));
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
            newNode->callsites = NULL;
            newNode->frame = (stack_frame) {.function = newNode, .is_root = true, .vars = {NULL,
                                                                                           NULL}, .parent = frame};
            newNode->code = make_scope(OPER_OPERANDS(n)[2]);
            SC_SCOPE(newNode->code) = &newNode->frame;
            for (linked_list* ptr = newNode->arglist; ptr; ptr = ptr->next)
            {
                var_list* var = check_add_var(VAR_NAME(ptr->value), &newNode->frame, WORD_TYPE);
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
    else if (AST_KIND(n) == k_scope)
    {
        traverse_funcs(SC_CODE(n), frame);
    }
}

void allocate_scope(stack_frame* frame)
{
    if (!frame->end)
        return;
    instr("FROM @%d", ++label);
    instr("'/|'0|'1|'[,'[,'_,'_ R,S,S,S");
    instr("'/|'0|'1|'[,'/,'_,'_ R,S,S,S");
    instr("'_,'/|'[,'_,'_ S,R,S,S @%d", label + 1);
    for (var_list* ptr = frame->vars.head; ptr; ptr = (var_list*) ptr->header.next)
    {
        if (ptr->type->type == T_CONST)
            continue;
        instr("# allocate variable %s (position %d, size %d)", ptr->header.name, ptr->position, type_size(ptr->type));
        char* str = ptr->initial;
        for (int j = 0; j < type_size(ptr->type); j++, (str && *str && str++))
        {
            if (str && *str)
            {
                char value = *str;
                for (int i = 0; i < INT_WIDTH; i++, value >>= 1)
                {
                    instr("FROM @%d", ++label);
                    instr("'_,'_,'_,'_ '%d,'_,'_,'_ R,S,S,S @%d", value & 1, label + 1);
                }
            }
            else
            {
                for (int i = 0; i < INT_WIDTH; i++)
                {
                    instr("FROM @%d", ++label);
                    instr("'_,'_,'_,'_ '0,'_,'_,'_ R,S,S,S @%d", label + 1);
                }
            }
            instr("FROM @%d", ++label);
            instr("'_,'_,'_,'_ '/,'_,'_,'_ R,S,S,S @%d", label + 1);
        }
    }
    instr("FROM @%d", ++label);
    instr("'_|'/|'0|'1,'_,'_,'_ L,S,S,S");
    instr("'[,'_,'_,'_ S,L,S,S @%d", label + 1);
}


// ----------------------------------------------------------------------
//		Code production
//			This version produces code for a stack machine
// ---------------------------------------------------------------------
void produce_code(ast_node* n)
{
    optimize = true;

    VOID_TYPE = NEW_TYPE();

    type_list* word_type = ADD_SYM(type_list, &types_head, &types_tail);
    word_type->header.name = "word";
    word_type->type = T_SCALAR;
    word_type->scalar_size = 1;

    static_fold(&n, &global_frame);

    //traverse_vars(n, &global_frame);
    //traverse_funcs(n, &global_frame);

    {
        printf("# Types\n");
        printf("# NAME        VALUE\n");
        for (type_list* ptr = types_head; ptr; ptr = (type_list*) ptr->header.next)
        {
            printf("# %-10s  %-10s\n", ptr->header.name, type_display_full(ptr, true, true));
        }
    }

    {
        printf("# Memory map\n");
        printf("# POSITION  SIZE  NAME        TYPE\n");
        for (var_list* ptr = global_frame.vars.head; ptr; ptr = (var_list*) ptr->header.next)
        {
            if (ptr->type->type == T_CONST)
                continue;
            printf("# %8d  %4d  %-10s  %-10s\n", ptr->position, type_size(ptr->type), ptr->header.name,
                   type_display(ptr->type));
        }
    }

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

    for (func_list* ptr = funcs_head; ptr; ptr = (func_list*) ptr->header.next)
    {
        if (!ptr->callsites)
            continue;
        instr("# %s %s (F%d)", ptr->is_void ? "procedure" : "function", ptr->header.name, ptr->header.id);
        instr("FROM @F%d", ptr->header.id);
        instr("'[,'/|'[,'_,'_ S,S,S,S @%d", label + 1);
        BLOCK("function code",
              {
                  exec(SC_CODE(ptr->code), &ptr->frame, NULL);
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
