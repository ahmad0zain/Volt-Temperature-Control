/**
 * @file gpio.c
 * @brief ATmega32/ATmega32A implementation of the nine APIs in gpio.h.
 * @version 0.2.0
 * @author Ahmad Saber Zain
 *
 * Toolchain: AVR-GCC. Uses our own register map, not vendor AVR headers.
 * Source: Microchip DS40002072A, sections 13.2 and 13.4.
 * https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/ATmega32A-DataSheet-Complete-DS40002072A.pdf
 *
 * Status contract:
 * - Every public function: VOLT_OK or VOLT_INVALID_ARG.
 * - SetPullup, WritePin, TogglePin, WritePort and WriteMaskedPort may also
 *   return VOLT_NOT_READY as specified in gpio.h.
 * - No operation waits for hardware: VOLT_TIMEOUT is never returned.
 * - Invalid API arguments use VOLT_INVALID_ARG, not VOLT_OUT_OF_RANGE.
 * - On error, registers and caller output values remain unchanged.
 *
 * GPIO_UNIT_TEST selects host register mocks and simulated interrupt state.
 * Never define it for firmware. Host tests do not validate AVR timing.
 * Enabled alternate peripherals can override GPIO. The caller owns pin muxing,
 * JTAG settings, wiring and electrical limits. No voltage/current is measured.
 */

#include "gpio.h"
#include "Registers.h"

#if !defined(GPIO_UNIT_TEST)
#if !defined(__GNUC__) || !defined(__AVR__) || \
    (!defined(__AVR_ATmega32__) && !defined(__AVR_ATmega32A__))
#error "gpio.c requires AVR-GCC targeting ATmega32 or ATmega32A."
#endif
#endif

#define GPIO_PUD_MASK ((u8)0x04U)

/* Private: translate a public port selection into register addresses. */
typedef struct
{
    volatile u8 *ddr;
    volatile u8 *port;
    const volatile u8 *pin;
} GPIO_Registers_t;

static volt_status_t GPIO_GetRegisters(
    GPIO_Port_t port,
    GPIO_Registers_t *registers)
{
    switch (port)
    {
        case GPIO_PORT_A:
            registers->ddr = &DDRA;
            registers->port = &PORTA;
            registers->pin = &PINA;
            break;

        case GPIO_PORT_B:
            registers->ddr = &DDRB;
            registers->port = &PORTB;
            registers->pin = &PINB;
            break;

        case GPIO_PORT_C:
            registers->ddr = &DDRC;
            registers->port = &PORTC;
            registers->pin = &PINC;
            break;

        case GPIO_PORT_D:
            registers->ddr = &DDRD;
            registers->port = &PORTD;
            registers->pin = &PIND;
            break;

        default:
            return VOLT_INVALID_ARG;
    }

    return VOLT_OK;
}

static u8 GPIO_IsValidPin(GPIO_Pin_t pin)
{
    /* Casting also rejects negative enum values before any bit shift. */
    return (u8)((unsigned int)pin <= (unsigned int)GPIO_PIN_7);
}

static u8 GPIO_EnterCritical(void)
{
    u8 savedSreg;

#ifdef GPIO_UNIT_TEST
    savedSreg = SREG;
    SREG = (u8)(SREG & 0x7FU);
#else
    /* SREG has I/O address 0x3f, data-memory address 0x5f on this MCU. */
    __asm__ __volatile__(
        "in %0, 0x3f\n\tcli"
        : "=r" (savedSreg)
        :
        : "memory");
#endif

    return savedSreg;
}

static void GPIO_ExitCritical(u8 savedSreg)
{
#ifdef GPIO_UNIT_TEST
    SREG = savedSreg;
#else
    /* Restore the original state; do not unconditionally enable interrupts. */
    __asm__ __volatile__(
        "out 0x3f, %0"
        :
        : "r" (savedSreg)
        : "memory");
#endif
}

static void GPIO_SynchronizeInput(void)
{
#ifndef GPIO_UNIT_TEST
    /* Allow synchronization when reading back a software-assigned pin level. */
    __asm__ __volatile__("nop" : : : "memory");
#endif
}

volt_status_t GPIO_SetDirection(
    GPIO_Port_t port,
    GPIO_Pin_t pin,
    GPIO_Direction_t direction)
{
    GPIO_Registers_t registers;
    u8 mask;
    u8 savedSreg;

    if ((GPIO_IsValidPin(pin) == 0U) ||
        ((direction != GPIO_INPUT) && (direction != GPIO_OUTPUT)))
    {
        return VOLT_INVALID_ARG;
    }

    if (GPIO_GetRegisters(port, &registers) != VOLT_OK)
    {
        return VOLT_INVALID_ARG;
    }

    mask = (u8)(1U << (u8)pin);
    savedSreg = GPIO_EnterCritical();

    if (direction == GPIO_OUTPUT)
    {
        *registers.ddr = (u8)(*registers.ddr | mask);
    }
    else
    {
        *registers.ddr = (u8)(*registers.ddr & (u8)~mask);
    }

    GPIO_ExitCritical(savedSreg);
    return VOLT_OK;
}

