/**
 * @file Registers.h
 * @brief ATmega32 and ATmega32A register definitions for the Volt MCU backend.
 * @version 0.1.0
 * @author ahmad
 *
 * Originally created on October 21, 2023.
 * Depends on our own StdTypes.h for exact-width integer aliases.
 * Addresses below are DATA MEMORY addresses, not unadjusted I/O addresses.
 * Source: Microchip ATmega32A datasheet DS40002072A, Register Summary.
 * https://ww1.microchip.com/downloads/en/DeviceDoc/Atmega32A-DataSheet-Complete-DS40002072A.pdf
 *
 * This header is target-specific and belongs in the MCAL backend.
 * Keep it separate from vendor register headers to avoid name collisions.
 * Volatile access does not make multi-byte operations atomic.
 * Special register access sequences must be implemented by the driver.
 */

#ifndef VOLT_REGISTERS_H
#define VOLT_REGISTERS_H

#include "StdTypes.h"

/* Digital input/output registers. PINx is read-only on this target. */
#define PINA  (*((const volatile u8 *)0x39U))
#define DDRA  (*((volatile u8 *)0x3AU))
#define PORTA (*((volatile u8 *)0x3BU))

#define PINB  (*((const volatile u8 *)0x36U))
#define DDRB  (*((volatile u8 *)0x37U))
#define PORTB (*((volatile u8 *)0x38U))

#define PINC  (*((const volatile u8 *)0x33U))
#define DDRC  (*((volatile u8 *)0x34U))
#define PORTC (*((volatile u8 *)0x35U))

#define PIND  (*((const volatile u8 *)0x30U))
#define DDRD  (*((volatile u8 *)0x31U))
#define PORTD (*((volatile u8 *)0x32U))

/* External interrupt control and flags. GIFR flags use write-one-to-clear. */
#define MCUCR  (*((volatile u8 *)0x55U))
#define MCUCSR (*((volatile u8 *)0x54U))
#define GICR   (*((volatile u8 *)0x5BU))
#define GIFR   (*((volatile u8 *)0x5AU))

/* CPU status register; bit 7 is the Global Interrupt Enable bit. */
#define SREG (*((volatile u8 *)0x5FU))

/*
 * ADC registers.
 * Read ADCL before ADCH to obtain a consistent conversion result.
 * ADC_DATA is retained for compatibility; a plain 16-bit C access does not
 * itself specify the byte access order. Prefer explicit byte reads in ADC code.
 * ADCSRA contains the write-one-to-clear ADIF flag.
 */
#define ADC_DATA (*((volatile u16 *)0x24U))
#define ADCL     (*((volatile u8 *)0x24U))
#define ADCH     (*((volatile u8 *)0x25U))
#define ADCSRA   (*((volatile u8 *)0x26U))
#define ADMUX    (*((volatile u8 *)0x27U))
#define SFIOR    (*((volatile u8 *)0x50U))

/*
 * USART registers.
 * UCSRC and UBRRH share address 0x40: write URSEL=1 for UCSRC, URSEL=0
 * for UBRRH. UCSRC reads require the datasheet's timed access sequence.
 * UBRRH was corrected from the original 0x29 (UBRRL) to 0x40.
 */
#define UDR   (*((volatile u8 *)0x2CU))
#define UCSRA (*((volatile u8 *)0x2BU))
#define UCSRB (*((volatile u8 *)0x2AU))
#define UBRRL (*((volatile u8 *)0x29U))
#define UCSRC (*((volatile u8 *)0x40U))
#define UBRRH (*((volatile u8 *)0x40U))

/* Internal EEPROM registers; the EEPROM driver owns the access sequence. */
#define EEARH (*((volatile u8 *)0x3FU))
#define EEARL (*((volatile u8 *)0x3EU))
#define EEDR  (*((volatile u8 *)0x3DU))
#define EECR  (*((volatile u8 *)0x3CU))

#endif /* VOLT_REGISTERS_H */

