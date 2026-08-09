#include "i2c.h"

/*
 * Placeholder for the platform I2C driver.
 * Add register-level I2C code here when the application needs it.
 */

void i2c_init(void)
{
}

void i2c_write_byte(uint8_t slave_addr, uint8_t reg, uint8_t data)
{
    (void)slave_addr;
    (void)reg;
    (void)data;
}

void i2c_read_byte(uint8_t slave_addr, uint8_t reg, uint8_t *data)
{
    (void)slave_addr;
    (void)reg;
    (void)data;
}
