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
