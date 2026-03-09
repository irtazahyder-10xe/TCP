#include <stdint.h>

#include "uart.h"
#include "aplic.h"

void change_privilege(uint8_t priv_mode);

int main()
{
    uint8_t exit_status = 0;
    aplic_init(96);

    /* Generating single interrupt using APLIC */
    printf("\n========= SINGLE INTERRUPT TEST =========\n");
    aplic_send_msi(2, 0, 0, 15);

    /* Generating multiple interrupts based on bit mask */
    printf("\n========= MULTIPLE INTERRUPT TEST =========\n");
    aplic_send_Nmsi(30, 5, 0, 0);

    /* Delegation Test */
    printf("\n========= DELEGATION TEST =========\n");
    aplic_conf_sourcecfg(ROOT_MIRQ_DOMAIN, 4, true, 0);
    aplic_conf_sourcecfg(C1_SIRQ_DOMAIN, 4, false, APLIC_SOURCECFG_SM_DETACH);

    /* Going to supervisor mode */
    // change_privilege(0x01);

    // __asm__ volatile ("ecall");

    aplic_send_msi(4, 0, 0, 15);

    aplic_Nirq_delegate(C1_SIRQ_DOMAIN, 30, 10, 1);
    aplic_send_Nmsi(30, 10, 0, 0);

    printf("Program exited!");

    return exit_status;
}

void change_privilege(uint8_t priv_mode)
{
    __asm__ volatile ("mv a0, %0 \n\t" 
                      "call priv_change \n\t"
                      ::"r" (priv_mode): "a0"
                      );
}
