/*
 * toy-runtime.c        -- The Toy programming language runtime library
 *
 * Copyright © 2015-2018 Erick Gallesio - I3S-CNRS/Polytech Nice-Sophia <eg@unice.fr>
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
 *    Creation date:  2-Jul-2015 09:44 (eg)
 * Last file update:  5-Dec-2018 14:59 (eg)
 */


#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "toy-runtime.h"


// ----------------------------------------------------------------------
//
//                       Arithmetic functions
//
// ----------------------------------------------------------------------
int _toy_powint(int a, int b) {         // function  and b should be unsigned
  unsigned int res = 1;

  while (b) {
    if (b & 1) res *= a;
    a *= a;
    b >>= 1;
  }
  return res;
}

_toy_string _toy_strcat(_toy_string a, _toy_string b) {
  if (b == NULL)
    return a;
  if (a == NULL)
    return b;
  _toy_string res = malloc(strlen(a) + strlen(b) + 1);
  _toy_string c = res;
  while(*a) *c++ = *a++;
  while(*b) *c++ = *b++;
  return res;
}

// ----------------------------------------------------------------------
//
//                       Print functions
//
// ----------------------------------------------------------------------
void _toy_print_int(int o)            { printf("%d", o); }
void _toy_print_float(float o)        { printf("%g", o); }
void _toy_print_bool(char o)          { printf("%s", o? "true": "false"); }
void _toy_print_string(_toy_string o) { printf("%s", o); }

// ----------------------------------------------------------------------
//
//                       Try-Catch-Finally & Throw
//
// ----------------------------------------------------------------------

// Just declare the variables...
jmp_buf _env[10];        // FIXME: No control on the length !!!!!!
int _index_jmpbuf;

