# GNU Make + AVR-GCC. Run from the extracted project root.
.DEFAULT_GOAL := all

MCU ?= atmega32
BOARD ?= volt_board
# Optional override, for example: make F_CPU=8000000UL
# Empty uses the default in boards/volt_board/Fcpu.h.
F_CPU ?=

AVR_CC ?= avr-gcc
AVR_OBJCOPY ?= avr-objcopy
AVR_SIZE ?= avr-size
PYTHON ?= python3

CLOCK_TAG := $(if $(strip $(F_CPU)),$(strip $(F_CPU)),board_default)
BUILD_DIR := build/$(MCU)/$(BOARD)/$(CLOCK_TAG)
TARGET := $(BUILD_DIR)/volt_lcd_demo

INCLUDE_DIRS := common boards/$(BOARD) hal/lcd mcal/atmega32 \
                mcal/atmega32/gpio mcal/atmega32/delay mcal/atmega32/interrupt
CPPFLAGS += $(addprefix -I,$(INCLUDE_DIRS))
ifneq ($(strip $(F_CPU)),)
CPPFLAGS += -DF_CPU=$(F_CPU)
endif

CFLAGS += -mmcu=$(MCU) -std=gnu99 -Os -g -Wall -Wextra -Werror \
          -ffunction-sections -fdata-sections
LDFLAGS += -mmcu=$(MCU) -Wl,--gc-sections,-Map=$(TARGET).map

SOURCES := app/main.c hal/lcd/lcd.c \
           mcal/atmega32/gpio/gpio.c mcal/atmega32/delay/delay.c \
           mcal/atmega32/interrupt/Gie.c
OBJECTS := $(addprefix $(BUILD_DIR)/,$(SOURCES:.c=.o))
DEPS := $(OBJECTS:.o=.d)

.PHONY: all check clean
all: $(TARGET).hex

$(BUILD_DIR)/%.o: %.c Makefile
	@mkdir -p $(@D)
	$(AVR_CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(TARGET).elf: $(OBJECTS)
	$(AVR_CC) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

$(TARGET).hex: $(TARGET).elf
	$(AVR_OBJCOPY) -O ihex -R .eeprom $< $@
	$(AVR_SIZE) $<

check:
	$(PYTHON) tests/run_host_tests.py

# Deletes only generated build outputs inside this project.
clean:
	$(PYTHON) -c "import shutil; shutil.rmtree('build', ignore_errors=True)"

-include $(DEPS)
