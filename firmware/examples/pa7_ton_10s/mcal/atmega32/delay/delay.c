/**
 * @file delay.c
 * @brief Runtime-argument busy waits, using a counted AVR assembly loop.
 * @version 0.1.0
 *
 * Source: Microchip AVR Instruction Set Manual DS40002198, SBIW/BRNE/NOP.
 * https://ww1.microchip.com/downloads/en/DeviceDoc/AVR-InstructionSet-Manual-DS40002198.pdf
 * For n>0 on this core: SBIW+BRNE loop = 4*n-1 cycles; final NOP adds one.
 * Arithmetic, calls and loop-chunk management only extend the requested wait.
 * Clock is rounded upward to kHz for a conservative runtime cycle calculation.
 */
#include "delay.h"
#include "Fcpu.h"

#if !defined(DELAY_UNIT_TEST)
#if !defined(__GNUC__) || !defined(__AVR__) || \
    (!defined(__AVR_ATmega32__) && !defined(__AVR_ATmega32A__))
#error "delay.c requires AVR-GCC targeting ATmega32 or ATmega32A."
#endif
#endif

#define DELAY_CLOCK_KHZ ((F_CPU + 999UL) / 1000UL)

static void Delay_Loop4(u16 iterations)
{
#ifdef DELAY_UNIT_TEST
    /* Implemented by host tests only; it records, rather than waits for, cycles. */
    extern void Delay_TestLoop4(u16 count);
    Delay_TestLoop4(iterations);
#else
    /* Caller guarantees iterations is nonzero. Never pass zero to this loop. */
    __asm__ __volatile__(
        "1: sbiw %0, 1\n\t"
        "brne 1b\n\t"
        "nop"
        : "+w" (iterations)
        :
        : "cc", "memory");
#endif
}

void Delay_us(u16 microseconds)
{
    u32 iterations;
    u16 chunk;

    if (microseconds == 0U)
    {
        return;
    }

    /* ceil(clock_kHz * time_us / 4000). At 16 MHz and 65535 us,
     * the numerator remains below 2^32. Round up, never under-delay.
     */
    iterations = (u32)(((u32)microseconds * (u32)DELAY_CLOCK_KHZ + 3999UL) / 4000UL);

    while (iterations != 0U)
    {
        chunk = (iterations > 65535UL) ? (u16)65535U : (u16)iterations;
        Delay_Loop4(chunk);
        iterations -= (u32)chunk;
    }
}

void Delay_ms(u16 milliseconds)
{
    while (milliseconds != 0U)
    {
        Delay_us(1000U);
        --milliseconds;
    }
}
