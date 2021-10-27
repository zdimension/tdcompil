	push	22
	store	a
	push	7
	store	b
	push	1
	store	res
L000:
	load	b
	push	0
	cmpne
	jumpz	L001
	load	res
	load	a
	mul
	store	res
	load	b
	push	1
	sub
	store	b
	jump	L000
L001:
	load	res
	print
