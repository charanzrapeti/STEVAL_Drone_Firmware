#ifndef PLATFORM_SPI_H
#define PLATFORM_SPI_H

#include <stdbool.h>
#include <stdint.h>

#include "stm32f4xx.h"

void spi_init(void);
void spi_select_device(bool select);
uint8_t spi_transfer(uint8_t data);
uint8_t spi_read_register(uint8_t reg);
void spi_write_register(uint8_t reg, uint8_t data);
void spi_read_registers(uint8_t reg, uint8_t *buffer, uint32_t length);
void spi_write_block(const uint8_t *data, uint32_t length);

#endif
