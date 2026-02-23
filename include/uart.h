#pragma once

/**
 * @brief Initializes UART
 *
 * @param addr MMIO address for UART
 */
void init_uart();

/**
 * @brief Transmits single character to UART
 *
 * @param c Character to transmit
 */
void putc(char c);

/**
 * @brief Displays formatted string on stdio
 *
 * @param fmt Formatted string similar to the C printf
 */
void printf(const char* fmt, ...);

/**
 * @brief Receives the character on UART
 *
 * @return char Character in RBR
 */
char getc();

/**
 * @brief Receives \r terminated string from UART
 *
 * NOTE: Enter key triggers \r not newline.
 * NOTE: Backspaces are not supported
 * @param s Pointer to buffer to store the string
 * @return int Size of the string
 */
int scans(char* s);
