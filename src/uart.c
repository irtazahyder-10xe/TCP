#include "uart.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "mmio.h"

#define UART_ADDR 0x10000000

#define RBR 0 /* Multiplexed with THR or DLL */
#define THR 0
#define DLL 0
#define IER 1 /* Multiplexed with DLM */
#define DLM 1
#define FCR 2 /* Multiplexed with IIR */
#define IIR 2
#define LCR 3
#define MCR 4
#define LSR 5
#define SSR 6
#define SCR 7

void init_uart()
{
    /* Setting baud to 9600, even though this does not matter for QEMU */
    mmio_write(UART_ADDR, LCR, 0x80);
    mmio_write(UART_ADDR, DLL, 0x0c);
    mmio_write(UART_ADDR, DLM, 0x00);
    mmio_write(UART_ADDR, LCR, 0x00);

    /* Configuring 8 bit UART, 1 stop bit, 0 parity */
    mmio_write(UART_ADDR, LCR, 0x03);
    mmio_write(UART_ADDR, FCR, 0x01);

    /* TODO: Change polling mechanism to interrupt based */
    mmio_write(UART_ADDR, IER, 0x00);
}

void putc(char c)
{
    /* Waiting for transmitter to become emtpy */
    while (!(mmio_read(UART_ADDR, LSR) & 0x40));
    mmio_write(UART_ADDR, RBR, c);
}

char getc()
{
    /* Waiting for LSR.DR to become 1 */
    while (!(mmio_read(UART_ADDR, LSR) & 0x01));
    return mmio_read(UART_ADDR, THR);
}

static void print_num(unsigned long num, size_t base, bool is_signed)
{
    int count = 0;
    /* Since the largest 64 bit number is:
     * 2^64 - 1 = 18446744073709551615
     * digits = 20
     */
    char num_buffer[20];

    /* Handle edge case when num == 0 */
    if (num == 0){
        putc('0');
        return;
    }

    if (is_signed && (num >> 31)) {
        /* Taking 2's compliment if we have a signed number and its MSB is 1 */
        num *= -1;
        putc('-');
    }
    for (; num > 0; num /= base) {
        if ((num % base) > 9) {
            num_buffer[count++] = 'a' + (num % 10);
        } else {
            num_buffer[count++] = '0' + (num % base);
        }
    }
    while(count) {
        putc(num_buffer[--count]);
    }
}

void printf(const char *fmt, ...)
{
    va_list args_p;
    char *str_val;

    va_start(args_p, fmt);
    for(size_t fmt_count = 0; fmt[fmt_count] != '\0'; fmt_count++) {
        if (fmt[fmt_count] != '%')
        {
            putc(fmt[fmt_count]);
        } else {
            switch (fmt[++fmt_count]) {
                case 's':
                    for (str_val = va_arg(args_p, char *); *str_val != '\0'; str_val++) {
                        putc(*str_val);
                    }
                    break;
                case 'd':
                    print_num(va_arg(args_p, unsigned long), 10, true);
                    break;
                case 'x':
                    print_num(va_arg(args_p, unsigned long), 16, false);
                    break;
                case 'u':
                    print_num(va_arg(args_p, unsigned long), 10, false);
                    break;
                case 'p':
                    print_num(va_arg(args_p, unsigned long), 16, false);
                    break;
                default:
                    putc('?');
            }
        }
    }
    va_end(args_p);
}

int scans(char *s)
{
    int str_len = 0;
    do {
        *s = getc();
        putc(*s);
        str_len++;
    } while (*s++ != '\r');

    *(--s) = '\0';
    putc('\n');
    return str_len - 1;
}
