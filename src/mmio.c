#include "mmio.h"

#include <stdint.h>

void mmio_write_N(uint8_t N, uint64_t address, uint32_t offset_bytes, uint64_t value)
{
    char *mmio = ((char *) address) + offset_bytes;
    for (int i = 0; i < N; i++) {
        *(mmio + i) = value & 0xFF;
        value >>= 8;
    }
}

uint64_t mmio_read_N(uint8_t N, uint64_t address, uint32_t offset_bytes)
{
    char *mmio = ((char *) address) + offset_bytes;
    uint64_t out = 0;
    for (int i = N - 1; i > 0; i--) {
        out |= *(mmio + i);
        out <<= 8;
    }
    out |= *(mmio);
    return out;
}
