#include "main.h"

void delay_cycles(volatile uint32_t cycles)
{
    while (cycles-- > 0U) {
        __NOP();
    }
}

int main(void)
{
    /*
     * The startup file calls SystemInit() before main().
     * On this project that leaves the STM32F401 running from the default HSI
     * clock, which is enough for a simple GPIO blink.
     */
    GPIO_Init_t led1 = {
        .Pin = LED_PIN,
        .Mode = GPIO_MODE_OUTPUT,
        .OType = GPIO_OTYPE_PUSHPULL,
        .Speed = GPIO_SPEED_MEDIUM,
        .PuPd = GPIO_PUPD_NONE,
        .Alternate = GPIO_AF0
    };
    GPIO_Init_t led2 = {
        .Pin = LED_PIN2,
        .Mode = GPIO_MODE_OUTPUT,
        .OType = GPIO_OTYPE_PUSHPULL,
        .Speed = GPIO_SPEED_MEDIUM,
        .PuPd = GPIO_PUPD_NONE,
        .Alternate = GPIO_AF0
    };

    GPIO_Init_t uart_rx = {
        .Pin = UART_RX_PIN,
        .Mode = GPIO_MODE_OUTPUT,
        .OType = GPIO_OTYPE_PUSHPULL,
        .Speed = GPIO_SPEED_MEDIUM,
        .PuPd = GPIO_PUPD_NONE,
        .Alternate = GPIO_AF0
    };

    GPIO_ClockEnable(LED_PORT);
    GPIO_Init(LED_PORT, &led1);
    GPIO_Init(LED_PORT, &led2);
    
    GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
    GPIO_WritePin(LED_PORT, LED_PIN2, GPIO_PIN_RESET);

    static const uint8_t message_buffer[] =
        "Hello From STEVAL Test A bit longer string \r\n";

    uart_config_t uart_config = {
        .baud_rate = 115200U,
        .word_length = UART_WORD_LENGTH_8B,
        .parity = UART_PARITY_NONE,
        .stop_bits = UART_STOP_BITS_1
    };

    uart1_init(&uart_config);
    GPIO_Init(UART_PORT, &uart_rx);
    GPIO_WritePin(UART_PORT, UART_RX_PIN, GPIO_PIN_RESET);

    while (1) {
        uart1_write_blocking(message_buffer, sizeof(message_buffer) - 1U);
        GPIO_TogglePin(LED_PORT, LED_PIN);
        GPIO_TogglePin(LED_PORT, LED_PIN2);
        GPIO_TogglePin(UART_PORT, UART_RX_PIN);
        delay_cycles(1600000U);
    }
}
