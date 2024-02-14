/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file memory/dynamic_allocator.h
 * @brief Provides an interface for a dynamic memory allocator data structure.
 */
#ifndef MEMORY_DYNAMIC_ALLOCATOR_H
#define MEMORY_DYNAMIC_ALLOCATOR_H

#include "common.h"

/** @brief Type definition for a dynamic allocator. */
typedef struct
{
    void* memory;
}
dynamic_allocator_t;

/**
 * @brief Initializes a dynamic allocator.
 * 
 * Call once to get the memory requirement; call a second time passing in a
 * valid memory buffer of the required size.
 * 
 * @param capacity The requested capacity in bytes.
 * @param memory_requirement Output buffer to hold the actual number of bytes
 * required to operate the dynamic allocator.
 * @param memory Pass 0 to read memory requirement. Otherwise, pass a buffer.
 * @param allocator Output buffer.
 * @return true on success; false otherwise.
 */
bool
dynamic_allocator_init
(   u64                     capacity
,   u64*                    memory_requirement
,   void*                   memory
,   dynamic_allocator_t*    allocator
);

/**
 * @brief Clears the memory used by a dynamic allocator.
 * 
 * @param allocator The allocator to clear.
 */
void
dynamic_allocator_clear
(   dynamic_allocator_t* allocator
);

/**
 * @brief Allocates memory using a dynamic allocator.
 * 
 * @param allocator The allocator.
 * @param size The number of bytes to allocate.
 * @return The address of the allocated memory, on success.
 * 0, on error.
 */
void*
dynamic_allocator_allocate
(   dynamic_allocator_t*    allocator
,   u64                     size
);

/**
 * @brief Allocates memory using a dynamic allocator.
 * 
 * @param allocator The allocator.
 * @param size The number of bytes to allocate.
 * @return The address of the allocated memory, on success.
 *         0, on error.
 */
void*
dynamic_allocator_allocate_aligned
(   dynamic_allocator_t*    allocator
,   u64                     size
,   u16                     alignment
);

/**
 * @brief Frees a single block of memory previously allocated by a dynamic
 * allocator.
 * 
 * @param allocator The allocator.
 * @param memory The memory block to free.
 * @return true on success; false otherwise.
 */
bool
dynamic_allocator_free
(   dynamic_allocator_t*    allocator
,   void*                   memory
);

/**
 * @brief Frees a single block of memory previously allocated by a dynamic
 * allocator.
 * 
 * @param allocator The allocator.
 * @param memory The memory block to free.
 * @return true on success; false otherwise.
 */
bool
dynamic_allocator_free_aligned
(   dynamic_allocator_t*    allocator
,   void*                   memory
);

/**
 * @brief Computes the size and alignment of the given block of memory. Fails if
 * invalid data is passed.
 * 
 * @param memory The block of memory.
 * @param size A buffer to hold the size.
 * @param alignment A buffer to hold the alignment.
 * @return true on success; false otherwise.
 */
bool
dynamic_allocator_size_alignment
(   void*   memory
,   u64*    size
,   u16*    alignment
);

/**
 * @brief Query free space remaining for an allocator.
 * 
 * This function is expensive and should be used sparingly, but it is useful for
 * unit testing.
 * 
 * @param allocator The allocator.
 * @return The number of free bytes of space remaining.
 */
u64
dynamic_allocator_query_free
(   const dynamic_allocator_t* allocator
);

/**
 * @brief Computes the header size of the allocator data structure.
 * 
 * This function is useful for unit testing.
 * 
 * @return The header size of the allocator data structure (in bytes).
 */
u64
dynamic_allocator_header_size
( void );

#endif  // MEMORY_DYNAMIC_ALLOCATOR_H