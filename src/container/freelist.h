/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/freelist.h
 * @brief Provides an interface for a freelist data structure.
 */
#ifndef FREELIST_H
#define FREELIST_H

#include "common.h"

/** @brief Type definition for a freelist data structure. */
typedef struct
{
    void* memory;
}
freelist_t;

/**
 * @brief Initializes a freelist.
 * 
 * Call once to get the memory requirement; call a second time passing in a
 * valid memory buffer of the required size.
 * 
 * @param capacity The requested capacity in bytes.
 * @param memory_requirement Output buffer to hold the actual number of bytes
 * required to operate the freelist.
 * @param memory Pass 0 to read memory requirement. Otherwise, pass a buffer.
 * @param freelist Output buffer.
 * @return true on success; false otherwise.
 */
bool
freelist_init
(   u64         capacity
,   u64*        memory_requirement
,   void*       memory
,   freelist_t* freelist
);

/**
 * @brief Clears the memory used by a freelist.
 * 
 * @param freelist The freelist to clear.
 */
void
freelist_clear
(   freelist_t* freelist
);

/**
 * @brief Allocates a memory block within a freelist.
 * 
 * @param freelist The freelist to mutate.
 * @param size The block size.
 * @param offset Output buffer for block offset.
 * @return true on success; false otherwise.
 */
bool
freelist_allocate
(   freelist_t* freelist
,   u64         size
,   u64*        offset
);

/**
 * @brief Frees an existing memory block within a freelist.
 * 
 * @param freelist The freelist to mutate.
 * @param size The block size.
 * @param offset The block offset.
 * @return true on success; false otherwise.
 */
bool
freelist_free
(   freelist_t* freelist
,   u64         size
,   u64         offset
);

/**
 * @brief Resizes a freelist to accomodate a new maximum capacity.
 *
 * Call once to get the memory requirement; call a second time passing in a
 * valid memory buffer of the required size.
 * 
 * @param freelist The freelist to resize.
 * @param memory_requirement Output buffer to hold the actual number of bytes
 * required to operate the freelist.
 * @param new_memory The starting memory address of the resized freelist.
 * @param new_capacity The requested new capacity in bytes.
 * @param old_memory Output buffer to hold the handle to the starting memory
 * address of the freelist prior to resizing.
 * @return true on success; false otherwise.
 */
bool
freelist_resize
(   freelist_t* freelist
,   u64*        memory_requirement
,   void*       new_memory
,   u64         new_capacity
,   void**      old_memory
);

/**
 * @brief Resets a freelist to its empty state, freeing and clearing all of its
 * memory.
 * 
 * @param freelist The freelist to reset.
 */
void
freelist_reset
(   freelist_t* freelist
);

/**
 * @brief Queries the amount of free space remaining within a freelist.
 * 
 * This function is expensive and should be used sparingly, but it is useful for
 * unit testing.
 * 
 * @param freelist The freelist to query.
 * @return The number of bytes of free space remaining within freelist.
 */
u64
freelist_query_free
(   freelist_t* freelist
);

#endif  // FREELIST_H