/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file math/root.h
 * @brief Defines n-th root operations.
 */
#ifndef MATH_ROOT_H
#define MATH_ROOT_H

#include "common.h"

// Global constants.
#define SQRT2           1.41421356237309504880f /** @brief sqrt(2). */
#define SQRT3           1.73205080756887729352f /** @brief sqrt(3). */
#define DIV_SQRT2       0.70710678118654752440f /** @brief 1 / sqrt(2). */
#define DIV_SQRT3       0.57735026918962576450f /** @brief 1 / sqrt(3). */

/**
 * @brief Defines an alias for the square root function. This resolves a name
 * conflict with the square root function included by the standard libc headers.
 */
#define sqrt(X) _sqrt ( X )

/**
 * @brief Square root function.
 * 
 * @param x A floating point number.
 * @return sqrt(x)
 */
f32
_sqrt
(   f32 x
);

#endif  // MATH_ROOT_H