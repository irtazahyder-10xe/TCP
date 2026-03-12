#pragma once

/**
 * @brief Base address for the UART controller.
 * Address pointing to the start of the MMIO range for the 16550 UART
 */
#define UART_ADDR 0x10000000

/**
 * @name UART Register Offsets
 * List of offsets for the UART control and status registers. 
 * @note that some registers are multiplexed based on the DLAB bit in the LCR.
 * @{
 */

/** Receive Buffer (R), Transmit Holding (W), or Divisor Latch Low (R/W) */
#define RBR 0 
#define THR 0
#define DLL 0

/** Interrupt Enable (R/W) or Divisor Latch High (R/W) */
#define IER 1 
#define DLM 1

/** FIFO Control (W) or Interrupt Identification (R) */
#define FCR 2 
#define IIR 2

#define LCR 3 /**< Line Control Register */
#define MCR 4 /**< Modem Control Register */
#define LSR 5 /**< Line Status Register */
#define SSR 6 /**< Status Register */
#define SCR 7 /**< Scratch Register */

/** @} */

/**
 * @brief Initializes UART at MMIO UART_ADDRESS
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
 * @param s Pointer to buffer to store the string
 * @return int Size of the string
 * @note Enter key triggers \r not newline.
 * @note Backspaces are not supported
 */
int scans(char* s);
