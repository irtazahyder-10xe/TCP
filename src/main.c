#include "uart.h"

int main() {
    int *UART_ADDRESS = (int *) 0x10000000;
    init_uart(UART_ADDRESS);

    /* Testing output monitor */
    printf("String: %s\nDecimal: %d\nHex: 0x%h\n", "Hello World", 0xa, UART_ADDRESS);

    /* Testing input and output functions */
    char buffer[100];
    scans(buffer);
    printf("User entered: %s", buffer);

    return 0;
}
