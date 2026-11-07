/**
 * @file Gie.h
 * @brief Global interrupt control interface for the Volt MCU backend.
 * @version 0.1.0
 * @author ahmad
 *
 * Originally created on September 23, 2023.
 * Declares the existing interface; definitions must be supplied by Gie.c.
 * Implementations are MCU/compiler-specific and must preserve other status
 * flags and provide any compiler barriers required for ordered memory access.
 *
 * Initialize interrupt sources before enabling global interrupts.
 * Disable then Enable is not a general save/restore critical section: it can
 * enable interrupts that were already disabled by an outer caller.
 */

#ifndef VOLT_GIE_H
#define VOLT_GIE_H

/* Enable global maskable interrupts on the target MCU. */
void Gie_Enable(void);

/* Disable global maskable interrupts on the target MCU. */
void Gie_Disable(void);

#endif /* VOLT_GIE_H */

