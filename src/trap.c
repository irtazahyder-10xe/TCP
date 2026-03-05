#include <stdint.h>
#include <stdbool.h>

#include "uart.h"

typedef enum EIIMode {
    S = 9,
    VS = 10,
    M = 11
} EIIMode;

uintptr_t *c_trap_handler(uint64_t mcause, uint64_t mstatus, uintptr_t *mepc);

static void external_interrupt_handler(EIIMode exp_code)
{
    // TODO: Program seperate interrupt handling routines for M, S, VS
    uint64_t eiid = 0;
    printf("MODE: ");
    switch (exp_code) {
    case S:
            printf("Supervisor\n");
            __asm__ volatile ("csrrw %0, stopei, zero \n\t"
                              : "=r" (eiid):);
            break;
    case VS:
            printf("Virtual Supervisor\n");
            __asm__ volatile ("csrrw %0, vstopei, zero \n\t"
                              : "=r" (eiid):);
            break;
    case M:
            printf("Machine\n");
            __asm__ volatile ("csrrw %0, mtopei, zero \n\t"
                              : "=r" (eiid):);
            break;
    }

    /* Fetching the eiid of the external interrupt */
    /* NOTE: Write to mtopei automatically clears the interrupt pending bit */
    eiid >>= 16;
    printf("--------------------\nMinor identity: %d\n--------------------\n", eiid);
}

static void exception_handler(uint64_t exp_code, uint64_t mstatus, uint64_t *mepc)
{
    printf("\n===========\n%s\nmcause: %d\n===========\n",
           "Exception",
           exp_code);
    /* Shutdown device if an exception takes place */
    __asm__ volatile ("li a0, 1\n\t"
                      "j end"
                      ::: "a0");
}

uintptr_t *c_trap_handler(uint64_t mcause, uint64_t mstatus, uintptr_t *mepc)
{
    /* Extracting interrupt and exception code from mcause */
    bool is_interrupt = (mcause >> 63);
    uint64_t exp_code = (mcause & 0x7FFFFFFFFFFFFFFF);

    // printf("\n===========\n%s\nmcause: %d\n===========\n",
    //        is_interrupt ? "Interrupt" : "Exception",
    //        exp_code);

    if (is_interrupt && (9 <= exp_code && exp_code <= 11))
        external_interrupt_handler((EIIMode) exp_code);
    else if (!is_interrupt)
        exception_handler(exp_code, mstatus, mepc);
    return mepc;
}
