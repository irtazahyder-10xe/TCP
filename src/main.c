#include <stdint.h>

#include "uart.h"
#include "aplic.h"

int main()
{
    uint8_t exit_status = 0;
    aplic_init(M_INTR_DOMAIN, 32);

    /* Generating single interrupt using APLIC */
    printf("\n========= SINGLE INTERRUPT TEST =========\n");
    aplic_send_msi(M_INTR_DOMAIN, 2, 0, 0, 15);

    /* Generating multiple interrupts based on bit mask */
    printf("\n========= MULTIPLE INTERRUPT TEST =========\n");
    aplic_send_Nmsi(M_INTR_DOMAIN, 0, 0, 0, 0xF000000E);

    printf("Program exited!");

    return exit_status;
}
