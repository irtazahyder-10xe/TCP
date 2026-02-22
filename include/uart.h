#pragma once

/**
 * @brief Initializes UART
 *
 * @param addr MMIO address for UART
 */
void init_uart(int *address);

/**
 * @brief Transmits single character to UART
 *
 * @param c Character to transmit
 */
void putc(char c);

/**
 * @brief Receives the character on UART
 *
 * @return char Character in RBR
 */
char getc();
