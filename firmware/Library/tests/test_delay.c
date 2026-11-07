#include <assert.h>
#include <stdio.h>
#include "delay.h"
#include "Fcpu.h"

static u64 cycles;
static u32 calls;

void Delay_TestLoop4(u16 count)
{
    assert(count != 0U);
    cycles += (u64)count * 4U;
    ++calls;
}

int main(void)
{
    u32 us;
    const u16 ms[] = {0, 1, 5, 50, 65535};
    unsigned int i;
    for (us = 0U; us <= 65535UL; ++us)
    {
        u64 requested;
        cycles = 0U; calls = 0U;
        Delay_us((u16)us);
        requested = (u64)F_CPU * us;
        assert(cycles * 1000000ULL >= requested);
        assert(cycles * 1000000ULL < requested + (u64)999U * us + 4000000ULL);
        assert((us != 0U) ? (calls >= 1U) : (calls == 0U));
    }
    for (i = 0U; i < sizeof(ms) / sizeof(ms[0]); ++i)
    {
        cycles = 0U; calls = 0U;
        Delay_ms(ms[i]);
        assert(cycles * 1000ULL >= (u64)F_CPU * ms[i]);
        assert((ms[i] != 0U) ? (calls >= ms[i]) : (calls == 0U));
    }
    printf("PASS delay: every u16 microsecond argument at F_CPU=%lu; ms boundaries.\n", (unsigned long)F_CPU);
    return 0;
}
