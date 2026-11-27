#include "Fcpu.h"
#include "timebase.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#if !defined(__AVR_ATmega32__) && !defined(__AVR_ATmega32A__)
#error "This timebase targets ATmega32/ATmega32A."
#endif

/* CTC interrupt period = 8 * (OCR0 + 1) / F_CPU seconds.
 * At 1 MHz: OCR0=124 => 8*125/1000000 = 1 ms.
 */
#if (F_CPU % 8000UL) != 0UL
#error "F_CPU must be divisible by 8000 for an exact 1 ms Timer0 tick."
#endif
#define TIMEBASE_COUNTS (F_CPU / 8000UL)
#if TIMEBASE_COUNTS < 1UL || TIMEBASE_COUNTS > 256UL
#error "Timer0 /8 count does not fit. This demo is designed for 1 MHz."
#endif

static volatile u32 milliseconds = 0UL;

void Timebase_Init(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        TCCR0 = 0U;
        TIMSK &= (u8)~(_BV(OCIE0) | _BV(TOIE0));
        TCNT0 = 0U;
        OCR0 = (u8)(TIMEBASE_COUNTS - 1UL);
        milliseconds = 0UL;
        /* TIFR flags are write-one-to-clear: clear only Timer0 flags. */
        TIFR = _BV(OCF0) | _BV(TOV0);
        TIMSK |= _BV(OCIE0);
        TCCR0 = _BV(WGM01) | _BV(CS01); /* CTC, prescaler /8. */
    }
}

ISR(TIMER0_COMP_vect)
{
    ++milliseconds;
}

u32 Timebase_Millis(void)
{
    u32 now;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        now = milliseconds;
    }
    return now;
}
