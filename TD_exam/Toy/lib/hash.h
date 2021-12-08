/*
 * hash.h       -- hash table implementation
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
 *          Author: Erick Gallesio [eg@unice.fr]
 *   Creation date: 13-Dec-2014 15:15 (eg)
 *Last file update: 20-Nov-2015 17:46 (eg)
 */

#ifndef _HASH_H_
#define _HASH_H_

typedef struct hash_table *Hash_table;

Hash_table hash_table_create(void);
void hash_table_destroy(Hash_table h);

void hash_table_add(Hash_table h, const char *key, void *value, void (*free)(void *));
void *hash_table_search(Hash_table h, const char *key);
void hash_table_delete(Hash_table h, const char *key);

void hash_table_iterate(Hash_table h, void (*fct)(const char * key, void *value));
void hash_table_stats(Hash_table h);

#endif /* _HASH_H_ */
