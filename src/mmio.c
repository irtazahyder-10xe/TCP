#include "mmio.h"

void mmio_write(unsigned long address, long offset_bytes, uint8_t value)
{
    volatile char *mmio = ((char *) address);
    *(mmio + offset_bytes) = value;
}

uint8_t mmio_read(unsigned long address, long offset_bytes)
{
    volatile char *mmio = ((char *) address);
    return *(mmio + offset_bytes);
}

