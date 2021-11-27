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

void add_symbol_existing(linked_list_header** head, linked_list_header** tail, linked_list_header* newNode)
{
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
}

// Internal.
linked_list_header* add_symbol(linked_list_header** head, linked_list_header** tail, size_t size)
{
    linked_list_header* newNode = malloc(size);
    newNode->next = NULL;
    add_symbol_existing(head, tail, newNode);
    return newNode;
}


type_list* check_add_type(const char* name, stack_frame* frame);

bool is_scalar(type_list const* type)
{
    return type->type == T_SCALAR;
}

bool is_pointer(type_list const* type)
{
    return type->type == T_POINTER;
}

/**
 * @return Whether the specified type is a scalar or a pointer
 */
bool is_numeric(type_list const* type)
{
    return type && (is_scalar(type) || is_pointer(type));
}

/**
 * @returns Whether the two type instances refer to the same type
 */
bool type_assignable(type_list const* given, type_list const* wanted)
{
    given = unalias(given);
    wanted = unalias(wanted);

    if (given == wanted)
        return true;

    if (given->type != wanted->type)
        return false;

    switch (given->type)
    {
        case T_SCALAR:
            return given->scalar_bits == wanted->scalar_bits;
        case T_ARRAY:
            return given->array_count == wanted->array_count &&
                   type_assignable(given->array_target, wanted->array_target);
        case T_POINTER:
            return type_assignable(given->pointer_target, wanted->pointer_target) &&
                   !(given->pointer_target->is_const && !wanted->pointer_target->is_const) &&
                   (given->pointer_is_global == wanted->pointer_is_global);
        case T_COMPOSITE:
            return given->composite_members.head == wanted->composite_members.head
                   && given->composite_methods.head == wanted->composite_methods.head;
    }
}

type_list const* make_scalar_type(int size)
{
    type_list* res = NEW_TYPE();
    res->type = T_SCALAR;
    res->scalar_bits = size;
    return res;
}

bool type_compatible(type_list const** given, type_list const* wanted)
{
    type_list const* gtype = unalias(*given);
    wanted = unalias(wanted);

    if (type_assignable(gtype, wanted))
        return true;

    if (is_scalar(gtype) && is_scalar(wanted))
    {
        if ((gtype)->scalar_bits == 0)
        {
            *given = make_scalar_type(wanted->scalar_bits);
            return true;
        }
    }

    return false;
}

bool type_compatible_symmetric(type_list const** a, type_list const** b)
{
    if (type_assignable(*a, *b))
        return true;

    if (is_scalar(*a) && (*a)->scalar_bits == 0 && is_numeric(*b))
    {
        *a = make_scalar_type(type_size_bits(*b));
        return true;
    }

    if (is_scalar(*b) && (*b)->scalar_bits == 0 && is_numeric(*a))
    {
        *b = make_scalar_type(type_size_bits(*a));
        return true;
    }

    return false;
}

