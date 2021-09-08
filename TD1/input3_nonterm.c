/*
 * input3.c     -- Un programme de test pour le Td2 de compilation
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 17-Sep-2016 12:28 (eg)
 * Last file update: 29-Sep-2016 08:17 (eg)
 */

#include <stdio.h>

// Un commentaires à la C++
int x = 0;  // Un commentaire C++ sur la fin de la ligne

/* un commentaire 'monoligne'  */

char *str[] = {
  "",                                            // test 0
  "I’m a string",                                // test 1
  /*  
  "Another string with embedded \"quotes\"",     // test 2
  "and another one with a \"\\\" !!!",           // test 3
  "Printed text on\n2 lines",                    // test 4
  "The TAB character: '\t' or '\011' or '\x9'.", // test 5
  "Two strings", /* on a */ "line",              // test 6
  "A // comment in a string",                    // test 7
  
  "A string spanning \
   2 lines",                                     // test 8
   NULL,
};


int main () {
  int x =1;
  printf("v1 = %d\n", 2-/*comment*/-x);
  printf("v2 = %d\n", 2 - - - x);
  for (char **p = str; *p; p++) {
    fputs(*p, stdout);
    fputc('\n', stdout);
  }
  return 0;
}
