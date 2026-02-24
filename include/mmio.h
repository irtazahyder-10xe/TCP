#pragma once

#include <stdint.h>

/**
 * @name MMIO Write Operations
 * Macros for default sizes to be written to MMIO.
 * All these macros internally call mmio_write_N with the respective byte size.
 * @{
 */
#define MMIO_WRITE_BYTE(addr,offset,val)   mmio_write_N(1, addr, offset, val)
#define MMIO_WRITE_SHORT(addr,offset,val)  mmio_write_N(2, addr, offset, val)
#define MMIO_WRITE_WORD(addr,offset,val)   mmio_write_N(4, addr, offset, val)
#define MMIO_WRITE_DOUBLE(addr,offset,val) mmio_write_N(8, addr, offset, val)
/** @} */

/**
 * @name MMIO Read Operations
 * Macros for default sizes to be read from MMIO.
 * @{
 */
#define MMIO_READ_BYTE(addr,offset)    mmio_read_N(1, addr, offset)
#define MMIO_READ_SHORT(addr,offset)   mmio_read_N(2, addr, offset)
#define MMIO_READ_WORD(addr,offset)    mmio_read_N(4, addr, offset)
#define MMIO_READ_DOUBLE(addr,offset)  mmio_read_N(8, addr, offset)
/** @} */

/**
 * @brief Write N bytes to MMIO + offset
 *
 * @param N Number of bytes to write
 * @param address MMIO base address
 * @param offset_bytes Offset in bytes from MMIO base address
 * @param value Byte to write in at (address + offset_bytes)
 *
 * @warning WARNING: User should ensure that the address given aligns with N
 */
void mmio_write_N(uint8_t N, uint64_t address, uint32_t offset_bytes, uint64_t value);

/**
 * @brief Read N bytes from MMIO + offset
 *
 * @param N Number of bytes to write
 * @param address MMIO base address
 * @param offset_bytes Offset in bytes from MMIO base address
 *                     Address to write = address + offset_bytes
 * @return uint64_t Byte read from (address + offset_bytes)
 * @warning User should ensure that the address given aligns with N
 */
uint64_t mmio_read_N(uint8_t N, uint64_t address, uint32_t offset_bytes);
