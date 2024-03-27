/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file memory/dynamic_allocator.h
 * @brief Provides an interface for a dynamic memory allocator data structure.
 */
#ifndef MEMORY_DYNAMIC_ALLOCATOR_H
#define MEMORY_DYNAMIC_ALLOCATOR_H

#include "common.h"

/** @brief Type declaration for a linear allocator. */
typedef void dynamic_allocator_t;

/**
 * @brief Initializes a dynamic allocator.
 * 
 * If pre-allocating a memory buffer:
 *   Call once to get the memory requirement; call a second time passing in a
 *   valid memory buffer of the required size.
 * 
 * If using implicit memory allocation:
 *   Uses dynamic memory allocation (see core/memory.h). Call
 *   dynamic_allocator_destroy to free.
 * 
 * @param capacity The requested capacity in bytes.
 * @param memory_requirement Output buffer to hold the actual number of bytes
 * required to operate the dynamic allocator. Only applicable if pre-allocating
 * a memory buffer of the required size. Pass 0 to use implicit memory
 * allocation.
 * @param memory Optional pre-allocated memory buffer. Only applicable if
 * memory_requirement is non-zero. Pass 0 to read memory requirement; otherwise,
 * pass a pre-allocated buffer of the required size.
 * @param allocator Output buffer for allocator.
 * @return true on success; false otherwise.
 */
bool
dynamic_allocator_create
(   u64                     capacity
,   u64*                    memory_requirement
,   void*                   memory
,   dynamic_allocator_t**   allocator
);

/**
 * @brief Frees the memory used by a dynamic allocator.
 * 
 * If the allocator was not pre-allocated, this function will free the memory
 * implicitly (see core/memory.h).
 * 
 * @param allocator Handle to the allocator to free.
 */
void
dynamic_allocator_destroy
(   dynamic_allocator_t** allocator
);


/**
 * @brief Queries the capacity of a dynamic allocator in bytes.
 * 
 * @param allocator The allocator to query. Must be non-zero.
 * @return The allocator capacity in bytes.
 */
u64
dynamic_allocator_capacity
(   const dynamic_allocator_t* allocator
);

/**
 * @brief Queries whether a dynamic allocator was created with implicit memory
 * allocation.
 * 
 * @param allocator The allocator to query. Must be non-zero.
 * @return true if allocator was created with implicit memory allocation; false
 * otherwise.
 */
bool
dynamic_allocator_owns_memory
(   const dynamic_allocator_t* allocator
);

/**
 * @brief Allocates memory using a dynamic allocator.
 * 
 * @param allocator The allocator to mutate. Must be non-zero.
 * @param size The number of bytes to allocate. Must be non-zero.
 * @return The address of the allocated memory, on success.
 * 0, on error.
 */
void*
dynamic_allocator_allocate
(   dynamic_allocator_t*    allocator
,   u64                     size
);

/**
 * @brief Aligned variant of dynamic_allocator_allocate.
 * (see dynamic_allocator_allocate)
 * 
 * @param allocator The allocator to mutate. Must be non-zero.
 * @param size The number of bytes to allocate. Must be non-zero.
 * @param alignment Memory alignment.
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
 * @param allocator The allocator to mutate.
 * @param memory The memory block to free. Must be non-zero.
 * @return true on success; false otherwise.
 */
bool
dynamic_allocator_free
(   dynamic_allocator_t*    allocator
,   void*                   memory
);

/**
 * @brief Aligned variant of dynamic_allocator_free.
 * (see dynamic_allocator_free)
 * 
 * @param allocator The allocator to mutate. Must be non-zero.
 * @param memory The memory block to free. Must be non-zero.
 * @return true on success; false otherwise.
 */
bool
dynamic_allocator_free_aligned
(   dynamic_allocator_t*    allocator
,   void*                   memory
);

/**
 * @brief Computes the size and alignment of the given block of memory.
 * 
 * Fails if invalid data is passed.
 * 
 * @param memory The block of memory. Must be non-zero.
 * @param size A buffer to hold the size. Must be non-zero.
 * @param alignment A buffer to hold the alignment. Must be non-zero.
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
 * @param allocator The allocator to query. Must be non-zero.
 * @return The number of free bytes of space remaining.
 */
u64
dynamic_allocator_query_free
(   const dynamic_allocator_t* allocator
);

/**
 * @brief Computes the header size of a dynamic allocator's internal data
 * structure.
 * 
 * This function is useful for unit testing.
 * 
 * @return The header size of an allocator data structure (in bytes).
 */
u64
dynamic_allocator_header_size
( void );

#endif  // MEMORY_DYNAMIC_ALLOCATOR_H