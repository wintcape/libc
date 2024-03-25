/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file core/bitv.h
 * @brief Functions which implement bit manipulation operations on bit vectors
 * of arbitrary length.
 */
#ifndef BITV_H
#define BITV_H

#include "core/memory.h"

/**
 * @brief Reads a bit from a bit vector.
 * 
 * @param bitv The bit vector. Must be non-zero.
 * @param n The bit to read.
 * @return true if bit n of bitv set; false otherwise.
 */
INLINE
bool
bitv_bit
(   const void* bitv
,   const u64   n
)
{
    const u64 byte = n / 8;
    const u64 bit = n % 8;
    return BIT ( ( ( const u8* ) bitv )[ byte ] , bit );
}

/**
 * @brief Sets a bit within a bit vector.
 * 
 * @param bitv The bit vector. Must be non-zero.
 * @param n The bit to set.
 * @return bitv with bit n set.
 */
INLINE
void*
bitv_set
(   void*       bitv
,   const u64   n
)
{
    const u64 byte = n / 8;
    const u64 bit = n % 8;
    BITSET ( ( ( u8* ) bitv )[ byte ] , bit );
    return bitv;
}

/**
 * @brief Clears a bit within a bit vector.
 * 
 * @param bitv The bit vector. Must be non-zero.
 * @param n The bit to clear.
 * @return bitv with bit n cleared.
 */
INLINE
void*
bitv_clr
(   void*       bitv
,   const u64   n
)
{
    const u64 byte = n / 8;
    const u64 bit = n % 8;
    BITCLR ( ( ( u8* ) bitv )[ byte ] , bit );
    return bitv;
}

/**
 * @brief Toggles a bit within a bit vector.
 * 
 * @param bitv A bit vector. Must be non-zero.
 * @param n The bit to toggle.
 * @return bitv with bit n toggled.
 */
INLINE
void*
bitv_swp
(   void*       bitv
,   const u64   n
)
{
    const u64 byte = n / 8;
    const u64 bit = n % 8;
    BITSWP ( ( ( u8* ) bitv )[ byte ] , bit );
    return bitv;
}

/**
 * @brief Sets every bit within a bit vector.
 * 
 * @param bitv A bit vector. Must be non-zero.
 * @param length The number of bits in the vector.
 * @return bitv with every bit set.
 */
INLINE
void*
bitv_set_all
(   void*       bitv
,   const u64   length
)
{
    if ( !length )
    {
        return bitv;
    }
    const u64 byte = length / 8;
    const u64 bit = length % 8;
    memory_set ( bitv , -1 , byte );
    for ( u64 i = 0; i < bit; ++i )
    {
        BITSET ( ( ( u8* ) bitv )[ byte ] , i );
    }
    return bitv;
}

/**
 * @brief Clears every bit within a bit vector.
 * 
 * @param bitv A bit vector. Must be non-zero.
 * @param length The number of bits in the vector.
 * @return bitv with every bit cleared.
 */
INLINE
void*
bitv_clr_all
(   void*       bitv
,   const u64   length
)
{
    if ( !length )
    {
        return bitv;
    }
    const u64 byte = length / 8;
    const u64 bit = length % 8;
    memory_set ( bitv , 0 , byte );
    for ( u64 i = 0; i < bit; ++i )
    {
        BITCLR ( ( ( u8* ) bitv )[ byte ] , i );
    }
    return bitv;
}

/**
 * @brief Toggles every bit within a bit vector.
 * 
 * @param bitv A bit vector. Must be non-zero.
 * @param length The number of bits in the vector.
 * @return bitv with every bit toggled.
 */
INLINE
void*
bitv_swp_all
(   void*       bitv
,   const u64   length
)
{
    if ( !length )
    {
        return bitv;
    }
    const u64 byte = length / 8;
    const u64 bit = length % 8;
    for ( u64 i = 0; i < byte; ++i )
    {
        ( ( u8* ) bitv )[ i ] = ~( ( ( u8* ) bitv )[ i ] );
    }
    for ( u64 i = 0; i < bit; ++i )
    {
        BITSWP ( ( ( u8* ) bitv )[ byte ] , i );
    }
    return bitv;
}

#endif  // BITV_H