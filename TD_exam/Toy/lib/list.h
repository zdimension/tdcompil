/*
 * list.h       -- Generic List implementation
 *
 * Copyright © 2015 Erick Gallesio - I3S-CNRS/Polytech Nice-Sophia <eg@unice.fr>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date:  4-Jan-2015 08:44 (eg)
 * Last file update:  2-Dec-2015 22:23 (eg)
 */

#ifndef _LIST_H_
#define _LIST_H_

//
// Type definitions
//
typedef struct s_list_item *List_item;
typedef struct s_list      *List;


//
// Basic operations on lists
//
List list_create(void);                     // returns  the created list or NULL
void list_destroy(List lst);                // Free list


//
// Adding an element in a list
//     - list_prepend adds element at the beginning of the list
//     - list_append adds element at the end of the list
//
// Both functions takes a free function pointer that will be used to free the element
// when list_destroy is called
// Functions returns 1 if insertion is possible and 0 otherwise
//
int list_prepend(List lst, void *user_data, void (*free)(void*));
int list_append(List lst, void *user_data, void (*free)(void *));

// Take back the first element of the list (aka pop)
void *list_take_first(List lst);

int list_size(List lst);                    // returns the number of items of lst


//
// Iteration on lists
//
// Exemple of use:
//      for (List_item p= list_head(l); p; p = list_item_next(p))
//        printf("%s\n", (char*) list_item_data(p);
// or using the FORLIST  macro
//      FORLIST(p, l) printf("%s\n", (char*) list_item_data(p);
//
struct s_list_item *list_head(List lst);
void *list_item_data(struct s_list_item *e);
struct s_list_item *list_item_next(struct s_list_item *e);

#define FORLIST(_p, _init) \
  for (List_item _p = list_head(_init); _p; _p = list_item_next(_p))


// A variant using a more functional approach (fct is called with user
// data as parameter)
typedef void (*list_iterator)(void *);

void list_for_each(List lst, list_iterator fct);

#endif /* _LIST_H_ */
