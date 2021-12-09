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
 * Last file update:  6-Dec-2021 14:55 (eg)
 */

#ifndef _TOY_RUNTIME_H_
#define _TOY_RUNTIME_H_
#include <stdio.h>
#include <string.h>

typedef char *_toy_string;
typedef int   _toy_int;
typedef float _toy_float;
typedef char  _toy_bool;


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


// ----------------------------------------------------------------------
//
//                       Traces
//
// ----------------------------------------------------------------------
extern int _trace_level;           // Trace indentation


// Code added when entering a function in debug mode
#define ENTER_VOID          printf("%*s>> Enter procedure '%s'\n",    \
                                   _trace_level++, " ", __func__)

#define ENTER_FUNC(_type)   _toy_##_type _res;                               \
                            printf("%*s>> Enter function '%s'\n",    \
                                   _trace_level++, " ", __func__)

// Code added when leaving a function in debug mode (void function)
#define LEAVE_VOID        goto _out; /* to avoid warnings if no return */   \
                          _out: printf("%*s<< Leave procedure '%s'\n",      \
                                       --_trace_level, " ", __func__)

#define LEAVE_FUNC(_type) goto _out; /* to avoid warnings if no return */   \
                          _out: printf("%*s<< Leave function '%s' <-",      \
                                       --_trace_level, " ", __func__);      \
                          _toy_print_##_type((_toy_##_type) _res);                 \
                          printf("\n");                                     \
                          return _res;

// Code for returning from a function in debug mode
#define RETURN                     goto _out
#define RETURN_VALUE(_type, _val)  do {_res = (_toy_##_type) (_val); goto _out; } while(0)

#endif /* _TOY_RUNTIME_H_ */
