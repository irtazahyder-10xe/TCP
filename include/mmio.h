#pragma once

#include <stdint.h>

/**
 * @brief Write byte to MMIO + offset
 *
 * @param address MMIO base address
 * @param offset_bytes Offset in bytes from MMIO base address
 * @param value Byte to write in at (address + offset+bytes)
 */
void mmio_write(unsigned long address, long offset_bytes, uint8_t value);

/**
 * @brief Read byte from MMIO + offset
 *
 * @param address MMIO base address
 * @param offset_bytes Offset in bytes from MMIO base address
 *                     Address to write = address + offset_bytes
 * @return uint8_t Byte read from (address + offset_bytes)
 */
uint8_t mmio_read(unsigned long address, long offset_bytes);
