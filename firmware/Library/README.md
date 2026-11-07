# Volt Embedded System — complete source bundle for the GPIO/LCD stage

Prepared for Ahmad Saber Zain. This package contains actual C source/header
files in the agreed folder layout, a working application example at the source
level, GNU Make build settings and reproducible host tests. It is not an empty
folder template. ADC/LM35 readings, fan control and FreeRTOS tasks are the next
project stages and are not implemented in this LCD demonstration.

## Start here

1. Extract the entire ZIP, retaining its directory structure.
2. Review boards/volt_board/Fcpu.h and lcd_config.h against the actual clock
   and wiring. The proposed defaults are documented below.
3. Build with an AVR-GCC toolchain using GNU Make, or add the five production
   .c files and the seven include directories listed below to your IDE.
4. Load the resulting HEX/ELF into the selected target or Proteus model with
   matching CPU frequency. First target: display the two demonstration lines.

## Files and responsibilities

| Folder | Files | Responsibility |
|---|---|---|
| app | main.c | LCD startup and two-line demonstration |
| boards/volt_board | Fcpu.h, lcd_config.h | Board clock and LCD pin/timing settings |
| common | StdTypes.h, volt_status.h | Shared data types and operation results |
| common | bit_math.h, Macros.h | General bit operations and compatibility names |
| hal/lcd | lcd.h, lcd.c | Public LCD API and LCD protocol implementation |
| mcal/atmega32 | Registers.h | Target-specific register addresses |
| mcal/atmega32/gpio | gpio.h, gpio.c | Approved GPIO API and implementation |
| mcal/atmega32/delay | delay.h, delay.c | CPU-specific blocking minimum delays |
| mcal/atmega32/interrupt | Gie.h, Gie.c, Interrupts.h | Interrupt enable/disable interface and ISR definitions |
| tests | run_host_tests.py, test_lcd.c, test_delay.c, test_bad_config.c | Host-only tests, excluded from firmware |

The existing Common, GPIO, LCD and delay sources were recovered from the current
saved files and retained. The source header's historic draft label does not
change which GPIO snapshot was approved. Gie.c supplies the previously missing
definitions declared by Gie.h. main.c is the saved LCD example at its agreed
application path. No interrupt is globally enabled by this demonstration.

### Application output

Row 1: `Volt Embedded`

Row 2: `Temp: --.- C`

The temperature is intentionally a display placeholder; it is not a measurement.
Check every LCD call's volt_status_t before continuing. The example stops in
its error loop if a call fails.

## Build

Prerequisites: GNU Make, avr-gcc, avr-objcopy and avr-size. The latter two are
provided by AVR binutils. Normal toolchain startup/runtime support is used;
the peripheral drivers do not use vendor register or delay headers.

```sh
make
```

Default output:

`build/atmega32/volt_board/board_default/volt_lcd_demo.hex`

Matching ELF and map files are generated in the same directory.

For a toolchain supporting the exact ATmega32A device name:

```sh
make MCU=atmega32a
```

To override the default clock for the entire build:

```sh
make F_CPU=8000000UL
```

Clock overrides use a separate build directory. When changing compiler options
other than MCU/BOARD/F_CPU, run `make clean` before rebuilding. These commands
build source; this ZIP does not contain an unverified prebuilt HEX.

### IDE setup

Compile exactly these production source files:

- app/main.c
- hal/lcd/lcd.c
- mcal/atmega32/gpio/gpio.c
- mcal/atmega32/delay/delay.c
- mcal/atmega32/interrupt/Gie.c

Add these compiler include directories relative to the project root:

- common
- boards/volt_board
- hal/lcd
- mcal/atmega32
- mcal/atmega32/gpio
- mcal/atmega32/delay
- mcal/atmega32/interrupt

Select ATmega32/ATmega32A and GNU C99. Retain the exact capitalization of header
filenames. Do not include .c files from other .c files. Do not add test .c files
to the firmware project: the tests provide mocks and their own main functions.
Do not define GPIO_UNIT_TEST or DELAY_UNIT_TEST in a firmware build.

## Board configuration and wiring

F_CPU defaults to 1000000UL (1 MHz). It must match the actual CPU and the
simulator's CPU setting. This macro does not configure fuses, oscillators,
prescalers or voltage. The current software delay accepts 1..16 MHz; that
software range does not establish valid voltage/frequency operation for a chip.

| LCD signal | Proposed connection |
|---|---|
| RS | PB0 |
| EN | PB1 |
| D4 | PA4 |
| D5 | PA5 |
| D6 | PA6 |
| D7 | PA7 |
| R/W | GND |
| VSS | GND |
| VDD | The actual LCD module's rated supply |
| VO | Contrast adjustment |

D0-D3 are unused in 4-bit mode. Follow the module's backlight current-limiting
requirements. Port A's AVCC must be connected as required by the MCU datasheet.
PA0 remains available for the future LM35 input. This mapping is proposed and
has not been established from a schematic of your kit.

## Current behavior and limits

- LCD1602 only, parallel 4-bit HD44780-compatible interface; no I2C backpack.
- LCD_ROW_1 and LCD_ROW_2 select rows; columns are 0..15.
- Strings must fit the remaining visible row. Overflow is rejected before
  writing that request. No automatic wrapping or newline interpretation.
- Text uses the LCD's character-ROM byte encoding, not UTF-8 or Arabic shaping.
- VOLT_OK means GPIO writes and waits completed; with R/W grounded, a missing
  LCD or its busy flag cannot be read. GPIO errors invalidate initialization;
  fix the cause and run LCD_Init again. Partial transfers cannot be rolled back.
- LCD access must be serialized between tasks; do not call the driver from ISR.
- Delay_us and Delay_ms are blocking minimum waits. Call/loop overhead and
  interrupts add time. They are not precision waveform or RTOS scheduling tools.
- Gie_Enable/Gie_Disable are explicit operations, not nested save/restore
  critical sections. GPIO uses its own critical-section implementation.

## Verification

Host tests need Python 3 and host GCC:

```sh
make check
```

They test LCD initialization/byte protocol, requested timing, every visible
cursor position, bounds, invalid configurations, fault propagation/recovery,
and every u16 microsecond argument at five clock settings. The tests record
delay loop counts rather than executing AVR assembly. ASan and UBSan are used.
See VALIDATION.md for the checks completed for this assembled folder layout.

AVR-GCC compilation, Proteus simulation and physical hardware timing remain
separate checks. No real LCD test or measured waveform is claimed.

## Primary references

- Hitachi HD44780U, Table 6 and Figure 24:
  https://cdn-shop.adafruit.com/datasheets/HD44780.pdf
- Microchip ATmega32A datasheet:
  https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/ATmega32A-DataSheet-Complete-DS40002072A.pdf
- Microchip AVR Instruction Set Manual:
  https://ww1.microchip.com/downloads/en/DeviceDoc/AVR-InstructionSet-Manual-DS40002198.pdf
