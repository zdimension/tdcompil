/*
 * hash.c       -- hash table implementation
 *
 * Copyright © 2014-2015 Erick Gallesio - I3S-CNRS/Polytech'Nice <eg@unice.fr>
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
 *    Creation date: 13-Dec-2014 13:19 (eg)
 * Last file update: 12-Jan-2015 12:10 (eg)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

struct elt {
  char *key;
  void *value;
  void (*free)(void *);
  struct elt *next;
};

struct hash_table {
    int size;           /* size of the table */
    int n;              /* number of elements stored */
    struct elt **table;
};

#define INITIAL_SIZE                    13
#define GROWTH_FACTOR                   3/2
#define MAX_LOAD_FACTOR                 2




static unsigned long hash(const char *str)
{
  // djb2: this function was first reported by Dan Bernstein many
  // years ago in comp.lang.c
  unsigned long hash = 5381;
  for (  ; *str; str++)
    hash = ((hash << 5) + hash) + *str; // hash*33 + *str
  return hash;
}


static Hash_table create_table(int size)
{
  Hash_table h;

  h = malloc(sizeof(struct hash_table));

  h->size = size;
  h->n = 0;
  h->table = calloc(sizeof(struct elt), size);

  return h;
}


static void enlarge_table(Hash_table h)
{
  int new_size = h->size * GROWTH_FACTOR;
  struct elt **new_table = calloc(sizeof(struct elt), new_size);

  //   printf("Enlarge table %p from %d to %d\n", h, h->size, new_size);

  /* store all the elements of the old table in the new one */
  for(int i = 0; i < h->size; i++) {
    struct elt *e = h->table[i];

    while (e) {
      unsigned long bucket_index =  hash(e->key) % new_size;
      struct elt *next = e->next;

      e->next = new_table[bucket_index];
      new_table[bucket_index] = e;
      e = next;
    }
  }

  /* Free the old table */
  free(h->table);

  /* keep the new table */
  h->size  = new_size;
  h->table = new_table;
}


/* ====================================================================== */
/* Public functions */

Hash_table hash_table_create(void)
{
  return create_table(INITIAL_SIZE);
}


void hash_table_destroy(Hash_table h)
{
  for(int i = 0; i < h->size; i++) {
    struct elt *e = h->table[i];

    while (e) {
      struct elt *next = e->next;

      free(e->key);
      if (e->free) e->free(e->value);
      free(e);
      e = next;
    }
  }

  free(h->table);
  free(h);
}


/* insert a new key-value pair into an existing hashionary */
void hash_table_add(Hash_table h, const char *key, void *value, void (*free)(void *))
{
  struct elt *e;
  unsigned long bucket_index =  hash(key) % h->size;

  /* Create a new element */
  e = malloc(sizeof(struct elt));
  e->key   = strdup(key);
  e->value = value;
  e->free  = free;

  /* enter it in the table */
  e->next = h->table[bucket_index];
  h->table[bucket_index] = e;


  h->n += 1;

  /* If the table seem to have not enough room, enlarge it */
  if(h->n >= (h->size * MAX_LOAD_FACTOR))
    enlarge_table(h);
}


void *hash_table_search(Hash_table h, const char *key)
{
  struct elt *e;
  unsigned long bucket_index = hash(key) % h->size;

  for(e = h->table[bucket_index]; e; e = e->next) {
    if(strcmp(e->key, key) == 0) return e->value;
  }

  return NULL;
}

void hash_table_delete(Hash_table h, const char *key)
{
  struct elt *e, *prev;
  unsigned long bucket_index =  hash(key) % h->size;

  for(prev=NULL, e=h->table[bucket_index]; e; prev=e, e=e->next) {
    if (strcmp(e->key, key) == 0) {
      if (prev)
        prev->next = e->next;
      else
        h->table[bucket_index] = e->next;

      free(e->key);
      if (e->free) e->free(e->value);
      free(e);
      return;
    }
  }
}


void hash_table_iterate(Hash_table h, void (*fct)(const char * key, void *value))
{
  for (int i = 0; i < h->size; i++) {
    for(struct elt *e = h->table[i]; e; e = e->next) {
      fct(e->key, e->value);
    }
  }
}



void hash_table_stats(Hash_table h)
{
  int maxlen = 0, minlen = h->n, used_buckets=0;
  printf("Statistics of hash table %p\n", h);
  printf("size: %d,  number of elements: %d\n", h->size, h->n);

  for (int i = 0; i < h->size; i++) {
    int len = 0;
    struct elt *e = h->table[i];

    if (e) {
      used_buckets += 1;
      while (e) {
        len += 1;
        e = e->next;
      }
      maxlen = (len > maxlen)? len: maxlen;
      minlen = (len < minlen)? len: minlen;
    }
 }
  printf("Length of longest chain: %d\n", maxlen);
  printf("Length of shortest used chain: %d\n", minlen);
  printf("Used buckets: %d/%d (%6.2f%%)\n", used_buckets, h->size,
         (float) used_buckets/ h->size * 100);
}

/* ====================================================================== */
#ifdef TEST_HASH

void hash_table_print(Hash_table h)
{
  for (int i = 0; i < h->size; i++) {
    printf("%d : ", i);
    for(struct elt *e = h->table[i]; e; e = e->next) {
      printf("%s ", e->key);
    }
    printf("\n");
  }
}


int test_hash() {
  Hash_table h = hash_table_create();

  hash_table_add(h, "foo", "bar", NULL);

  for (int i=0; i < 10000; i++) {
    char buffer[20];

    sprintf(buffer, "%d", i);
    hash_table_add(h, buffer, strdup(buffer), free);
  }

  /*
  for (int i=0; i < 20; i++) {
    char buffer[20];

    sprintf(buffer, "%d", i);
    printf("%s => %s\n", buffer, hash_table_search(h, buffer));
  }
  */
  hash_table_stats(h);
  /*   hash_table_print(h);*/

  printf("Recherches\n");
  printf("foo    => %s\n", (char *) hash_table_search(h, "foo"));
  printf("10     => %s\n", (char *) hash_table_search(h, "10"));
  printf("absent => %s\n", (char *) hash_table_search(h, "absent"));
  
  hash_table_destroy(h);
  return 0;
}
#endif
