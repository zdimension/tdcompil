/*
 * utils.c      -- Utilities for toy compiler
 *
 * Copyright © 2015-2019 Erick Gallesio - I3S-CNRS/Polytech Nice-Sophia <eg@unice.fr>
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
 * Last file update: 15-Dec-2019 14:34 (eg)
 */

#include <stdarg.h>
#include <string.h>

#include "toy.h"
#include "ast.h"
#include "utils.h"

extern int yylineno;            ///< the line number managed by lex
int error_detected = 0;         ///< The number of errors while compiling a file


/// \file
/// This module implements things which does not fit elsewhere:
///
/// - printing error message using the GNU conventions to have a good
///   integration with standard Unix tools (e.g. editors)
///
/// - printing a fatal error with the line where the function was
///   called. This function is used when we detect an error in the
///   compiler itself.


/// Function for allocating memory. This function is not used directly. Use the
/// must_malloc macro, which provides the location of the call.
void *_must_malloc(size_t n, char *file, int line) {
  void * res = malloc(n);
  if (!res) {
    fprintf(stderr, "Allocation error during compilation (%s:%d)\n", file, line);
    fprintf(stderr, "Abort\n");
    exit(2);
  }
  return res;
}

/// Concatenation of two strings (result is allocated and must bee freed)
char *string_concat(char *s1, char *s2) {
  char *res = must_malloc(strlen(s1) + strlen(s2) + 1);
  sprintf(res, "%s%s", s1, s2);
  /* strings contains the external quotes!!! */
  return res;
}

/// Display error message using the GNU conventions
void error_msg(ast_node *culprit, char *format, ...) {
  va_list ap;
  int line = culprit? AST_LINE(culprit): yylineno;

  if (input_path) fprintf(stderr, "%s:", input_path);
  fprintf(stderr, "%d: ", line);

  va_start(ap, format);
  vfprintf(stderr, format, ap);
  va_end(ap);

  fprintf(stderr, "\n");

  error_detected += 1;
}

/// Print an error message and exits the program. Do not use this function but the
/// die() macro instead.
void _die(char *message, char *file, int line)
{
  fprintf(stderr, "FATAL ERROR: %s in file %s line %d\n", message, file, line);
  exit(1);
}
