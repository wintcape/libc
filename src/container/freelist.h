/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/freelist.h
 * @brief Provides an interface for a freelist data structure.
 */
#ifndef FREELIST_H
#define FREELIST_H

#include "common.h"

/** @brief Type declaration for a freelist. */
typedef void freelist_t;

/**
 * @brief Initializes a freelist.
 * 
 * If pre-allocating a memory buffer:
 *   Call once to get the memory requirement; call a second time passing in a
 *   valid memory buffer of the required size.
 * 
 * If using implicit memory allocation:
 *   Uses dynamic memory allocation (see core/memory.h). Call freelist_destroy
 *   to free.
 * 
 * @param capacity The requested capacity in bytes.
 * @param memory_requirement Output buffer to hold the actual number of bytes
 * required to operate the freelist. Only applicable if pre-allocating a memory
 * buffer of the required size. Pass 0 to use implicit memory allocation.
 * @param memory Optional pre-allocated memory buffer. Only applicable if
 * memory is being pre-allocated. Pass 0 to read memory requirement; otherwise,
 * pass a pre-allocated buffer of the required size.
 * @param freelist Output buffer for freelist.
 * @return true on success; false otherwise.
 */
bool
freelist_create
(   u64             capacity
,   u64*            memory_requirement
,   void*           memory
,   freelist_t**    freelist
);

/**
 * @brief Frees the memory used by a freelist.
 * 
 * If the freelist was not pre-allocated, this function will free the memory
 * implicitly (see core/memory.h).
 * 
 * @param freelist Handle to the freelist to free.
 */
void
freelist_destroy
(   freelist_t** freelist
);

/**
 * @brief Queries whether a freelist was created with implicit memory
 * allocation.
 * 
 * @param freelist The freelist to query. Must be non-zero.
 * @return true if freelist was created with implicit memory allocation; false
 * otherwise.
 */
bool
freelist_owns_memory
(   const freelist_t* freelist
);

/**
 * @brief Allocates a memory block within a freelist.
 * 
 * @param freelist The freelist to mutate. Must be non-zero.
 * @param size The block size. Must be non-zero.
 * @param offset Output buffer for block offset. Must be non-zero.
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
 * @param freelist The freelist to mutate. Must be non-zero.
 * @param size The block size. Must be non-zero.
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
 * If pre-allocating a memory buffer:
 *   Call once to get the memory requirement; call a second time passing in a
 *   valid memory buffer of the required size.
 * 
 * If using implicit memory allocation:
 *   Uses dynamic memory allocation (see core/memory.h). Call freelist_destroy
 *   to free.
 * 
 * @param freelist Handle to the freelist to resize.
 * @param memory_requirement Output buffer to hold the actual number of bytes
 * required to operate the freelist.
 * @param minimum_capacity The minimum capacity in bytes the freelist is
 * required to hold.
 * @param memory_requirement Output buffer to hold the actual number of bytes
 * required to operate the dynamic allocator. Only applicable if pre-allocating
 * a memory buffer of the required size. Pass 0 to use implicit memory
 * allocation.
 * @param new_memory Optional pre-allocated memory buffer. Only applicable if
 * memory is being pre-allocated. Pass 0 to read memory requirement; otherwise,
 * pass a pre-allocated buffer of the required size.
 * @param old_memory Output buffer for the old pre-allocated memory buffer. Only
 * applicable if both memory_requirement and new_memory arguments are non-zero.
 * @return true on success; false otherwise.
 */
bool
freelist_resize
(   freelist_t**    freelist
,   u64             minimum_capacity
,   u64*            memory_requirement
,   void*           new_memory
,   void**          old_memory
);

/**
 * @brief Resets a freelist to its empty state, freeing and clearing all of its
 * memory.
 * 
 * @param freelist The freelist to reset. Must be non-zero.
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
 * @param freelist The freelist to query. Must be non-zero.
 * @return The number of bytes of free space remaining within freelist.
 */
u64
freelist_query_free
(   freelist_t* freelist
);

#endif  // FREELIST_H