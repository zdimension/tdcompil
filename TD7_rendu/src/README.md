# README for the src directory

This directory contains the *Toy* compiler source files. 

The compiler uses the *flex* and *bison* tools: 
	- The lex/flex lexicl rules are in the file `lexical.l`
	- The bison/yacc grammar rules are in the file `syntax.y`

The `Makefile` provided in this directory will call flex/bison to make
the tables and compile the files lying around. The main files of the
compiler are:
	- `ast.c` which is in charge of building (and destroying) the
      Abstrac Syntax Tree
	- `analysis.c` which is in charge of doing all the semantic
      analyses on the AST (type controls, declarations, ...)
	- `prodcode.c` which travels the AST and produces the
      corresponding C code
	- `symbols.c` which provides the symbol tables for the program
      identifiers, but also a table for each class of a program.


------ 
Erick Gallesio
