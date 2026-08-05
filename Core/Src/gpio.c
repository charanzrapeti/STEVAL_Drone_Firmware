#include "gpio.h"

void gpio_enable_clock(GPIO_TypeDef *port)
{
    if (port == GPIOA) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    } else if (port == GPIOB) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    } else if (port == GPIOC) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    } else if (port == GPIOD) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    } else if (port == GPIOE) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    } else if (port == GPIOH) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;
    }

    /*
     * Read back the register after enabling the peripheral clock.
     * This gives the clock gate a moment to settle before GPIO registers are used.
     */
    (void)RCC->AHB1ENR;
}

void gpio_config_output(GPIO_TypeDef *port, uint32_t pin)
{
    const uint32_t position = pin * 2U;
    const uint32_t mask = 0x3UL << position;

    /* 01: general purpose output mode. */
    port->MODER = (port->MODER & ~mask) | (0x1UL << position);

    /* 0: push-pull output. */
    port->OTYPER &= ~(1UL << pin);

    /* 10: high speed. */
    port->OSPEEDR = (port->OSPEEDR & ~mask) | (0x2UL << position);

    /* 00: no pull-up, no pull-down. */
    port->PUPDR &= ~mask;
}

void gpio_write(GPIO_TypeDef *port, uint32_t pin, bool high)
{
    if (high) {
        port->BSRR = 1UL << pin;
    } else {
        port->BSRR = 1UL << (pin + 16U);
    }
}

void gpio_toggle(GPIO_TypeDef *port, uint32_t pin)
{
    port->ODR ^= 1UL << pin;
}
