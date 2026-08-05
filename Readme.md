# Boilerplate code quick generation

You can use, CubeMX to generate Boilerplatecode 

Steps: 

1. Select the MCU 
2. Go to Project finder, and select Makefile
3. Save it to a folder
4. Copy, startup file,  linker script form folder generated
5. Copy CMSIS filder in Drivers, HAL we are going to write our own (in platform folder)
6. Copy system_stm32f4xx (in this case) from Core/src folder

## Build and flash

This project builds a bare-metal STM32F401 image with `arm-none-eabi-gcc`.

```sh
make
```

Build outputs are written to `bin/`:

- `bin/blink_pb5.elf`
- `bin/blink_pb5.bin`
- `bin/blink_pb5.hex`
- `bin/blink_pb5.map`

Flash with an ST-Link using `st-flash`:

```sh
make flash
```

Or flash with OpenOCD:

```sh
make flash-openocd
```

The application toggles PB5. Connect the LED so the pin drives it safely
through a current-limiting resistor.

