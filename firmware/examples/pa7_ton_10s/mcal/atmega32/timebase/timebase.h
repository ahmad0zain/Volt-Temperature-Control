#ifndef TIMEBASE_H
#define TIMEBASE_H

#include "StdTypes.h"

/* Owns Timer0 and TIMER0_COMP_vect. Initialize once at startup, before sei().
 * Does not change global interrupt enable. Application must enable interrupts.
 * 1 ms tick; IRQs must be serviced before the next compare (no >=1 ms masks).
 * Timer1/2 are not configured. Do not use Timer0 for another feature.
 */
void Timebase_Init(void);

/* Atomic snapshot; wraps at 2^32 ms (about 49.7 days). */
u32 Timebase_Millis(void);

#endif
