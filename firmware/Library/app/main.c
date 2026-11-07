/* Proposed lcd_config.h wiring must be checked against your actual board. */
#include "lcd.h"

int main(void)
{
    volt_status_t status = LCD_Init();
    if (status == VOLT_OK)
    {
        status = LCD_WriteStringAt(LCD_ROW_1, 0U, "Volt Embedded");
    }
    if (status == VOLT_OK)
    {
        status = LCD_WriteStringAt(LCD_ROW_2, 0U, "Temp: --.- C");
    }
    if (status != VOLT_OK)
    {
        for (;;) { /* Debug status and correct the configuration/cause. */ }
    }
    for (;;) { /* Temperature measurement will be added separately. */ }
}
