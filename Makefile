######################################
# STM32F401 bare-metal build
######################################

TARGET := blink_pb5
ARTIFACT_DIR := bin
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj

PREFIX := arm-none-eabi-
CC := $(PREFIX)gcc
AS := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy
SIZE := $(PREFIX)size

MCU := cortex-m4
LDSCRIPT := STM32F401XX_FLASH.ld

C_SOURCES := \
main.c \
drivers/Src/gpio.c \
drivers/Src/i2c.c \
drivers/Src/uart.c \
drivers/Src/spi.c \
drivers/Src/system_stm32f4xx.c

ASM_SOURCES := \
drivers/Src/startup_stm32f401xc.s

INCLUDES := \
-I. \
-Idrivers/Inc \
-ICMSIS/Core/Include \
-ICMSIS/Device/ST/STM32F4xx/Include

DEFINES := \
-DSTM32F401xC

CPU_FLAGS := -mcpu=$(MCU) -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

CFLAGS := $(CPU_FLAGS) $(DEFINES) $(INCLUDES) -std=gnu11 -Wall -Wextra -Werror \
          -ffunction-sections -fdata-sections -O2 -g3
ASFLAGS := $(CPU_FLAGS) $(DEFINES) $(INCLUDES) -x assembler-with-cpp -g3
LDFLAGS := $(CPU_FLAGS) -T$(LDSCRIPT) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map \
           -Wl,--gc-sections -Wl,--print-memory-usage --specs=nano.specs --specs=nosys.specs

OBJECTS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
OBJECTS += $(patsubst %.s,$(OBJ_DIR)/%.o,$(ASM_SOURCES))
DEPS := $(OBJECTS:.o=.d)

.PHONY: all clean flash flash-openocd size

all: $(ARTIFACT_DIR)/$(TARGET).elf $(ARTIFACT_DIR)/$(TARGET).bin $(ARTIFACT_DIR)/$(TARGET).hex size

$(ARTIFACT_DIR)/$(TARGET).elf: $(OBJECTS)
	@mkdir -p $(ARTIFACT_DIR)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(ARTIFACT_DIR)/%.bin: $(ARTIFACT_DIR)/%.elf
	$(OBJCOPY) -O binary $< $@

$(ARTIFACT_DIR)/%.hex: $(ARTIFACT_DIR)/%.elf
	$(OBJCOPY) -O ihex $< $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -MMD -MP -c $< -o $@

size: $(ARTIFACT_DIR)/$(TARGET).elf
	$(SIZE) $<

flash: $(ARTIFACT_DIR)/$(TARGET).bin
	st-flash write $< 0x08000000

flash-openocd: $(ARTIFACT_DIR)/$(TARGET).elf
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program $< verify reset exit"

clean:
	rm -rf $(ARTIFACT_DIR) $(BUILD_DIR)

-include $(DEPS)
