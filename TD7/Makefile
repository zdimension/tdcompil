# Makefile for the toy programming language
#
#           Author: Erick Gallesio [eg@unice.fr]
#    Creation date: 24-Jun-2015 18:19 (eg)
# Last file update:  5-Dec-2018 18:33 (eg)

SUBDIRS = lib src runtime tests
ALLDIRS = $(SUBDIRS) doc
ARCHIVE = Toy-base-TD07.tar.gz

.PHONY: all doc tests clean distclean archive

all:
	for i in $(SUBDIRS); do $(MAKE) -C $$i all; done

tests:  all
	@($(MAKE) -C tests tests)

doc:
	@($(MAKE) -C doc doc)

archive:
	git archive --format tar.gz --prefix Toy-base-Td07/ -o ../$(ARCHIVE) master

clean:
	for i in $(ALLDIRS); do $(MAKE) -C $$i clean; done
	rm -f *~

distclean: clean
	for i in $(ALLDIRS); do $(MAKE) -C $$i distclean; done
