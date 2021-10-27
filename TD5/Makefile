#
#  Makefile 	-- Makefile pour le Td 2 de compilation
#
#           Author: Erick Gallesio [eg@unice.fr]
#    Creation date: 18-Oct-2014 08:59 (eg)
# Last file update: 15-Nov-2017 18:40 (eg)
#

# Outils
YACC=bison
YFLAGS= -g -v -d

CC=gcc
CFLAGS= -Wall -std=gnu99 -Wno-unused-function -Wextra
LDLIBS=

LEX=flex
LFLAGS=

EXE= calc1 calc2 calc3

COMMON_OBJ= syntax.o lexical.o calc.o


# Règles implicites
%.c: %.y
	$(YACC) $(YFLAGS) -o $*.c $*.y

%.c: %.l
	$(LEX) $(LFLAGS) -o $*.c $*.l

all: $(EXE)

calc1: $(COMMON_OBJ) code1.o
	$(CC) $(CFLAGS) -o $@  $(COMMON_OBJ) code1.o $(LDLIBS)

calc2: $(COMMON_OBJ) code2.o
	$(CC) $(CFLAGS) -o $@  $(COMMON_OBJ) code2.o $(LDLIBS)

calc3: $(COMMON_OBJ) code3.o
	$(CC) $(CFLAGS) -o $@  $(COMMON_OBJ) code3.o $(LDLIBS)

clean:
	rm -f $(EXE) *.o *~
	rm -f syntax.h syntax.dot syntax.output

distclean: clean
