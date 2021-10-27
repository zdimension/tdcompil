	push	12
	store	n
	push	1
	store	prod
L000:
	load	n
	push	1
	cmpgt
	jumpz	L001
	load	prod
	load	n
	mul
	store	prod
	load	n
	push	1
	sub
	store	n
	jump	L000
L001:
	load	prod
	print
