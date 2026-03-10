.include "startup/macro.S"

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

	/* Setting up the scratch registers */
	la t0, _mscratch
	csrw mscratch, t0

	la t0, _sscratch
	csrw sscratch, t0
	
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

	/* Configuring AIA */
	/* Interrupt register files already delcared */

	/* Setting Machine IF */
	/* Setting eidelivery */
	li t0, 0x70
	csrw miselect, t0
	csrwi mireg, 1

	/* Setting eithreshold */
	li t0, 0x72
	csrw miselect, t0
	csrwi mireg, 0

	/* Setting Supervisor IF */
	li t0, 0x70
	csrw siselect, t0
	csrwi sireg, 1

	li t0, 0x72
	csrw siselect, t0
	csrwi sireg, 0

	/* Initializing interrupt file eie CSRs*/
	li t0, EIE_ADDR_START
	li t1, EIE_ADDR_END
	li t2, -1
1:
	csrw miselect, t0
	csrw mireg, t2
	
	csrw siselect, t0
	csrw sireg, t2

	addi t0, t0, EIE_ADDR_OFFSET
	bltu t0, t1, 1b

	/* Clearing out .bss section */
	la t0, _bss_start
	la t1, _bss_end
1:
	sw zero, 0(t0)
	addi t0, t0, 4
	bltu t0, t1, 1b

2:
	/* Configuring PMP */
	la t0, _mscratch
	srli t0, t0, 2
	li t1, 0x003FFFFFFFFFFF
	and t0, t0, t1
	csrw pmpaddr0, t0

	li t0, 0x0F
	csrs pmpcfg0, t0

	/* Initializing the uart */
	call init_uart

	/* By default we execute everything in supervisor mode */
	li a0, 1
	call priv_change

	/* Initialzing APLIC */
	li a0, 0x60
	call aplic_init

	/* Jumping to main.c:main */
	call main
	j end

	/**
	 * Trap handler wrapper function.
	 */
	.section .text
	.global trap_handler
	.type trap_handler, @function
	.option norvc
trap_handler:
	PUSH_ALL mscratch
	csrr a0, mcause
	csrr a1, mstatus
	csrr a2, mepc

	call c_trap_handler
	csrw mepc, a0

	POP_ALL mscratch

	mret
	.size trap_handler, .-trap_handler

	/**
	 * Change privilege
	 *
	 * param a0 new privilege mode
	 */
	.section .text
	.global priv_change
	.type priv_change, @function
	.option norvc
priv_change:
	andi a0, a0, 0x3
	slli a0, a0, 11

	li t0, (3 << 11)
	csrc mstatus, t0
	csrs mstatus, a0

	csrw mepc, ra
	mret
	.size priv_change, .-priv_change

	/**
	 * Device termination routine
	 */
	.section .text.init
	.global end
	.type end, @function
	.option norvc
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
