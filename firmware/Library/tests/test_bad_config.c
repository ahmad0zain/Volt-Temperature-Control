#include <assert.h>
#include "lcd.h"
#include "gpio.h"
#include "delay.h"
static unsigned int calls;
volt_status_t GPIO_SetDirection(GPIO_Port_t p, GPIO_Pin_t n, GPIO_Direction_t d)
{ (void)p; (void)n; (void)d; ++calls; return VOLT_OK; }
volt_status_t GPIO_SetPullup(GPIO_Port_t p, GPIO_Pin_t n, GPIO_Pullup_t v)
{ (void)p; (void)n; (void)v; ++calls; return VOLT_OK; }
volt_status_t GPIO_WritePin(GPIO_Port_t p, GPIO_Pin_t n, GPIO_Level_t v)
{ (void)p; (void)n; (void)v; ++calls; return VOLT_OK; }
void Delay_us(u16 v) { (void)v; }
void Delay_ms(u16 v) { (void)v; }
int main(void)
{
    assert(LCD_Init() == VOLT_INVALID_ARG);
    assert(calls == 0U);
    assert(LCD_WriteChar('X') == VOLT_NOT_READY);
    return 0;
}
