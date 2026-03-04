#include <stdint.h>

#include "uart.h"

int main()
{
    uint8_t exit_status = 0;
    /* Generating an interrupt */
    __asm__ volatile ("li t0, 0x80      \n\t"
                      "csrw miselect, t0\n\t"
                      "li t1, -1        \n\t"
                      "csrw mireg, t1   \n\t"
                      :
                      :
                      : "t0"
                      );

    printf("All interrupts serviced successfully\n");
    return exit_status;
}
