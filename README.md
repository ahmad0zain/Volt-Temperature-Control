# Volt-Temperature-Control
Temperature monitoring and fan control using ATmega32A, LM35 and LCD1602.

## PA7 TON 10-second LCD example

A complete, self-contained ATmega32 example is in [`firmware/examples/pa7_ton_10s`](firmware/examples/pa7_ton_10s).

- Hold the PA7-to-GND button for 10 seconds; releasing it resets the TON.
- LCD shows live elapsed milliseconds (ET), output Q and input IN.
- The PC0 LED follows Q. The MCU clock must be set to 1 MHz.
- Includes source, a ready HEX file, wiring instructions and timer/display checks.

After pulling the repository, load `firmware/examples/pa7_ton_10s/PA7_TON_10s_1MHz.hex` in Proteus.
