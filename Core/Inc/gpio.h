#ifndef PLATFORM_GPIO_H
#define PLATFORM_GPIO_H

#include <stdbool.h>
#include <stdint.h>

#include "stm32f4xx.h"

void gpio_enable_clock(GPIO_TypeDef *port);
void gpio_config_output(GPIO_TypeDef *port, uint32_t pin);
void gpio_write(GPIO_TypeDef *port, uint32_t pin, bool high);
void gpio_toggle(GPIO_TypeDef *port, uint32_t pin);

#endif
