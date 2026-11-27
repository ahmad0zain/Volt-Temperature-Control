/**
 * @file Fcpu.h
 * @brief CPU clock frequency configuration for the Volt embedded project.
 * @version 0.2.0
 * @author ahmad
 *
 * Originally created on October 20, 2023.
 * No MCU headers or external libraries are required.
 *
 * F_CPU expresses the CPU clock frequency in hertz. It must match the actual
 * CPU clock and the processor frequency configured in the simulator.
 * This definition does not configure the oscillator, prescaler or fuse bits.
 * Include this file before code that uses F_CPU for timing calculations.
 */

#ifndef VOLT_FCPU_H
#define VOLT_FCPU_H

/* Default: 1 MHz. A build-wide -DF_CPU value may override it. */
#ifndef F_CPU
#define F_CPU 1000000UL
#endif

/* Supported range of our current ATmega32/A software-delay implementation.
 * This check does not establish a permitted clock/voltage operating point.
 */
#if (F_CPU < 1000000UL) || (F_CPU > 16000000UL)
#error "Volt delay supports integer F_CPU values from 1 MHz to 16 MHz."
#endif

#endif /* VOLT_FCPU_H */
