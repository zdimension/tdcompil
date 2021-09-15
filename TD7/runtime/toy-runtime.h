/*
 * toy-runtime.h        -- The Toy runtime header
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
 *    Creation date:  5-Jan-2015 18:23 (eg)
 * Last file update:  5-Dec-2018 16:52 (eg)
 */

#ifndef _TOY_RUNTIME_H_
#define _TOY_RUNTIME_H_
#include <stdio.h>
#include <string.h>
#include <setjmp.h>


typedef char *_toy_string;

// ----------------------------------------------------------------------
//
//                       Arithmetic functions
//
// ----------------------------------------------------------------------
int _toy_powint(int a, int b);
_toy_string _toy_strcat(_toy_string a, _toy_string b);

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
//                       Try-Catch-Finally & Throw
//
// ----------------------------------------------------------------------
extern jmp_buf _env[10];        // FIXME: No control on the length !!!!!!
extern int _index_jmpbuf;

#define THROW    longjmp(_env[_index_jmpbuf-1], 1)

// CATCH also uses a setjmp in case there is a throw in a catch. This ensures
// the execution of the finally even if we throw inside the body of  a catch!!
#define TRY      do { if (setjmp(_env[_index_jmpbuf++]) == 0)
#define CATCH    else { if (setjmp(_env[_index_jmpbuf-1]) == 0) {
#define FINALLY  } } { _index_jmpbuf--;
#define ENDTRY   } } while(0);

#endif /* _TOY_RUNTIME_H_ */
