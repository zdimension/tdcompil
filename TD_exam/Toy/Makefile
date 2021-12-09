# Makefile for the toy programming language
#
#           Author: Erick Gallesio [eg@unice.fr]
#    Creation date: 24-Jun-2015 18:19 (eg)
# Last file update: 21-Nov-2018 20:03 (eg)

SUBDIRS = lib src runtime tests
ALLDIRS = $(SUBDIRS) doc
ARCHIVE = Toy-base.tar.gz

.PHONY: all doc tests clean distclean archive

all:
	for i in $(SUBDIRS); do $(MAKE) -C $$i all; done

tests:  all
	@($(MAKE) -C tests tests)

doc:
	@($(MAKE) -C doc doc)

archive:
	git archive --format tar.gz --prefix Toy-base/ -o ../$(ARCHIVE) master

clean:
	for i in $(ALLDIRS); do $(MAKE) -C $$i clean; done
	rm -f *~

distclean: clean
	for i in $(ALLDIRS); do $(MAKE) -C $$i distclean; done
