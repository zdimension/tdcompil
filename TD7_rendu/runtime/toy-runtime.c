/*
 * toy-runtime.c        -- The Toy programming language runtime library
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
 *    Creation date:  2-Jul-2015 09:44 (eg)
 * Last file update: 17-Nov-2021 11:10 (eg)
 */


#include <stdio.h>
#include <stdlib.h>
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
//                       String functions
//
// ----------------------------------------------------------------------

// Compare two Toy string as strcmp in C (accepts NULL)
int _toy_strcmp(_toy_string s1, _toy_string s2) {
  if (s1 && s2)
    return strcmp(s1, s2);
  return !(s1 == s2);
}

// Returns the slice of str[idx1: idx2]. Note that, by convention, idx1 and
// idx2 must be set to UNSPECIFIED_SLICE_LIMIT when idx1 of idx2 is not
// specified.
//
// In a real implementation we should exit from the program when a string's
// access is out of bounds.  However, here we just return NULL to mechanically
// permit to test that the error has been detected.

_toy_string _toy_str_slice(_toy_string s, int idx1, int idx2) {
  if (s != NULL) {
    int len = strlen(s);
    int n1 = (idx1 == UNSPECIFIED_SLICE_LIMIT)? 0  : idx1;
    int n2 = (idx2 == UNSPECIFIED_SLICE_LIMIT)? len: idx2;

    // n1 is the lower limit of the slice and n2 its upper one
    if (n1 < 0) n1 += len;
    if (n1 < 0 || n1 >= len) {
      fprintf(stderr, "*** ERROR: low index %d is out of bounds in '%s'\n", idx1, s);
      return NULL;
    }

    if (n2 < 0) n2 += len;
    if (n2 < 0 || n2 > len) {
      fprintf(stderr, "*** ERROR: upper index %d is out of bounds in '%s'\n", idx2, s);
      return NULL;
    }

    if (n2 <= n1)
      // return an empty string
      return "";
    else {
      // n2 > n1 ⟹ we need to allocate a atring and return it
      _toy_string p = p = malloc(n2-n1+1);    // +1 for the '\0'
      if (!p) {
        fprintf(stderr, "slice allocation failed\n");
        exit(1);
      }
      memmove(p, s+n1, n2-n1);
      p[n2-n1] = '\0';
      return p;
    }
  }
  return NULL;
}


// Return the element s[idx] as a string
_toy_string _toy_str_element(_toy_string s, int n) {
  if (s) {
    // We use a slice of 1 character here to extract a single element The
    // slice is s[n:n+1] except when n is -1 where we nedd to compuute the
    // length of the string since the sclide is s[n:strlen(s)]
    int next = ((n == -1) ? (int) strlen(s): n+1);
    return _toy_str_slice(s, n, next);
  }
  return NULL;
}
