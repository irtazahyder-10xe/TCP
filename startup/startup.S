.include "startup/consts.S"

	.section .text.init, "x"
	.global _start
	.type _start, @function
	.option norvc
_start:
	/* Checking HART ID, and making only the 0th core do initialization */
	csrr t0, mhartid
	bnez t0, 3f

	/* TODO: Add multi hart logic */

	/* Initializing stack and global pointer */
	la sp, _estack
	.option push
	.option norelax
	la gp, _global_pointer
	.option pop

	/* Initialize trap handler, and using direct mode */
	la t0, trap_handler
	csrw mtvec, t0
	li t0, 0x3
	csrc mtvec, t0
	
	/* Enabling global and local interrupts */
	li t0, MSTATUS_SIE
	ori t0, t0, MSTATUS_SPIE
	ori t0, t0, MSTATUS_MIE
	ori t0, t0, MSTATUS_MPIE
	csrs mstatus, t0

	li t0, XIE_BASE
	csrw mie, t0

	li t0, XIE_BASE
	csrw sie, t0

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
	la t0, 2f
	csrw mepc, t0
	mret

2:
	/* Jumping to main.c */
	jal main

end:
	/* Shutdown sequence */
	li t0, test_dev_addr
	bnez a0, .fail
.pass:
	li t1, test_dev_poweroff
	j .exit
.reboot:
	li t1, test_dev_reboot
	j .exit
.fail:
	/* Operates identical to the to the poweroff device */
	/* TODO: Define pass and fail conditions */
	li t1, test_dev_fail
.exit:
	sw t1, 0(t0)
	.size _start, .-_start


3:
	wfi
	j 3b
