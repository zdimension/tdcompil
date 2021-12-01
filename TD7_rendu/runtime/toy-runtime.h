/*
 * toy-runtime.h        -- The Toy runtime header
 *
 * Copyright © 2015-2021 Erick Gallesio - I3S-CNRS/Polytech Nice-Sophia <eg@unice.fr>
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
 *    Creation date:  5-Jan-2015 18:23 (eg)
 * Last file update: 16-Jan-2021 14:53 (eg)
 */

#ifndef _TOY_RUNTIME_H_
#define _TOY_RUNTIME_H_
#include <stdio.h>
#include <string.h>

typedef char *_toy_string;

// ----------------------------------------------------------------------
//
//                       Arithmetic functions
//
// ----------------------------------------------------------------------
int _toy_powint(int a, int b);

// ----------------------------------------------------------------------
//
//                       Print functions
//
// ----------------------------------------------------------------------
void _toy_print_int(int o);
void _toy_print_float(float o);
void _toy_print_bool(char o);
void _toy_print_string(_toy_string o);

// ----------------------------------------------------------------------
//
//                       String functions
//
// ----------------------------------------------------------------------

// Value to be used when a slice limit is unspecified
#define  UNSPECIFIED_SLICE_LIMIT (1<<31)

// Compare two Toy string as strcmp in C (accepts NULL)
int _toy_strcmp(_toy_string s1, _toy_string s2);

// Return the slice of str[idx1: idx2]. Note that, by convention, idx1 and
// idx2 must be set to UNSPECIFIED_SLICE_LIMIT when idx1 of idx2 is not
// specified.
_toy_string _toy_str_slice(_toy_string s, int idx1, int idx2);

// Return the element s[idx] as a string
_toy_string _toy_str_element(_toy_string s, int idx); 

#endif /* _TOY_RUNTIME_H_ */
