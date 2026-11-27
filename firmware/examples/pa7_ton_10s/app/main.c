/* ATmega32, 1 MHz, matched to Zain's supplied Proteus schematic.
 * Button: PA7 (package pin 33) to GND; internal pull-up enabled.
 * LCD: RS=PD0, E=PD1, D4..D7=PD4..PD7, R/W=GND.
 * Existing LED: PC0 through its resistor to LED, then GND.
 */
#include "lcd.h"
#include "gpio.h"
#include "ton.h"
#include "timebase.h"
#include <avr/interrupt.h>
#include <stdio.h>

#define TON_PRESET_MS 10000UL
#define DEBOUNCE_MS     20UL
#define LCD_REFRESH_MS 100UL

int main(void)
{
    TON_Timer_t timer1 = {0};
    u8 last_raw = 0U;
    u8 input = 0U;
    u32 changed_at = 0UL;
    u32 refreshed_at = 0UL;
    char line[17];

    /* PA7 reads HIGH when released, LOW when pressed. */
    GPIO_SetDirection(GPIO_PORT_A, GPIO_PIN_7, GPIO_INPUT);
    GPIO_SetPullup(GPIO_PORT_A, GPIO_PIN_7, GPIO_PULLUP_ENABLED);

    GPIO_SetDirection(GPIO_PORT_C, GPIO_PIN_0, GPIO_OUTPUT);
    GPIO_WritePin(GPIO_PORT_C, GPIO_PIN_0, GPIO_LOW);

    if (LCD_Init() != VOLT_OK)
    {
        while (1) { /* Check LCD configuration before continuing. */ }
    }
    LCD_WriteStringAt(LCD_ROW_1, 0U, "ET:00000 ms     ");
    LCD_WriteStringAt(LCD_ROW_2, 0U, "Q:0 IN:0 PT:10s ");

    Timebase_Init();
    sei();

    while (1)
    {
        u32 now = Timebase_Millis();
        u8 raw = (GPIO_ReadPin(GPIO_PORT_A, GPIO_PIN_7) == GPIO_LOW);

        /* Accept each button transition after 20 ms of stable readings. */
        if (raw != last_raw)
        {
            last_raw = raw;
            changed_at = now;
        }
        if ((u32)(now - changed_at) >= DEBOUNCE_MS)
        {
            input = last_raw;
        }

        /* Standard TON: hold for 10 s; release resets both Q and ET. */
        TON_Update(&timer1, input, TON_PRESET_MS, now);

        GPIO_WritePin(GPIO_PORT_C, GPIO_PIN_0,
                      timer1.Q ? GPIO_HIGH : GPIO_LOW);

        /* ET and Q update every scan. Only the LCD refresh is scheduled. */
        if ((u32)(now - refreshed_at) >= LCD_REFRESH_MS)
        {
            refreshed_at = now;

            snprintf(line, sizeof line, "ET:%05u ms     ",
                     (unsigned int)timer1.ET);
            LCD_WriteStringAt(LCD_ROW_1, 0U, line);

            snprintf(line, sizeof line, "Q:%c IN:%c PT:10s ",
                     timer1.Q ? '1' : '0', input ? '1' : '0');
            LCD_WriteStringAt(LCD_ROW_2, 0U, line);
        }
    }
}
