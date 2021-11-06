#include "turing.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define NEW_TYPE() (calloc(1, sizeof(type_list)))
#define WORD_TYPE (type_list const*)types_head
#define U16_TYPE (type_list const*)(types_head->header.next)
#define U32_TYPE (type_list const*)(types_head->header.next->next)

/**
 * Adds an item to a linked list
 */
#define ADD_SYM(type, head, tail) ((type*) add_symbol((linked_list_header**)(head), (linked_list_header**)(tail), sizeof(type)))

// Internal.
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


type_list* check_add_type(const char* name);

/**
 * @returns Whether the two type instances refer to the same type
 */
bool type_same(type_list const* a, type_list const* b)
{
    if (a->type != b->type)
        return false;

    switch (a->type)
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

/**
 * @return The size of the specified type
 */
int type_size(type_list const* type)
{
    switch (type->type)
    {
        case T_SCALAR:
            return 1;
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

/**
 * @param inner If true, and the type has a name, return that name
 * @param expand If true, will expand composite types, otherwise will display "<anonymous type>"
 * @return A human-readable textual representation of the specified type
 */
const char* type_display_full(type_list const* type, bool inner, bool expand)
{
    if (!inner && type->header.name)
        return type->header.name;

    switch (type->type)
    {
        case T_VOID:
            return "void";
        case T_SCALAR:
        {
            char* buf = malloc(256);
            sprintf(buf, "u%d", type->scalar_size * 8);
            return buf;
        }
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
            for (var_list* ptr = type->composite_members.head; ptr; ptr = (var_list*) ptr->header.next)
            {
                p += sprintf(buf + p, "%s: %s; ", ptr->header.name, type_display_full(ptr->type, false, true));
            }
            strcat(buf + p, "}");
            return buf;
        }
    }
}

/**
 * @return A human-readable textual representation of the specified type
 */
const char* type_display(type_list const* type)
{
    return type_display_full(type, false, false);
}

/**
 * Statically evaluates the specified node.
 * @throws exit If the node cannot be resolved to a compile-time constant
 */
int static_eval(ast_node* n, stack_frame* frame)
{
    if (AST_KIND(n) != k_number)
    {
        error_msg(n, "Value must be compile-time constant\n");
        exit(1);
    }

    return NUMBER_VALUE(n);
}

/**
 * @return The type of the specified expression node.
 * @throws exit If the type-checker could not resolve a type for the node
 */
type_list const* infer_type(ast_node* n)
{
    if (!n)
        return NULL;

    if (AST_INFERRED(n))
        return AST_INFERRED(n);

    error_msg(n, "Node without type\n");
    exit(1);
}

type_list const* make_pointer(type_list const* target)
{
    type_list* ret = NEW_TYPE();
    ret->type = T_POINTER;
    ret->pointer_target = target;
    return ret;
}

/**
 * @return The pointer type corresponding to the specified array type. If a non-array type is passed, will do nothing
 * @example The array type u8[10] decays to the pointer type u8*
 */
type_list const* decay_array_ptr(type_list const* t)
{
    if (t->type != T_ARRAY)
        return t;

    return make_pointer(t->array_target);
}

/**
 * @return A type instance corresponding to the specified type specification node
 * @throws exit If the node is invalid
 */
type_list const* decode_spec(ast_node* spec, stack_frame* frame)
{
    if (AST_KIND(spec) == k_ident)
    {
        return FIND_SYM(type_list, types_head, spec);
    }
    else
    {
        ast_node** op = OPER_OPERANDS(spec);
        switch (OPER_OPERATOR(spec))
        {
            case '*':
            {
                return make_pointer(decode_spec(op[0], frame));
            }
            case KCONST:
            {
                type_list* ret = NEW_TYPE();
                ret->type = T_CONST;
                ret->const_target = decode_spec(op[0], frame);
                return ret;
            }
            case '[':
            {
                type_list* ret = NEW_TYPE();
                ret->type = T_ARRAY;
                ret->array_target = decode_spec(op[0], frame);
                ret->array_size = static_eval(op[1], frame);
                return ret;
            }
            case KTYPEOF:
                analysis(&op[0], frame);
                return infer_type(op[0]);
            case KSTRUCT:
            {
                type_list* ret = NEW_TYPE();
                ret->type = T_COMPOSITE;
                ast_node* first = op[0], * next = NULL;
                while (first)
                {
                    if (OPER_OPERATOR(first) == ':')
                    {
                        const char* name = VAR_NAME(OPER_OPERANDS(first)[0]);
                        type_list const* type = decode_spec(OPER_OPERANDS(first)[1], frame);

                        int pos = 0;
                        for (var_list* ptr = ret->composite_members.head; ptr; ptr = (var_list*) ptr->header.next)
                        {
                            if (!strcmp(ptr->header.name, name))
                            {
                                error_msg(first, "Duplicate member '%s'\n", name);
                                exit(1);
                            }

                        }

                        if (ret->composite_members.tail)
                        {
                            pos = ret->composite_members.tail->position + type_size(ret->composite_members.tail->type);
                        }

                        var_list* new = ADD_SYM(var_list, &ret->composite_members.head, &ret->composite_members.tail);
                        new->header.name = name;
                        new->header.owner = NULL;
                        new->position = pos;
                        new->initial = NULL;
                        new->type = type;

                        first = next;
                        next = NULL;
                    }
                    else
                    {
                        next = OPER_OPERANDS(first)[1];
                        first = OPER_OPERANDS(first)[0];
                    }
                }
                return ret;
            }
        }
    }

    error_msg(spec, "Invalid type specification\n");
    exit(1);
}


/**
 * @return A new type instance with the specified name, or NULL if a type with the same name already exists
 */
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
 * @return A new variable with the specified name, or NULL if a variable with the same name already exists
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
            if (ptr->type->type != T_CONST) // constants are not stored in memory
                i += type_size(ptr->type);
        }
    }
    if (!found)
    {
        // variable not found, create a new one
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


#define RETURN(x, type) do{*n = x; AST_INFERRED(*n) = type; return;}while(0)
#define RETURN_VAL(x) do{*n = x; return;}while(0)
#define SET_TYPE(type) do {AST_INFERRED(*n) = type; return;}while(0)

ast_node* set_inferred_type(ast_node* n, type_list const* type)
{
    AST_INFERRED(n) = type;
    return n;
}

#define ALWAYS_ZERO() info_msg(*n, "Value is always zero\n")

/**
 * Static analysis + type checking + constant folding
 */
void analysis(ast_node** n, stack_frame* frame)
{
    assert(frame);

    if (!*n)
        return;

    /* if (AST_INFERRED_POS(*n) != *n)
         analysis(&AST_INFERRED_POS(*n), frame);*/

    switch (AST_KIND(*n))
    {
        case k_number:
        {
            // preserve inferred type
            if (!AST_INFERRED(*n))
            {
                unsigned int value = (unsigned int) NUMBER_VALUE(*n);
                if (value >= 65536)
                    SET_TYPE(U32_TYPE);
                else if (value >= 256)
                    SET_TYPE(U16_TYPE);
                else
                    SET_TYPE(WORD_TYPE);
            }
            return;
        }
        case k_ident:
        {
            var_list* ptr = get_var_id(*n, frame, F_NULLABLE);
            if (ptr)
            {
                if (ptr->type->type == T_ARRAY) // referencing an array directly is equivalent to &array[0]
                {
                    ast_node* ret = set_inferred_type(make_number(var_position(ptr)), decay_array_ptr(ptr->type));
                    AST_INFERRED_POS(ret) = ret;
                    RETURN_VAL(ret);
                }
                AST_INFERRED_POS(*n) = set_inferred_type(make_number(var_position(ptr)), make_pointer(ptr->type));
                SET_TYPE(ptr->type);
            }
            ptr = get_var_id(*n, frame, F_RECURSE);
            if (ptr->type->type == T_CONST &&
                ptr->type->const_target == WORD_TYPE) // allow resolving constants for parent frames
            {
                RETURN(make_number(ptr->value), ptr->type);
            }
            missing_symbol(*n, VAR_NAME(*n));
        }
        case k_operator:
        {
            ast_node** op = OPER_OPERANDS(*n);
            int arity = OPER_ARITY(*n);

            switch (OPER_OPERATOR(*n))
            {
                case '.':
                {
                    analysis(&op[0], frame);
                    type_list const* ltype = infer_type(op[0]);
                    if (ltype->type != T_COMPOSITE)
                    {
                        error_msg(*n, "Member access on non-composite type\n");
                        exit(1);
                    }
                    var_list* member = FIND_SYM(var_list, ltype->composite_members.head, op[1]);
                    AST_INFERRED_POS(*n) = set_inferred_type(
                            make_node('+', 2, op[0]->inferred_position, make_number(member->position)),
                            make_pointer(member->type));
                    analysis(&AST_INFERRED_POS(*n), frame);
                    SET_TYPE(member->type);
                }
                case KSIZEOF:
                {
                    type_list const* type = decode_spec(op[0], frame);
                    RETURN(make_number(type_size(type)), WORD_TYPE);
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
                    newNode->return_type = OPER_OPERATOR(*n) == KPROC ? VOID_TYPE :
                                           (op[3] ? decode_spec(op[3], frame) : WORD_TYPE);
                    newNode->arglist = ((ast_linked_list*) op[1])->list;
                    newNode->callsites = NULL;
                    newNode->frame = (stack_frame) {
                            .function = newNode,
                            .loop = NULL,
                            .is_root = true,
                            .vars = {NULL, NULL},
                            .parent = frame
                    };
                    newNode->code = make_scope(op[2]);
                    SC_SCOPE(newNode->code) = &newNode->frame;
                    for (linked_list* ptr = newNode->arglist; ptr; ptr = ptr->next)
                    {
                        check_add_var(VAR_NAME(OPER_OPERANDS(ptr->value)[0]), &newNode->frame,
                                      decode_spec(OPER_OPERANDS(ptr->value)[1], frame));
                    }
                    analysis(&newNode->code, &newNode->frame);
                    SET_TYPE(VOID_TYPE);
                }
                case ';':
                {
                    if (arity)
                    {
                        analysis(&op[0], frame);
                        analysis(&op[1], frame);
                    }
                    SET_TYPE(VOID_TYPE);
                }
                case '(':
                {
                    func_list* func = FIND_SYM(func_list, funcs_head, op[0]);
                    linked_list* list, *flist;
                    var_list* fargs;
                    int i = 1;
                    for (list = ((ast_linked_list*) op[1])->list,
                            flist = func->arglist, fargs = func->frame.vars.head;
                         list && flist;
                         list = list->next, flist = flist->next, fargs = (var_list*)fargs->header.next, i++)
                    {
                        analysis(&list->value, frame);
                        if (!type_same(infer_type(list->value), fargs->type))
                        {
                            error_msg(*n, "Type mismatch for argument %d in call to '%s'; expected %s, got %s\n",
                                      i, type_display(fargs->type), type_display(infer_type(list->value)));
                            exit(1);
                        }
                    }
                    if (flist || list)
                    {
                        error_msg(*n, "Invalid number of arguments in call to '%s'\n", func->header.name);
                        exit(1);
                    }
                    SET_TYPE(func->return_type);
                }
                case KDO:
                {
                    op[0] = make_scope(op[0]);
                    stack_frame* sc_frame = malloc(sizeof(*sc_frame));
                    *sc_frame = (stack_frame) {.function = frame->function, .loop = malloc(sizeof(loop_info)), .is_root = false, .vars = {.head = NULL, .tail = NULL}, .parent = frame};
                    SC_SCOPE(op[0]) = sc_frame;
                    break;
                }
                case KFOR:
                {
                    op[3] = make_scope(op[3]);
                    stack_frame* sc_frame = malloc(sizeof(*sc_frame));
                    *sc_frame = (stack_frame) {.function = frame->function, .loop = malloc(sizeof(loop_info)), .is_root = false, .vars = {.head = NULL, .tail = NULL}, .parent = frame};
                    SC_SCOPE(op[3]) = sc_frame;
                    break;
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
                analysis(&op[i], frame);
                if ((o[i].is_num = AST_KIND(op[i]) == k_number))
                    o[i].value = NUMBER_VALUE(op[i]);
                if (!AST_INFERRED(op[i]))
                    return;
                o[i].type = decay_array_ptr(AST_INFERRED(op[i]));
            }

            type_list const* result;

            // Common type checks
            switch (OPER_OPERATOR(*n))
            {
                case '<':
                case '>':
                case GE:
                case LE:
                case NE:
                case EQ:
                {
                    type_list const* left = o[0].type;
                    type_list const* right = o[1].type;
                    if (!type_same(left, right))
                    {
                        error_msg(*n, "Cannot perform arithmetic comparison '%c' on different types %s and %s",
                                  OPER_OPERATOR(*n), type_display(left), type_display(right));
                        exit(1);
                    }
                }
            }
            switch (OPER_OPERATOR(*n))
            {
                case '<':
                case '>':
                case GE:
                case LE:
                case NE:
                case EQ:
                {
                    type_list const* left = o[0].type;
                    type_list const* right = o[1].type;

                    if (!(left->type == T_SCALAR || left->type == T_POINTER))
                    {
                        error_msg(*n, "Cannot perform arithmetic comparison '%c' on non-numeric types %s and %s\n",
                                  OPER_OPERATOR(*n), type_display(left), type_display(right));
                        exit(1);
                    }
                    result = left;
                    break;
                }
                case SHL:
                case SHR:
                {
                    type_list const* left = o[0].type;
                    type_list const* right = o[1].type;

                    if (left->type != T_SCALAR)
                    {
                        error_msg(*n, "Cannot perform bitwise shift on non-numeric types %s and %s\n",
                                  type_display(left), type_display(right));
                        exit(1);
                    }
                    result = left;
                    break;
                }
            }

            switch (OPER_OPERATOR(*n))
            {
                case SHL:
                {
                    type_list const* left = o[0].type;
                    if (o[0].is_num && o[0].value == 0 || o[1].is_num && o[1].value >= (INT_WIDTH * left->scalar_size))
                    {
                        ALWAYS_ZERO();
                        RETURN(make_number(0), left);
                    }
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value << o[1].value), left);
                    }
                    SET_TYPE(left);
                }
                case SHR:
                {
                    type_list const* left = o[0].type;
                    if (o[0].is_num && o[0].value == 0 || o[1].is_num && o[1].value >= (INT_WIDTH * left->scalar_size))
                    {
                        ALWAYS_ZERO();
                        RETURN(make_number(0), left);
                    }
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value >> o[1].value), left);
                    }
                    SET_TYPE(left);
                }
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
                    ast_node* pos = AST_INFERRED_POS(op[0]);
                    if (!pos)
                    {
                        error_msg(*n, "Cannot take address of expression\n");
                        exit(1);
                    }
                    *n = pos;
                    return;
                }
                case '+':
                {
                    type_list const* left = o[0].type;
                    type_list const* right = o[1].type;
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
                case '*':
                {
                    type_list const* left = infer_type(op[0]);
                    type_list const* right = infer_type(op[1]);
                    if (left->type != T_SCALAR || right->type != T_SCALAR)
                    {
                        error_msg(*n, "Cannot perform multiplication on types %s and %s\n",
                                  type_display(left), type_display(right));
                        exit(1);
                    }
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value * o[1].value), left);
                    }
                    else if (o[0].is_num)
                    {
                        if (o[0].value == 1)
                        {
                            RETURN(op[1], left);
                        }
                        else if (o[0].value == 0)
                        {
                            RETURN(make_number(0), left);
                        }
                    }
                    else if (o[1].is_num)
                    {
                        if (o[1].value == 1)
                        {
                            RETURN(op[0], left);
                        }
                        else if (o[1].value == 0)
                        {
                            RETURN(make_number(0), left);
                        }
                    }
                    SET_TYPE(left);
                }
                    /*case '/':
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
                case AND:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value && o[1].value), WORD_TYPE);
                    }
                    SET_TYPE(WORD_TYPE);
                }
                case OR:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value || o[1].value), WORD_TYPE);
                    }
                    SET_TYPE(WORD_TYPE); //TODO
                }
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
                    if (left->type == T_SCALAR && right->type == T_SCALAR && left->scalar_size >= right->scalar_size)
                    {
                        // ok, can upcast scalar to bigger scalar without loss
                    }
                    else
                    {
                        if (!type_same(left, right))
                        {
                            error_msg(*n, "Cannot assign rvalue of type '%s' to lvalue of type '%s'\n",
                                      type_display(right),
                                      type_display(left));
                            exit(1);
                        }
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
                    AST_INFERRED_POS(*n) = op[0];
                    SET_TYPE(inner->pointer_target);
                }
                case KREAD:
                    SET_TYPE(VOID_TYPE);
                case KBREAK:
                {
                    if (!frame->loop)
                    {
                        error_msg(*n, "Break statement only permitted in loop\n");
                        exit(1);
                    }
                    SET_TYPE(VOID_TYPE);
                }
                case KCONTINUE:
                {
                    if (!frame->loop)
                    {
                        error_msg(*n, "Continue statement only permitted in loop\n");
                        exit(1);
                    }
                    SET_TYPE(VOID_TYPE);
                }
                case KIF:
                {
                    if (infer_type(op[0]) == VOID_TYPE)
                    {
                        error_msg(op[0], "Expected condition, got void\n");
                        exit(1);
                    }
                    SET_TYPE(VOID_TYPE);
                }
                case KDO:
                {
                    if (infer_type(op[1]) == VOID_TYPE)
                    {
                        error_msg(op[1], "Expected condition, got void\n");
                        exit(1);
                    }
                    SET_TYPE(VOID_TYPE);
                }
                case KFOR:
                {
                    if (op[1] && infer_type(op[1]) == VOID_TYPE)
                    {
                        error_msg(op[1], "Expected condition, got void\n");
                        exit(1);
                    }
                    SET_TYPE(VOID_TYPE);
                }
                case KPRINT:
                {
                    if (infer_type(op[0]) == VOID_TYPE)
                    {
                        error_msg(op[0], "Expected value, got void\n");
                        exit(1);
                    }
                    SET_TYPE(VOID_TYPE);
                }
                case KRETURN:
                {
                    if (!frame->function)
                    {
                        error_msg(*n, "Can only return from a function or procedure\n");
                        exit(1);
                    }
                    if (frame->function->return_type == VOID_TYPE)
                    {
                        if (op[0])
                        {
                            error_msg(op[0], "Cannot return value from procedure\n");
                            exit(1);
                        }
                    }
                    else
                    {
                        if (!op[0])
                        {
                            error_msg(*n, "Function must return a value\n");
                            exit(1);
                        }

                        if (!type_same(infer_type(op[0]), frame->function->return_type))
                        {
                            error_msg(op[0], "Return type mismatch; expected %s got %s\n",
                                      type_display(frame->function->return_type), type_display(infer_type(op[0])));
                            exit(1);
                        }
                    }
                    SET_TYPE(VOID_TYPE);
                }
                case '{':
                    SET_TYPE(infer_type(op[1]));
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
                *sc_frame = (stack_frame) {.function = frame->function, .loop = frame->loop, .is_root = false, .vars = {.head = NULL, .tail = NULL}, .parent = frame};
                SC_SCOPE(*n) = sc_frame;
            }
            AST_INFERRED(*n) = VOID_TYPE;
            analysis(&SC_CODE(*n), SC_SCOPE(*n));
            return;
        }
    }

    error_msg(*n, "Could not type check\n");
    exit(1);
}

#undef RETURN
#undef SET_TYPE

/**
 * Initialize builtin types (void and u*)
 */
void init_builtin_types()
{
    VOID_TYPE = NEW_TYPE();

    for (int i = 1; i <= 4; i *= 2)
    {
        type_list* word_type = ADD_SYM(type_list, &types_head, &types_tail);
        char* name = malloc(8);
        sprintf(name, "u%d", 8 * i);
        word_type->header.name = name;
        word_type->type = T_SCALAR;
        word_type->scalar_size = i;
    }
}