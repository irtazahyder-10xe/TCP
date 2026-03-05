#include <stdint.h>

#include "uart.h"
#include "aplic.h"

void change_privilege(uint8_t priv_mode);

int main()
{
    uint8_t exit_status = 0;
    change_privilege(0x01);
    __asm__ volatile ("li t0, 0x80          \n\t"
                          "csrw siselect, t0\n\t"
                          "li t1, -1        \n\t"
                          "csrw sireg, t1   \n\t"
                          :
                          :
                          : "t0"
                          );

    printf("All interrupts serviced successfully\n");
    return exit_status;
}

void change_privilege(uint8_t priv_mode)
{
    __asm__ volatile ("mv a0, %0 \n\t" 
                      "call priv_change \n\t"
                      ::"r" (priv_mode): "a0"
                      );
}
