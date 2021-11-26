#include <stdbool.h>
#include "calc.h"
#include "syntax.h"

#define POINTER_BITS 8

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
                const char* initial;
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
    ast_node* node;
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
    struct
    {
        struct type_list_s* head, * tail;
    } types;
    struct stack_frame_s* parent;
    struct type_list_s* impl_parent;
} stack_frame;
extern stack_frame global_frame;

typedef struct func_list_s
{
    linked_list_header header;
    linked_list* arglist;
    bool is_generic;
    union
    {
        struct
        {
            linked_list* type_params;
            ast_node* fct_def;
            struct func_list_s* instances;
        };
        struct
        {
            struct type_list_s const* return_type;
            ast_node* code;
            call_site_list* callsites;
            stack_frame frame;
        };
    };
} func_list;
extern func_list* funcs_head, * funcs_tail;

typedef enum
{
    T_VOID,
    T_SCALAR,
    T_ARRAY,
    T_POINTER,
    T_COMPOSITE,
    T_GENERIC,
    T_GENERIC_VARIABLE,
    T_INTERFACE,
    T_ALIAS
} type_type;

typedef struct
{

    struct type_list_s const* instance;
} generic_cache;

typedef struct type_list_s
{
    linked_list_header header;
    type_type type;
    bool is_const;
    union
    {
        int scalar_bits; // always 1 for now
        struct
        {
            int array_count;
            struct type_list_s const* array_target;
        };
        struct
        {
            struct type_list_s const* pointer_target;
            bool pointer_is_global;
        };
        struct
        {
            struct
            {
                var_list* head, * tail;
            } composite_members;
            struct
            {
                func_list* head, * tail;
            } composite_methods;
        };
        struct
        {
            linked_list* params;
            ast_node* spec;
            linked_list* methods;
        } generic;
        struct
        {
            struct type_list_s const* alias_target;
        };
    };
} type_list;
//extern type_list* types_head, * types_tail;
extern type_list const* VOID_TYPE;
extern type_list const* WORD_TYPE;
extern type_list const* BOOL_TYPE;

typedef struct
{
    func_list* function;
    call_site_list* site;
} func_data;

extern int label;
extern bool write_c;

typedef enum
{
    F_DEFAULT = 0,
    F_NULLABLE = 1,
    F_RECURSE = 2
} find_flags;


const char* type_display(type_list const* type);

int type_size_cells(type_list const* type);

const char* type_display_full(type_list const* type, int inner, bool expand);

void analysis(ast_node** n, stack_frame* frame, bool force);

var_list* get_var_id(ast_node* node, stack_frame* frame, find_flags flags);

type_list* get_type(ast_node* node, stack_frame* frame);

#define FIND_SYM(type, list, name) ((type*)find_symbol(&((list)->header), name))

linked_list_header* find_symbol(linked_list_header* list, ast_node* node);

#define FIND_SYM_OR_NULL(type, list, name) ((type*)find_symbol_or_null(&((list)->header), name))

linked_list_header* find_symbol_or_null(linked_list_header* list, ast_node* node);

_Noreturn void missing_symbol(ast_node* node, const char* name);

int var_position(var_list* var);

void init_builtin_types();

int type_size_bits(type_list const* type);

type_list const* decode_spec(ast_node* spec, stack_frame* frame);

void write_code(ast_node* n);

const char* stringify_operator(enum yytokentype op);

const char* stringify_operator_or_null(enum yytokentype op);

type_list const* unalias(type_list const* type);