/**
 ******************************************************************************
 * @file    gpio.h
 * @brief   Bare-metal GPIO driver header for STM32F401xC (Cortex-M4)
 *          Register-level access only, no HAL/CubeMX.
 * @note    Reference: RM0368 (STM32F401xB/C and STM32F401xD/E Reference Manual)
 ******************************************************************************
 */

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "stm32f4xx.h"

/* ==========================================================================
 * Enumerations for driver API arguments
 * ========================================================================== */

/* Pin numbers 0-15, matches bit position in each register */
typedef enum
{
    GPIO_PIN_0 = 0, GPIO_PIN_1,  GPIO_PIN_2,  GPIO_PIN_3,
    GPIO_PIN_4,     GPIO_PIN_5,  GPIO_PIN_6,  GPIO_PIN_7,
    GPIO_PIN_8,     GPIO_PIN_9,  GPIO_PIN_10, GPIO_PIN_11,
    GPIO_PIN_12,    GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15
} GPIO_Pin_t;

/* MODER: 2 bits per pin (RM0368, Section 6.4.1) */
typedef enum
{
    GPIO_MODE_INPUT  = 0x0, /* 00: Input (reset state)       */
    GPIO_MODE_OUTPUT = 0x1, /* 01: General purpose output     */
    GPIO_MODE_AF     = 0x2, /* 10: Alternate function          */
    GPIO_MODE_ANALOG = 0x3  /* 11: Analog                      */
} GPIO_Mode_t;

/* OTYPER: 1 bit per pin (RM0368, Section 6.4.2) */
typedef enum
{
    GPIO_OTYPE_PUSHPULL  = 0x0, /* 0: Push-pull (reset state) */
    GPIO_OTYPE_OPENDRAIN = 0x1  /* 1: Open-drain               */
} GPIO_OType_t;

/* OSPEEDR: 2 bits per pin (RM0368, Section 6.4.3) */
typedef enum
{
    GPIO_SPEED_LOW    = 0x0, /* 00: Low speed        */
    GPIO_SPEED_MEDIUM = 0x1, /* 01: Medium speed      */
    GPIO_SPEED_FAST   = 0x2, /* 10: Fast speed        */
    GPIO_SPEED_HIGH   = 0x3  /* 11: High speed        */
} GPIO_Speed_t;

/* PUPDR: 2 bits per pin (RM0368, Section 6.4.4) */
typedef enum
{
    GPIO_PUPD_NONE = 0x0, /* 00: No pull-up/pull-down (reset state) */
    GPIO_PUPD_UP   = 0x1, /* 01: Pull-up                             */
    GPIO_PUPD_DOWN = 0x2  /* 10: Pull-down                            */
    /* 0x3 (11) is reserved */
} GPIO_PuPd_t;

/* Logical pin state, used with GPIO_WritePin() and returned by GPIO_ReadPin() */
typedef enum
{
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET   = 1
} GPIO_PinState_t;

/* Alternate function numbers AF0-AF15 (RM0368, Section 6.4.9/6.4.10 + datasheet AF table) */
typedef enum
{
    GPIO_AF0 = 0x0,  GPIO_AF1 = 0x1,  GPIO_AF2 = 0x2,  GPIO_AF3 = 0x3,
    GPIO_AF4 = 0x4,  GPIO_AF5 = 0x5,  GPIO_AF6 = 0x6,  GPIO_AF7 = 0x7,
    GPIO_AF8 = 0x8,  GPIO_AF9 = 0x9,  GPIO_AF10 = 0xA, GPIO_AF11 = 0xB,
    GPIO_AF12 = 0xC, GPIO_AF13 = 0xD, GPIO_AF14 = 0xE, GPIO_AF15 = 0xF
} GPIO_AF_t;

/* ==========================================================================
 * Configuration struct passed to GPIO_Init()
 * ========================================================================== */
typedef struct
{
    GPIO_Pin_t   Pin;
    GPIO_Mode_t  Mode;
    GPIO_OType_t OType;   /* Ignored when Mode == GPIO_MODE_INPUT/ANALOG */
    GPIO_Speed_t Speed;   /* Ignored when Mode == GPIO_MODE_INPUT/ANALOG */
    GPIO_PuPd_t  PuPd;
    GPIO_AF_t    Alternate; /* Only used when Mode == GPIO_MODE_AF */
} GPIO_Init_t;

/* ==========================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief  Enables the peripheral clock for a given GPIO port on AHB1.
 * @param  port: pointer to the GPIO port (e.g. GPIOA, GPIOC)
 * @note   Must be called before any other operation on that port.
 */
void GPIO_ClockEnable(GPIO_TypeDef *port);

/**
 * @brief  Configures a single pin according to the given config struct.
 *         Sets MODER, OTYPER, OSPEEDR, PUPDR, and AFR as applicable.
 * @param  port:   pointer to the GPIO port
 * @param  config: pointer to a filled GPIO_Init_t struct
 * @note   Does NOT enable the port clock; call GPIO_ClockEnable() first.
 */
void GPIO_Init(GPIO_TypeDef *port, GPIO_Init_t *config);

/**
 * @brief  Sets or clears an output pin using the atomic BSRR register.
 * @param  port:  pointer to the GPIO port
 * @param  pin:   pin number
 * @param  state: GPIO_PIN_SET or GPIO_PIN_RESET
 */
void GPIO_WritePin(GPIO_TypeDef *port, GPIO_Pin_t pin, GPIO_PinState_t state);

/**
 * @brief  Toggles the current state of an output pin.
 * @param  port: pointer to the GPIO port
 * @param  pin:  pin number
 */
void GPIO_TogglePin(GPIO_TypeDef *port, GPIO_Pin_t pin);

/**
 * @brief  Reads the current logic level of a pin from IDR.
 * @param  port: pointer to the GPIO port
 * @param  pin:  pin number
 * @retval GPIO_PIN_SET if high, GPIO_PIN_RESET if low
 */
GPIO_PinState_t GPIO_ReadPin(GPIO_TypeDef *port, GPIO_Pin_t pin);

#endif /* GPIO_H */