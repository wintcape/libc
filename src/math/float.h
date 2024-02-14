/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file math/float.h
 * @brief Defines miscellaneous floating point constants and functions.
 */
#ifndef MATH_FLOAT_H
#define MATH_FLOAT_H

#include "common.h"

// Global constants.
#define INFINITY        ( 1e30f * 1e30f ) /** @brief Placeholder value for floating point overflow. */
#define FLOAT_EPSILON   1.192092896e-07f  /** @brief Smallest possible non-zero floating point number. */

/**
 * @brief Defines an alias for the absolute value function. This avoids function
 * signatures which cause name conflicts with those included by the standard
 * libc headers.
 */
#define abs(X) ( _abs ( X ) )

/**
 * @brief Absolute value function.
 * 
 * @param x A floating point number.
 * @return abs(x)
 */
f32
_abs
(   f32 x
);

#endif  // MATH_FLOAT_H