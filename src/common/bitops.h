/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file common/bitops.h
 * @brief Functions and preprocessor bindings which implement bit manipulation
 * operations.
 */
#ifndef BITOPS_H
#define BITOPS_H

#include "common/inline.h"
#include "common/types.h"

// Internal preprocessor bindings.
// Use inline functions instead for type safety.

#define U64_1       ( ( u64 ) 1 )
#define BIT(x,n)    ( ( ( ( u64 )(x) ) >> (n) ) & U64_1 ) /** @brief Internal preprocessor binding (use inline function instead for type safety). */
#define BITSET(x,n) ( (x) |= ( U64_1 << (n) ) )           /** @brief Internal preprocessor binding (use inline function instead for type safety). */
#define BITCLR(x,n) ( (x) &= ~( U64_1 << (n) ) )          /** @brief Internal preprocessor binding (use inline function instead for type safety). */
#define BITSWP(x,n) ( (x) ^= ( U64_1 << (n) ) )           /** @brief Internal preprocessor binding (use inline function instead for type safety). */

/**
 * @brief Reads a bit from a bit vector.
 * 
 * @param x A bit vector.
 * @param n The bit to read.
 * @return true if bit n of x set; false otherwise.
 */
INLINE
bool
bit
(   const u64   x
,   const u8    n
)
{
    return BIT ( x , n );
}

/**
 * @brief Sets a bit within a bit vector.
 * 
 * @param x A bit vector.
 * @param n The bit to set.
 * @return x with bit n set.
 */
INLINE
u64
bitset
(   u64         x
,   const u8    n
)
{
    return BITSET ( x , n );
}

/**
 * @brief Clears a bit within a bit vector.
 * 
 * @param x A bit vector.
 * @param n The bit to clear.
 * @return x with bit n cleared.
 */
INLINE
u64
bitclr
(   u64         x
,   const u8    n
)
{
    return BITCLR ( x , n );
}

/**
 * @brief Toggles a bit within a bit vector.
 * 
 * @param x A bit vector.
 * @param n The bit to toggle.
 * @return x with bit n toggled.
 */
INLINE
u64
bitswp
(   u64         x
,   const u8    n
)
{
    return BITSWP ( x , n );
}

#endif  // BITOPS_H