# Makefile for src directory 
#
#           Author: Erick Gallesio [eg@unice.fr]
#    Creation date:  7-Dec-2014 18:52 (eg)
# Last file update: 11-Dec-2017 12:14 (eg)


LIB=../lib
CC=gcc
CFLAGS=-Wall -std=gnu99 -I$(LIB) -g -DDEBUG
#CC=clang
#CFLAGS=-I$(LIB) -g -DDEBUG



LEX=flex
LFLACS=

YACC=bison
YFLAGS= -d -t --report=all


# ----------------------------------------------------------------------
# Implicit rules
# ----------------------------------------------------------------------
%.o: %.l
	$(LEX) $(LFLAGS) -o $*.c $*.l && \
	$(CC) -c $(CFLAGS) -Wno-unused-function $*.c && \
	rm $*.c

%.o: %.y
	$(YACC) $(YFLAGS) -o $*.c $*.y && \
	$(CC) -c $(CFLAGS) $*.c  && \
	rm $*.c


# ----------------------------------------------------------------------
# Build rules
# ----------------------------------------------------------------------
SRC = $(wildcard *.c)
OBJC= $(filter-out syntax.tab.o,$(SRC:.c=.o))

LOBJ= lexical.c
YOBJ= syntax.c syntax.h

OBJ=  syntax.o lexical.o  $(OBJC)

UTILLIB=$(LIB)/libutil.a
EXE=toy

# ----------------------------------------------------------------------
all: prepare $(UTILLIB) $(EXE)

$(EXE): depend $(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(UTILLIB)

$(UTILLIB):
	(cd $(LIB); $(MAKE) all)

prepare:
	# Delete (eventually syntax.tab.c) lying around in this directory
	# If there is an error, Bison creates sometime this file
	@rm -f syntax.tab.c 

tags:
	etags *.c *.h *.l *.y

clean:
	rm -f $(OBJ) $(LOBJ) $(YOBJ) $(EXE) syntax.h *~ TAGS
	rm -f syntax.tab.c syntax.tab.h syntax.output
	rm -f Makefile.depend

distclean: clean


depend:
	( echo "syntax.h: syntax.y";  \
	  echo "lexical.c: syntax.h"; \
	  gcc -MM -MG $(CFLAGS) $(SRC) ) > Makefile.depend

sinclude Makefile.depend
