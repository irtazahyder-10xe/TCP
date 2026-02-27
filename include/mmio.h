#pragma once

#include <stdint.h>

/**
 * @brief Write byte to MMIO + offset
 *
 * @param address MMIO base address
 * @param offset_bytes Offset in bytes from MMIO base address
 * @param value Byte to write at (address + offset)
 */
void mmio_write_byte(uintptr_t address, uint32_t offset_bytes, uint8_t value);

/**
 * @brief Read byte from MMIO + offset
 *
 * @param address MMIO base address
 * @param offset_bytes Offset in bytes from MMIO base address
 *
 * @return uint8_t Byte read from address (MMIO + offset)
 */
uint8_t mmio_read_byte(uintptr_t address, uint32_t offset_bytes);

/**
 * @brief Write half word (2 bytes) to MMIO + offset
 *
 * @param address MMIO base address
 * @param offset_shorts Offset in half word (2 bytes) from MMIO base address
 * @param value Half word to write at (address + offset)
 *
 * @warning WARNING: User should ensure that the address is 2 bytes aligned
 */
void mmio_write_short(uintptr_t address, uint32_t offset_shorts, uint16_t value);

/**
 * @brief Read half word (2 bytes) from MMIO + offset
 *
 * @param address MMIO base address
 * @param offset_shorts Offset in half word from MMIO base address
 *
 * @return uint16_t Half word read from address (MMIO + offset)
 * @warning WARNING: User should ensure that the address is 2 bytes aligned
 */
uint16_t mmio_read_short(uintptr_t address, uint32_t offset_shorts);

/**
 * @brief Write word (4 bytes) to MMIO + offset
 *
 * @param address MMIO base address
 * @param offset_words Offset in words from MMIO base address
 * @param value Word to write at (address + offset)
 *
 * @return uint32_t Word read from address (MMIO + offset)
 * @warning WARNING: User should ensure that the address is 4 bytes aligned
 */
void mmio_write_word(uintptr_t address, uint32_t offset_words, uint32_t value);

/**
 * @brief Read word (4 bytes) from MMIO + offset
 *
 * @param address MMIO base address
 * @param offset_words Offset in words from MMIO base address
 *
 * @warning WARNING: User should ensure that the address is 4 bytes aligned
 */
uint32_t mmio_read_word(uintptr_t address, uint32_t offset_words);

/**
 * @brief Write double (8 bytes) to MMIO + offset
 *
 * @param address MMIO base address
 * @param offset_doubles Offset in doubles from MMIO base address
 * @param value Double to write at (address + offset_doubles)
 *
 * @return uint64_t Double read from address (MMIO + offset)
 * @warning WARNING: User should ensure that the address is 8 bytes aligned
 */
void mmio_write_double(uintptr_t address, uint32_t offset_doubles, uint64_t value);

/**
 * @brief Read double (8 bytes) from MMIO + offset
 *
 * @param address MMIO base address
 * @param offset_doubles Offset in doubles from MMIO base address
 *
 * @warning WARNING: User should ensure that the address is 8 bytes aligned
 */
uint64_t mmio_read_double(uintptr_t address, uint32_t offset_doubles);
