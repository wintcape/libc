/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file memory/linear_allocator.h
 * @brief Provides an interface for a linear memory allocator data structure.
 */
#ifndef MEMORY_LINEAR_ALLOCATOR_H
#define MEMORY_LINEAR_ALLOCATOR_H

#include "common.h"

/** @brief Type declaration for a linear allocator. */
typedef void linear_allocator_t;

/**
 * @brief Initializes a freelist.
 * 
 * If pre-allocating a memory buffer:
 *   Call once to get the memory requirement; call a second time passing in a
 *   valid memory buffer of the required size.
 * 
 * If using implicit memory allocation:
 *   Uses dynamic memory allocation (see core/memory.h). Call
 *   linear_allocator_destroy to free.
 * 
 * @param capacity The requested capacity in bytes.
 * @param memory_requirement Output buffer to hold the actual number of bytes
 * required to operate the allocator. Only applicable if pre-allocating a memory
 * buffer of the required size. Pass 0 to use implicit memory allocation.
 * @param memory Optional pre-allocated memory buffer. Only applicable if
 * memory is being pre-allocated. Pass 0 to read memory requirement; otherwise,
 * pass a pre-allocated buffer of the required size.
 * @param allocator Output buffer for allocator.
 * @return true on success; false otherwise.
 */
bool
linear_allocator_create
(   u64                     capacity
,   u64*                    memory_requirement
,   void*                   memory
,   linear_allocator_t**    allocator
);

/**
 * @brief Frees the memory used by a linear allocator.
 * 
 * If the allocator was not pre-allocated, this function will free the memory
 * implicitly (see core/memory.h).
 * 
 * @param allocator Handle to the allocator to free.
 */
void
linear_allocator_destroy
(   linear_allocator_t** allocator
);

/**
 * @brief Queries the number of bytes currently allocated by a linear allocator.
 * 
 * @param allocator The allocator to query. Must be non-zero.
 * @return The number of bytes currently allocated by allocator.
 */
u64
linear_allocator_allocated
(   const linear_allocator_t* allocator
);

/**
 * @brief Queries the capacity of a linear allocator in bytes.
 * 
 * @param allocator The allocator to query. Must be non-zero.
 * @return The allocator capacity in bytes.
 */
u64
linear_allocator_capacity
(   const linear_allocator_t* allocator
);

/**
 * @brief Queries whether a linear allocator was created with implicit memory
 * allocation.
 * 
 * @param allocator The allocator to query. Must be non-zero.
 * @return true if freelist was created with implicit memory allocation; false
 * otherwise.
 */
bool
linear_allocator_owns_memory
(   const linear_allocator_t* allocator
);

/**
 * @brief Allocates memory using a linear allocator.
 * 
 * @param allocator The allocator to mutate. Must be non-zero.
 * @param size The number of bytes to allocate. Must be non-zero.
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
 * @param allocator The allocator to mutate. Must be non-zero.
 * @return true on success; false otherwise.
 */
bool
linear_allocator_free
(   linear_allocator_t* allocator
);

#endif  // MEMORY_LINEAR_ALLOCATOR_H