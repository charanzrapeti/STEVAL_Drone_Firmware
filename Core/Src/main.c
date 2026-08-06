#include "main.h"

#define WHO_AM_I_REG           0x0FU
#define CTRL_REG1_REG          0x10U
#define PRESS_OUT_XL_REG       0x28U
#define PRESS_OUT_L_REG        0x29U
#define PRESS_OUT_H_REG        0x2AU
#define LPS22HD_WHO_AM_I_VALUE 0xB1U
#define CTRL_REG1_ODR_1HZ      0x10U

void delay_cycles(volatile uint32_t cycles)
{
    while (cycles-- > 0U) {
        __NOP();
    }
}

static void uart_write_decimal(int32_t value)
{
    char buffer[16];
    uint32_t index = 0U;
    uint32_t start;
    uint32_t digit;

    if (value < 0) {
        buffer[index++] = '-';
        value = (uint32_t)(-value);
    }

    if (value == 0) {
        buffer[index++] = '0';
    } else {
        while (value > 0) {
            digit = (uint32_t)(value % 10);
            value /= 10;
            buffer[index++] = (char)('0' + digit);
        }
    }

    start = (index > 0U && buffer[0] == '-') ? 1U : 0U;

    for (uint32_t i = start; i < index / 2U + start; i++) {
        char temp = buffer[i];
        buffer[i] = buffer[index - 1U - (i - start)];
        buffer[index - 1U - (i - start)] = temp;
    }

    uart1_write_blocking((const uint8_t *)buffer, index);
}

static void uart_write_hex_byte(uint8_t value)
{
    static const char hex_chars[] = "0123456789ABCDEF";
    char buffer[2];

    buffer[0] = hex_chars[(value >> 4U) & 0x0FU];
    buffer[1] = hex_chars[value & 0x0FU];
    uart1_write_blocking((const uint8_t *)buffer, 2U);
}

static void uart_write_hex24(uint32_t value)
{
    const uint8_t prefix[] = "0x";
    uart1_write_blocking(prefix, sizeof(prefix) - 1U);

    for (int32_t shift = 20; shift >= 0; shift -= 4) {
        uart_write_hex_byte((uint8_t)((value >> shift) & 0x0FU));
    }
}

static void uart_write_pressure(int32_t pressure_raw)
{
    const uint8_t prefix[] = "Pressure: ";
    const uint8_t suffix[] = " hPa\r\n";
    int32_t whole;
    int32_t fract;
    int32_t sign = 1;

    uart1_write_blocking(prefix, sizeof(prefix) - 1U);

    if (pressure_raw < 0) {
        sign = -1;
        pressure_raw = -pressure_raw;
    }

    whole = pressure_raw / 4096;
    fract = ((pressure_raw % 4096) * 1000) / 4096;

    if (sign < 0) {
        uart1_write_blocking((const uint8_t *)"-", 1U);
    }

    uart_write_decimal(whole);
    uart1_write_blocking((const uint8_t *)".", 1U);

    if (fract < 100) {
        uart1_write_blocking((const uint8_t *)"0", 1U);
    }
    if (fract < 10) {
        uart1_write_blocking((const uint8_t *)"0", 1U);
    }

    uart_write_decimal(fract);
    uart1_write_blocking(suffix, sizeof(suffix) - 1U);
}

int main(void)
{
    uint8_t who_am_i;
    uint8_t pressure_xl;
    uint8_t pressure_l;
    uint8_t pressure_h;
    uint32_t pressure_word;
    int32_t pressure_raw;

    gpio_enable_clock(LED_PORT);
    gpio_config_output(LED_PORT, LED_PIN);
    gpio_config_output(LED_PORT, LED_PIN2);
    gpio_write(LED_PORT, LED_PIN, false);
    gpio_write(LED_PORT, LED_PIN2, false);

    uart_config_t uart_config = {
        .baud_rate = 115200U,
        .word_length = UART_WORD_LENGTH_8B,
        .parity = UART_PARITY_NONE,
        .stop_bits = UART_STOP_BITS_1
    };

    uart1_init(&uart_config);
    spi_init();

    uart1_write_blocking((const uint8_t *)"SPI pressure demo started\r\n", 28U);

    who_am_i = spi_read_register(WHO_AM_I_REG);
    uart1_write_blocking((const uint8_t *)"WHO_AM_I = 0x", 13U);
    uart_write_hex_byte(who_am_i);
    uart1_write_blocking((const uint8_t *)"\r\n", 2U);

    if (who_am_i != LPS22HD_WHO_AM_I_VALUE) {
        uart1_write_blocking((const uint8_t *)"WHO_AM_I mismatch: sensor not responding correctly\r\n", 53U);
        while (1) {
            gpio_toggle(LED_PORT, LED_PIN);
            gpio_toggle(LED_PORT, LED_PIN2);
            delay_cycles(8000000U);
        }
    }

    spi_write_register(CTRL_REG1_REG, CTRL_REG1_ODR_1HZ);
    uart1_write_blocking((const uint8_t *)"CTRL_REG1 set to 0x10 (ODR=1Hz)\r\n", 34U);

    while (1) {
        pressure_xl = spi_read_register(PRESS_OUT_XL_REG);
        pressure_l = spi_read_register(PRESS_OUT_L_REG);
        pressure_h = spi_read_register(PRESS_OUT_H_REG);

        pressure_word = ((uint32_t)pressure_h << 16U) |
                        ((uint32_t)pressure_l << 8U) |
                        (uint32_t)pressure_xl;

        pressure_raw = (int32_t)pressure_word;
        if ((pressure_raw & 0x00800000) != 0U) {
            pressure_raw |= (int32_t)0xFF000000;
        }

        uart1_write_blocking((const uint8_t *)"XL = 0x", 7U);
        uart_write_hex_byte(pressure_xl);
        uart1_write_blocking((const uint8_t *)"  L = 0x", 8U);
        uart_write_hex_byte(pressure_l);
        uart1_write_blocking((const uint8_t *)"  H = 0x", 8U);
        uart_write_hex_byte(pressure_h);
        uart1_write_blocking((const uint8_t *)"\r\n", 2U);

        uart1_write_blocking((const uint8_t *)"RAW = ", 6U);
        uart_write_hex24(pressure_word);
        uart1_write_blocking((const uint8_t *)"\r\n", 2U);

        uart_write_pressure(pressure_raw);
        gpio_toggle(LED_PORT, LED_PIN);
        gpio_toggle(LED_PORT, LED_PIN2);

        delay_cycles(4000000U);
    }
}
