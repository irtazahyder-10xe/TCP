#include "trap.h"

#include <stdint.h>
#include <stdbool.h>

#include "uart.h"

void trap_handler()
{
    // TODO: remove volatile keyword
    // For now use to prevent variables from getting optimized out
    volatile uint64_t mcause, mstatus, mepc;

    __asm__ volatile ("csrr %0, mcause  \n\t"
                      "csrr %1, mstatus \n\t"
                      "csrr %2, mepc    \n\t"
                      : "=&r" (mcause), "=&r" (mstatus), "=&r" (mepc)
                      :
                      );

    /* Extracting interrupt and exception code from mcause */
    bool is_interrupt = (mcause >> 63);
    uint64_t exp_code = (mcause & 0x7FFFFFFFFFFFFFFF);

    printf("\n===========\n%s\nmcause: %d\n===========\n",
           is_interrupt ? "Interrupt" : "Exception",
           exp_code);
    mepc += 8;

    /* Make changes to mstatus if required */

    __asm__ volatile ("csrw mepc, %0    \n\t"
                      "mret"
                      :
                      : "r" (mepc)
                      );
}
