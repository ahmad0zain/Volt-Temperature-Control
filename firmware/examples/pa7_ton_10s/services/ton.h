#ifndef TON_H
#define TON_H

#include "StdTypes.h"

/* Create each timer with {0}. All times are milliseconds.
 * Read Q and ET after TON_Update; leave internal fields unchanged.
 */
typedef struct
{
    u8 Q;           /* Output: 0=OFF, 1=ON. */
    u32 ET;         /* Elapsed time; stops at PT. */
    u32 start_time; /* Internal: time at the start of this cycle. */
    u8 running;     /* Internal: a timing cycle has started. */
} TON_Timer_t;

/* Call EVERY main-loop scan, including when IN=0, using a valid pointer.
 * now_ms is the current wrapping u32 millisecond clock, not a delay.
 * Keep PT fixed during a cycle. Set IN=0 before changing PT.
 * Clock wrap is supported if an unfinished cycle is checked before a full
 * 2^32 ms interval elapses. Use from main-loop context only.
 */
void TON_Update(TON_Timer_t *timer, u8 IN, u32 PT, u32 now_ms);

#endif
