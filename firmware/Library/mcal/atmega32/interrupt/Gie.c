/**
 * @file Gie.c
 * @brief ATmega32/ATmega32A implementation of the existing Gie.h interface.
 * @version 0.1.0
 *
 * SEI/CLI change the global interrupt enable bit and preserve other SREG bits.
 * Compiler memory barriers keep memory accesses ordered around these calls.
 * These are explicit enable/disable operations, not a nested critical section.
 * The GPIO driver's own save/restore critical sections remain independent.
 */
#include "Gie.h"

#if !defined(__GNUC__) || !defined(__AVR__) || \
    (!defined(__AVR_ATmega32__) && !defined(__AVR_ATmega32A__))
#error "Gie.c requires AVR-GCC targeting ATmega32 or ATmega32A."
#endif

void Gie_Enable(void)
{
    __asm__ __volatile__("sei" : : : "memory");
}

void Gie_Disable(void)
{
    __asm__ __volatile__("cli" : : : "memory");
}
