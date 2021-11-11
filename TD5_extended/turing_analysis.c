#include "turing.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define NEW_TYPE() (calloc(1, sizeof(type_list)))

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


type_list* check_add_type(const char* name, stack_frame* frame);

/**
 * @returns Whether the two type instances refer to the same type
 */
bool type_same(type_list const* a, type_list const* b)
{
    if (a == b)
        return true;

    if (a->type != b->type)
        return false;

    switch (a->type)
    {
        case T_SCALAR:
            return a->scalar_bits == b->scalar_bits;
        case T_ARRAY:
            return a->array_count == b->array_count && type_same(a->array_target, b->array_target);
        case T_POINTER:
            return type_same(a->pointer_target, b->pointer_target);
        case T_CONST:
            return type_same(a->const_target, b->const_target);
        case T_COMPOSITE:
            return false;
    }
}

int type_size_bits(type_list const* type)
{
    switch (type->type)
    {
        case T_SCALAR:
            return type->scalar_bits;
        case T_POINTER:
            return POINTER_BITS;
        case T_GENERIC:
        {
            error_msg(NULL, "Generic type '%s' must be instanciated\n", type_display(type));
            exit(1);
        }
        default:
        {
            error_msg(NULL, "Cannot calculate size of type '%s'\n", type_display(type));
            exit(1);
        }
    }
}

/**
 * @return The size of the specified type
 */
