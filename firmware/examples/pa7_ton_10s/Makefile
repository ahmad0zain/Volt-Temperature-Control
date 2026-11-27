.DEFAULT_GOAL := all
MCU ?= atmega32
AVR_CC ?= avr-gcc
AVR_OBJCOPY ?= avr-objcopy
HOST_CC ?= gcc
TARGET := build/pa7_ton_10s
INCLUDES := common boards/volt_board hal/lcd mcal/atmega32 \
            mcal/atmega32/gpio mcal/atmega32/delay \
            mcal/atmega32/timebase services
CPPFLAGS += $(addprefix -I,$(INCLUDES)) -DF_CPU=1000000UL
CFLAGS += -mmcu=$(MCU) -std=gnu99 -Os -g -Wall -Wextra -Werror \
          -ffunction-sections -fdata-sections
SOURCES := app/main.c services/ton.c hal/lcd/lcd.c \
           mcal/atmega32/gpio/gpio.c mcal/atmega32/delay/delay.c \
           mcal/atmega32/timebase/timebase.c
OBJECTS := $(addprefix build/,$(SOURCES:.c=.o))

.PHONY: all check clean
all: $(TARGET).hex

build/%.o: %.c Makefile
	@mkdir -p $(@D)
	$(AVR_CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(TARGET).elf: $(OBJECTS)
	$(AVR_CC) -mmcu=$(MCU) -Wl,--gc-sections,-Map=$(TARGET).map $^ -o $@

$(TARGET).hex: $(TARGET).elf
	$(AVR_OBJCOPY) -O ihex -R .eeprom $< $@

check:
	@mkdir -p build
	$(HOST_CC) -std=c99 -Wall -Wextra -Werror -fsanitize=undefined \
	    -Icommon -Iservices services/ton.c tests/test_ton_10s.c \
	    -o build/test_ton_10s
	./build/test_ton_10s

clean:
	python3 -c "import shutil; shutil.rmtree('build', ignore_errors=True)"

-include $(OBJECTS:.o=.d)
