/*
 * utils.h      -- Utilities for toy compiler
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
 *    Creation date: 15-Jan-2015 09:43 (eg)
 * Last file update: 24-Sep-2015 15:57 (eg)
 */

#ifndef _UTILS_H_
#define _UTILS_H_

extern int error_detected;      ///< The number of errors while compiling a file
extern int yylineno;            ///< Lex variable containing current line number


/// Function for allocating memory. This function is not used directly. Use the 
/// must_malloc macro, which provides the location of the call.
void *_must_malloc(size_t n, char* file, int line);
#define must_malloc(n) (_must_malloc(n, __FILE__, __LINE__))

/// Display error message using the GNU conventions
void error_msg(ast_node *culprit, char *format, ...);

/// Force exit with a message and showing current location in the source file.
#define die(msg) _die(msg, __FILE__, __LINE__)

/// Print an error message and exits the program. Do not use this function but the
/// die() macro instead.
void _die(char *message, char *file, int line);

#endif /* _UTILS_H_ */
