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
#include "turing.h"


stack_frame global_frame = {.function = NULL, .is_root = true, .end = 0, .vars = {NULL, NULL}, .parent = NULL};


int frame_start(stack_frame* frame)
{
    if (frame->is_root)
        return 0;

    return frame->parent->end + frame_start(frame->parent);
}

int var_position(var_list* var)
{
    return frame_start((stack_frame*) var->header.owner) + var->position;
}

func_list* funcs_head = NULL, * funcs_tail = NULL;

type_list* types_head = NULL, * types_tail = NULL;

type_list* VOID_TYPE;


int label = 0;


_Noreturn void missing_symbol(ast_node* node, const char* name)
{
    error_msg(node, "SYMBOL NOT FOUND: %s\n", name);
    exit(1);
}

linked_list_header* find_symbol(linked_list_header* list, ast_node* node)
{
    const char* name = VAR_NAME(node);
    for (linked_list_header* ptr = list; ptr; ptr = ptr->next)
    {
        if (!strcmp(ptr->name, name))
        {
            return ptr;
        }
    }
    if (list && ((stack_frame*) list->owner)->parent)
        return find_symbol(&((stack_frame*) list->owner)->parent->vars.head->header, node);
    missing_symbol(NULL, name);
}


var_list* get_var_id(ast_node* node, stack_frame* frame, find_flags flags)
{
    const char* name = VAR_NAME(node);
    for (linked_list_header* ptr = &frame->vars.head->header; ptr; ptr = ptr->next)
    {
        if (!strcmp(ptr->name, name))
        {
            return (var_list*) ptr;
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


void emit_functions();

void emit_functions_epilogues();

void emit_main(ast_node* n);


// ----------------------------------------------------------------------
//		Code production
//			This version produces code for a stack machine
// ---------------------------------------------------------------------
void produce_code(ast_node* n)
{
    optimize = true;

    init_builtin_types();

    analysis(&n, &global_frame);

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

    emit_main(n);

    emit_functions();

    emit_functions_epilogues();
}


