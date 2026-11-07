/**
 * @file StdTypes.h
 * @brief Shared data types and legacy return codes for the Volt code library.
 * @version 0.1.0
 *
 * Uses the ISO C99 integer type definitions, not MCU-specific headers.
 * Integer aliases have the stated exact widths on supported toolchains.
 * Floating-point aliases retain their original native C types: their names
 * do not guarantee 32, 64 or 128 bit storage. Check the target compiler.
 *
 * StdReturnType is retained for existing drivers. New APIs should use
 * volt_status_t from volt_status.h when detailed error reporting is needed.
 * NULL_PTR is an object null pointer for C code.
 */

#ifndef VOLT_STD_TYPES_H
#define VOLT_STD_TYPES_H

#include <stdint.h>

/* Unsigned and signed integer aliases. */
typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint64_t u64;
typedef int64_t  s64;

/* Legacy floating-point aliases; actual widths are compiler-dependent. */
typedef float       f32;
typedef double      f64;
typedef long double f128;

/* Legacy success/failure result; enum storage size is compiler-dependent. */
typedef enum
{
    E_OK     = 0, /* Operation completed successfully. */
    E_NOT_OK = 1  /* Operation failed; no detailed cause is provided. */
} StdReturnType;

#define NULL_PTR ((void *)0)

#endif /* VOLT_STD_TYPES_H */

