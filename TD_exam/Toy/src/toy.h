/*
 * toy.h        -- Toy Language
 *
 * Copyright © 2014-2021 Erick Gallesio - I3S-CNRS/Polytech Nice-Sophia<eg@unice.fr>
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
 *   Creation date:  7-Dec-2014 19:19 (eg)
 *Last file update:  6-Dec-2021 13:25 (eg)
 */

#ifndef _TOY_H_
#define _TOY_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TOY_VERSION "0.5"

extern char *progname;                  /* Name of the compiler */
extern char *input_path;                /* path of the input file */
extern char *output_path;               /* path of the output file */
extern bool trace_mode;                 /* Add traces to functions? */ 
extern FILE *fout;                      /* Compiler output file */

#endif /* _TOY_H_ */
