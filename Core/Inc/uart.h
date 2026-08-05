#ifndef PLATFORM_UART_H
#define PLATFORM_UART_H

/*
 * Public UART driver API will live here.
 * Kept as a separate module so app code stays independent of USART registers.
 */
#include <stddef.h>
#include <stdint.h>

#include "stm32f4xx.h"

typedef enum
{
    UART_WORD_LENGTH_8B = 0U,
    UART_WORD_LENGTH_9B = USART_CR1_M
} uart_word_length_t;

typedef enum
{
    UART_PARITY_NONE = 0U,
    UART_PARITY_EVEN = USART_CR1_PCE,
    UART_PARITY_ODD  = USART_CR1_PCE | USART_CR1_PS
} uart_parity_t;

typedef enum
{
    UART_STOP_BITS_1   = 0U,
    UART_STOP_BITS_0_5 = USART_CR2_STOP_0,
    UART_STOP_BITS_2   = USART_CR2_STOP_1,
    UART_STOP_BITS_1_5 = USART_CR2_STOP_0 | USART_CR2_STOP_1
} uart_stop_bits_t;

typedef struct
{
    uint32_t baud_rate;
    uart_word_length_t word_length;
    uart_parity_t parity;
    uart_stop_bits_t stop_bits;
} uart_config_t;


void uart1_init(const uart_config_t *config);
void uart1_enable(void);
void uart1_disable(void);
void uart1_set_baud_rate(uint32_t baud_rate);

size_t uart1_write(const uint8_t *data, size_t length);
void uart1_write_blocking(const uint8_t *data, size_t length);
void uart1_write_string(const char *text);
uint8_t uart1_tx_busy(void);

#endif
