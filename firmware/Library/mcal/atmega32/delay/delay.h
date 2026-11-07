/**
 * @file delay.h
 * @brief Blocking minimum-delay interface for Volt ATmega32/ATmega32A.
 * @version 0.1.0
 *
 * Arguments: 0..65535; zero requests no wait. No timer or interrupt is owned.
 * Interrupts remain in their existing state. Execution overhead and ISRs add
 * time, so these are minimum waits, not precision sampling or RTOS scheduling.
 * F_CPU must match the real CPU clock. No vendor AVR headers are required.
 */
#ifndef VOLT_DELAY_H
#define VOLT_DELAY_H

#include "StdTypes.h"

void Delay_us(u16 microseconds);
void Delay_ms(u16 milliseconds);

#endif /* VOLT_DELAY_H */
