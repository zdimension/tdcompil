#
# Makefile for our C library of utilities
#
#           Author: Erick Gallesio [eg@unice.fr]
#    Creation date:  4-Jan-2015 10:25 (eg)
# Last file update: 11-Dec-2017 12:11 (eg)

CC=gcc
CFLAGS=-Wall -std=gnu99
#CC= clang
#CFLAGS=


SRC = $(wildcard *.c)
OBJC= $(SRC:.c=.o)
LIB=  libutil.a

all: depend $(LIB)

$(LIB): $(OBJC)
	rm -f $(LIB)
	ar cq $(LIB) $(OBJC)
	ranlib $(LIB)

clean:
	rm -f $(OBJC) $(LIB) *~
	rm -f Makefile.depend

distclean: clean

depend:
	gcc -MM $(SRC) > Makefile.depend

sinclude Makefile.depend
