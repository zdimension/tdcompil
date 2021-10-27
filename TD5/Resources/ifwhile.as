	push	10
	store	n
L000:
	load	n
	push	0
	cmpgt
	jumpz	L001
	load	n
	print
	load	n
	push	1
	sub
	store	n
	jump	L000
L001:
L002:
	load	n
	push	10
	cmple
	jumpz	L003
	load	n
	print
	load	n
	push	1
	add
	store	n
	jump	L002
L003:
	push	1
	push	2
	cmplt
	jumpz	L004
	push	1
	print
	jump	L005
L004:
	push	1
	negate
	print
L005:
	push	2
	push	1
	cmplt
	jumpz	L006
	push	1
	negate
	print
	jump	L007
L006:
	push	1
	print
L007:
	push	1
	jumpz	L008
	push	0
	jumpz	L009
	push	1
	negate
	print
	jump	L010
L009:
	push	1
	print
L010:
L008:
