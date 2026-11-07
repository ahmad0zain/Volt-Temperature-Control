/**
 * @file lcd.h
 * @brief One HD44780-compatible LCD1602, 4-bit parallel, R/W tied to GND.
 * @version 0.1.0
 *
 * Blocking, write-only and NOT reentrant. Serialize access between tasks;
 * do not call this LCD interface from an ISR. No heap allocation is used.
 * Rows have names; column indexes are 0..15. No automatic line wrapping.
 * Characters are controller character-ROM bytes, not UTF-8/Arabic shaping.
 *
 * VOLT_OK means writes/delays were completed, NOT hardware acknowledgement.
 * A disconnected display cannot be detected with R/W tied to ground.
 * VOLT_INVALID_ARG: invalid enum, NULL pointer or invalid pin configuration.
 * VOLT_OUT_OF_RANGE: column/string exceeds the configured visible row.
 * VOLT_NOT_READY: not initialized, or a GPIO operation cannot proceed.
 * VOLT_TIMEOUT is not generated: fixed waits are used, not busy-flag polling.
 * GPIO failures are propagated, may leave a partial transfer/text on the LCD,
 * and invalidate initialization. Call LCD_Init again after fixing the cause.
 * Argument checks reject bad input before sending a command or data.
 */
#ifndef VOLT_LCD_H
#define VOLT_LCD_H

#include "StdTypes.h"
#include "volt_status.h"

typedef enum
{
    LCD_ROW_1 = 0,
    LCD_ROW_2 = 1
} LCD_Row_t;

typedef enum
{
    LCD_OFF = 0,
    LCD_ON = 1
} LCD_State_t;

/* Run the complete startup sequence; can also reinitialize after an error. */
volt_status_t LCD_Init(void);

/* Clear visible/display memory and put the cursor at row 1, column 0. */
volt_status_t LCD_Clear(void);

/* Return cursor/display shift home without erasing text. */
volt_status_t LCD_Home(void);

volt_status_t LCD_SetCursor(LCD_Row_t row, u8 column);

/* A char is transmitted as one byte. A full row rejects another character. */
volt_status_t LCD_WriteChar(char character);

/* NUL-terminated RAM string; must fit the remaining visible row.
 * Empty strings are accepted. No special newline/escape processing is done.
 * The length is checked before any byte is sent; overflow writes nothing.
 */
volt_status_t LCD_WriteString(const char *text);

/* Validate the whole request, position the cursor, then write the string. */
volt_status_t LCD_WriteStringAt(LCD_Row_t row, u8 column, const char *text);

/* Control display visibility, cursor underline and cursor blink separately. */
volt_status_t LCD_SetDisplayControl(
    LCD_State_t display,
    LCD_State_t cursor,
    LCD_State_t blink);

#endif /* VOLT_LCD_H */