volt_status_t GPIO_SetPullup(
    GPIO_Port_t port,
    GPIO_Pin_t pin,
    GPIO_Pullup_t pullup)
{
    GPIO_Registers_t registers;
    u8 mask;
    u8 savedSreg;

    if ((GPIO_IsValidPin(pin) == 0U) ||
        ((pullup != GPIO_PULLUP_DISABLED) && (pullup != GPIO_PULLUP_ENABLED)))
    {
        return VOLT_INVALID_ARG;
    }

    if (GPIO_GetRegisters(port, &registers) != VOLT_OK)
    {
        return VOLT_INVALID_ARG;
    }

    mask = (u8)(1U << (u8)pin);
    savedSreg = GPIO_EnterCritical();

    if (((*registers.ddr & mask) != 0U) ||
        ((pullup == GPIO_PULLUP_ENABLED) && ((SFIOR & GPIO_PUD_MASK) != 0U)))
    {
        GPIO_ExitCritical(savedSreg);
        return VOLT_NOT_READY;
    }

    if (pullup == GPIO_PULLUP_ENABLED)
    {
        *registers.port = (u8)(*registers.port | mask);
    }
    else
    {
        *registers.port = (u8)(*registers.port & (u8)~mask);
    }

    GPIO_ExitCritical(savedSreg);
    return VOLT_OK;
}

volt_status_t GPIO_WritePin(
    GPIO_Port_t port,
    GPIO_Pin_t pin,
    GPIO_Level_t level)
{
    GPIO_Registers_t registers;
    u8 mask;
    u8 savedSreg;

    if ((GPIO_IsValidPin(pin) == 0U) ||
        ((level != GPIO_LOW) && (level != GPIO_HIGH)))
    {
        return VOLT_INVALID_ARG;
    }

    if (GPIO_GetRegisters(port, &registers) != VOLT_OK)
    {
        return VOLT_INVALID_ARG;
    }

    mask = (u8)(1U << (u8)pin);
    savedSreg = GPIO_EnterCritical();

    if ((*registers.ddr & mask) == 0U)
    {
        GPIO_ExitCritical(savedSreg);
        return VOLT_NOT_READY;
    }

    if (level == GPIO_HIGH)
    {
        *registers.port = (u8)(*registers.port | mask);
    }
    else
    {
        *registers.port = (u8)(*registers.port & (u8)~mask);
    }

    GPIO_ExitCritical(savedSreg);
    return VOLT_OK;
}

volt_status_t GPIO_TogglePin(
    GPIO_Port_t port,
    GPIO_Pin_t pin)
{
    GPIO_Registers_t registers;
    u8 mask;
    u8 savedSreg;

    if (GPIO_IsValidPin(pin) == 0U)
    {
        return VOLT_INVALID_ARG;
    }

    if (GPIO_GetRegisters(port, &registers) != VOLT_OK)
    {
        return VOLT_INVALID_ARG;
    }

    mask = (u8)(1U << (u8)pin);
    savedSreg = GPIO_EnterCritical();

    if ((*registers.ddr & mask) == 0U)
    {
        GPIO_ExitCritical(savedSreg);
        return VOLT_NOT_READY;
    }

    /* ATmega32/A PINx is read-only; toggle the PORT latch. */
    *registers.port = (u8)(*registers.port ^ mask);

    GPIO_ExitCritical(savedSreg);
    return VOLT_OK;
}

volt_status_t GPIO_ReadPin(
    GPIO_Port_t port,
    GPIO_Pin_t pin,
    GPIO_Level_t *level)
{
    GPIO_Registers_t registers;

    if ((level == NULL_PTR) || (GPIO_IsValidPin(pin) == 0U))
    {
        return VOLT_INVALID_ARG;
    }

    if (GPIO_GetRegisters(port, &registers) != VOLT_OK)
    {
        return VOLT_INVALID_ARG;
    }

    GPIO_SynchronizeInput();
    *level = ((*registers.pin & (u8)(1U << (u8)pin)) != 0U)
        ? GPIO_HIGH : GPIO_LOW;

    return VOLT_OK;
}

volt_status_t GPIO_SetPortDirection(
    GPIO_Port_t port,
    u8 directionMask)
{
    GPIO_Registers_t registers;
    u8 savedSreg;

    if (GPIO_GetRegisters(port, &registers) != VOLT_OK)
    {
        return VOLT_INVALID_ARG;
    }

    savedSreg = GPIO_EnterCritical();
    *registers.ddr = directionMask;
    GPIO_ExitCritical(savedSreg);

    return VOLT_OK;
}

volt_status_t GPIO_WritePort(
    GPIO_Port_t port,
    u8 value)
{
    /* A full-port write is the special case with all eight bits selected. */
    return GPIO_WriteMaskedPort(port, 0xFFU, value);
}

volt_status_t GPIO_WriteMaskedPort(
    GPIO_Port_t port,
    u8 mask,
    u8 value)
{
    GPIO_Registers_t registers;
    u8 savedSreg;

    if (GPIO_GetRegisters(port, &registers) != VOLT_OK)
    {
        return VOLT_INVALID_ARG;
    }

    if (mask == 0U)
    {
        return VOLT_OK;
    }

    savedSreg = GPIO_EnterCritical();

    if ((*registers.ddr & mask) != mask)
    {
        GPIO_ExitCritical(savedSreg);
        return VOLT_NOT_READY;
    }

    *registers.port = (u8)((*registers.port & (u8)~mask) | (value & mask));

    GPIO_ExitCritical(savedSreg);
    return VOLT_OK;
}

volt_status_t GPIO_ReadPort(
    GPIO_Port_t port,
    u8 *value)
{
    GPIO_Registers_t registers;

    if (value == NULL_PTR)
    {
        return VOLT_INVALID_ARG;
    }

    if (GPIO_GetRegisters(port, &registers) != VOLT_OK)
    {
        return VOLT_INVALID_ARG;
    }

    GPIO_SynchronizeInput();
    *value = *registers.pin;

    return VOLT_OK;
}
