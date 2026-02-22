#include "uart.h"

#include <stdarg.h>

typedef unsigned char uint8_t;

typedef struct {
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

void printf(const char *fmt, ...)
{
    va_list args_p;
    char *fmt_p, *str_val;
    long int int_val, count;

    va_start(args_p, fmt);
    for(fmt_p = fmt; *fmt_p != '\0'; fmt_p++) {
        if (*fmt_p != '%')
        {
            putc(*fmt_p);
        } else {
            switch (*(++fmt_p)) {
                case 's':
                    for (str_val = va_arg(args_p, char *); *str_val != '\0'; str_val++) {
                        putc(*str_val);
                    }
                    break;
                case 'd':
                case 'h':
                    count = 0;
                    int base = (*fmt_p == 'd') ? 10 : 16;
                    for (int_val = va_arg(args_p, int); int_val > 0; int_val /= base) {
                        if ((int_val % base) > 9) {
                            str_val[count++] = 'a' + (int_val % 10);
                        } else {
                            str_val[count++] = '0' + (int_val % base);
                        }
                    }
                    while(count) {
                        putc(str_val[--count]);
                    }
                    break;
                default:
                    putc('?');
            }
        }
    }
}

void scans(char *s)
{
    do {
        *s = getc();
        putc(*s);
    } while (*s++ != '\r');

    *(--s) = '\0';
    putc('\n');
}
