#include <assert.h>
#include <stdio.h>
#include "gpio.h"
#include "Registers.h"

volatile u8 DDRA, DDRB, DDRC, DDRD;
volatile u8 PORTA, PORTB, PORTC, PORTD;
volatile u8 PINA, PINB, PINC, PIND;
volatile u8 SREG, SFIOR;

int main(void)
{
    SREG = 0x80U;
    GPIO_SetDirection(GPIO_PORT_A, GPIO_PIN_0, GPIO_OUTPUT);
    GPIO_WritePin(GPIO_PORT_A, GPIO_PIN_0, GPIO_HIGH);
    assert(DDRA == 1U && PORTA == 1U && SREG == 0x80U);
    GPIO_WritePin(GPIO_PORT_A, GPIO_PIN_1, GPIO_HIGH);
    GPIO_WriteMaskedPort(GPIO_PORT_A, 3U, 0U);
    GPIO_WritePort(GPIO_PORT_A, 0xFFU);
    assert(PORTA == 1U);
    GPIO_TogglePin(GPIO_PORT_A, GPIO_PIN_0);
    assert(PORTA == 0U);

    SREG = 0U;
    GPIO_SetPullup(GPIO_PORT_B, GPIO_PIN_7, GPIO_PULLUP_ENABLED);
    assert(PORTB == 0x80U);
    GPIO_SetPullup(GPIO_PORT_B, GPIO_PIN_7, GPIO_PULLUP_DISABLED);
    SFIOR = 4U;
    GPIO_SetPullup(GPIO_PORT_B, GPIO_PIN_7, GPIO_PULLUP_ENABLED);
    assert(PORTB == 0U && SREG == 0U);

    GPIO_SetPortDirection(GPIO_PORT_C, 0xFFU);
    GPIO_WritePort(GPIO_PORT_C, 0xA5U);
    GPIO_WriteMaskedPort(GPIO_PORT_C, 0x0FU, 0xFAU);
    assert(DDRC == 0xFFU && PORTC == 0xAAU);
    PIND = 0xFFU;
    assert(GPIO_ReadPort(GPIO_PORT_D) == 255);
    assert(GPIO_ReadPin(GPIO_PORT_D, GPIO_PIN_7) == GPIO_HIGH);
    PIND = 0U;
    assert(GPIO_ReadPin(GPIO_PORT_D, GPIO_PIN_7) == GPIO_LOW);
    assert(GPIO_ReadPin(GPIO_PORT_A, (GPIO_Pin_t)8) == GPIO_LEVEL_INVALID);
    assert(GPIO_ReadPin((GPIO_Port_t)4, GPIO_PIN_0) == GPIO_LEVEL_INVALID);
    assert(GPIO_ReadPort((GPIO_Port_t)4) == -1);
    puts("PASS GPIO: pin and port operations, blocked writes and read errors.");
    return 0;
}
