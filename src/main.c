#include "uart.h"

int main(){
    int *UART_ADDRESS = (int *) 0x10000000;
    init_uart(UART_ADDRESS);

    /* Testing output monitor */
    prints("Hello World\n");

    /* Testing input and output functions */
    char buffer[100];
    scans(buffer);
    prints("User entered: ");
    prints(buffer);

    return 0;
}
