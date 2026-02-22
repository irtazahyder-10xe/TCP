#include "uart.h"

int main(){
    int *UART_ADDRESS = (int *) 0x10000000;
    init_uart(UART_ADDRESS);
    char str[] = "Hello World";
    for (int i = 0; i < sizeof(str)/sizeof(char); i++)
        putc(str[i]);
    return 0;
}
