#include <assert.h>
#include <stdio.h>
#include "lcd.h"
#include "lcd_config.h"
#include "delay.h"

static u8 direction[4], latch[4];
static u8 nibbles[1024], rsValues[1024];
static u64 times[1024], nowUs, riseTime, dataTime;
static unsigned int edges, calls, failAt;
static volt_status_t injected = VOLT_NOT_READY;

static u8 readLine(GPIO_Port_t port, GPIO_Pin_t pin)
{
    return (u8)(((unsigned int)latch[port] >> (unsigned int)pin) & 1U);
}

static volt_status_t startCall(GPIO_Port_t port, GPIO_Pin_t pin)
{
    ++calls;
    assert((unsigned int)port <= 3U && (unsigned int)pin <= 7U);
    if ((failAt != 0U) && (calls == failAt)) { return injected; }
    return VOLT_OK;
}

volt_status_t GPIO_SetDirection(GPIO_Port_t port, GPIO_Pin_t pin, GPIO_Direction_t d)
{
    volt_status_t s = startCall(port, pin);
    u8 mask = (u8)(1U << pin);
    if (s != VOLT_OK) { return s; }
    direction[port] = (d == GPIO_OUTPUT) ? (u8)(direction[port] | mask)
        : (u8)(direction[port] & (u8)~mask);
    return VOLT_OK;
}

volt_status_t GPIO_SetPullup(GPIO_Port_t port, GPIO_Pin_t pin, GPIO_Pullup_t p)
{
    volt_status_t s = startCall(port, pin);
    u8 mask = (u8)(1U << pin);
    if (s != VOLT_OK) { return s; }
    assert((direction[port] & mask) == 0U);
    latch[port] = (p == GPIO_PULLUP_ENABLED) ? (u8)(latch[port] | mask)
        : (u8)(latch[port] & (u8)~mask);
    return VOLT_OK;
}

volt_status_t GPIO_WritePin(GPIO_Port_t port, GPIO_Pin_t pin, GPIO_Level_t level)
{
    volt_status_t s = startCall(port, pin);
    u8 oldEn = readLine(LCD_EN_PORT, LCD_EN_PIN);
    u8 mask = (u8)(1U << pin);
    if (s != VOLT_OK) { return s; }
    if ((direction[port] & mask) == 0U) { return VOLT_NOT_READY; }
    latch[port] = (level == GPIO_HIGH) ? (u8)(latch[port] | mask)
        : (u8)(latch[port] & (u8)~mask);
    if ((port != LCD_EN_PORT) || (pin != LCD_EN_PIN)) { dataTime = nowUs; }
    if ((oldEn == 0U) && (readLine(LCD_EN_PORT, LCD_EN_PIN) != 0U))
    {
        assert(nowUs - dataTime >= LCD_DATA_SETUP_US);
        riseTime = nowUs;
    }
    if ((oldEn != 0U) && (readLine(LCD_EN_PORT, LCD_EN_PIN) == 0U))
    {
        assert(nowUs - riseTime >= LCD_ENABLE_HIGH_US);
        assert(edges < 1024U);
        nibbles[edges] = (u8)(readLine(LCD_D4_PORT, LCD_D4_PIN) |
            (readLine(LCD_D5_PORT, LCD_D5_PIN) << 1U) |
            (readLine(LCD_D6_PORT, LCD_D6_PIN) << 2U) |
            (readLine(LCD_D7_PORT, LCD_D7_PIN) << 3U));
        rsValues[edges] = readLine(LCD_RS_PORT, LCD_RS_PIN);
        times[edges] = nowUs;
        ++edges;
    }
    return VOLT_OK;
}

void Delay_us(u16 us) { nowUs += us; }
void Delay_ms(u16 ms) { nowUs += (u64)ms * 1000U; }

static void resetMock(void)
{
    unsigned int i;
    for (i = 0U; i < 4U; ++i) { direction[i] = 0U; latch[i] = 0U; }
    edges = 0U; calls = 0U; failAt = 0U;
    nowUs = 0U; riseTime = 0U; dataTime = 0U;
}

static void expectByte(unsigned int edge, u8 value, u8 rs)
{
    assert(edge + 1U < edges);
    assert(nibbles[edge] == (value >> 4U));
    assert(nibbles[edge + 1U] == (value & 15U));
    assert(rsValues[edge] == rs && rsValues[edge + 1U] == rs);
}

