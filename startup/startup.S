	.section .text.init, "x"
	.global _start
	.type _start, @function
_start:
	addi t0, zero, 5
	// Not calling mret causes the openSBI code to be executed
	// Because of this our value of t0 is overwritten
	// mret
	.size _start, .-_start
