/*
 * list.c       -- Generic List implementation
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
 * Last file update: 20-Nov-2015 17:47 (eg)
 */

#include <stdlib.h>
#include "list.h"

// ----------------------------------------------------------------------
//
// Type definitions
//
// ----------------------------------------------------------------------
struct s_list_item {
  void *data;
  void (*free)(void *);         // Free function for user_data
  struct s_list_item *next;
};


struct s_list {
  int size;                     // size of the list
  struct s_list_item *first;    // First item of the list
  struct s_list_item *last;     // Last item of the list
};

// ----------------------------------------------------------------------
//
// Basic operations on lists
//
// ----------------------------------------------------------------------
List list_create(void)
{
  List res = malloc(sizeof(struct s_list));

  if (res) {
    res->size = 0;
    res->first = res->last= NULL;
  }
  return res;
}


void list_destroy(List lst)
{
  List_item p, tmp;
  if (!lst) return;

  /* destroy the list cells  */
  p = lst->first;
  while (p) {
    tmp = p->next;
    if (p->free) (p->free)(p->data);
    free(p);
    p = tmp;
  }

  /* destroy the list itself */
  free(lst);
}


//
// Adding an element in a list
//     - list_prepend adds element at the beginning of the list
//     - list_append adds element at the end of the list
// Both functions takes a free function pointer that will be used to free the element
// when list_destroy is called
// Functions returns 1 if insertion is possible and 0 otherwise
//
int list_prepend(List lst, void *user_data, void (*free)(void *))
{
  List_item new = malloc(sizeof(struct s_list_item));

  if (!new) return 0;

  new->data     = user_data;
  new->free     = free;
  new->next     = lst->first;
  lst->size    += 1;
  lst->first    = new;

  return 1;
}


int list_append(List lst, void *user_data, void (*free)(void *))
{
  List_item new = malloc(sizeof(struct s_list_item));

  if (!new) return 0;

  new->data  = user_data;
  new->free  = free;
  new->next  = NULL;
  lst->size += 1;
  if (lst->last) {
    lst->last->next = new;
    lst->last       = new;
  } else {
    lst->first = lst->last = new;
  }

  return 1;
}


// Take back the first element of the list (aka pop) 
void *list_take_first(List lst)
{
  List_item tmp;
  void * res;

  if (!lst) return NULL;

  tmp        = lst->first;
  lst->first = tmp->next;
  lst->size -= 1;
  res        = tmp->data;

  free(tmp);   // Free the container but not the user data

  return res;
}




// ----------------------------------------------------------------------
//
// Iteration on lists
//
// Exemple of use:
//      for (List_item_ptr p= list_head(l); p; p = list_item_next(p))
//        printf("%s\n", (char*) list_item_data(p);
//
// ----------------------------------------------------------------------
int list_size(List lst)
{
  return lst->size;
}

List_item list_head(List lst)
{
  return lst->first;
}

void *list_item_data(List_item e)
{
  return e->data;
}

List_item list_item_next(List_item e)
{
  return e->next;
}


// A variant using a more functional approach (fct is called with user
// data as parameter)
void list_for_each(List lst, list_iterator fct)
{
  if (!lst) return;

  for (List_item p=lst->first; p; p = p->next)
    fct(p->data);
}


/* ********************************************************************** */
#ifdef TEST_LIST

#include <stdio.h>

void pr(void *data)
{
  printf("%s\n", (char *) data);
}


int main() {
  List l = list_create();

  printf("taille de la liste %d\n", list_size(l));

  list_append(l,  "foo", NULL);
  list_append(l,  "bar", NULL);
  list_prepend(l, "start", NULL);
  list_append(l,  "end", NULL);

  printf("taille de la liste %d\n", list_size(l));

  list_for_each(l, pr);

  printf("----\n");
  for (List_item p = list_head(l); p; p = list_item_next(p))
    printf("%s\n", (char*) list_item_data(p));
  list_destroy(l);
  return 0;
}
#endif
