/**
 * @file gpio.h
 * @brief Public GPIO interface for Volt ATmega32/ATmega32A drivers.
 */
#ifndef VOLT_GPIO_H
#define VOLT_GPIO_H

#include "StdTypes.h"

/* Select the port. */
typedef enum
{
    GPIO_PORT_A = 0,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D
} GPIO_Port_t;

/* Bit number inside the selected port, not a package pin number or mask. */
typedef enum
{
    GPIO_PIN_0 = 0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7
} GPIO_Pin_t;

/* Direction, logical level and pull-up are separate choices. */
typedef enum
{
    GPIO_INPUT = 0,
    GPIO_OUTPUT
} GPIO_Direction_t;

typedef enum
{
    GPIO_LEVEL_INVALID = -1,
    GPIO_LOW = 0,
    GPIO_HIGH
} GPIO_Level_t;

typedef enum
{
    GPIO_PULLUP_DISABLED = 0,
    GPIO_PULLUP_ENABLED
} GPIO_Pullup_t;

/* Invalid or blocked writes are ignored. Set the direction before writing.
 * Direction changes preserve the output latch and interrupt state.
 */
void GPIO_SetDirection(GPIO_Port_t port, GPIO_Pin_t pin, GPIO_Direction_t direction);
void GPIO_SetPullup(GPIO_Port_t port, GPIO_Pin_t pin, GPIO_Pullup_t pullup);
void GPIO_WritePin(GPIO_Port_t port, GPIO_Pin_t pin, GPIO_Level_t level);
void GPIO_TogglePin(GPIO_Port_t port, GPIO_Pin_t pin);

/* Read the physical level. Invalid arguments return GPIO_LEVEL_INVALID. */
GPIO_Level_t GPIO_ReadPin(GPIO_Port_t port, GPIO_Pin_t pin);

/* A set direction bit selects output. All selected pins must be outputs
 * for a write to take effect. WritePort selects all eight pins.
 */
void GPIO_SetPortDirection(GPIO_Port_t port, u8 directionMask);
void GPIO_WritePort(GPIO_Port_t port, u8 value);
void GPIO_WriteMaskedPort(GPIO_Port_t port, u8 mask, u8 value);

/* Read all physical pin levels (0..255). An invalid port returns -1. */
int GPIO_ReadPort(GPIO_Port_t port);

#endif /* VOLT_GPIO_H */
