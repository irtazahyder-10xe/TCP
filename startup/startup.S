	.section .text.init, "x"
	.global _start
	.type _start, @function
	.option norvc
_start:
	/* Checking HART ID, and making only the 0th core do initialization */
	csrr t0, mhartid
	bnez t0, 1f

	/* TODO: Add multi hart logic */

	/* Initializing stack and global pointer */
	la sp, _sstack
	.option push
	.option norelax
	la gp, _global_pointer
	.option pop

	/* TODO: Initialize mtvec */

	/* Clearing out .bss section */
	la t0, _bss_start
	la t1, _bss_end

1:
	sw zero, 0(t0)
	addi t0, t0, 4
	bltu t0, t1, 1b

	/* Setting MPP to be machine mode */
	li t0, (0x3 << 11)
	csrs mstatus, t0

	/* Jumping to main.c */
	la t0, main
	csrw mepc, t0

	mret
	.size _start, .-_start

2:
	wfi
	j 2b
