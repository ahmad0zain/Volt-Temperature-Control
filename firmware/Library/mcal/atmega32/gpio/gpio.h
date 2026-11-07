/**
 * @file gpio.h
 * @brief Public GPIO interface for Volt ATmega32/ATmega32A drivers.
 * @version 0.2.0-draft
 *
 * Review draft: declarations only; requires a matching gpio.c.
 * Uses the project's Common headers. Register definitions belong in gpio.c.
 */
#ifndef VOLT_GPIO_H
#define VOLT_GPIO_H

#include "StdTypes.h"
#include "volt_status.h"

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
    GPIO_LOW = 0,
    GPIO_HIGH
} GPIO_Level_t;

typedef enum
{
    GPIO_PULLUP_DISABLED = 0,
    GPIO_PULLUP_ENABLED
} GPIO_Pullup_t;

/*
 * Common API contract for the future implementation:
 * - Success: VOLT_OK. Invalid enum/port/pin/null pointer: VOLT_INVALID_ARG.
 * - VOLT_NOT_READY: direction or global pull-up configuration blocks a call.
 * - Failure leaves registers and caller output values unchanged.
 * - Mutating calls preserve the caller's interrupt enable state and protect
 *   register read-modify-write operations with short critical sections.
 * - Peripheral overrides, JTAG, wiring and electrical limits are the caller's
 *   responsibility. VOLT_OK does not establish physical ownership of a pin.
 */

/* ---------- Single-pin operations: 5 functions ---------- */

/* Set direction, preserving the PORT latch.
 * When enabling output, the existing latch determines the initial level.
 * When selecting input, an existing high latch requests a pull-up (if allowed).
 * This function changes direction only; use SetPullup for explicit pull control.
 */
volt_status_t GPIO_SetDirection(
    GPIO_Port_t port,
    GPIO_Pin_t pin,
    GPIO_Direction_t direction);

/* Input only; output pins return VOLT_NOT_READY.
 * Enabling while the global pull-up disable bit is set returns VOLT_NOT_READY.
 * Does not modify the global pull-up disable setting.
 */
volt_status_t GPIO_SetPullup(
    GPIO_Port_t port,
    GPIO_Pin_t pin,
    GPIO_Pullup_t pullup);

/* Output only; input pins return VOLT_NOT_READY. */
volt_status_t GPIO_WritePin(
    GPIO_Port_t port,
    GPIO_Pin_t pin,
    GPIO_Level_t level);

/* Output only; input pins return VOLT_NOT_READY. */
volt_status_t GPIO_TogglePin(
    GPIO_Port_t port,
    GPIO_Pin_t pin);

/* Read the physical pin level, on either an input or an output. */
volt_status_t GPIO_ReadPin(
    GPIO_Port_t port,
    GPIO_Pin_t pin,
    GPIO_Level_t *level);

/* ---------- Port operations: 4 functions ---------- */

/* Configure ALL eight directions: bit 0=input, bit 1=output.
 * Preserves the PORT latch; account for initial output levels/input pull-ups.
 */
volt_status_t GPIO_SetPortDirection(
    GPIO_Port_t port,
    u8 directionMask);

/* Write all eight output latches. Requires ALL eight pins to be outputs;
 * otherwise returns VOLT_NOT_READY without changing any pin.
 */
volt_status_t GPIO_WritePort(
    GPIO_Port_t port,
    u8 value);

/* Write only bits selected by mask; preserve unselected bits.
 * Every selected pin must be an output, otherwise VOLT_NOT_READY.
 * Bits of value outside mask are ignored.
 * mask=0 is a successful no-op for a valid port.
 */
volt_status_t GPIO_WriteMaskedPort(
    GPIO_Port_t port,
    u8 mask,
    u8 value);

/* Read ALL eight physical pin levels, not the output latch. */
volt_status_t GPIO_ReadPort(
    GPIO_Port_t port,
    u8 *value);

#endif /* VOLT_GPIO_H */
