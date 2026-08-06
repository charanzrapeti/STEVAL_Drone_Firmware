#include "spi.h"
#include "gpio.h"

#define SPI2_SCK_PIN  13U
#define SPI2_SDA_PIN  15U
#define SPI2_CS_PIN   13U

static void spi2_enable_clocks(void);
static void spi2_configure_pins(void);
static void spi2_configure_mode(void);
static void spi_set_output(void);
static void spi_set_input(void);

void spi_init(void)
{
    spi2_enable_clocks();
    spi2_configure_pins();
    spi2_configure_mode();

    gpio_enable_clock(GPIOC);
    gpio_config_output(GPIOC, SPI2_CS_PIN);
    gpio_write(GPIOC, SPI2_CS_PIN, true);
}

void spi_select_device(bool select)
{
    gpio_write(GPIOC, SPI2_CS_PIN, !select);
}

uint8_t spi_transfer(uint8_t data)
{
    while ((SPI2->SR & SPI_SR_TXE) == 0U)
    {
    }

    SPI2->DR = (uint16_t)data;

    while ((SPI2->SR & SPI_SR_RXNE) == 0U)
    {
    }

    return (uint8_t)SPI2->DR;
}

uint8_t spi_read_register(uint8_t reg)
{
    uint8_t value;

    spi_select_device(true);
    spi_set_output();
    (void)spi_transfer((uint8_t)(reg | 0x80U));

    spi_set_input();
    value = spi_transfer(0x00U);

    spi_select_device(false);
    return value;
}

void spi_write_register(uint8_t reg, uint8_t data)
{
    spi_select_device(true);
    spi_set_output();
    (void)spi_transfer(reg);
    (void)spi_transfer(data);
    spi_select_device(false);
}

void spi_read_registers(uint8_t reg, uint8_t *buffer, uint32_t length)
{
    uint32_t index;

    if ((buffer == 0U) || (length == 0U))
    {
        return;
    }

    spi_select_device(true);
    spi_set_output();
    (void)spi_transfer((uint8_t)(0xC0U | reg));

    spi_set_input();
    for (index = 0U; index < length; index++)
    {
        buffer[index] = spi_transfer(0x00U);
    }

    spi_select_device(false);
}

void spi_write_block(const uint8_t *data, uint32_t length)
{
    uint32_t index;

    if (data == 0U)
    {
        return;
    }

    spi_select_device(true);
    spi_set_output();

    for (index = 0U; index < length; index++)
    {
        (void)spi_transfer(data[index]);
    }

    spi_select_device(false);
}

static void spi_set_output(void)
{
    SPI2->CR1 &= ~SPI_CR1_SPE;
    SPI2->CR1 |= SPI_CR1_BIDIOE;
    SPI2->CR1 |= SPI_CR1_SPE;
}

static void spi_set_input(void)
{
    SPI2->CR1 &= ~SPI_CR1_SPE;
    SPI2->CR1 &= ~SPI_CR1_BIDIOE;
    SPI2->CR1 |= SPI_CR1_SPE;
}

static void spi2_enable_clocks(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    (void)RCC->APB1ENR;
}

static void spi2_configure_pins(void)
{
    const uint32_t af5 = 5U;

    GPIOB->MODER &= ~((0x3UL << (SPI2_SCK_PIN * 2U)) |
                      (0x3UL << (SPI2_SDA_PIN * 2U)));
    GPIOB->MODER |= ((0x2UL << (SPI2_SCK_PIN * 2U)) |
                     (0x2UL << (SPI2_SDA_PIN * 2U)));

    GPIOB->OTYPER &= ~((1UL << SPI2_SCK_PIN) |
                       (1UL << SPI2_SDA_PIN));

    GPIOB->OSPEEDR |= ((0x2UL << (SPI2_SCK_PIN * 2U)) |
                       (0x2UL << (SPI2_SDA_PIN * 2U)));

    GPIOB->PUPDR &= ~((0x3UL << (SPI2_SCK_PIN * 2U)) |
                      (0x3UL << (SPI2_SDA_PIN * 2U)));

    GPIOB->AFR[1] &= ~((0xFU << ((SPI2_SCK_PIN - 8U) * 4U)) |
                       (0xFU << ((SPI2_SDA_PIN - 8U) * 4U)));
    GPIOB->AFR[1] |= ((af5 << ((SPI2_SCK_PIN - 8U) * 4U)) |
                      (af5 << ((SPI2_SDA_PIN - 8U) * 4U)));
}

static void spi2_configure_mode(void)
{
    SPI2->CR1 = SPI_CR1_MSTR |
                SPI_CR1_SSM |
                SPI_CR1_SSI |
                SPI_CR1_BIDIMODE |
                SPI_CR1_BR_1 |
                SPI_CR1_BR_0;

    SPI2->CR2 = 0U;
    SPI2->CR1 |= SPI_CR1_BIDIOE | SPI_CR1_SPE;
}
