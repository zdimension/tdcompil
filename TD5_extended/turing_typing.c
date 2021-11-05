#include "turing.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define NEW_TYPE() (calloc(1, sizeof(type_list)))
#define WORD_TYPE types_head

type_list* check_add_type(const char* name);

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
            for (var_list* ptr = type->composite_members.head; ptr; ptr = (var_list*) ptr->header.next)
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

int static_eval(ast_node* n, stack_frame* frame)
{
    if (AST_KIND(n) != k_number)
    {
        error_msg(n, "Value must be compile-time constant\n");
        exit(1);
    }

    return NUMBER_VALUE(n);
}


type_list const* infer_type(ast_node* n)
{
    if (!n)
        return NULL;

    if (AST_INFERRED(n))
        return AST_INFERRED(n);

    error_msg(n, "Node without type\n");
    exit(1);
}

type_list const* decay_array_ptr(type_list const* t)
{
    if (t->type != T_ARRAY)
        return t;

    type_list* ret = NEW_TYPE();
    ret->type = T_POINTER;
    ret->pointer_target = t->array_target;

    return ret;
}


type_list const* decode_spec(ast_node* spec, stack_frame* frame)
{
    if (AST_KIND(spec) == k_ident)
    {
        return FIND_SYM(type_list, types_head, spec);
    }
    else
    {
        switch (OPER_OPERATOR(spec))
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
                analysis(&OPER_OPERANDS(spec)[0], frame);
                return infer_type(OPER_OPERANDS(spec)[0]);
        }
    }

    error_msg(spec, "Invalid type specification\n");
    exit(1);
}

#define ADD_SYM(type, head, tail) ((type*) add_symbol((linked_list_header**)(head), (linked_list_header**)(tail), sizeof(type)))

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


#define RETURN(x, type) do{*n = x; AST_INFERRED(*n) = type; return;}while(0)
#define SET_TYPE(type) do {AST_INFERRED(*n) = type; return;}while(0)

void analysis(ast_node** n, stack_frame* frame)
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
                    for (linked_list* list = ((ast_linked_list*) op[1])->list; list; list = list->next)
                    {
                        analysis(&list->value, frame);
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
                analysis(&op[i], frame);
                if ((o[i].is_num = AST_KIND(op[i]) == k_number))
                    o[i].value = NUMBER_VALUE(op[i]);
                if (!AST_INFERRED(op[i]))
                    return;
                o[i].type = decay_array_ptr(AST_INFERRED(op[i]));
            }

            type_list const* result;

            switch (OPER_OPERATOR(*n))
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
                        else if (AST_KIND(op[0]) == k_operator && OPER_OPERATOR(op[0]) == DEREF)
                        {
                            ast_node* inner = OPER_OPERANDS(op[0])[0];
                            analysis(&inner, frame);
                            RETURN(inner, AST_INFERRED(inner));
                        }
                        else
                        {
                            error_msg(*n, "Cannot take address of expression\n");
                            exit(1);
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
                    if (!type_same(left, right))
                    {
                        error_msg(*n, "Cannot assign rvalue of type '%s' to lvalue of type '%s'\n", type_display(right),
                                  type_display(left));
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
                *sc_frame = (stack_frame) {.function = frame->function, .is_root = false, .vars = {.head = NULL, .tail = NULL}, .parent = frame};
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

void init_builtin_types()
{

    VOID_TYPE = NEW_TYPE();

    type_list* word_type = ADD_SYM(type_list, &types_head, &types_tail);
    word_type->header.name = "word";
    word_type->type = T_SCALAR;
    word_type->scalar_size = 1;
}