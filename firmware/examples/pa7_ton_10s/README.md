# PA7 -> TON 10 seconds -> live LCD ET and Q

Ready firmware for Zain's supplied ATmega32 Proteus schematic.
ATmega32 clock: 1 MHz. TON preset: 10000 ms (10 seconds).
Standard non-retentive TON: hold the button to time; release resets Q and ET.
A brief press does not latch a 10-second cycle.

## Run the supplied HEX in Proteus

1. Open this example folder after pulling the repository.
2. Double-click U1 (ATMEGA32).
3. Select PA7_TON_10s_1MHz.hex as Program File.
4. Set Clock Frequency to 1 MHz (1000000 Hz).
5. Run and HOLD the PA7 button. Do not use the RESET button for timing.

This example includes every required .c and .h file; no previous ZIP is
needed. The HEX is already compiled; source compilation is optional.

## Wiring matched to the screenshot

| Device signal | MCU/net | ATmega32 package pin |
|---|---|---:|
| Button terminal 1 | PA7/ADC7 | 33 |
| Button terminal 2 | GND | - |
| LCD RS (pin 4) | PD0 | 14 |
| LCD E (pin 6) | PD1 | 15 |
| LCD D4 (pin 11) | PD4 | 18 |
| LCD D5 (pin 12) | PD5 | 19 |
| LCD D6 (pin 13) | PD6 | 20 |
| LCD D7 (pin 14) | PD7 | 21 |
| LCD R/W (pin 5) | GND | - |
| LED through existing resistor | PC0 | 22 |

PA7 uses an internal pull-up, so released reads HIGH and pressed reads LOW.
The application converts pressed to IN=1. No extra pull-up is needed.
LCD VSS=GND; VDD=5 V; VEE/VO=potentiometer wiper (POT net).
LCD D0-D3 remain unused. The LM35 on PA0 is unused by this test.
The existing PC0 LED follows Q; it lights after the completed 10-second hold.

## What the LCD shows

First row:  `ET:00000 ms     `
Second row: `Q:0 IN:0 PT:10s `

ET is elapsed milliseconds, updated on the LCD about every 100 ms.
Q is the timer's logical output, IN is the accepted button state.
At completion: ET=10000, Q=1, IN=1; ET stays capped at 10000.
On release: ET=0, Q=0, IN=0. Another press starts a fresh interval.
Release before 10 seconds also clears the elapsed time; it does not pause.

| Step | Expected result |
|---|---|
| Start with button released | ET=00000, Q=0, IN=0, LED off |
| Hold for about 3 seconds | ET increasing near 03000, Q=0, IN=1 |
| Release early | ET resets to 00000, Q=0, IN=0 |
| Hold again for at least 10 seconds | ET reaches 10000; Q=1 and LED lights |
| Keep holding | ET=10000 and Q=1 remain |
| Release | ET=00000, Q=0 and LED turns off |

Input changes are accepted after 20 ms of stable sampled readings (debounce).
Timing starts when the input is accepted, rather than at the physical edge.
Q is evaluated every main-loop scan. The displayed value also has the LCD
refresh and transfer latency; use a hold slightly longer than 10 seconds
when checking the visible output.

## Source map for learning

- app/main.c: PA7 input, button debounce, timer update, LED, LCD formatting.
- services/ton.h and services/ton.c: the simple TON API already discussed.
- mcal/atmega32/timebase/: 1 ms clock from hardware Timer0 interrupt.
- hal/lcd/: existing Volt LCD driver.
- mcal/atmega32/gpio/ and delay/: existing Volt drivers.
- common/ and boards/volt_board/: types, clock and LCD pin configuration.
- tests/test_ton_10s.c: host checks for 10-second timeout and LCD row format.

Main timer call:

    TON_Update(&timer1, input, 10000UL, Timebase_Millis());

Read timer1.Q for the output and timer1.ET for elapsed milliseconds.
Call the timer every scan, even when input is zero. Keep PT fixed for a
cycle, and zero-initialize each timer instance with {0}.

## Rebuild

Requires AVR-GCC, AVR-Libc, AVR binutils and GNU Make:

    make
    make check

Firmware: build/pa7_ton_10s.hex. The supplied root HEX is the validated build.
The Makefile uses -mmcu=atmega32, -DF_CPU=1000000UL and -Os.
For Atmel/Microchip Studio, add exactly these C source files:

- app/main.c
- services/ton.c
- hal/lcd/lcd.c
- mcal/atmega32/gpio/gpio.c
- mcal/atmega32/delay/delay.c
- mcal/atmega32/timebase/timebase.c

Compile only one main.c. Include directories are listed in Makefile INCLUDES.
The supplied main.c includes avr/interrupt.h. Keep custom Registers.h confined
to the GPIO backend; do not include it with avr/io.h in one translation unit.

## Timing assumptions and validation

Timebase owns Timer0 and TIMER0_COMP_vect. At 1 MHz it uses CTC, /8 prescaler,
and OCR0=124. Interrupt period = 8*(124+1)/1000000 = 0.001 seconds.
Application enables global interrupts with sei(). The 32-bit clock is read
atomically and supports wrap-aware elapsed arithmetic.
Do not block interrupts or run another ISR for 1 ms or more: tick events can
be lost. Do not assign Timer0 to another module. Timer1/2 are not configured.
LCD protocol waits remain short blocking operations; the 10-second interval
does not block the application. Accuracy follows the configured MCU clock.

Existing Volt GPIO/LCD/delay drivers originate from GitHub commit
8b4536774a2a274c3b2ffd7b8007cf134f0ab7db:
https://github.com/ahmad0zain/Volt-Temperature-Control
Repository path: `firmware/examples/pa7_ton_10s/`.

Timer0 reference: Microchip ATmega32A datasheet DS40002072A, Timer0 CTC and
register descriptions:
https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/ATmega32A-DataSheet-Complete-DS40002072A.pdf

See BUILD_VALIDATION.txt for compile, timer-test and HEX checks. Proteus has
not been executed in this environment; follow the run/check steps above.
