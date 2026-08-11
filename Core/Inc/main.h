#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

#include "stm32f4xx.h"
#include "gpio.h"
#include "i2c.h"
#include "spi.h"
#include "uart.h"

#define LED_PORT GPIOB
#define LED_PIN  5U
#define LED_PIN2 4U
#define UART_PORT GPIOA 
#define UART_RX_PIN 10U     

void delay_cycles(volatile uint32_t cycles);

#endif
