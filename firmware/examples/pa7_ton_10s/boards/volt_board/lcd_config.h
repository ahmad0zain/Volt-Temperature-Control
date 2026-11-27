/**
 * @file lcd_config.h
 * @brief LCD wiring and timing matched to Zain's Proteus screenshot.
 * @version 0.1.0
 *
 * Mapping below follows the supplied ATmega32 Proteus schematic.
 * Confirm wiring before running. R/W MUST be tied to GND; D0-D3 are unused.
 * VSS=GND, VDD=module-rated supply, VO=contrast adjustment. Backlight power
 * and its current limiting follow the actual module specification.
 * PA0 is left free for the project's LM35/ADC input.
 *
 * Source: Hitachi HD44780U, Table 6 and Figure 24 (4-bit initialization).
 * https://cdn-shop.adafruit.com/datasheets/HD44780.pdf
 * Conservative waits below are configurable for the actual controller clone.
 */
#ifndef VOLT_LCD_CONFIG_H
#define VOLT_LCD_CONFIG_H

#include "gpio.h"

#define LCD_RS_PORT GPIO_PORT_D
#define LCD_RS_PIN  GPIO_PIN_0
#define LCD_EN_PORT GPIO_PORT_D
#define LCD_EN_PIN  GPIO_PIN_1

#define LCD_D4_PORT GPIO_PORT_D
#define LCD_D4_PIN  GPIO_PIN_4
#define LCD_D5_PORT GPIO_PORT_D
#define LCD_D5_PIN  GPIO_PIN_5
#define LCD_D6_PORT GPIO_PORT_D
#define LCD_D6_PIN  GPIO_PIN_6
#define LCD_D7_PORT GPIO_PORT_D
#define LCD_D7_PIN  GPIO_PIN_7

/* This first version intentionally supports only the 16-column, 2-row layout. */
#define LCD_COLUMNS 16U
#define LCD_ROWS     2U

#define LCD_POWER_ON_DELAY_MS 50U
#define LCD_RESET_FIRST_MS     5U
#define LCD_RESET_NEXT_US    200U
#define LCD_COMMAND_DELAY_US 100U
#define LCD_CLEAR_HOME_MS      3U
#define LCD_DATA_SETUP_US      1U
#define LCD_ENABLE_HIGH_US     1U
#define LCD_DATA_HOLD_US       1U

/* Delay_us and Delay_ms take u16. Zero disables the requested wait. */
#define LCD_DELAY_INVALID(n) (((n) < 1) || ((n) > 65535UL))
#if LCD_DELAY_INVALID(LCD_POWER_ON_DELAY_MS) || \
    LCD_DELAY_INVALID(LCD_RESET_FIRST_MS) || \
    LCD_DELAY_INVALID(LCD_RESET_NEXT_US) || \
    LCD_DELAY_INVALID(LCD_COMMAND_DELAY_US) || \
    LCD_DELAY_INVALID(LCD_CLEAR_HOME_MS) || \
    LCD_DELAY_INVALID(LCD_DATA_SETUP_US) || \
    LCD_DELAY_INVALID(LCD_ENABLE_HIGH_US) || \
    LCD_DELAY_INVALID(LCD_DATA_HOLD_US)
#error "LCD delay settings must be integers from 1 to 65535 in the specified units."
#endif
#undef LCD_DELAY_INVALID

#if (LCD_COLUMNS != 16U) || (LCD_ROWS != 2U)
#error "This driver version supports only LCD1602 (16 columns, 2 rows)."
#endif

#endif /* VOLT_LCD_CONFIG_H */
