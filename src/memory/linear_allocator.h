/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file memory/linear_allocator.h
 * @brief Provides an interface for a linear memory allocator data structure.
 */
#ifndef MEMORY_LINEAR_ALLOCATOR_H
#define MEMORY_LINEAR_ALLOCATOR_H

#include "common.h"

/** @brief Type definition for a linear allocator. */
typedef struct
{
    u64     capacity;
    u64     allocated;
    void*   memory;
    bool    owns_memory;
}
linear_allocator_t;

/**
 * @brief Initializes a linear allocator.
 * 
 * If the capacity is preallocated, the starting memory index should be passed
 * to this function; otherwise, this function will use implicit memory
 * allocation (see core/memory.h) and will need to be freed via
 * linear_allocator_destroy.
 * 
 * @param capacity The capacity in bytes.
 * @param memory Starting memory index (optional). Pass 0 to use implicit memory
 * allocation.
 * @param allocator Output buffer.
 * @return true on success; false otherwise.
 */
bool
linear_allocator_create
(   u64                 capacity
,   void*               memory
,   linear_allocator_t* allocator
);

/**
 * @brief Frees the memory used by a linear allocator.
 * 
 * If the linear allocator was not preallocated, this function will free the
 * memory implicitly (see core/memory.h).
 * 
 * @param allocator The allocator to free.
 */
void
linear_allocator_destroy
(   linear_allocator_t* allocator
);

/**
 * @brief Allocates memory using a linear allocator.
 * 
 * @param allocator The allocator.
 * @param size The number of bytes to allocate.
 * @return The address of the allocated memory, on success. 0, on error.
 */
void*
linear_allocator_allocate
(   linear_allocator_t* allocator
,   u64                 size
);

/**
 * @brief Clears all memory currently being managed by a linear allocator.
 * 
 * @param allocator The allocator.
 * @return true on success; false otherwise.
 */
bool
linear_allocator_free
(   linear_allocator_t* allocator
);

#endif  // MEMORY_LINEAR_ALLOCATOR_H