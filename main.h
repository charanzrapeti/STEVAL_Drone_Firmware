#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

#include "stm32f4xx.h"
#include "gpio.h"
#include "i2c.h"
#include "spi.h"
#include "uart.h"

#define LED_PORT GPIOB
#define UART_PORT GPIOA
#define LED_PIN  GPIO_PIN_5
#define LED_PIN2 GPIO_PIN_4
#define UART_RX_PIN GPIO_PIN_10

void delay_cycles(volatile uint32_t cycles);

#endif
