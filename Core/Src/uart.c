#include "uart.h"
#include "system_stm32f4xx.h"

#define UART1_TX_BUFFER_SIZE 128U

static volatile uint8_t uart1_tx_buffer[UART1_TX_BUFFER_SIZE];
static volatile uint16_t uart1_tx_head;
static volatile uint16_t uart1_tx_tail;

static uint32_t uart1_get_pclk2_hz(void);
static void uart1_enable_gpio_and_usart_clocks(void);
static void uart1_configure_pins(void);
static uint16_t uart1_next_index(uint16_t index);
static void uart1_start_tx_interrupt(void);

void uart1_init(const uart_config_t *config)
{
    uart_config_t default_config = {
        .baud_rate = 9600U,
        .word_length = UART_WORD_LENGTH_8B,
        .parity = UART_PARITY_NONE,
        .stop_bits = UART_STOP_BITS_1
    };

    if (config == 0)
    {
        config = &default_config;
    }

    uart1_enable_gpio_and_usart_clocks();
    uart1_configure_pins();

    USART1->CR1 &= ~USART_CR1_UE;

    uart1_tx_head = 0U;
    uart1_tx_tail = 0U;

    uart1_set_baud_rate(config->baud_rate);

    USART1->CR1 = (uint32_t)config->word_length |
                  (uint32_t)config->parity |
                  USART_CR1_TE |
                  USART_CR1_RE;

    USART1->CR2 = (uint32_t)config->stop_bits;
    USART1->CR3 = 0U;

    NVIC_ClearPendingIRQ(USART1_IRQn);
    NVIC_EnableIRQ(USART1_IRQn);

    uart1_enable();
}

void uart1_enable(void)
{
    USART1->CR1 |= USART_CR1_UE;
}

void uart1_disable(void)
{
    USART1->CR1 &= ~(USART_CR1_TXEIE | USART_CR1_UE);
    NVIC_DisableIRQ(USART1_IRQn);
}

void uart1_set_baud_rate(uint32_t baud_rate)
{
    uint32_t pclk2_hz;

    if (baud_rate == 0U)
    {
        return;
    }

    pclk2_hz = uart1_get_pclk2_hz();

    /*
     * STM32F1 USART oversamples by 16.
     * BRR stores USARTDIV as mantissa:fraction, and rounding pclk/baud
     * gives the correct 12-bit mantissa plus 4-bit fraction encoding.
     */
    USART1->BRR = (pclk2_hz + (baud_rate / 2U)) / baud_rate;
}

size_t uart1_write(const uint8_t *data, size_t length)
{
    size_t written = 0U;

    while ((written < length) && (data != 0))
    {
        uint16_t next_head = uart1_next_index(uart1_tx_head);

        if (next_head == uart1_tx_tail)
        {
            break;
        }

        uart1_tx_buffer[uart1_tx_head] = data[written];
        uart1_tx_head = next_head;
        written++;
    }

    uart1_start_tx_interrupt();
    return written;
}

void uart1_write_blocking(const uint8_t *data, size_t length)
{
    size_t offset = 0U;

    if (data == 0)
    {
        return;
    }

    while (offset < length)
    {
        offset += uart1_write(&data[offset], length - offset);
    }

    while (uart1_tx_busy() != 0U)
    {
    }
}

void uart1_write_string(const char *text)
{
    const char *cursor = text;

    if (text == 0)
    {
        return;
    }

    while (*cursor != '\0')
    {
        cursor++;
    }

    uart1_write_blocking((const uint8_t *)text, (size_t)(cursor - text));
}

uint8_t uart1_tx_busy(void)
{
    if (uart1_tx_head != uart1_tx_tail)
    {
        return 1U;
    }

    return ((USART1->SR & USART_SR_TC) == 0U) ? 1U : 0U;
}

void USART1_IRQHandler(void)
{
    uint32_t status = USART1->SR;

    if (((status & USART_SR_TXE) != 0U) && ((USART1->CR1 & USART_CR1_TXEIE) != 0U))
    {
        if (uart1_tx_head == uart1_tx_tail)
        {
            USART1->CR1 &= ~USART_CR1_TXEIE;
        }
        else
        {
            USART1->DR = uart1_tx_buffer[uart1_tx_tail];
            uart1_tx_tail = uart1_next_index(uart1_tx_tail);
        }
    }

    if ((status & (USART_SR_ORE | USART_SR_NE | USART_SR_FE | USART_SR_PE)) != 0U)
    {
        /*
         * Error flags clear by reading SR and then DR. RX is enabled so this
         * keeps a stray byte from holding the USART in an error state.
         */
        (void)USART1->DR;
    }
}

static uint32_t uart1_get_pclk2_hz(void)
{
    uint32_t hclk_hz = SystemCoreClock;
    uint32_t ppre2 = (RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos;

    if (ppre2 < 4U)
    {
        return hclk_hz;
    }

    return hclk_hz >> (ppre2 - 3U);
}

static void uart1_enable_gpio_and_usart_clocks(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
}

static void uart1_configure_pins(void)
{
    const uint32_t pin9_pos = 18U;
    const uint32_t pin10_pos = 20U;
    const uint32_t af7 = 7U;

    /*
     * USART1 on STM32F401 uses PA9 = TX and PA10 = RX.
     * Both pins must be configured as alternate-function mode.
     */
    GPIOA->MODER &= ~((0x3UL << pin9_pos) | (0x3UL << pin10_pos));
    GPIOA->MODER |= ((0x2UL << pin9_pos) | (0x2UL << pin10_pos));

    GPIOA->OTYPER &= ~((1UL << 9U) | (1UL << 10U));
    GPIOA->OSPEEDR |= ((0x3UL << pin9_pos) | (0x3UL << pin10_pos));

    GPIOA->PUPDR &= ~((0x3UL << pin9_pos) | (0x3UL << pin10_pos));
    GPIOA->PUPDR |= (0x1UL << pin10_pos);

    GPIOA->AFR[1] &= ~((0xFU << ((9U - 8U) * 4U)) |
                        (0xFU << ((10U - 8U) * 4U)));
    GPIOA->AFR[1] |= ((af7 << ((9U - 8U) * 4U)) |
                       (af7 << ((10U - 8U) * 4U)));
}

static uint16_t uart1_next_index(uint16_t index)
{
    index++;

    if (index >= UART1_TX_BUFFER_SIZE)
    {
        index = 0U;
    }

    return index;
}

static void uart1_start_tx_interrupt(void)
{
    USART1->CR1 |= USART_CR1_TXEIE;
}
