/**
 * @file bit_math.h
 * @brief Set, clear, toggle and read individual bits.
 * @version 0.1.0
 *
 * No MCU headers, register addresses or external libraries are required.
 *
 * Contract:
 * - REG must have an unsigned integer type of 8, 16 or 32 bits.
 * - Modifying operations require a writable variable or register (an lvalue).
 * - BIT is a zero-based position: 0 <= BIT < the width of REG, at most 31.
 * - Invalid positions are a caller error; these macros do not check them.
 * - Each argument expression is evaluated once. GET_BIT returns 0UL or 1UL.
 *
 * Register use:
 * - Use only with ordinary read/write registers whose datasheet permits a
 *   read-modify-write operation. Special registers (for example, write-one-
 *   to-clear flags) require their documented access sequence instead.
 * - These operations are not guaranteed atomic. Interrupt/shared-access
 *   protection belongs in the MCU-specific driver. Volatile is not a lock.
 * - GET_BIT reads REG; use it only when reading the register is permitted.
 */

#ifndef VOLT_BIT_MATH_H
#define VOLT_BIT_MATH_H

/* Set the selected bit to 1 and preserve all other bits. */
#define SET_BIT(REG, BIT)                  \
    do                                   \
    {                                    \
        (REG) |= (1UL << (BIT));          \
    } while (0)

/* Clear the selected bit to 0 and preserve all other bits. */
#define CLEAR_BIT(REG, BIT)                \
    do                                   \
    {                                    \
        (REG) &= ~(1UL << (BIT));         \
    } while (0)

/* Invert the selected bit and preserve all other bits. */
#define TOGGLE_BIT(REG, BIT)               \
    do                                   \
    {                                    \
        (REG) ^= (1UL << (BIT));          \
    } while (0)

/* Read the selected bit as 0UL or 1UL without modifying REG. */
#define GET_BIT(REG, BIT) (((REG) >> (BIT)) & 1UL)

#endif /* VOLT_BIT_MATH_H */
