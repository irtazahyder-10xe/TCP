#include "uart.h"

typedef unsigned char uint8_t;

typedef struct
{
    uint8_t RBR; /* Multiplexed with THR or DLL */
    uint8_t IER; /* Multiplexed with DLM */
    uint8_t FCR; /* Multiplexed with IIR */
    uint8_t LCR;
    uint8_t MCR;
    uint8_t LSR;
    uint8_t SSR;
    uint8_t SCR;
} UartS;

volatile UartS *UART;

void init_uart(int *address)
{
    UART = (UartS *) address;

    /* Setting baud to 9600, even though this does not matter for QEMU */
    UART->LCR = 0x80;
    UART->RBR = 0x0c;
    UART->IER = 0x00;
    UART->LCR = 0x00;

    /* Configuring 8 bit UART, 1 stop bit, 0 parity */
    UART->LCR = 0x03;
    UART->FCR = 0x01;
    /* TODO: Change polling mechanism to interrupt based */
    UART->IER = 0x00;
}

void putc(char c)
{
    /* Waiting for transmitter to become emtpy */
    while (!(UART->LSR & 0x40));
    UART->RBR = c;
}

char getc()
{
    /* Waiting for LSR.DR to become 1 */
    while (!(UART->LSR & 0x01));
    return UART->RBR;
}
