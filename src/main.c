#include <stdint.h>
#include <sys/types.h>

#include "uart.h"

int main() {

    init_uart();
    /* Testing output monitor */
    for (int i = -1; i < 2; i++) {
        int *p = &i;
        printf("String: %s\nDecimal: %d\nHex: 0x%x\nUnsigned: %u\nPointer: 0x%p\n",
               "Hello World",
               i,
               i,
               i,
               p);
        printf("---------------\n");
    }

    /* Testing input and output functions */
    char buffer[100];
    scans(buffer);
    printf("User entered: %s", buffer);

    return 0;
}
