/**
 * @file Macros.h
 * @brief Compatibility names for the Volt common bit operations.
 * @version 0.1.0
 * @author ahmad
 *
 * Originally created on January 27, 2023.
 * Depends on our own bit_math.h. SET_BIT and GET_BIT are provided by that file.
 * CLR_BIT and TOG_BIT retain the original driver interface as aliases.
 * New code may use CLEAR_BIT and TOGGLE_BIT directly.
 *
 * All argument, width, register-access and non-atomicity restrictions of
 * bit_math.h apply. This file does not duplicate the macro implementations.
 */

#ifndef VOLT_MACROS_H
#define VOLT_MACROS_H

#include "bit_math.h"

#define CLR_BIT(REG, BIT) CLEAR_BIT((REG), (BIT))
#define TOG_BIT(REG, BIT) TOGGLE_BIT((REG), (BIT))

#endif /* VOLT_MACROS_H */

