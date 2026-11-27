#include "ton.h"

void TON_Update(TON_Timer_t *timer, u8 IN, u32 PT, u32 now_ms)
{
    /* Input OFF: reset the timer and its output. */
    if (IN == 0U)
    {
        timer->Q = 0U;
        timer->ET = 0UL;
        timer->start_time = 0UL;
        timer->running = 0U;
        return;
    }

    /* First scan with input ON: save the start time once. */
    if (timer->running == 0U)
    {
        timer->start_time = now_ms;
        timer->running = 1U;
    }

    /* Keep a completed timer ON until the input turns OFF. */
    if (timer->Q != 0U)
    {
        return;
    }

    timer->ET = (u32)(now_ms - timer->start_time);

    if (timer->ET >= PT)
    {
        timer->ET = PT;
        timer->Q = 1U;
    }
}
