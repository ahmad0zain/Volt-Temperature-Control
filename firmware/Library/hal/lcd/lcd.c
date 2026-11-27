/**
 * @file lcd.c
 * @brief LCD1602 protocol implementation using public GPIO and delay APIs.
 * @version 0.1.0
 *
 * HD44780U: high nibble first, then low nibble; Figure 24 startup sequence.
 * No MCU registers, vendor headers, raw-command API or busy-flag reads.
 * Commands are private so cursor tracking and 4-bit mode remain consistent.
 */
#include "lcd.h"
#include "lcd_config.h"
#include "delay.h"

#define LCD_PIN_COUNT          6U
#define LCD_EN_INDEX           0U
#define LCD_RS_INDEX           1U
#define LCD_DATA_FIRST_INDEX   2U
#define LCD_CMD_CLEAR          0x01U
#define LCD_CMD_HOME           0x02U
#define LCD_CMD_ENTRY          0x06U
#define LCD_CMD_DISPLAY_OFF    0x08U
#define LCD_CMD_DISPLAY_ON     0x0CU
#define LCD_CMD_FUNCTION_4BIT   0x28U
#define LCD_CMD_SET_DDRAM      0x80U

typedef struct
{
    GPIO_Port_t port;
    GPIO_Pin_t pin;
} LCD_Pin_t;

/* EN is configured first and held low before other lines are prepared. */
static const LCD_Pin_t lcdPins[LCD_PIN_COUNT] =
{
    { LCD_EN_PORT, LCD_EN_PIN },
    { LCD_RS_PORT, LCD_RS_PIN },
    { LCD_D4_PORT, LCD_D4_PIN },
    { LCD_D5_PORT, LCD_D5_PIN },
    { LCD_D6_PORT, LCD_D6_PIN },
    { LCD_D7_PORT, LCD_D7_PIN }
};

static u8 lcdReady = 0U;
/* Column may be LCD_COLUMNS: one-past-end, so another write is rejected. */
static u8 lcdColumn = 0U;

static volt_status_t LCD_ValidateConfig(void)
{
    u8 i, j;
    for (i = 0U; i < LCD_PIN_COUNT; ++i)
    {
        if (((unsigned int)lcdPins[i].port > (unsigned int)GPIO_PORT_D) ||
            ((unsigned int)lcdPins[i].pin > (unsigned int)GPIO_PIN_7))
        {
            return VOLT_INVALID_ARG;
        }
        for (j = 0U; j < i; ++j)
        {
            if ((lcdPins[i].port == lcdPins[j].port) &&
                (lcdPins[i].pin == lcdPins[j].pin))
            {
                return VOLT_INVALID_ARG;
            }
        }
    }
    return VOLT_OK;
}

static void LCD_WriteLine(u8 index, GPIO_Level_t level)
{
    GPIO_WritePin(lcdPins[index].port, lcdPins[index].pin, level);
}

static void LCD_WriteNibble(u8 nibble)
{
    u8 bit;

    LCD_WriteLine(LCD_EN_INDEX, GPIO_LOW);

    for (bit = 0U; bit < 4U; ++bit)
    {
        LCD_WriteLine((u8)(LCD_DATA_FIRST_INDEX + bit),
            ((nibble & (u8)(1U << bit)) != 0U) ? GPIO_HIGH : GPIO_LOW);
    }

    Delay_us(LCD_DATA_SETUP_US);
    LCD_WriteLine(LCD_EN_INDEX, GPIO_HIGH);
    Delay_us(LCD_ENABLE_HIGH_US);
    LCD_WriteLine(LCD_EN_INDEX, GPIO_LOW);
    Delay_us(LCD_DATA_HOLD_US);
}

static void LCD_SendByte(u8 value, GPIO_Level_t rs)
{
    LCD_WriteLine(LCD_RS_INDEX, rs);
    LCD_WriteNibble((u8)(value >> 4U));
    LCD_WriteNibble((u8)(value & 0x0FU));

    if ((rs == GPIO_LOW) &&
        ((value == LCD_CMD_CLEAR) || (value == LCD_CMD_HOME)))
    {
        Delay_ms(LCD_CLEAR_HOME_MS);
    }
    else
    {
        Delay_us(LCD_COMMAND_DELAY_US);
    }
}

/* Read at most capacity+1 bytes to decide whether the text fits. */
static volt_status_t LCD_CheckText(const char *text, u8 capacity)
{
    u8 length = 0U;
    if (text == NULL_PTR) { return VOLT_INVALID_ARG; }
    while (text[length] != '\0')
    {
        if (length >= capacity) { return VOLT_OUT_OF_RANGE; }
        ++length;
    }
    return VOLT_OK;
}