int type_size_cells(type_list const* type)
{
    switch (type->type)
    {
        case T_SCALAR:
            return 1;
        case T_ARRAY:
            return type->array_count;
        case T_POINTER:
            return 1;
        case T_CONST:
            return type_size_cells(type->const_target);
        case T_COMPOSITE:
        {
            int sum = 0;
            for (var_list* ptr = type->composite_members.head; ptr; ptr = (var_list*) ptr->header.next)
            {
                sum += type_size_cells(ptr->type);
            }
            return sum;
        }
        case T_GENERIC:
        {
            error_msg(NULL, "Generic type '%s' must be instanciated\n", type_display(type));
            exit(1);
        }
        default:
        {
            error_msg(NULL, "Cannot calculate size of type '%s'\n", type_display(type));
            exit(1);
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
            if (type->scalar_bits == 1)
                return "bool";
            char* buf = malloc(256);
            sprintf(buf, "u%d", type->scalar_bits);
            return buf;
        }
        case T_ARRAY:
        {
            char* buf = malloc(256);
            sprintf(buf, "%s[%d]", type_display(type->array_target), type->array_count);
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
                p += sprintf(buf + p, "%s: %s; ", ptr->header.name, type_display_full(ptr->type, inner, true));
            }
            strcat(buf + p, "}");
            return buf;
        }
        case T_GENERIC:
            return "<generic type>";
        default:
            return "<unknown>";
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

type_list const* make_scalar_type(int size)
{
    type_list* res = NEW_TYPE();
    res->type = T_SCALAR;
    res->scalar_bits = size;
    return res;
}

/**
 * @return A type instance corresponding to the specified type specification node
 * @throws exit If the node is invalid
 */
type_list const* decode_spec(ast_node* spec, stack_frame* frame)
{
    if (AST_KIND(spec) == k_ident)
    {
        const char* name = VAR_NAME(spec);
        if (name[0] == 'u')
        {
            char* end = NULL;
            int size = (int) strtol(name + 1, &end, 10);
            if (!*end)
            {
                return make_scalar_type(size);
            }
        }
        return get_type(spec, frame);
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
                ret->array_count = static_eval(op[1], frame);
                return ret;
            }
            case KTYPEOF:
                analysis(&op[0], frame);
                return infer_type(op[0]);
            case KNEW:
            {
                type_list* generic = get_type(op[0], frame);
                stack_frame* sc_frame = malloc(sizeof(*sc_frame));
                *sc_frame = (stack_frame) {.function = frame->function, .loop = frame->loop, .is_root = false, .parent = frame};
                linked_list* param, *arg;
                for (param = generic->generic.params,
                        arg = ((ast_linked_list*)op[1])->list; param && arg; param = param->next, arg = arg->next)
                {
                    type_list* argt = check_add_type(VAR_NAME(param->value), sc_frame);
                    linked_list_header h = argt->header;
                    *argt = *decode_spec(arg->value, frame);
                    argt->header = h;
                }
                if (param || arg)
                {
                    error_msg(spec, "Invalid number of arguments in instanciation of generic type\n");
                    exit(1);
                }
                return decode_spec(generic->generic.spec, sc_frame);
            }
            case KSTRUCT:
            {
                type_list* ret = NEW_TYPE();
                ret->type = T_COMPOSITE;
                for (linked_list* list = ((ast_linked_list*)op[0])->list; list; list = list->next)
                {
                    ast_node* field = list->value;
                    const char* name = VAR_NAME(OPER_OPERANDS(field)[0]);
                    type_list const* type = decode_spec(OPER_OPERANDS(field)[1], frame);

                    int pos = 0;
                    for (var_list* ptr = ret->composite_members.head; ptr; ptr = (var_list*) ptr->header.next)
                    {
                        if (!strcmp(ptr->header.name, name))
                        {
                            error_msg(field, "Duplicate member '%s'\n", name);
                            exit(1);
                        }

                    }

                    if (ret->composite_members.tail)
                    {
                        pos = ret->composite_members.tail->position +
                              type_size_cells(ret->composite_members.tail->type);
                    }

                    var_list* new = ADD_SYM(var_list, &ret->composite_members.head, &ret->composite_members.tail);
                    new->header.name = name;
                    new->header.owner = NULL;
                    new->position = pos;
                    new->initial = NULL;
                    new->type = type;
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
type_list* check_add_type(const char* name, stack_frame* frame)
{
    for (type_list* ptr = frame->types.head; ptr; ptr = (type_list*) ptr->header.next)
    {
        if (!strcmp(ptr->header.name, name))
        {
            return NULL;
        }
    }
    type_list* newNode = ADD_SYM(type_list, &frame->types.head, &frame->types.tail);
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
                i += type_size_cells(ptr->type);
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
            frame->size += type_size_cells(type);
        return newNode;
    }
    return NULL;
}


#define RETURN(x, type) do{*n = x; AST_INFERRED(*n) = type; return;}while(0)
#define RETURN_VAL(x) do{*n = x; return;}while(0)
#define SET_TYPE(type) do {AST_INFERRED(*n) = type; return;}while(0)
#define RETURN_LVALUE(x) do {*n = make_node(DEREF, 1, x); analysis(n, frame); return;}while(0)

ast_node* set_inferred_type(ast_node* n, type_list const* type)
{
    AST_INFERRED(n) = type;
    return n;
}

#define ALWAYS_ZERO() info_msg(*n, "Value is always zero\n")

ast_node* get_position(ast_node* n)
{
    if (AST_KIND(n) != k_operator && OPER_OPERATOR(n) != DEREF)
    {
        error_msg(n, "Cannot take address of expression\n");
        exit(1);
    }

    return OPER_OPERANDS(n)[0];
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

bool is_power_of_2(int x)
{
    return x && !(x & (x - 1));
}

int highest_set_bit(int x) // uses ctz
{
    return __builtin_ctz(x);
}

/**
 * Static analysis + type checking + constant folding
 */
void analysis(ast_node** n, stack_frame* frame)
{
    assert(frame);

    if (!*n)
        return;

    yylloc.first_line = (*n)->info.line;
    yylloc.first_column = (*n)->info.col;
    yylloc.code = (*n)->info.code;

    /* if (AST_INFERRED_POS(*n) != *n)
         analysis(&AST_INFERRED_POS(*n), frame);*/

    switch (AST_KIND(*n))
    {
        case k_number:
        {
            // preserve inferred type
            if (!AST_INFERRED(*n))
            {
                int size = NUMBER_SIZE(*n);
                if (size == 0)
                {
                    unsigned int value = (unsigned int) NUMBER_VALUE(*n);
                    if (value >= 65536)
                        SET_TYPE(make_scalar_type(32));
                    else if (value >= 256)
                        SET_TYPE(make_scalar_type(16));
                    else
                        SET_TYPE(WORD_TYPE);
                }
                else
                {
                    NUMBER_VALUE(*n) &= (1L << size) - 1;
                    SET_TYPE(make_scalar_type(size));
                }
            }
            return;
        }
        case k_ident:
        {
            if (AST_INFERRED(*n))
                return;
            var_list* ptr = get_var_id(*n, frame, F_NULLABLE);
            if (ptr)
            {
                if (ptr->type->type == T_ARRAY) // referencing an array directly is equivalent to &array[0]
                {
                    ast_node* ret = set_inferred_type(make_number(var_position(ptr)), decay_array_ptr(ptr->type));
                    *n = ret;
                    return;
                }
                RETURN_LVALUE(set_inferred_type(make_number(var_position(ptr)), make_pointer(ptr->type)));
            }
            ptr = get_var_id(*n, frame, F_RECURSE);
            if (ptr->type->type == T_CONST &&
                ptr->type->const_target->type == T_SCALAR) // allow resolving constants for parent frames
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
                case KASSERT:
                {
                    analysis(&op[0], frame);
                    if (AST_KIND(op[0]) == k_number)
                    {
                        if (NUMBER_VALUE(op[0]) != 0)
                            SET_TYPE(VOID_TYPE);
                        else
                        {
                            error_msg(*n, "Assertion failed\n");
                            exit(1);
                        }
                    }
                    else
                    {
                        error_msg(*n, "Assertion condition must numeric and resolvable at compile time\n");
                        exit(1);
                    }
                }
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
                    ast_node* lhs_ptr = get_position(op[0]);
                    set_inferred_type(lhs_ptr, make_pointer(member->type)); // convert to pointer to member
                    ast_node* ret = make_node('+', 2, lhs_ptr, make_number(member->position));
                    analysis(&ret, frame);
                    RETURN_LVALUE(ret);
                }
                case KSIZEOF:
                {
                    type_list const* type = decode_spec(op[0], frame);
                    RETURN(make_number(type_size_cells(type)), WORD_TYPE);
                }
                case KBITSOF:
                {
                    type_list const* type = decode_spec(op[0], frame);
                    RETURN(make_number(type_size_bits(type)), WORD_TYPE);
                }
                case KNEW:
                {
                    type_list const* type = decode_spec(op[0], frame);
                    SET_TYPE(make_pointer(type));
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
                    type_list* type = check_add_type(VAR_NAME(op[0]), frame);
                    if (!type)
                    {
                        error_msg(*n, "Cannot redeclare type '%s'\n", VAR_NAME(op[0]));
                        exit(1);
                    }
                    ast_node* spec = op[1];
                    ast_node* args = op[2];
                    stack_frame* new_frame = frame;
                    if (args) // type is generic
                    {
                        type->type = T_GENERIC;
                        type->generic.params = ((ast_linked_list*)args)->list;
                        type->generic.spec = spec;
                        /*stack_frame* sc_frame = malloc(sizeof(*sc_frame));
                        *sc_frame = (stack_frame) {.function = frame->function, .loop = frame->loop, .is_root = false, .parent = frame};
                        for (linked_list* arg = ((ast_linked_list*)args)->list; arg; arg = arg->next)
                        {
                            type_list* argt = check_add_type(VAR_NAME(arg->value), frame);
                            argt->type = T_GENERIC_VARIABLE;
                        }
                        new_frame = sc_frame;*/
                    }
                    else
                    {
                        linked_list_header h = type->header;
                        *type = *decode_spec(spec, new_frame);
                        type->header = h;
                    }
                    printf("# %-10s  %-10s\n", VAR_NAME(op[0]), type_display_full(type, true, true));
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
                            type->array_count = static_eval(op[1], frame);
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
                        SET_TYPE(infer_type(op[1]));
                    }
                    SET_TYPE(VOID_TYPE);
                }
                case '(':
                {
                    if (!AST_DATA(*n))
                    {
                        func_list* func = FIND_SYM(func_list, funcs_head, op[0]);
                        linked_list* list, * flist;
                        var_list* fargs;
                        int i = 1;
                        for (list = ((ast_linked_list*) op[1])->list,
                             flist = func->arglist, fargs = func->frame.vars.head;
                             list && flist;
                             list = list->next, flist = flist->next, fargs = (var_list*) fargs->header.next, i++)
                        {
                            analysis(&list->value, frame);
                            if (!type_same(infer_type(list->value), fargs->type))
                            {
                                error_msg(list->value,
                                          "Type mismatch for argument %d in call to '%s'; expected %s, got %s\n",
                                          i, func->header.name, type_display(fargs->type),
                                          type_display(infer_type(list->value)));
                                exit(1);
                            }
                        }
                        if (flist || list)
                        {
                            error_msg(*n, "Invalid number of arguments in call to '%s'\n", func->header.name);
                            exit(1);
                        }
                        AST_DATA(*n) = add_call_site(&func->callsites);
                        SET_TYPE(func->return_type);
                    }
                    return;
                }
                case KDO:
                {
                    op[0] = make_scope(op[0]);
                    stack_frame* sc_frame = malloc(sizeof(*sc_frame));
                    *sc_frame = (stack_frame) {.function = frame->function, .loop = malloc(
                            sizeof(loop_info)), .is_root = false, .vars = {.head = NULL, .tail = NULL}, .parent = frame};
                    SC_SCOPE(op[0]) = sc_frame;
                    analysis(&op[0], sc_frame);
                    analysis(&op[1], sc_frame);
                    break;
                }
                case KLOOP:
                {
                    op[0] = make_scope(op[0]);
                    stack_frame* sc_frame = malloc(sizeof(*sc_frame));
                    *sc_frame = (stack_frame) {.function = frame->function, .loop = malloc(
                            sizeof(loop_info)), .is_root = false, .vars = {.head = NULL, .tail = NULL}, .parent = frame};
                    SC_SCOPE(op[0]) = sc_frame;
                    analysis(&op[0], sc_frame);
                    break;
                }
                case KFOR:
                {
                    op[3] = make_scope(op[3]);
                    stack_frame* sc_frame = malloc(sizeof(*sc_frame));
                    *sc_frame = (stack_frame) {.function = frame->function, .loop = malloc(
                            sizeof(loop_info)), .is_root = false, .vars = {.head = NULL, .tail = NULL}, .parent = frame};
                    SC_SCOPE(op[3]) = sc_frame;
                    analysis(&op[0], sc_frame);
                    analysis(&op[1], sc_frame);
                    analysis(&op[2], sc_frame);
                    analysis(&op[3], sc_frame);
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
                o[i].type = decay_array_ptr(infer_type(op[i]));
            }

            type_list const* result;

            switch (OPER_OPERATOR(*n))
            {
                case '{':
                {
                    SET_TYPE(o[1].type);
                }

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
                    if (o[0].is_num && o[0].value == 0 || o[1].is_num && o[1].value >= left->scalar_bits)
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
                    if (o[0].is_num && o[0].value == 0 || o[1].is_num && o[1].value >= left->scalar_bits)
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
                    *n = get_position(op[0]);
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
                        else if (is_power_of_2(o[0].value))
                        {
                            *n = make_node(SHL, 2, op[1], make_number(highest_set_bit(o[0].value)));
                            analysis(n, frame);
                            return;
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
                        else if (is_power_of_2(o[1].value))
                        {
                            *n = make_node(SHL, 2, op[0], make_number(highest_set_bit(o[1].value)));
                            analysis(n, frame);
                            return;
                        }
                    }
                    SET_TYPE(left);
                }
                case '/':
                {
                    type_list const* left = infer_type(op[0]);
                    type_list const* right = infer_type(op[1]);
                    if (left->type != T_SCALAR || right->type != T_SCALAR)
                    {
                        error_msg(*n, "Cannot perform division on types %s and %s\n",
                                  type_display(left), type_display(right));
                        exit(1);
                    }
                    if (o[1].is_num && o[1].value == 0)
                    {
                        error_msg(*n, "Division by zero\n");
                        exit(1);
                    }
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value / o[1].value), left);
                    }
                    else if (o[0].is_num)
                    {
                        if (o[0].value == 0)
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
                        else if (is_power_of_2(o[1].value))
                        {
                            *n = make_node(SHR, 2, op[0], make_number(highest_set_bit(o[1].value)));
                            analysis(n, frame);
                            return;
                        }
                    }
                    SET_TYPE(left);
                }
                case '<':
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value < o[1].value), BOOL_TYPE);
                    }
                    SET_TYPE(BOOL_TYPE);
                }
                case '>':
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value > o[1].value), BOOL_TYPE);
                    }
                    SET_TYPE(BOOL_TYPE);
                }
                case GE:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value >= o[1].value), BOOL_TYPE);
                    }
                    SET_TYPE(BOOL_TYPE);
                }
                case LE:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value <= o[1].value), BOOL_TYPE);
                    }
                    SET_TYPE(BOOL_TYPE);
                }
                case NE:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value != o[1].value), BOOL_TYPE);
                    }
                    SET_TYPE(BOOL_TYPE);
                }
                case EQ:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value == o[1].value), BOOL_TYPE);
                    }
                    SET_TYPE(BOOL_TYPE);
                }
                case AND:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value && o[1].value), BOOL_TYPE);
                    }
                    SET_TYPE(BOOL_TYPE);
                }
                case OR:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value || o[1].value), BOOL_TYPE);
                    }
                    SET_TYPE(BOOL_TYPE); //TODO
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
                    if (left->type == T_SCALAR && right->type == T_SCALAR && left->scalar_bits >= right->scalar_bits)
                    {
                        // ok, can upcast scalar to bigger scalar without loss
                    }
                    else
                    {
                        if (!type_same(left, right))
                        {
                            error_msg(op[1], "Cannot assign rvalue of type '%s' to lvalue of type '%s'\n",
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
                    if (AST_INFERRED(*n))
                        return;
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
                    if (o[0].is_num)
                    {
                        if (o[0].value == 0)
                        {
                            *n = op[2]; // ELSE
                        }
                        else
                        {
                            *n = op[1]; // THEN
                        }
                        return;
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
                    if (o[1].is_num && o[1].value == 0)
                    {
                        *n = op[0];
                        return;
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
                    if (o[1].is_num && o[1].value == 0)
                    {
                        *n = op[0];
                        return;
                    }
                    SET_TYPE(VOID_TYPE);
                }
                case KLOOP:
                {
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
            if (!AST_INFERRED(*n))
            {
                if (!SC_SCOPE(*n))
                {
                    stack_frame* sc_frame = malloc(sizeof(*sc_frame));
                    *sc_frame = (stack_frame) {.function = frame->function, .loop = frame->loop, .is_root = false, .vars = {.head = NULL, .tail = NULL}, .parent = frame};
                    SC_SCOPE(*n) = sc_frame;
                }
                analysis(&SC_CODE(*n), SC_SCOPE(*n));
                AST_INFERRED(*n) = infer_type(SC_CODE(*n));
            }
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

    type_list* word_type = ADD_SYM(type_list, &global_frame.types.head, &global_frame.types.tail);
    word_type->header.name = "u8";
    word_type->type = T_SCALAR;
    word_type->scalar_bits = 8;
    WORD_TYPE = word_type;

    type_list* bool_type = ADD_SYM(type_list, &global_frame.types.head, &global_frame.types.tail);
    bool_type->header.name = "bool";
    bool_type->type = T_SCALAR;
    bool_type->scalar_bits = 1;
    BOOL_TYPE = bool_type;
}