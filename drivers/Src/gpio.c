/**
 ******************************************************************************
 * @file    gpio.c
 * @brief   Bare-metal GPIO driver implementation for STM32F401xC
 ******************************************************************************
 */

#include "gpio.h"

/**
 * @brief  Enables the AHB1 peripheral clock for the given GPIO port.
 *         Compares the port's base address to select the correct
 *         RCC_AHB1ENR enable bit (RM0368, Section 6.3.10).
 */
void GPIO_ClockEnable(GPIO_TypeDef *port)
{
    if (port == GPIOA)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    }
    else if (port == GPIOB)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    }
    else if (port == GPIOC)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    }
    else if (port == GPIOD)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    }
    else if (port == GPIOE)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    }
    else if (port == GPIOH)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;
    }

    /* Dummy read-back: RM0368 recommends a read after enabling a
     * peripheral clock, to ensure the clock is stable before the
     * peripheral registers are accessed on the next instruction. */
    (void)RCC->AHB1ENR;
}

/**
 * @brief  Configures a single GPIO pin: mode, output type, speed,
 *         pull-up/down, and alternate function (if applicable).
 *
 *         Each field below is a read-modify-write:
 *         1. Clear the 2 (or 1) bits belonging to this pin.
 *         2. OR in the new value shifted into position.
 */
void GPIO_Init(GPIO_TypeDef *port, GPIO_Init_t *config)
{
    uint32_t pin = (uint32_t)config->Pin;

    /* ---- MODER: 2 bits per pin ---- */
    port->MODER &= ~(0x3UL << (pin * 2));
    port->MODER |= ((uint32_t)config->Mode << (pin * 2));

    /* ---- OTYPER: 1 bit per pin, only meaningful for output/AF ---- */
    if (config->Mode == GPIO_MODE_OUTPUT || config->Mode == GPIO_MODE_AF)
    {
        port->OTYPER &= ~(0x1UL << pin);
        port->OTYPER |= ((uint32_t)config->OType << pin);

        /* ---- OSPEEDR: 2 bits per pin, only meaningful for output/AF ---- */
        port->OSPEEDR &= ~(0x3UL << (pin * 2));
        port->OSPEEDR |= ((uint32_t)config->Speed << (pin * 2));
    }

    /* ---- PUPDR: 2 bits per pin, valid for any mode ---- */
    port->PUPDR &= ~(0x3UL << (pin * 2));
    port->PUPDR |= ((uint32_t)config->PuPd << (pin * 2));

    /* ---- AFR: 4 bits per pin, split across two 32-bit registers ----
     * AFR[0] (AFRL) covers pins 0-7, AFR[1] (AFRH) covers pins 8-15.
     * Only written when the pin is actually in AF mode. */
    if (config->Mode == GPIO_MODE_AF)
    {
        uint32_t afr_index = pin / 8U;        /* 0 for pins 0-7, 1 for pins 8-15 */
        uint32_t afr_shift = (pin % 8U) * 4U; /* 4 bits per pin within the register */

        port->AFR[afr_index] &= ~(0xFUL << afr_shift);
        port->AFR[afr_index] |= ((uint32_t)config->Alternate << afr_shift);
    }
}

/**
 * @brief  Sets or clears a pin atomically via BSRR.
 *         Writing to BSRR bits [15:0] sets the pin, bits [31:16] resets it.
 *         This is a single write, unlike read-modify-write on ODR, so it's
 *         safe from race conditions (e.g. inside an ISR).
 */
void GPIO_WritePin(GPIO_TypeDef *port, GPIO_Pin_t pin, GPIO_PinState_t state)
{
    if (state == GPIO_PIN_SET)
    {
        port->BSRR = (1UL << pin);          /* Lower half: set */
    }
    else
    {
        port->BSRR = (1UL << (pin + 16U));  /* Upper half: reset */
    }
}

/**
 * @brief  Toggles a pin by reading its current ODR bit and writing the
 *         opposite value back through BSRR (keeps the atomic-write property).
 */
void GPIO_TogglePin(GPIO_TypeDef *port, GPIO_Pin_t pin)
{
    if (port->ODR & (1UL << pin))
    {
        port->BSRR = (1UL << (pin + 16U));  /* Currently high -> reset */
    }
    else
    {
        port->BSRR = (1UL << pin);          /* Currently low  -> set */
    }
}

/**
 * @brief  Reads the input data register bit for the given pin.
 */
GPIO_PinState_t GPIO_ReadPin(GPIO_TypeDef *port, GPIO_Pin_t pin)
{
    return (port->IDR & (1UL << pin)) ? GPIO_PIN_SET : GPIO_PIN_RESET;
}