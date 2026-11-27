#include "ton.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static unsigned checkpoints = 0U;

static void check(const TON_Timer_t *t, u8 q, u32 et)
{
    assert(t->Q == q);
    assert(t->ET == et);
    ++checkpoints;
}

static void timeout_trace(void)
{
    TON_Timer_t t = {0};
    TON_Update(&t, 0U, 10000UL, 0UL); check(&t, 0U, 0UL);
    TON_Update(&t, 1U, 10000UL, 250UL); check(&t, 0U, 0UL);
    TON_Update(&t, 1U, 10000UL, 1250UL); check(&t, 0U, 1000UL);
    TON_Update(&t, 1U, 10000UL, 5250UL); check(&t, 0U, 5000UL);
    TON_Update(&t, 1U, 10000UL, 10249UL); check(&t, 0U, 9999UL);
    TON_Update(&t, 1U, 10000UL, 10250UL); check(&t, 1U, 10000UL);
    TON_Update(&t, 1U, 10000UL, 20000UL); check(&t, 1U, 10000UL);
    TON_Update(&t, 0U, 10000UL, 20001UL); check(&t, 0U, 0UL);
}

static void cancel_and_restart(void)
{
    TON_Timer_t t = {0};
    TON_Update(&t, 1U, 10000UL, 100UL);
    TON_Update(&t, 1U, 10000UL, 6100UL); check(&t, 0U, 6000UL);
    TON_Update(&t, 0U, 10000UL, 6101UL); check(&t, 0U, 0UL);
    TON_Update(&t, 1U, 10000UL, 6200UL); check(&t, 0U, 0UL);
    TON_Update(&t, 1U, 10000UL, 16199UL); check(&t, 0U, 9999UL);
    TON_Update(&t, 1U, 10000UL, 16200UL); check(&t, 1U, 10000UL);
}

static void wrapping_clock_and_instances(void)
{
    TON_Timer_t a = {0}, b = {0};
    u32 start = UINT32_MAX - 4999UL;
    TON_Update(&a, 1U, 10000UL, start);
    TON_Update(&b, 1U, 10000UL, start + 100UL);
    TON_Update(&a, 1U, 10000UL, 4999UL); check(&a, 0U, 9999UL);
    TON_Update(&a, 1U, 10000UL, 5000UL); check(&a, 1U, 10000UL);
    TON_Update(&b, 1U, 10000UL, 5000UL); check(&b, 0U, 9900UL);
    TON_Update(&b, 1U, 10000UL, 5100UL); check(&b, 1U, 10000UL);
    TON_Update(&a, 1U, 10000UL, start); check(&a, 1U, 10000UL);
    TON_Update(&a, 0U, 10000UL, start + 1UL); check(&a, 0U, 0UL);
}

static void other_presets(void)
{
    TON_Timer_t t = {0};
    TON_Update(&t, 1U, 0UL, 0UL); check(&t, 1U, 0UL);
    TON_Update(&t, 0U, 0UL, 1UL); check(&t, 0U, 0UL);
    TON_Update(&t, 1U, 1UL, 2UL); check(&t, 0U, 0UL);
    TON_Update(&t, 1U, 1UL, 3UL); check(&t, 1U, 1UL);
}

static void lcd_frames(void)
{
    char line[17];
    int n;
    n = snprintf(line, sizeof line, "ET:%05u ms     ", 0U);
    assert(n == 16 && strcmp(line, "ET:00000 ms     ") == 0); ++checkpoints;
    n = snprintf(line, sizeof line, "ET:%05u ms     ", 10000U);
    assert(n == 16 && strcmp(line, "ET:10000 ms     ") == 0); ++checkpoints;
    n = snprintf(line, sizeof line, "Q:%c IN:%c PT:10s ", '1', '1');
    assert(n == 16 && strcmp(line, "Q:1 IN:1 PT:10s ") == 0); ++checkpoints;
    n = snprintf(line, sizeof line, "Q:%c IN:%c PT:10s ", '0', '0');
    assert(n == 16 && strlen(line) == 16U); ++checkpoints;
}

int main(void)
{
    timeout_trace();
    cancel_and_restart();
    wrapping_clock_and_instances();
    other_presets();
    lcd_frames();
    printf("PASS: %u timer/display checkpoints; undefined-behavior sanitizer.\n",
           checkpoints);
    return 0;
}
