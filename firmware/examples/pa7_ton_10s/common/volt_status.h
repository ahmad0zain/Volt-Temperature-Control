/**
 * @file volt_status.h
 * @brief Shared operation results for the Volt embedded code library.
 * @version 0.1.0
 *
 * No MCU headers or external libraries are required.
 * Compare results explicitly with VOLT_OK before using output data.
 * Each module must document which statuses it can return and whether an
 * operation may have partially completed before an error was reported.
 *
 * Read API convention: on failure, leave the caller's output value unchanged
 * unless the individual API explicitly documents a different behaviour.
 * An old output value must not be treated as a successful new reading.
 *
 * The enum's storage size is compiler-dependent. Do not assume a fixed byte
 * size or serialize its raw memory representation for communications/storage.
 */

#ifndef VOLT_STATUS_H
#define VOLT_STATUS_H

typedef enum
{
    VOLT_OK           = 0, /* Operation completed successfully. */
    VOLT_INVALID_ARG  = 1, /* Invalid argument, pointer or configuration. */
    VOLT_TIMEOUT      = 2, /* Operation did not finish within its deadline. */
    VOLT_NOT_READY    = 3, /* Module or requested data is not ready. */
    VOLT_OUT_OF_RANGE = 4  /* Measured/result value is outside supported limits. */
} volt_status_t;

#endif /* VOLT_STATUS_H */
