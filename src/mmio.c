#include "mmio.h"

#include <stdint.h>

void mmio_write_byte(uintptr_t address, uint32_t offset_bytes, uint8_t value)
{
    volatile uint8_t *mmio_addr = (uint8_t *) address + offset_bytes;
    (*mmio_addr) = value;
}

uint8_t mmio_read_byte(uintptr_t address, uint32_t offset_bytes)
{
    volatile uint8_t *mmio_addr = (uint8_t *) address + offset_bytes;
    return *(mmio_addr);
}

void mmio_write_short(uintptr_t address, uint32_t offset_shorts, uint16_t value)
{
    volatile uint16_t *mmio_addr = (uint16_t *) address + offset_shorts;
    (*mmio_addr) = value;
}

uint16_t mmio_read_short(uintptr_t address, uint32_t offset_shorts)
{
    volatile uint16_t *mmio_addr = (uint16_t *) address + offset_shorts;
    return *(mmio_addr);
}

void mmio_write_word(uintptr_t address, uint32_t offset_words, uint32_t value)
{
    volatile uint32_t *mmio_addr = (uint32_t *) address + offset_words;
    (*mmio_addr) = value;
}

uint32_t mmio_read_word(uintptr_t address, uint32_t offset_words)
{
    volatile uint32_t *mmio_addr = (uint32_t *) address + offset_words;
    return *(mmio_addr);
}

void mmio_write_double(uintptr_t address, uint32_t offset_doubles, uint64_t value)
{
    volatile uint64_t *mmio_addr = (uint64_t *) address + offset_doubles;
    (*mmio_addr) = value;
}

uint64_t mmio_read_double(uintptr_t address, uint32_t offset_doubles)
{
    volatile uint64_t *mmio_addr = (uint64_t *) address + offset_doubles;
    return *(mmio_addr);
}
