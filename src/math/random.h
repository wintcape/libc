/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file math/random.h
 * @brief Random number generator functions.
 */
#ifndef MATH_RANDOM_H
#define MATH_RANDOM_H

#include "common.h"

/**
 * @brief Defines an alias for the random function. This avoids function
 * signatures which cause name conflicts with those included by the standard
 * libc headers.
 */
#define random()  ( _random () )

/**
 * @brief Generates a random integer.
 * 
 * @return A random integer.
 */
i32
_random
( void );

/**
 * @brief Generates a random integer in the specified range.
 * 
 * @param min lower bound (inclusive)
 * @param max upper bound (inclusive)
 * @return A random integer in the range [ min , max ].
 */
i32
random2
(   i32 min
,   i32 max
);

/**
 * @brief Generates a random 64-bit integer.
 * 
 * @return A random 64-bit integer.
 */
i64
random64
( void );

/**
 * @brief Generates a random floating point number.
 * 
 * @return A random floating point number.
 */
f32
randomf
( void );

/**
 * @brief Generates a random floating point number in the specified range.
 * 
 * @param min lower bound (inclusive)
 * @param max upper bound (inclusive)
 * @return A random floating point number in the range [ min , max ].
 */
f32
randomf2
(   f32 min
,   f32 max
);

#endif  // MATH_RANDOM_H