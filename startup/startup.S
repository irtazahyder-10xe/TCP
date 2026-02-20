	.section .text.init, "x"
	.global _start
	.type _start, @function
_start:
	/* Storing the hart misa register content in a0 */
	csrr a0, misa
	.size _start, .-_start
