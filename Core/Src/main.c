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
    gpio_enable_clock(LED_PORT);
    gpio_config_output(LED_PORT, LED_PIN);
    gpio_config_output(LED_PORT, LED_PIN2);
    gpio_write(LED_PORT, LED_PIN, false);
    gpio_write(LED_PORT, LED_PIN2, false);

    static const uint8_t message_buffer[] =
        "Hello From STEVAL \r\n";

    uart_config_t uart_config = {
        .baud_rate = 9600U,
        .word_length = UART_WORD_LENGTH_8B,
        .parity = UART_PARITY_NONE,
        .stop_bits = UART_STOP_BITS_1
    };

    uart1_init(&uart_config);



    while (1) {
        uart1_write_blocking(message_buffer, sizeof(message_buffer) - 1U);
        gpio_toggle(LED_PORT, LED_PIN);
        gpio_toggle(LED_PORT, LED_PIN2);
        delay_cycles(1600000U);
    }
}
