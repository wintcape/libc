/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file math/math.h
 * @brief Datatypes and operations for performing geometric operations.
 */
#ifndef MATH_H
#define MATH_H

#include "common.h"

#include "math/conversion.h"
#include "math/div.h"
#include "math/float.h"
#include "math/random.h"
#include "math/trig.h"

/**
 * @brief Computes the minimum of two numeric values.
 * 
 * @param a A numeric value.
 * @param b A numeric value.
 * @return The minimum of a and b.
 */
#define MIN(a,b)                  \
    ({                            \
        __typeof__ (a) a__ = (a); \
        __typeof__ (b) b__ = (b); \
       ( a__ < b__ ) ? a__ : b__; \
    })

/**
 * @brief Computes the maximum of two integer values.
 * 
 * @param a A numeric value.
 * @param b A numeric value.
 * @return The maximum of a and b.
 */
#define MAX(a,b)                  \
    ({                            \
        __typeof__ (a) a__ = (a); \
        __typeof__ (b) b__ = (b); \
       ( a__ > b__ ) ? a__ : b__; \
    })

/**
 * @brief Clamps a numeric value between an upper and lower bound (inclusive).
 * 
 * @param value The value to be clamped.
 * @param min Lower bound (inclusive).
 * @param max Upper bound (inclusive).
 * @return The clamped value.
 */
#define CLAMP(value,min,max)                                                   \
    ({                                                                         \
        __typeof__ (value) value__ = (value);                                  \
        __typeof__ (min) min__ = (min);                                        \
        __typeof__ (max) max__ = (max);                                        \
        ( value__ <= min__ ) ? min__ : ( value__ >= max__ ) ? max__ : value__; \
    })

#endif  // MATH_H