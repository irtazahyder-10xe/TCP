#include <stdint.h>

#include "uart.h"
#include "aplic.h"

void change_privilege(uint8_t priv_mode);

int main()
{
    uint8_t exit_status = 0;

    // __asm__ volatile ("ecall");

    printf("\n========= SINGLE INTERRUPT TEST =========\n");
    aplic_send_msi(64, 0, 0, 64);

    printf("\n========= MULTIPLE INTERRUPT TEST =========\n");
    aplic_send_Nmsi(30, 5, 0, 0);

    printf("\n========= DELEGATION TEST =========\n");
    aplic_Nirq_delegate(50, 10, 0);
    aplic_Nirq_delegate(60, 5, 1);

    aplic_Nirq_delegate(50, 5, 0);
    aplic_Nirq_delegate(55, 5, 1);

    aplic_clrie(L0_SIRQ_DOMAIN, 1, (1UL << (61 % 32)));
    aplic_clrie(L1_MIRQ_DOMAIN, 1, (1UL << (53 % 32)));
    aplic_clrie(L1_SIRQ_DOMAIN, 1, (1UL << (57 % 32)));

    printf("\n========= MULTIPLE INTERRUPT TEST =========\n");
    aplic_send_Nmsi(50, 5, 0, 0);
    aplic_send_Nmsi(55, 5, 0, 0);
    aplic_send_Nmsi(60, 5, 0, 0);
    aplic_send_Nmsi(65, 5, 0, 0);

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
