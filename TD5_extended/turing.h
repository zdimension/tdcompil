#include <stdbool.h>
#include "calc.h"
#include "syntax.h"

#define INT_WIDTH 8
#define POINTER_SIZE 1

// ----------------------------------------------------------------------
//		Utilities
// ---------------------------------------------------------------------



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
            int position; // in cells
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

typedef struct
{
    int address;
} loop_info;

typedef struct stack_frame_s
{
    struct func_list_s* function;
    loop_info* loop;
    bool is_root;
    int size;
    struct
    {
        var_list* head, * tail;
    } vars;
    struct stack_frame_s* parent;
} stack_frame;
extern stack_frame global_frame;

typedef struct func_list_s
{
    linked_list_header header;
    struct type_list_s const* return_type;
    linked_list* arglist;
    ast_node* code;
    call_site_list* callsites;
    stack_frame frame;
} func_list;
extern func_list* funcs_head, * funcs_tail;

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
            int array_count;
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
extern type_list* types_head, * types_tail;
extern type_list const* VOID_TYPE;

extern int label;

typedef enum
{
    F_DEFAULT = 0,
    F_NULLABLE = 1,
    F_RECURSE = 2
} find_flags;


const char* type_display(type_list const* type);

int type_size(type_list const* type);

const char* type_display_full(type_list const* type, bool inner, bool expand);

void analysis(ast_node** n, stack_frame* frame);

var_list* get_var_id(ast_node* node, stack_frame* frame, find_flags flags);

#define FIND_SYM(type, list, name) ((type*)find_symbol(&((list)->header), name))

linked_list_header* find_symbol(linked_list_header* list, ast_node* node);

_Noreturn void missing_symbol(ast_node* node, const char* name);

int var_position(var_list* var);

void init_builtin_types();