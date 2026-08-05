#ifndef PLATFORM_SPI_H
#define PLATFORM_SPI_H

#include <stdint.h>

#include "stm32f4xx.h"

void spi_init(void);
void spi_write_byte(uint8_t data);
void spi_write_block(const uint8_t *data, uint32_t length);

#endif