int main(void)
{
    unsigned int i, startupCalls, writeCalls, before;
    const u8 initCommands[] = {0x28, 0x08, 0x01, 0x06, 0x0C};
    resetMock();
    assert(LCD_Clear() == VOLT_NOT_READY);
    assert(LCD_Home() == VOLT_NOT_READY);
    assert(LCD_SetCursor(LCD_ROW_1, 0U) == VOLT_NOT_READY);
    assert(LCD_WriteChar('A') == VOLT_NOT_READY);
    assert(LCD_WriteString("A") == VOLT_NOT_READY);
    assert(LCD_WriteStringAt(LCD_ROW_1, 0U, "A") == VOLT_NOT_READY);
    assert(LCD_SetDisplayControl(LCD_ON, LCD_OFF, LCD_OFF) == VOLT_NOT_READY);
    assert(calls == 0U);

    assert(LCD_Init() == VOLT_OK);
    startupCalls = calls;
    assert(edges == 14U);
    assert(nibbles[0] == 3U && nibbles[1] == 3U && nibbles[2] == 3U && nibbles[3] == 2U);
    assert(times[0] >= 50000U);
    assert(times[1] - times[0] >= 5000U);
    assert(times[2] - times[1] >= 200U && times[3] - times[2] >= 200U);
    for (i = 0U; i < 5U; ++i) { expectByte(4U + 2U * i, initCommands[i], 0U); }
    assert(times[10] - times[9] >= 3000U);

    edges = 0U;
    assert(LCD_WriteStringAt(LCD_ROW_2, 3U, "AB") == VOLT_OK);
    assert(edges == 6U);
    expectByte(0U, 0xC3U, 0U); expectByte(2U, 'A', 1U); expectByte(4U, 'B', 1U);
    edges = 0U;
    assert(LCD_SetDisplayControl(LCD_ON, LCD_OFF, LCD_ON) == VOLT_OK);
    expectByte(0U, 0x0DU, 0U);

    before = calls;
    assert(LCD_SetCursor((LCD_Row_t)-1, 0U) == VOLT_INVALID_ARG);
    assert(LCD_SetCursor((LCD_Row_t)2, 0U) == VOLT_INVALID_ARG);
    assert(LCD_SetCursor(LCD_ROW_1, 16U) == VOLT_OUT_OF_RANGE);
    assert(LCD_WriteString(NULL_PTR) == VOLT_INVALID_ARG);
    assert(LCD_WriteStringAt(LCD_ROW_1, 0U, NULL_PTR) == VOLT_INVALID_ARG);
    assert(LCD_WriteStringAt(LCD_ROW_1, 15U, "AB") == VOLT_OUT_OF_RANGE);
    assert(LCD_SetDisplayControl((LCD_State_t)2, LCD_OFF, LCD_OFF) == VOLT_INVALID_ARG);
    assert(LCD_SetDisplayControl(LCD_ON, (LCD_State_t)-1, LCD_OFF) == VOLT_INVALID_ARG);
    assert(LCD_SetDisplayControl(LCD_ON, LCD_OFF, (LCD_State_t)2) == VOLT_INVALID_ARG);
    assert(calls == before);

    edges = 0U;
    assert(LCD_Clear() == VOLT_OK); expectByte(0U, 1U, 0U);
    assert(LCD_WriteString("12345678901234567") == VOLT_OUT_OF_RANGE);
    assert(edges == 2U);
    assert(LCD_WriteString("1234567890123456") == VOLT_OK);
    before = calls;
    assert(LCD_WriteChar('!') == VOLT_OUT_OF_RANGE);
    assert(LCD_WriteString("!") == VOLT_OUT_OF_RANGE);
    assert(LCD_WriteString("") == VOLT_OK);
    assert(calls == before);
    edges = 0U;
    assert(LCD_Home() == VOLT_OK); expectByte(0U, 2U, 0U);
    assert(LCD_WriteChar('H') == VOLT_OK);

    for (i = 0U; i < 32U; ++i)
    {
        edges = 0U;
        assert(LCD_SetCursor((i < 16U) ? LCD_ROW_1 : LCD_ROW_2, (u8)(i % 16U)) == VOLT_OK);
        expectByte(0U, (u8)(0x80U + ((i < 16U) ? 0U : 0x40U) + i % 16U), 0U);
    }

    /* Every GPIO operation in startup is a possible injected failure point. */
    for (i = 1U; i <= startupCalls; ++i)
    {
        resetMock();
        failAt = i;
        injected = (i % 2U) ? VOLT_NOT_READY : VOLT_INVALID_ARG;
        assert(LCD_Init() == injected);
        before = calls;
        assert(LCD_WriteChar('X') == VOLT_NOT_READY);
        assert(calls == before);
        failAt = 0U;
        assert(LCD_Init() == VOLT_OK);
    }
    resetMock(); assert(LCD_Init() == VOLT_OK);
    calls = 0U; edges = 0U;
    assert(LCD_WriteChar('X') == VOLT_OK);
    writeCalls = calls;
    for (i = 1U; i <= writeCalls; ++i)
    {
        resetMock(); assert(LCD_Init() == VOLT_OK);
        calls = 0U; edges = 0U; failAt = i; injected = VOLT_NOT_READY;
        assert(LCD_WriteChar('X') == VOLT_NOT_READY);
        before = calls;
        assert(LCD_Clear() == VOLT_NOT_READY);
        assert(calls == before);
        failAt = 0U; assert(LCD_Init() == VOLT_OK);
    }
    puts("PASS LCD: startup, nibbles, RS/EN timing, row bounds, text validation and GPIO fault recovery.");
    return 0;
}