int type_size_bits(type_list const* type)
{
    switch (type->type)
    {
        case T_SCALAR:
            if (type->scalar_bits == 0)
            {
                error_msg(NULL, "Number literal type missing\n");
                exit(1);
            }
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
            if (type->scalar_bits == 0)
            {
                error_msg(NULL, "Number literal type missing\n");
                exit(1);
            }
            return 1;
        case T_ARRAY:
            return type->array_count;
        case T_POINTER:
            return 1;
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

const char* type_display_full_inner(type_list const* type, int inner, bool expand)
{
    if (!type)
        return "<NULL TYPE!!>";

    if (inner <= 0 && type->header.name)
        return type->header.name;

    switch (type->type)
    {
        case T_VOID:
            return "void";
        case T_SCALAR:
        {
            if (type->scalar_bits == 0)
                return "<number literal>";
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
            sprintf(buf, "%s*%s", type_display(type->pointer_target), type->pointer_is_global ? " global" : "");
            return buf;
        }
        case T_COMPOSITE:
        {
            /*if (inner <= 0)
                return "<anonymous type>";*/
            char* buf = malloc(1024);
            int p = sprintf(buf, "struct { ");
            for (var_list* ptr = type->composite_members.head; ptr; ptr = (var_list*) ptr->header.next)
            {
                p += sprintf(buf + p, "%s: %s; ", ptr->header.name, type_display_full(ptr->type, inner - 1, true));
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
 * @param inner If true, and the type has a name, return that name
 * @param expand If true, will expand composite types, otherwise will display "<anonymous type>"
 * @return A human-readable textual representation of the specified type
 */
const char* type_display_full(type_list const* type, int inner, bool expand)
{
    const char* res = type_display_full_inner(type, inner, expand);
    if (type && type->is_const)
    {
        char* buf = malloc(1024);
        sprintf(buf, "%s const", res);
        return buf;
    }
    return res;
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
int static_eval(ast_node** n, stack_frame* frame)
{
    if (!AST_INFERRED(*n))
    {
        analysis(n, frame, false);
    }

    if (AST_KIND(*n) != k_number)
    {
        error_msg(*n, "Value must be compile-time constant\n");
        exit(1);
    }

    return NUMBER_VALUE(*n);
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
 * @return The pointer type corresponding to the specified array type. If a non-array type is passed, will do nav_back_local
 * @example The array type u8[10] decays to the pointer type u8*
 */
type_list const* decay_array_ptr(type_list const* t)
{
    if (t->type != T_ARRAY)
        return t;

    return make_pointer(t->array_target);
}

stack_frame* make_child_frame(stack_frame* frame)
{
    stack_frame* sc_frame = malloc(sizeof(*sc_frame));
    *sc_frame = (stack_frame)
            {
                    .function = frame->function,
                    .loop = frame->loop,
                    .is_root = false,
                    .vars = {.head = NULL, .tail = NULL},
                    .parent = frame
            };
    return sc_frame;
}

type_list const* scalar_auto_type(ast_node* n)
{
    unsigned int value1 = (unsigned int) NUMBER_VALUE(n);
    if (value1 >= 65536)
        return make_scalar_type(32);
    else if (value1 >= 256)
        return make_scalar_type(16);
    else
        return WORD_TYPE;
}

type_list const* scalar_infer_auto(ast_node* n)
{
    if (infer_type(n)->type == T_SCALAR && infer_type(n)->scalar_bits == 0)
    {
        return AST_INFERRED(n) = scalar_auto_type(n);
    }
    return AST_INFERRED(n);
}

loop_info* new_loop_info(ast_node* node)
{
    loop_info* res = malloc(sizeof(*res));
    res->address = 0;
    res->node = node;
    return res;
}

type_list const* unalias(type_list const* type)
{
    while (type->type == T_ALIAS)
        type = type->alias_target;

    return type;
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
        return unalias(get_type(spec, frame));
    }
    else
    {
        ast_node** op = OPER_OPERANDS(spec);
        switch (OPER_OPERATOR(spec))
        {
            case KSELF:
            {
                while (frame->impl_parent == NULL)
                {
                    frame = frame->parent;

                    if (!frame)
                    {
                        error_msg(spec, "'self' is only valid inside type declaration'\n");
                        exit(1);
                    }
                }
                return frame->impl_parent;
            }
            case '*':
            {
                return make_pointer(decode_spec(op[0], frame));
            }
            case KCONST:
            {
                type_list* ret = NEW_TYPE();
                *ret = *decode_spec(op[0], frame);
                ret->is_const = true;
                return ret;
            }
            case KGLOBAL:
            {
                type_list* ret = NEW_TYPE();
                *ret = *decode_spec(op[0], frame);
                if (ret->type != T_POINTER)
                {
                    error_msg(spec, "Global specifier only applies to pointer types\n");
                    exit(1);
                }
                ret->pointer_is_global = true;
                return ret;
            }
            case '[':
            {
                type_list* ret = NEW_TYPE();
                ret->type = T_ARRAY;
                ret->array_target = decode_spec(op[0], frame);
                ret->array_count = static_eval(&op[1], frame);
                return ret;
            }
            case KTYPEOF:
            {
                analysis(&op[0], frame, false);
                scalar_infer_auto(op[0]);
                return infer_type(op[0]);
            }
            case GENINST:
            {
                type_list* generic = get_type(op[0], frame);
                stack_frame* sc_frame = make_child_frame(frame);
                linked_list* param, * arg;
                char* namebuf = malloc(256);
                sprintf(namebuf, "%s<", VAR_NAME(op[0]));
                char* end = namebuf;
                for (param = generic->generic.params,
                             arg = ((ast_linked_list*) op[1])->list; param && arg; param = param->next, arg = arg->next)
                {
                    type_list* argt = check_add_type(VAR_NAME(param->value), sc_frame);
                    argt->type = T_ALIAS;
                    argt->alias_target = decode_spec(arg->value, frame);
                    end = strcat(end, type_display(argt->alias_target));
                    if (arg->next)
                        end = strcat(end, ", ");
                }
                if (param || arg)
                {
                    error_msg(spec, "Invalid number of arguments in instanciation of generic type\n");
                    exit(1);
                }
                type_list* res = (type_list*) decode_spec(generic->generic.spec, sc_frame);
                strcat(end, ">");
                res->header.name = namebuf;
                if (res->type == T_GENERIC)
                {
                    res->generic.methods = generic->generic.methods;
                }
                else if (res->type == T_COMPOSITE)
                {
                    stack_frame* temp_frame = malloc(sizeof(stack_frame));
                    *temp_frame = (stack_frame) {
                            .function = NULL,
                            .loop = NULL,
                            .is_root = true,
                            .vars = {NULL, NULL},
                            .impl_parent = res,
                            .parent = sc_frame
                    };
                    for (linked_list* lst = generic->generic.methods; lst; lst = lst->next)
                    {
                        ast_node* method = ast_copy(lst->value);

                        analysis(&method, temp_frame, true);
                    }
                }
                return res;
            }
            case KINTERFACE:
            {
                type_list* ret = NEW_TYPE();
                ret->type = T_INTERFACE;
                stack_frame* temp_frame = malloc(sizeof(stack_frame));
                *temp_frame = (stack_frame) {
                        .function = NULL,
                        .loop = NULL,
                        .is_root = true,
                        .vars = {NULL, NULL},
                        .impl_parent = ret,
                        .parent = frame
                };
                for (linked_list* lst = AST_LIST_HEAD(op[0]); lst; lst = lst->next)
                {
                    ast_node* method = lst->value;

                    analysis(&method, temp_frame, false);
                }
                return ret;
            }
            case KSTRUCT:
            {
                type_list* ret = NEW_TYPE();
                ret->type = T_COMPOSITE;
                for (linked_list* list = ((ast_linked_list*) op[0])->list; list; list = list->next)
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

const type_list* make_pointer_global_if(const type_list* type, bool global)
{
    if (!global)
        return type;

    assert(type->type == T_POINTER);

    if (type->pointer_is_global)
        return type;

    type_list* ret = NEW_TYPE();
    *ret = *type;
    memset(&ret->header, 0, sizeof(ret->header));
    ret->pointer_is_global = true;
    return ret;
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

void check_assignment(ast_node* op0, ast_node* op1)
{
    type_list const* left = infer_type(op0);
    type_list const* right = infer_type(op1);
    if (!type_compatible(&AST_INFERRED(op1), left))
    {
        if (left->type == T_SCALAR && right->type == T_SCALAR && left->scalar_bits >= right->scalar_bits)
        {
            // ok, can upcast scalar to bigger scalar without loss
        }
        else
        {
            error_msg(op1, "Cannot assign rvalue of type '%s' to lvalue of type '%s'\n",
                      type_display(right),
                      type_display(left));
            exit(1);
        }
    }
}


/**
 * @return A new variable with the specified name, or NULL if a variable with the same name already exists
 */
var_list* check_add_var(const char* name, stack_frame* frame, type_list const* type)
{
    assert(name);

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
            if (!ptr->type->is_const) // constants are not stored in memory
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
#define RETURN_LVALUE(x) do {*n = make_node(DEREF, 1, x); analysis(n, frame, true); return;}while(0)

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

#define TLEFT AST_INFERRED(op[0])
#define TRIGHT AST_INFERRED(op[1])

void expect_non_void(ast_node* n)
{
    if (infer_type(n) == VOID_TYPE)
    {
        error_msg(n, "Expected value, got void\n");
        exit(1);
    }
}

bool is_pure_var(ast_node* op)
{
    return AST_KIND(op) == k_operator && OPER_OPERATOR(op) == DEREF && AST_KIND(OPER_OPERANDS(op)[0]) == k_number;
}

/**
 * Static analysis + type checking + constant folding
 */
void analysis(ast_node** n, stack_frame* frame, bool force)
{
    assert(frame);

    if (!*n)
        return;

    if (!force && AST_INFERRED(*n))
    {
        return;
    }

    yylloc.first_line = (*n)->info.line;
    yylloc.first_column = (*n)->info.col;
    yylloc.code = (*n)->info.code;

    switch (AST_KIND(*n))
    {
        case k_number:
        {
            int size = NUMBER_SIZE(*n);
            if (size == 0)
            {
                SET_TYPE(make_scalar_type(0));
            }
            else
            {
                NUMBER_VALUE(*n) &= (1L << size) - 1;
                SET_TYPE(make_scalar_type(size));
            }
        }
        case k_ident:
        {
            func_list* func = FIND_SYM_OR_NULL(func_list, funcs_head, *n);
            if (func)
            {
                AST_DATA(*n) = func;
                SET_TYPE(FUNC_TYPE);
            }
            type_list* type = get_type_or_null(*n, frame);
            if (type)
            {
                AST_DATA(*n) = type;
                SET_TYPE(TYPE_TYPE);
            }
            var_list* ptr = get_var_id(*n, frame, F_NULLABLE);
            if (ptr)
            {
                if (ptr->type->type == T_ARRAY) // referencing an array directly is equivalent to &array[0]
                {
                    ast_node* ret = set_inferred_type(make_number(var_position(ptr)),
                                                      make_pointer_global_if(decay_array_ptr(ptr->type),
                                                                             frame->function == NULL));
                    *n = ret;
                    return;
                }
                if (ptr->type->is_const && is_numeric(ptr->type))
                {
                    RETURN(make_number(ptr->value), ptr->type);
                }
                RETURN_LVALUE(set_inferred_type(make_number(var_position(ptr)),
                                                make_pointer_global_if(make_pointer(ptr->type),
                                                                       frame->function == NULL)));
            }
            ptr = get_var_id(*n, frame, F_RECURSE);
            if (ptr->type->is_const && is_numeric(ptr->type)) // allow resolving constants for parent frames
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
                case KIS:
                {
                    analysis(&op[0], frame, true);
                    expect_non_void(op[0]);

                    if (!op[1])
                    {
                        RETURN(make_number(1), BOOL_TYPE);
                    }
                    else if (AST_KIND(op[1]) == k_operator)
                    {
                        switch (OPER_OPERATOR(op[1]))
                        {
                            case STRUCTLIT:
                            {
                                ast_node* lit = op[1];
                                ast_node* lit_type = OPER_OPERANDS(lit)[0];
                                const type_list* type = decode_spec(lit_type, frame);

                                if (type != AST_INFERRED(op[0]))
                                {
                                    error_msg(lit, "Type mismatch in struct pattern; expected '%s' got '%s'\n",
                                              type_display(AST_INFERRED(op[0])), type_display(type));
                                    exit(1);
                                }

                                ast_node* base = make_number_sized(1, 1);
                                ast_node* temp, * init;
                                if (is_pure_var(op[0]))
                                {
                                    temp = make_node(DEREF, 1, make_number(NUMBER_VALUE(get_position(op[0]))));
                                    AST_INFERRED(temp) = AST_INFERRED(op[0]);
                                    init = make_node(';', 0);
                                }
                                else
                                {
                                    temp = make_ident("$");
                                    init = make_node(';', 2,
                                                     make_node(KVAR, 2, temp, lit_type),
                                                     clean_stack(make_node('=', 2, temp, op[0])));
                                }
                                ast_node* res = make_scope(
                                        make_node('{', 2,
                                                  init,
                                                  base));
                                analysis(&res, frame, false);

                                ast_node* assignments = make_number_sized(1, 1);

                                if (OPER_ARITY(lit) == 3) // auto
                                {
                                    error_msg(lit,
                                              "Struct pattern with implicit fields not supported; please specify field names\n");
                                    exit(1);
                                }

                                for (linked_list* lst = AST_LIST_HEAD(OPER_OPERANDS(lit)[1]); lst; lst = lst->next)
                                {
                                    ast_node* member = OPER_OPERANDS(lst->value)[0];
                                    ast_node* value = OPER_OPERANDS(lst->value)[1];
                                    ast_node* assignment = clean_stack(
                                            make_node(KIS, 2, make_node('.', 2, temp, member), value));
                                    analysis(&assignment, SC_SCOPE(res), false);
                                    assignments = make_node(AND, 2, assignments, assignment);
                                }

                                OPER_OPERANDS(SC_CODE(res))[1] = assignments;
                                analysis(&OPER_OPERANDS(SC_CODE(res))[1], frame, true);

                                *n = res;
                                return;
                            }
                        }
                        analysis(&op[1], frame, true);
                        switch (OPER_OPERATOR(op[1]))
                        {
                            case '|':
                            {
                                ast_node* truc = make_node(OR, 2,
                                                           make_node(KIS, 2, op[0], OPER_OPERANDS(op[1])[0]),
                                                           make_node(KIS, 2, op[0], OPER_OPERANDS(op[1])[1]));
                                analysis(&truc, frame, false);
                                RETURN(truc, BOOL_TYPE);
                            }
                            case RANGE:
                            {
                                ast_node* left = OPER_OPERANDS(op[1])[0];
                                ast_node* right = OPER_OPERANDS(op[1])[1];
                                ast_node* res;
                                if (left && right)
                                {
                                    if (is_pure_var(op[0]))
                                    {
                                        res = make_node(AND, 2,
                                                        make_node(GE, 2, op[0], left),
                                                        make_node(AST_DATA(op[1]) ? LE : '<', 2, op[0], right));
                                    }
                                    else
                                    {
                                        res = make_scope(make_node('{', 2,
                                                                   make_node(';', 2,
                                                                             make_node(KVAR, 2, make_ident("$"),
                                                                                       make_node(KTYPEOF, 1, op[0])),
                                                                             clean_stack(
                                                                                     make_node('=', 2, make_ident("$"),
                                                                                               op[0]))),
                                                                   make_node(AND, 2,
                                                                             make_node(GE, 2, make_ident("$"), left),
                                                                             make_node(AST_DATA(op[1]) ? LE : '<', 2,
                                                                                       make_ident("$"), right)
                                                                   )));
                                    }
                                }
                                else if (left)
                                    res = make_node(GE, 2, op[0], left);
                                else
                                    res = make_node(AST_DATA(op[1]) ? LE : '<', 2, op[0], right);
                                analysis(&res, frame, false);
                                RETURN(res, BOOL_TYPE);
                            }
                        }
                    }

                    ast_node* ret = make_node(EQ, 2, op[0], op[1]);
                    analysis(&ret, frame, false);
                    RETURN(ret, BOOL_TYPE);

                    /*error_msg(*n, "Could not analyse pattern\n");
                    exit(1);*/
                }
                case STRUCTLIT:
                {
                    const type_list* type = decode_spec(op[0], frame);

                    if (type->type != T_COMPOSITE)
                    {
                        error_msg(op[0], "Expected struct type, got %s\n", type_display(type));
                        exit(1);
                    }

                    ast_node* assignments = make_node(';', 2, NULL, NULL);
                    AST_INFERRED(assignments) = VOID_TYPE;
                    ast_node* temp, *res;
                    if (frame->assign_target)
                    {
                        temp = frame->assign_target;
                        res = make_scope(assignments);
                    }
                    else
                    {
                        temp = make_ident("$");
                        res = make_scope(make_node('{', 2,
                                                   make_node(';', 2,
                                                             make_node(KVAR, 2, temp, op[0]),
                                                             assignments),
                                                   temp));
                    }
                    analysis(&res, frame, false);

#define assignment() do{ \
                    ast_node* assignment = clean_stack(\
                    make_node('=', 2, make_node('.', 2, temp, member), value));\
                    analysis(&assignment, SC_SCOPE(res), false);\
                    OPER_OPERANDS(assignments)[0] = make_node(';', 2, OPER_OPERANDS(assignments)[0],\
                    assignment);\
                    }while(0)

                    if (arity == 3)
                    {
                        var_list* fields = type->composite_members.head;
                        linked_list* lst;
                        for (lst = AST_LIST_HEAD(op[1]);
                             lst && fields; lst = lst->next, fields = (var_list*) fields->header.next)
                        {
                            ast_node* member = make_ident(fields->header.name);
                            ast_node* value = lst->value;
                            assignment();
                        }

                        if (lst)
                        {
                            error_msg(op[1], "Too many arguments to struct literal\n");
                            exit(1);
                        }
                        else if (fields)
                        {
                            error_msg(op[1], "Too few arguments to struct literal\n");
                            exit(1);
                        }
                    }
                    else
                    {
                        for (linked_list* lst = AST_LIST_HEAD(op[1]); lst; lst = lst->next)
                        {
                            ast_node* member = OPER_OPERANDS(lst->value)[0];
                            ast_node* value = OPER_OPERANDS(lst->value)[1];
                            assignment();
                        }
                    }

                    analysis(&OPER_OPERANDS(assignments)[0], frame, true);

                    *n = res;
                    return;
                }
                case KASSERT:
                {
                    analysis(&op[0], frame, false);
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
                    analysis(&op[0], frame, false);
                    type_list const* ltype = infer_type(op[0]);
                    ast_node* lhs_ptr = NULL;
                    if (ltype == TYPE_TYPE)
                    {
                        ltype = AST_DATA(op[0]);
                    }
                    else if (ltype->type == T_POINTER && ltype->pointer_target->type == T_COMPOSITE)
                    {
                        lhs_ptr = op[0];
                        ltype = ltype->pointer_target;
                    }
                    else if (ltype->type == T_COMPOSITE)
                    {
                        lhs_ptr = get_position(op[0]);
                    }
                    else
                    {
                        error_msg(*n, "Member access on non-composite type\n");
                        exit(1);
                    }
                    var_list* member = FIND_SYM_OR_NULL(var_list, ltype->composite_members.head, op[1]);
                    if (!member)
                    {
                        member = FIND_SYM(var_list, ltype->composite_methods.head, op[1]);
                        SET_TYPE(FUNC_TYPE);
                    }
                    set_inferred_type(lhs_ptr, make_pointer_global_if(make_pointer(member->type),
                                                                      lhs_ptr->inferred_type->pointer_is_global)); // convert to pointer to member
                    ast_node* ret = make_node('+', 2, lhs_ptr, make_number(member->position));
                    analysis(&ret, frame, false);
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
                    *type = *WORD_TYPE;
                    type->is_const = true;
                    var->type = type;
                    var->value = static_eval(&op[1], frame);
                    *n = NULL;
                    return;
                    //SET_TYPE(VOID_TYPE);
                }
                case KIMPL:
                {
                    type_list* type = get_type(op[0], frame);
                    AST_INFERRED(op[0]) = type;
                    if (type->type == T_GENERIC)
                    {
                        type->generic.methods = AST_LIST_HEAD(op[1]);
                    }
                    else
                    {
                        stack_frame* temp_frame = malloc(sizeof(stack_frame));
                        *temp_frame = (stack_frame) {
                                .function = NULL,
                                .loop = NULL,
                                .is_root = true,
                                .vars = {NULL, NULL},
                                .impl_parent = type,
                                .parent = frame
                        };
                        for (linked_list* lst = AST_LIST_HEAD(op[1]); lst; lst = lst->next)
                        {
                            ast_node* method = lst->value;

                            analysis(&method, temp_frame, false);
                        }
                    }
                    SET_TYPE(VOID_TYPE);
                }
                case KTYPE:
                {
                    type_list* type = check_add_type(VAR_NAME(op[0]), frame);
                    AST_INFERRED(op[0]) = type;
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
                        type->generic.params = ((ast_linked_list*) args)->list;
                        type->generic.spec = spec;
                    }
                    else
                    {
                        if (AST_KIND(spec) == k_ident)
                        {
                            type->type = T_ALIAS;
                            type->alias_target = decode_spec(spec, new_frame);
                        }
                        else
                        {
                            linked_list_header h = type->header;
                            *type = *decode_spec(spec, new_frame);
                            type->header = h;
                        }
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
                            AST_DATA(op[0]) = (void*) type;
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
                            type->array_count = static_eval(&op[1], frame);
                            type->array_target = WORD_TYPE;
                            AST_DATA(op[0]) = (void*) type;
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
                {
                    func_list* newNode;
                    if (frame->impl_parent)
                    {
                        /*if (!AST_LIST_HEAD(op[1]))
                        {
                            error_msg(*n, "Method '%s' missing instance parameter\n", VAR_NAME(op[0]));
                            exit(1);
                        }*/
                        newNode = ADD_SYM(func_list, &frame->impl_parent->composite_methods.head,
                                          &frame->impl_parent->composite_methods.tail);
                        add_symbol_existing((linked_list_header**) &funcs_head, (linked_list_header**) &funcs_tail,
                                            &newNode->header);
                    }
                    else
                    {
                        newNode = ADD_SYM(func_list, &funcs_head, &funcs_tail);
                    }
                    newNode->header.name = VAR_NAME(op[0]);
                    newNode->header.owner = NULL;
                    newNode->arglist = AST_LIST_HEAD(op[1]);

                    ast_node* type_params = op[4];
                    stack_frame* fct_def_frame = frame;
                    if (type_params) // function is generic
                    {
                        newNode->is_generic = true;
                        newNode->type_params = AST_LIST_HEAD(type_params);
                        newNode->fct_def = *n;
                        newNode->instances = NULL;
                    }
                    else
                    {
                        newNode->return_type = decode_spec(op[3], frame);
                        AST_DATA(op[0]) = (void*) newNode->return_type;
                        newNode->callsites = NULL;
                        newNode->frame = (stack_frame) {
                                .function = newNode,
                                .loop = NULL,
                                .is_root = true,
                                .vars = {NULL, NULL},
                                .parent = frame
                        };
                        if (!op[2])
                        {
                            newNode->code = NULL;
                        }
                        else
                        {
                            newNode->code = make_scope(op[2]);
                            SC_SCOPE(newNode->code) = &newNode->frame;
                            for (linked_list* ptr = newNode->arglist; ptr; ptr = ptr->next)
                            {
                                ast_node* argname = OPER_OPERANDS(ptr->value)[0];
                                const type_list* spec = decode_spec(OPER_OPERANDS(ptr->value)[1], frame);
                                spec = make_pointer_global_if(spec, ptr == newNode->arglist && spec->type == T_POINTER);
                                AST_INFERRED(argname) = spec;
                                check_add_var(VAR_NAME(argname), &newNode->frame, spec);
                            }
                            analysis(&newNode->code, &newNode->frame, force);

                            // tail recursion

                            ast_node** code = &newNode->code;
                            stack_frame* c_frame = NULL;
                            while (AST_KIND(*code) == k_scope)
                            {
                                c_frame = SC_SCOPE(*code);
                                code = &SC_CODE(*code);
                            }
                            if (AST_KIND(*code) == k_operator && OPER_OPERATOR(*code) == ';' && OPER_ARITY(*code) == 2)
                            {
                                ast_node* second = OPER_OPERANDS(*code)[1];
                                if (AST_KIND(second) == k_operator &&
                                    OPER_OPERATOR(second) == KRETURN) // if last statement is return
                                {
                                    ast_node* returned = OPER_OPERANDS(second)[0];
                                    if (AST_KIND(returned) == k_operator &&
                                        OPER_OPERATOR(returned) == '(') // which is a function call
                                    {
                                        ast_node* function = OPER_OPERANDS(returned)[0];
                                        if (AST_KIND(function) == k_ident &&
                                            !strcmp(VAR_NAME(function), newNode->header.name)) // of the function itself
                                        {
                                            // tail recursive call
                                            ast_node* list = make_list(OPER_OPERANDS(newNode->arglist->value)[0]);
                                            for (linked_list* ptr = newNode->arglist->next, * target = ((ast_linked_list*) list)->list; ptr; ptr = ptr->next, target = target->next)
                                            {
                                                target->next = malloc(sizeof(linked_list));
                                                target->next->value = OPER_OPERANDS(ptr->value)[0];
                                                target->next->next = NULL;
                                            }
                                            ast_node* assignment = make_node(TUPLEASSIGN, 2, list,
                                                                             OPER_OPERANDS(returned)[2]);
                                            AST_CLEAN_STACK(assignment) = true;
                                            ast_node* loop_code = (make_node(';', 2, OPER_OPERANDS(*code)[0],
                                                                             assignment));
                                            *code = make_node(KLOOP, 1, loop_code);
                                            analysis(code, c_frame, false);
                                            AST_CLEAN_STACK(*code) = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    SET_TYPE(VOID_TYPE);
                }
                case ';':
                {
                    if (arity)
                    {
                        analysis(&op[0], frame, force);
                        if (op[1])
                        {
#if 0
                            if (op[0])
                            {
                                ast_node* first = op[0];
                                while (first && AST_KIND(first) == k_operator && OPER_OPERATOR(first) == ';')
                                    first = OPER_OPERANDS(first)[0];
                                if (first && AST_KIND(first) == k_operator && OPER_OPERATOR(first) == KVAR)
                                {
                                    op[1] = make_scope(op[1]);
                                }
                            }
#endif // TODO


                            analysis(&op[1], frame, force);
                            SET_TYPE(infer_type(op[1]));
                        }
                    }
                    SET_TYPE(VOID_TYPE);
                }
                case KDO:
                {
                    op[0] = make_scope(op[0]);
                    stack_frame* sc_frame = malloc(sizeof(*sc_frame));
                    *sc_frame = (stack_frame) {.function = frame->function, .loop = new_loop_info(
                            *n), .is_root = false, .vars = {.head = NULL, .tail = NULL}, .parent = frame};
                    SC_SCOPE(op[0]) = sc_frame;
                    analysis(&op[0], sc_frame, false);
                    analysis(&op[1], sc_frame, false);
                    break;
                }
                case KLOOP:
                {
                    op[0] = make_scope(op[0]);
                    stack_frame* sc_frame = malloc(sizeof(*sc_frame));
                    *sc_frame = (stack_frame) {.function = frame->function, .loop = new_loop_info(
                            *n), .is_root = false, .vars = {.head = NULL, .tail = NULL}, .parent = frame};
                    SC_SCOPE(op[0]) = sc_frame;
                    AST_CLEAN_STACK(op[0]) = AST_CLEAN_STACK(*n);
                    analysis(&op[0], sc_frame, false);
                    break;
                }
                case KFOR:
                {
                    op[3] = make_scope(op[3]);
                    stack_frame* sc_frame = malloc(sizeof(*sc_frame));
                    *sc_frame = (stack_frame) {.function = frame->function, .loop = new_loop_info(
                            *n), .is_root = false, .vars = {.head = NULL, .tail = NULL}, .parent = frame};
                    SC_SCOPE(op[3]) = sc_frame;
                    analysis(&op[0], frame, false);
                    analysis(&op[1], sc_frame, false);
                    analysis(&op[2], sc_frame, false);
                    analysis(&op[3], sc_frame, false);
                    break;
                }
                case KFOREACH:
                {
                    ast_node* variable = op[0], * iter = op[1], * code = op[2];
                    analysis(&iter, frame, false);
                    ast_node* iter_decl = NULL;
                    if (!is_pure_var(iter))
                    {
                        ast_node* temp_name = make_ident("$iter");
                        iter_decl = make_node(';', 2,
                                              make_node(KVAR, 2, temp_name, make_node(KTYPEOF, 1, iter)),
                                              clean_stack(make_node('=', 2, temp_name, iter)));
                        iter = temp_name;
                    }
                    ast_node* next_fct_call = make_node('(', 3, make_node('.', 2, iter, make_ident("next")), NULL, NULL);
                    ast_node* has_fct_call = make_node('(', 3, make_node('.', 2, iter, make_ident("hasNext")), NULL, NULL);
                    *n = make_scope(make_node(KFOR, 4, make_node(';', 2, iter_decl, make_node(KVAR, 2, variable,
                                                                                              make_node(KTYPEOF, 1,
                                                                                                        next_fct_call))),
                                              has_fct_call,
                                              NULL,
                                              make_node(';', 2, clean_stack(make_node('=', 2, variable, next_fct_call)),
                                                        code)));
                    analysis(n, frame, false);
                    return;
                }
                case '=':
                {
                    analysis(&op[0], frame, false);
                    if (AST_KIND(op[1]) == k_operator && OPER_OPERATOR(op[1]) == STRUCTLIT)
                    {
                        stack_frame* fr = make_child_frame(frame);
                        fr->assign_target = op[0];
                        analysis(&op[1], fr, false);
                        *n = op[1];
                    }
                    else
                    {
                        analysis(&op[1], frame, false);
                        expect_non_void(op[1]);
                        check_assignment(op[0], op[1]);
                        if (AST_KIND(op[1]) == k_ident && !strcmp(VAR_NAME(op[0]), VAR_NAME(op[1])))
                        {
                            info_msg(*n, "Assignment has no effect\n");
                        }
                    }
                    SET_TYPE(infer_type(op[0]));
                }
            }

            struct
            {
                bool is_num;
                int value;
            } * o = malloc(arity * sizeof(*o));
            for (int i = 0; i < arity; i++)
            {
                if (!op[i])
                    continue;
                analysis(&op[i], frame, false);
                if ((o[i].is_num = AST_KIND(op[i]) == k_number))
                    o[i].value = NUMBER_VALUE(op[i]);
            }

            if (op[0] && TLEFT)
            {
                type_list const* tleft = TLEFT;
                if (tleft == TYPE_TYPE && ((type_list*)AST_DATA(op[0]))->type == T_COMPOSITE)
                {
                    tleft = ((type_list*)AST_DATA(op[0]));
                }
                const char* op_str = stringify_operator_or_null(OPER_OPERATOR(*n));
                if (op_str)
                {
                    ast_node* func_name = make_ident(op_str);
                    func_list* func = FIND_SYM_OR_NULL(func_list, tleft->composite_methods.head, func_name);
                    if (func)
                    {
                        ast_node* args = NULL;
                        if (OPER_OPERATOR(*n) == '(')
                        {
                            args = op[2];
                        }
                        else
                        {
                            if (arity > 1)
                            {
                                linked_list* list = NULL;
                                linked_list** tail = &list;
                                for (int i = 2; i < arity; i++)
                                {
                                    linked_list* item = make_list_item(op[i]);
                                    *tail = item;
                                    tail = &item->next;
                                }
                                args = make_node_from_list(list);
                            }
                        }

                        *n = make_node('(', 3, make_node('.', 2, op[0], func_name), NULL, args);
                        analysis(n, frame, false);
                        return;
                    }
                }
            }

            type_list const* result;

            switch (OPER_OPERATOR(*n))
            {
                case '(':
                {
                    if (!AST_DATA(*n))
                    {
                        func_list* func;
                        ast_node** member = NULL;
                        if (AST_KIND(op[0]) == k_ident)
                        {
                            func = FIND_SYM(func_list, funcs_head, op[0]);
                        }
                        else if (AST_KIND(op[0]) == k_operator && OPER_OPERATOR(op[0]) == '.' &&
                                 AST_KIND(OPER_OPERANDS(op[0])[1]) == k_ident)
                        {
                            ast_node** left = &OPER_OPERANDS(op[0])[0];
                            analysis(left, frame, false);
                            type_list const* member_type = infer_type(*left);
                            ast_node* right = OPER_OPERANDS(op[0])[1];
                            if (member_type->type == T_POINTER)
                            {
                                member_type = member_type->pointer_target;
                            }
                            if (member_type->type == T_COMPOSITE)
                            {
                                func = FIND_SYM(func_list, member_type->composite_methods.head, right);
                                member = left;
                            }
                            else if (member_type == TYPE_TYPE)
                            {
                                func = FIND_SYM(func_list, ((type_list*)AST_DATA(*left))->composite_methods.head, right);
                            }
                            else
                            {
                                error_msg(*n, "Method access on non-composite type\n");
                                exit(1);
                            }
                        }
                        else
                        {
                            error_msg(op[0], "Invalid call target\n");
                            exit(1);
                        }

                        linked_list* list, * flist;

                        if (func->is_generic)
                        {
                            stack_frame* sc_frame = malloc(sizeof(*sc_frame));
                            *sc_frame = (stack_frame)
                                    {
                                            .is_root = false,
                                            .vars = {.head = NULL, .tail = NULL},
                                            .parent = frame
                                    };
                            typedef struct type_param_list_s
                            {
                                const char* name;
                                type_list const* type;
                                struct type_param_list_s* next;
                            } type_param_list;
                            type_param_list* tpl = NULL;
                            for (linked_list* tptr = func->type_params; tptr; tptr = tptr->next)
                            {
                                type_list* tty = ADD_SYM(type_list, &sc_frame->types.head, &sc_frame->types.tail);
                                tty->header.name = VAR_NAME(tptr->value);
                                tty->type = T_GENERIC_VARIABLE;
                            }

                            type_list* fargs;
                            int i = 1;
                            for (flist = func->arglist, list = AST_LIST_HEAD(op[2]);
                                 list && flist;
                                 list = list->next, flist = flist->next, i++)
                            {
                                analysis(&list->value, frame, false);
                                ast_node* typenode = OPER_OPERANDS(flist->value)[1];
                                /*
                                type_list const* argtype = decode_spec(, sc_frame);
                                if (argtype->type == T_GENERIC_VARIABLE)
                                {

                                }*/
                                /*   if (!type_compatible(&AST_INFERRED(list->value), fargs->type))
                                   {
                                       error_msg(list->value,
                                                 "Type mismatch for argument %d in call to '%s'; expected '%s', got '%s'\n",
                                                 i, func->header.name, type_display(fargs->type),
                                                 type_display(infer_type(list->value)));
                                       exit(1);
                                   }*/
                            }

                            func_list* newNode = ADD_SYM(func_list, &funcs_head, &funcs_tail);
                            newNode->header.name = "";
                            newNode->header.owner = NULL;
                        }

                        list = AST_LIST_HEAD(op[2]);

                        if (member)
                        {
                            if (!func->frame.vars.head)
                            {
                                error_msg(*n, "Method call on static function\n");// todo, better detection
                                exit(1);
                            }

                            linked_list* nlist = malloc(sizeof(linked_list));
                            if (func->frame.vars.head->type->type == T_POINTER &&
                                infer_type(*member)->type != T_POINTER)
                            {
                                *member = make_node(REF, 1, *member);
                                analysis(member, frame, false);
                            }
                            nlist->value = *member;
                            nlist->next = list;

                            if (op[2])
                            {
                                ((ast_linked_list*) op[2])->list = nlist;
                            }
                            else
                            {
                                op[2] = make_node_from_list(nlist);
                            }

                            list = nlist;
                        }

                        var_list* fargs;
                        int i = 1;
                        for (flist = func->arglist, fargs = func->frame.vars.head;
                             list && flist;
                             list = list->next, flist = flist->next, fargs = (var_list*) fargs->header.next, i++)
                        {
                            analysis(&list->value, frame, false);
                            if (!type_compatible(&AST_INFERRED(list->value), fargs->type))
                            {
                                error_msg(list->value,
                                          "Type mismatch for argument %d in call to '%s'; expected '%s', got '%s'\n",
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
                        func_data* call_site = malloc(sizeof(*call_site));
                        call_site->function = func;
                        call_site->site = add_call_site(&func->callsites);
                        AST_DATA(*n) = call_site;
                        SET_TYPE(func->return_type);
                    }
                    return;
                }

                case '{':
                {
                    SET_TYPE(TRIGHT);
                }

                case '<':
                case '>':
                case GE:
                case LE:
                case NE:
                case EQ:
                {
                    if (!type_compatible_symmetric(&TLEFT, &TRIGHT))
                    {
                        error_msg(*n, "Cannot perform arithmetic comparison '%c' on different types %s and %s\n",
                                  OPER_OPERATOR(*n), type_display(TLEFT), type_display(TRIGHT));
                        exit(1);
                    }

                    if (!is_numeric(TLEFT))
                    {
                        error_msg(*n, "Cannot perform arithmetic comparison '%c' on non-numeric types %s and %s\n",
                                  OPER_OPERATOR(*n), type_display(TLEFT), type_display(TRIGHT));
                        exit(1);
                    }

                    break;
                }
                case SHL:
                case SHR:
                {
                    if (!is_scalar(TLEFT))
                    {
                        error_msg(*n, "Cannot perform bitwise shift on non-numeric types %s and %s\n",
                                  type_display(TLEFT), type_display(TRIGHT));
                        exit(1);
                    }
                    scalar_infer_auto(op[1]);
                    if (o[0].is_num && o[0].value == 0 || o[1].is_num && o[1].value >= type_size_bits(TLEFT))
                    {
                        ALWAYS_ZERO();
                        RETURN(make_number(0), TLEFT);
                    }
                    result = TLEFT;
                    break;
                }
            }

            switch (OPER_OPERATOR(*n))
            {
                case '|':
                {
                    SET_TYPE(VOID_TYPE);// todo: pattern type?
                }
                case RANGE:
                {
                    if (!is_numeric(TLEFT))
                    {
                        error_msg(op[0], "Expected numeric type, got %s\n", type_display(TLEFT));
                        exit(1);
                    }

                    if (!is_numeric(TRIGHT))
                    {
                        error_msg(op[1], "Expected numeric type, got %s\n", type_display(TRIGHT));
                        exit(1);
                    }

                    if (o[0].is_num && o[1].is_num)
                    {
                        if (AST_DATA(*n))
                        {
                            if (o[0].value > o[1].is_num)
                            {
                                error_msg(*n, "Invalid range: %d..=%d\n", o[0].value, o[1].value);
                                exit(1);
                            }
                        }
                        else
                        {
                            if (o[0].value >= o[1].is_num)
                            {
                                error_msg(*n, "Invalid range: %d..%d\n", o[0].value, o[1].value);
                                exit(1);
                            }
                        }
                    }


                    SET_TYPE(VOID_TYPE);// todo: pattern type?
                }
                case SHL:
                {
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value << o[1].value), TLEFT);
                    }
                    SET_TYPE(TLEFT);
                }
                case SHR:
                {

                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value >> o[1].value), TLEFT);
                    }
                    SET_TYPE(TLEFT);
                }
                case UMINUS:
                {
                    if (!is_scalar(TLEFT))
                    {
                        error_msg(*n, "Cannot perform unary minus on non-integral type %s\n", type_display(TLEFT));
                        exit(1);
                    }
                    if (o[0].is_num)
                    {
                        RETURN(make_number(-o[0].value), TLEFT);
                    }
                    SET_TYPE(TLEFT);
                }
                case '~':
                {
                    if (!is_scalar(TLEFT))
                    {
                        error_msg(*n, "Cannot perform binary complement on non-integral type %s\n",
                                  type_display(TLEFT));
                        exit(1);
                    }
                    if (o[0].is_num)
                    {
                        RETURN(make_number(~o[0].value), TLEFT);
                    }
                    SET_TYPE(TLEFT);
                }
                case REF:
                {
                    *n = get_position(op[0]);
                    return;
                }
                case '+':
                {
                    type_compatible_symmetric(&TLEFT, &TRIGHT);
                    if (TLEFT->type == T_POINTER && TRIGHT->type == T_POINTER)
                    {
                        error_msg(*n, "Cannot add two pointers\n");
                        exit(1);
                    }
                    if (TLEFT->type == T_POINTER && TRIGHT->type == T_SCALAR)
                        result = TLEFT;
                    else if (TLEFT->type == T_SCALAR && TRIGHT->type == T_POINTER)
                        result = TRIGHT;
                    else if (TLEFT->type == T_SCALAR && TRIGHT->type == T_SCALAR)
                        result = TLEFT;
                    else
                    {
                        error_msg(*n, "Cannot add types %s and %s\n", type_display(TLEFT), type_display(TRIGHT));
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
                    type_compatible_symmetric(&TLEFT, &TRIGHT);
                    if (TLEFT->type == T_POINTER && TRIGHT->type == T_POINTER)
                        result = WORD_TYPE;
                    else if (TLEFT->type == T_POINTER && TRIGHT->type == T_SCALAR)
                        result = TLEFT;
                    else if (TLEFT->type == T_SCALAR && TRIGHT->type == T_SCALAR)
                        result = TLEFT;
                    else
                    {
                        error_msg(*n, "Cannot subtract types %s and %s\n", type_display(TLEFT), type_display(TRIGHT));
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
                    type_compatible_symmetric(&TLEFT, &TRIGHT);
                    if (TLEFT->type != T_SCALAR || TRIGHT->type != T_SCALAR)
                    {
                        error_msg(*n, "Cannot perform multiplication on types %s and %s\n",
                                  type_display(TLEFT), type_display(TRIGHT));
                        exit(1);
                    }
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value * o[1].value), TLEFT);
                    }
                    else if (o[0].is_num)
                    {
                        if (o[0].value == 1)
                        {
                            RETURN(op[1], TLEFT);
                        }
                        else if (o[0].value == 0)
                        {
                            RETURN(make_number(0), TLEFT);
                        }
                        else if (is_power_of_2(o[0].value))
                        {
                            *n = make_node(SHL, 2, op[1], make_number(highest_set_bit(o[0].value)));
                            analysis(n, frame, false);
                            return;
                        }
                    }
                    else if (o[1].is_num)
                    {
                        if (o[1].value == 1)
                        {
                            RETURN(op[0], TLEFT);
                        }
                        else if (o[1].value == 0)
                        {
                            RETURN(make_number(0), TLEFT);
                        }
                        else if (is_power_of_2(o[1].value))
                        {
                            *n = make_node(SHL, 2, op[0], make_number(highest_set_bit(o[1].value)));
                            analysis(n, frame, false);
                            return;
                        }
                    }
                    SET_TYPE(TLEFT);
                }
                case '/':
                {
                    type_compatible_symmetric(&TLEFT, &TRIGHT);
                    if (TLEFT->type != T_SCALAR || TRIGHT->type != T_SCALAR)
                    {
                        error_msg(*n, "Cannot perform division on types %s and %s\n",
                                  type_display(TLEFT), type_display(TRIGHT));
                        exit(1);
                    }
                    if (o[1].is_num && o[1].value == 0)
                    {
                        error_msg(*n, "Division by zero\n");
                        exit(1);
                    }
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value / o[1].value), TLEFT);
                    }
                    else if (o[0].is_num)
                    {
                        if (o[0].value == 0)
                        {
                            RETURN(make_number(0), TLEFT);
                        }
                    }
                    else if (o[1].is_num)
                    {
                        if (o[1].value == 1)
                        {
                            RETURN(op[0], TLEFT);
                        }
                        else if (is_power_of_2(o[1].value))
                        {
                            *n = make_node(SHR, 2, op[0], make_number(highest_set_bit(o[1].value)));
                            analysis(n, frame, false);
                            return;
                        }
                    }
                    SET_TYPE(TLEFT);
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
                    type_compatible_symmetric(&TLEFT, &TRIGHT);
                    if (TLEFT->type != T_SCALAR || TRIGHT->type != T_SCALAR)
                    {
                        error_msg(*n, "Cannot perform boolean AND on types %s and %s\n",
                                  type_display(TLEFT), type_display(TRIGHT));
                        exit(1);
                    }
                    if (o[0].is_num && o[1].is_num)
                    {
                        RETURN(make_number(o[0].value && o[1].value), BOOL_TYPE);
                    }
                    else if (o[0].is_num)
                    {
                        if (o[0].value == 1)
                        {
                            RETURN(op[1], BOOL_TYPE);
                        }
                        else if (o[0].value == 0)
                        {
                            RETURN(make_number(0), BOOL_TYPE);
                        }
                    }
                    else if (o[1].is_num)
                    {
                        if (o[1].value == 1)
                        {
                            RETURN(op[0], BOOL_TYPE);
                        }
                        else if (o[1].value == 0)
                        {
                            RETURN(make_number(0), BOOL_TYPE);
                        }
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
                    if (!is_numeric(left))
                    {
                        error_msg(*n, "Cannot perform arithmetic operation '%s' on non-numeric type %s\n",
                                  OPER_OPERATOR(*n) == INC ? "++" : "--",
                                  type_display(left));
                        exit(1);
                    }
                    else
                        SET_TYPE(left);
                }
                case TUPLEASSIGN:
                {
                    linked_list* left = ((ast_linked_list*) op[0])->list;
                    linked_list* right = ((ast_linked_list*) op[1])->list;

                    for (; left && right; left = left->next, right = right->next)
                    {
                        analysis(&left->value, frame, true);
                        analysis(&right->value, frame, false);
                        expect_non_void(right->value);
                        check_assignment(left->value, right->value);
                    }

                    if (left || right)
                    {
                        error_msg(*n, "Tuple assignment must have the same number of elements\n");
                        exit(1);
                    }

                    SET_TYPE(VOID_TYPE);
                }
                case GENINST:
                {
                    AST_DATA(*n) = decode_spec(*n, frame);
                    SET_TYPE(TYPE_TYPE);
                }
                case DEREF:
                {
                    if (!TLEFT)
                        break;
                    expect_non_void(op[0]);
                    if (!is_pointer(TLEFT))
                    {
                        info_msg(*n, "Dereferencing a non-pointer value\n");
                        break;
                    }
                    SET_TYPE(TLEFT->pointer_target);
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
                    if (arity == 1)
                    {
                        expect_non_void(op[0]);
                        if (AST_INFERRED(frame->loop->node))
                        {
                            if (!type_compatible(&TLEFT, AST_INFERRED(frame->loop->node)))
                            {
                                error_msg(op[0], "Loop return type mismatch; expected '%s' got '%s'\n",
                                          type_display(AST_INFERRED(frame->loop->node)), type_display(TLEFT));
                                exit(1);
                            }
                        }
                        else
                        {
                            AST_INFERRED(frame->loop->node) = TLEFT;
                        }
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
                    expect_non_void(op[0]);
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
                    expect_non_void(op[1]);
                    if (o[1].is_num && o[1].value == 0)
                    {
                        *n = op[0];
                        return;
                    }
                    SET_TYPE(VOID_TYPE);
                }
                case KFOR:
                {
                    expect_non_void(op[1]);
                    if (o[1].is_num && o[1].value == 0)
                    {
                        *n = op[0];
                        return;
                    }
                    SET_TYPE(VOID_TYPE);
                }
                case KLOOP:
                {
                    if (!AST_INFERRED(*n))
                        SET_TYPE(VOID_TYPE);
                    return;
                }
                case KPRINT:
                {
                    scalar_infer_auto(op[0]);
                    expect_non_void(op[0]);
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

                        if (!type_compatible(&TLEFT, frame->function->return_type))
                        {
                            error_msg(op[0], "Return type mismatch; expected '%s' got '%s'\n",
                                      type_display(frame->function->return_type), type_display(TLEFT));
                            exit(1);
                        }
                    }
                    SET_TYPE(VOID_TYPE);
                }
                case KMATCH:
                {
                    expect_non_void(op[0]);

                    ast_node* res;
                    ast_node** body_else = &res;

                    AST_INFERRED(*n) = NULL;

                    bool wildcard_found = false;

                    for (linked_list* lst = AST_LIST_HEAD(op[1]); lst; lst = lst->next)
                    {
                        ast_node* pattern = OPER_OPERANDS(lst->value)[0];

                        if (!pattern)
                        {
                            if (wildcard_found)
                            {
                                error_msg(pattern, "Only one wildcard pattern allowed\n");
                                exit(1);
                            }
                            wildcard_found = true;
                        }

                        ast_node* body = OPER_OPERANDS(lst->value)[1];

                        analysis(&body, frame, false);
                        scalar_infer_auto(body);

                        if (AST_INFERRED(*n))
                            check_assignment(*n, body);
                        else
                            AST_INFERRED(*n) = AST_INFERRED(body);

                        *body_else = make_node(KIF, 3,
                                               make_node(KIS, 2, op[0], pattern),
                                               body,
                                               NULL);

                        body_else = &OPER_OPERANDS(*body_else)[2];
                    }

                    if (!AST_CLEAN_STACK(*n) && !wildcard_found)
                    {
                        error_msg(op[1], "Patterns must be exhaustive\n");
                        exit(1);
                    }

                    const type_list* tl = AST_INFERRED(*n);
                    *n = res;
                    analysis(n, frame, false);
                    AST_INFERRED(*n) = tl;
                    return;
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
            if (!SC_SCOPE(*n))
            {
                SC_SCOPE(*n) = make_child_frame(frame);
            }
            analysis(&SC_CODE(*n), SC_SCOPE(*n), force);
            SET_TYPE(infer_type(SC_CODE(*n)));
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
    type_list* void_type = ADD_SYM(type_list, &global_frame.types.head, &global_frame.types.tail);
    void_type->header.name = "void";
    void_type->type = T_VOID;
    VOID_TYPE = void_type;

    type_list* func_type = ADD_SYM(type_list, &global_frame.types.head, &global_frame.types.tail);
    func_type->header.name = "func";
    func_type->type = T_VOID;
    FUNC_TYPE = func_type;

    type_list* type_type = ADD_SYM(type_list, &global_frame.types.head, &global_frame.types.tail);
    type_type->header.name = "type";
    type_type->type = T_VOID;
    TYPE_TYPE = type_type;

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