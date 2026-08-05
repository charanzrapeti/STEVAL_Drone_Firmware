#ifndef PLATFORM_I2C_H
#define PLATFORM_I2C_H

/*
 * Public I2C driver API will live here.
 * Kept as a separate module so app code never needs raw peripheral details.
 */
#include <stdint.h>

#include "stm32f4xx.h"

void i2c_init(void);
void i2c_write_byte(uint8_t slave_addr, uint8_t reg, uint8_t data);
void i2c_read_byte(uint8_t slave_addr, uint8_t reg, uint8_t *data);

#endif
