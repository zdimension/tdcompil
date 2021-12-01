#
#  Makefile 	-- Makefile for testing the Toy programming language
#
# Copyright © 2015-2019 Erick Gallesio - Polytech Nice-Sophia <eg@unice.fr>
#
#           Author: Erick Gallesio [eg@unice.fr]
#    Creation date: 24-Jun-2015 19:39 (eg)
# Last file update: 14-Dec-2019 22:15 (eg)
#


IGNORE      = $(shell sed -e '/\#/d' IGNORE)
ALL_SUCCESS = $(sort $(wildcard ok-*.toy))
ALL_FAIL    = $(sort $(wildcard fail-*.toy))

SUCCESS     = $(filter-out $(IGNORE), $(ALL_SUCCESS))
FAIL        = $(filter-out $(IGNORE), $(ALL_FAIL))

SUCCESS_EXE = $(SUCCESS:.toy=)
FAIL_EXE    = $(FAIL:.toy=)
ALL_C       = $(SUCCESS:.toy=.c) $(FAIL:.toy=.c)

RUNTIMEDIR =../runtime
RUNTIME    =$(RUNTIMEDIR)/toy-runtime.a

# Compilers
TOY=../src/toy
CC=gcc
CFLAGS=-std=gnu99 -g -Wall -Wextra -Wno-unused-parameter \
       -Wno-misleading-indentation -Wno-unused-label -I$(RUNTIMEDIR)

# Colours
red="\\e[31m"
rst="\\e[0m\\n"
pr=printf

# No colours
#red=""
#rst=""
#pr=echo


# ----------------------------------------------------------------------
# Implicit rules
#
%.c:%.toy
	$(TOY) -o $*.c $*.toy

%:%.c
	@$(CC) $(CFLAGS) -o $* $*.c $(RUNTIME)

all: $(SUCCESS_EXE)

tests: successes failures
	@$(pr) "${red}All tests (`ls $(SUCCESS) $(FAIL) | wc -l`) passed${rst}"


untested:
	@echo "FAIL:" $(filter $(IGNORE), $(ALL_FAIL))
	@echo "SUCCESS:" $(filter $(IGNORE), $(ALL_SUCCESS))

successes: all
	@$(pr) "${red}==== Testing successes ====${rst}"
	@for i in $(SUCCESS_EXE) ;do \
	   if ! ./exec-success $$i ;then \
	     exit 1; \
           fi; \
         done

failures:
	@$(pr) "${red}==== Testing failures ====${rst}"
	@for i in $(FAIL_EXE); do \
	   if ! ./exec-fail $$i ;then \
	     exit 1; \
           fi; \
         done


clean:
	rm -f $(SUCCESS_EXE) $(ALL_C) *.c *.out *~

distclean: clean
