#
#  Makefile 	-- Makefile pour le Td1 de LFA
#
# Copyright © 2016-2017 Erick Gallesio - Polytech Nice-Sophia <eg@unice.fr>
#
#           Author: Erick Gallesio [eg@unice.fr]
#    Creation date: 17-Sep-2016 11:43 (eg)
# Last file update: 24-Apr-2017 16:16 (eg)
#

CC=gcc
CFLAGS=-Wall -Wextra -std=gnu99 -Wno-unused-function
LEX=lex
LEXFILES=$(wildcard *.l)
EXE=$(LEXFILES:.l=)
C=$(LEXFILES:.l=.c)

#
# Règles implicites pour Lex/Flex
#
%.c: %.l
	$(LEX) -o $*.c $*.l




# ----------------------------------------------------------------------

all: $(EXE)

clean:
	rm -f *~ $(EXE) $(C)

distclean: clean
