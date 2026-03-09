#include <stdint.h>

#include "uart.h"
#include "aplic.h"

void change_privilege(uint8_t priv_mode);

int main()
{
    uint8_t exit_status = 0;
    aplic_init(32);

    /* Generating single interrupt using APLIC */
    printf("\n========= SINGLE INTERRUPT TEST =========\n");
    aplic_send_msi(2, 0, 0, 15);

    /* Generating multiple interrupts based on bit mask */
    printf("\n========= MULTIPLE INTERRUPT TEST =========\n");
    aplic_send_Nmsi(0, 0, 0, 0xF000000E);

    /* Delegation Test */
//     printf("\n========= DELEGATION TEST =========\n");
//     aplic_conf_sourcecfg(ROOT_MINTR_DOMAIN, 4, 1, 0);
//     aplic_conf_sourcecfg(C0_SINTR_DOMAIN, 4, 0, APLIC_SOURCECFG_SM_DETACH);
// 
//     /* Going to supervisor mode */
//     // change_privilege(0x01);
// 
//     // __asm__ volatile ("ecall");
// 
//     aplic_send_msi(4, 0, 0, 15);
// 
//     printf("Program exited!");
// 
    return exit_status;
}

void change_privilege(uint8_t priv_mode)
{
    __asm__ volatile ("mv a0, %0 \n\t" 
                      "call priv_change \n\t"
                      ::"r" (priv_mode): "a0"
                      );
}
