/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file common/align.h
 * @brief Datatypes and operations for handling memory alignment.
 */
#ifndef ALIGN_H
#define ALIGN_H

#include "common/inline.h"
#include "common/types.h"

/** @brief Type definition for an aligned memory range. */
typedef struct
{
    u64 offset;
    u64 size;
}
aligned_range_t;

/**
 * @brief Obtains an aligned representation of the provided operand.
 * 
 * @param operand Alignment operand.
 * @param granularity Alignment granularity.
 * @return An aligned representation of operand.
 */
INLINE
u64
aligned
(   const u64 operand
,   const u64 granularity
)
{
    return ( ( operand + ( granularity - 1 ) ) & ~( granularity - 1 ) );
}

/**
 * @brief Variant of aligned which takes as input and yields as output a memory
 * range.
 * 
 * @param operand Alignment operand.
 * @param size Operand bytesize.
 * @param granularity Alignment granularity.
 * @return An aligned representation of operand.
 */
INLINE
aligned_range_t
aligned_range
(   const u64 offset
,   const u64 size
,   const u64 granularity
)
{
    return ( aligned_range_t ){ aligned ( offset , granularity )
                              , aligned ( size , granularity )
                              };
}

#endif  // ALIGN_H