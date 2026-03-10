#include <stdint.h>

#include "uart.h"
#include "aplic.h"

void change_privilege(uint8_t priv_mode);

int main()
{
    uint8_t exit_status = 0;

    // __asm__ volatile ("ecall");

    printf("\n========= SINGLE INTERRUPT TEST =========\n");
    aplic_send_msi(2, 0, 0, 15);

    printf("\n========= MULTIPLE INTERRUPT TEST =========\n");
    aplic_send_Nmsi(30, 5, 0, 0);

    printf("\n========= DELEGATION TEST =========\n");
    aplic_conf_sourcecfg(4, ROOT_MIRQ_DOMAIN, 0, true);
    aplic_conf_sourcecfg(4, C1_SIRQ_DOMAIN, APLIC_SOURCECFG_SM_DETACH, false);

    printf("\n========= SINGLE INTERRUPT TEST =========\n");
    aplic_send_msi(4, 0, 0, 15);

    printf("\n========= MULTIPLE INTERRUPT TEST =========\n");
    aplic_Nirq_delegate(30, 10, C1_SIRQ_DOMAIN, 0);
    aplic_in_clrie(C1_SIRQ_DOMAIN, 1, 0x0000000F);

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
