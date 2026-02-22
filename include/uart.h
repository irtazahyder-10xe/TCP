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
 * @brief Transmits null terminated string to UART
 *
 * @param s Null terminated string to transmit to UART
 */
void prints(char* s);

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
 */
void scans(char* s);
