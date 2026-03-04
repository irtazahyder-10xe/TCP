/* CONSTANTS */
.equ test_dev_addr, 0x100000
.equ test_dev_poweroff, 0x5555
.equ test_dev_reboot, 0x7777
.equ test_dev_fail, 0x3333

.equ MSTATUS_SIE, (1 << 1)
.equ MSTATUS_MIE, (1 << 3)
.equ MSTATUS_SPIE, (1 << 5)
.equ MSTATUS_MPIE, (1 << 7)

.equ MSTATUS_MPP_M, (3 << 11)
.equ MSTATUS_MPP_S, (1 << 11)

.equ XIP_BASE, (7 << 9)
.equ XIE_BASE, (7 << 9)

.equ EIE_ADDR_START, 0xC0
.equ EIE_ADDR_END, 0xC2
/* NOTE: For RV64 only even number eie & eip registers are configured */
.equ EIE_ADDR_OFFSET, 0x2

/* MACROS */
.altmacro

.macro PUSH from=1, to=31
  sd x\from, 8*\from(t0)
  .if \to - \from
    PUSH %(\from+1), \to
  .endif
.endm

.macro PUSH_ALL scratch
  csrrw t0, scratch, t0
  PUSH 1, 31
  csrrw t0, scratch, t0
.endm

.macro POP from=1, to=31
  ld x\from, 8*\from(t0)
  .if \to - \from
    POP %(\from+1), \to
  .endif
.endm

.macro POP_ALL scratch
  csrrw t0, scratch, t0
  POP 1, 31
  csrrw t0, scratch, t0
.endm
