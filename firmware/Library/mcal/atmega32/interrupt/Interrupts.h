/**
 * @file Interrupts.h
 * @brief AVR-GCC ISR declarations and ATmega32/ATmega32A vector aliases.
 * @version 0.1.0
 * @author ahmad
 *
 * Originally created on September 23, 2023.
 * Target-specific MCAL header; no vendor interrupt header is included.
 * The vector aliases retain the original subset of interrupt sources.
 * GCC __vector_N suffixes count INT0 as 1; the datasheet table also includes
 * RESET as its first entry. These suffixes are not program-memory addresses.
 *
 * A compatible startup/vector table must reference the handler symbols.
 * Define each ISR once, at file scope, with no parameters or return value.
 * Do not call an ISR like a normal C function.
 * Source: ATmega32A datasheet Table 12-1 and GCC AVR attribute documentation.
 * https://gcc.gnu.org/onlinedocs/gcc/AVR-Attributes.html
 */

#ifndef VOLT_INTERRUPTS_H
#define VOLT_INTERRUPTS_H

#if !defined(__AVR__) || !defined(__GNUC__)
#error "Interrupts.h requires an AVR-GCC compatible target toolchain."
#endif

/*
 * signal requests the target ISR entry/exit convention.
 * used and externally_visible preserve externally referenced ISR definitions.
 */
#define ISR(vector)                                                    \
    void vector(void) __attribute__((signal, used, externally_visible)); \
    void vector(void)

/* External interrupts. */
#define VECTOR_INT0 __vector_1
#define VECTOR_INT1 __vector_2
#define VECTOR_INT2 __vector_3

/* Timer/Counter2 interrupts. */
#define VECTOR_TIM2_COMP __vector_4
#define VECTOR_TIM2_OVF  __vector_5

/* Timer/Counter1 interrupts. */
#define VECTOR_TIM1_CAPT  __vector_6
#define VECTOR_TIM1_COMPA __vector_7
#define VECTOR_TIM1_COMPB __vector_8
#define VECTOR_TIM1_OVF   __vector_9

/* Timer/Counter0 interrupts. */
#define VECTOR_TIM0_COMP __vector_10
#define VECTOR_TIM0_OVF  __vector_11

/* SPI interrupt. */
#define VECTOR_SPI __vector_12

/* USART interrupts; original alias names are retained. */
#define VECTOR_UART_RX      __vector_13
#define VECTOR_UART_REG_EMP __vector_14
#define VECTOR_UART_TX      __vector_15

/* ADC conversion complete interrupt. */
#define VECTOR_ADC __vector_16

#endif /* VOLT_INTERRUPTS_H */

