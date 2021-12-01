/*
 * main.c       -- Main of the Toy Language compiler
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
 *    Creation date: 13-Dec-2014 11:29 (eg)
 * Last file update: 24-Sep-2015 16:48 (eg)
 */

#include <string.h>

#include "toy.h"
#include "ast.h"
#include "syntax.h"
#include "prodcode.h"
#include "symbols.h"

/// \file
/// This files contains the main() function of the Toy compiler. 


char* progname = NULL;  ///< Name of the compiler for error messages
char* input_path = NULL;  ///< The name of the toy source file (NULL if stdin)
char* output_path = NULL;  ///< The name of the output file (NULL if stdout)

extern FILE* yyin;         ///< input stream used by flex (yyin is declared by flex)


static void usage(int full)
{
    fprintf(stderr, "Toy compiler " TOY_VERSION "\n");
    if (full)
    {
        fprintf(stderr, "Usage: %s [options] input-file\n", progname);
        fprintf(stderr, "Options:\n"
                        "   -o f: output in file 'f'\n"
                        "   -v:   print number version\n"
                        "   -h|?: print this help message\n");
    }
    exit(full);
}


static void initialize_compiler(void)
{
    init_ast();                   // Initialize the AST module
    init_symbols();               // Init the symbol table
}


static int compile(char* in, char* out)
{
    yyin = stdin;
    fout = stdout;

    if (in)
    {
        if (!(yyin = fopen(in, "r")))
        {
            fprintf(stderr, "%s: cannot open source file %s\n", progname, in);
            exit(1);
        }
        input_path = in;
    }
    if (out)
    {
        if (!(fout = fopen(out, "w")))
        {
            fprintf(stderr, "%s: cannot open output file %s\n", progname, out);
            exit(1);
        }
        output_path = out;
    }

    initialize_compiler();

    return yyparse();
}


int main(int argc, char* argv[])
{
    char* out_file_name = NULL;

    /* Option analysis */
    progname = argv[0];
    for (argc--, argv++; *argv && argv[0][0] == '-'; argv++, argc--)
    {
        char* opt;

        if (strcmp(*argv, "--") == 0)
        {
            /* special case '--' → end of options */
            argc--, argv++;
            break;
        }

        for (opt = &argv[0][1]; *opt; opt++)
            switch (*opt)
            {
                case 'o':
                    if ((out_file_name = argv[1]) == NULL)
                    {
                        fprintf(stderr, "output file name missing\n");
                        exit(1);
                    }
                    argv++;
                    argc--;
                    break;
                case 'v':
                    usage(0);
                    break;
                case 'h':
                case '?':
                    usage(1);
                    break;
                default:
                    fprintf(stderr, "bad option %c\n", *opt);
            }
    }

    /* Parameters */
    if (argc > 1)
        usage(1);

    return compile(*argv, out_file_name);
}