volt_status_t LCD_Init(void)
{
    u8 i;
    volt_status_t status;
    lcdReady = 0U;
    lcdColumn = 0U;
    status = LCD_ValidateConfig();
    if (status != VOLT_OK) { return status; }

    for (i = 0U; i < LCD_PIN_COUNT; ++i)
    {
        /* Preload a low PORT latch using only public GPIO operations, then
         * enable output. This also avoids inheriting a previous pull-up latch.
         */
        GPIO_SetDirection(lcdPins[i].port, lcdPins[i].pin, GPIO_INPUT);
        GPIO_SetPullup(lcdPins[i].port, lcdPins[i].pin, GPIO_PULLUP_DISABLED);
        GPIO_SetDirection(lcdPins[i].port, lcdPins[i].pin, GPIO_OUTPUT);
        LCD_WriteLine(i, GPIO_LOW);
    }

    Delay_ms(LCD_POWER_ON_DELAY_MS);
    LCD_WriteNibble(0x03U);
    Delay_ms(LCD_RESET_FIRST_MS);
    LCD_WriteNibble(0x03U);
    Delay_us(LCD_RESET_NEXT_US);
    LCD_WriteNibble(0x03U);
    Delay_us(LCD_RESET_NEXT_US);
    LCD_WriteNibble(0x02U);
    Delay_us(LCD_RESET_NEXT_US);

    LCD_SendByte(LCD_CMD_FUNCTION_4BIT, GPIO_LOW);
    LCD_SendByte(LCD_CMD_DISPLAY_OFF, GPIO_LOW);
    LCD_SendByte(LCD_CMD_CLEAR, GPIO_LOW);
    LCD_SendByte(LCD_CMD_ENTRY, GPIO_LOW);
    LCD_SendByte(LCD_CMD_DISPLAY_ON, GPIO_LOW);

    lcdReady = 1U;
    return VOLT_OK;
}

volt_status_t LCD_Clear(void)
{
    if (lcdReady == 0U) { return VOLT_NOT_READY; }
    LCD_SendByte(LCD_CMD_CLEAR, GPIO_LOW);
    lcdColumn = 0U;
    return VOLT_OK;
}

volt_status_t LCD_Home(void)
{
    if (lcdReady == 0U) { return VOLT_NOT_READY; }
    LCD_SendByte(LCD_CMD_HOME, GPIO_LOW);
    lcdColumn = 0U;
    return VOLT_OK;
}

static void LCD_PositionCursor(LCD_Row_t row, u8 column)
{
    u8 address = (u8)(((row == LCD_ROW_1) ? 0x00U : 0x40U) + column);
    LCD_SendByte((u8)(LCD_CMD_SET_DDRAM | address), GPIO_LOW);
    lcdColumn = column;
}

volt_status_t LCD_SetCursor(LCD_Row_t row, u8 column)
{
    if ((row != LCD_ROW_1) && (row != LCD_ROW_2)) { return VOLT_INVALID_ARG; }
    if (column >= LCD_COLUMNS) { return VOLT_OUT_OF_RANGE; }
    if (lcdReady == 0U) { return VOLT_NOT_READY; }

    LCD_PositionCursor(row, column);
    return VOLT_OK;
}

volt_status_t LCD_WriteChar(char character)
{
    if (lcdReady == 0U) { return VOLT_NOT_READY; }
    if (lcdColumn >= LCD_COLUMNS) { return VOLT_OUT_OF_RANGE; }
    LCD_SendByte((u8)character, GPIO_HIGH);
    ++lcdColumn;
    return VOLT_OK;
}

/* Both callers validate the entire string before the first transfer. */
static void LCD_SendText(const char *text)
{
    while (*text != '\0')
    {
        LCD_SendByte((u8)*text, GPIO_HIGH);
        ++lcdColumn;
        ++text;
    }
}

volt_status_t LCD_WriteString(const char *text)
{
    volt_status_t status;
    if (text == NULL_PTR) { return VOLT_INVALID_ARG; }
    if (lcdReady == 0U) { return VOLT_NOT_READY; }
    status = LCD_CheckText(text, (u8)(LCD_COLUMNS - lcdColumn));
    if (status != VOLT_OK) { return status; }
    LCD_SendText(text);
    return VOLT_OK;
}

volt_status_t LCD_WriteStringAt(LCD_Row_t row, u8 column, const char *text)
{
    volt_status_t status;
    if ((row != LCD_ROW_1) && (row != LCD_ROW_2)) { return VOLT_INVALID_ARG; }
    if (column >= LCD_COLUMNS) { return VOLT_OUT_OF_RANGE; }
    status = LCD_CheckText(text, (u8)(LCD_COLUMNS - column));
    if (status != VOLT_OK) { return status; }
    if (lcdReady == 0U) { return VOLT_NOT_READY; }
    LCD_PositionCursor(row, column);
    LCD_SendText(text);
    return VOLT_OK;
}

volt_status_t LCD_SetDisplayControl(
    LCD_State_t display, LCD_State_t cursor, LCD_State_t blink)
{
    u8 command = LCD_CMD_DISPLAY_OFF;
    if (((display != LCD_OFF) && (display != LCD_ON)) ||
        ((cursor != LCD_OFF) && (cursor != LCD_ON)) ||
        ((blink != LCD_OFF) && (blink != LCD_ON)))
    {
        return VOLT_INVALID_ARG;
    }
    if (lcdReady == 0U) { return VOLT_NOT_READY; }
    if (display == LCD_ON) { command = (u8)(command | 0x04U); }
    if (cursor == LCD_ON) { command = (u8)(command | 0x02U); }
    if (blink == LCD_ON) { command = (u8)(command | 0x01U); }
    LCD_SendByte(command, GPIO_LOW);
    return VOLT_OK;
}
