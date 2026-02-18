	.section .text.init, "x"
	.global _start
	.type _start, @function
_start:
	addi t0, zero, 5
	// mret
	.size _start, .-_start
