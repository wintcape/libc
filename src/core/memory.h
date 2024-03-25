/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file core/memory.h
 * @brief Provides an interface for a thread-safe application subsystem which
 * handles memory allocation and management.
 */
#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"

/** @brief Type and instance definitions for memory allocation tags. */
typedef enum
{
    MEMORY_TAG_UNKNOWN
,   MEMORY_TAG_ARRAY
,   MEMORY_TAG_STRING
,   MEMORY_TAG_HASHTABLE
,   MEMORY_TAG_QUEUE
,   MEMORY_TAG_LINEAR_ALLOCATOR
,   MEMORY_TAG_DYNAMIC_ALLOCATOR
,   MEMORY_TAG_THREAD
,   MEMORY_TAG_MUTEX
,   MEMORY_TAG_FILE
,   MEMORY_TAG_APPLICATION

,   MEMORY_TAG_COUNT
}
MEMORY_TAG;

/** @brief (see memory_amount_allocated). */
#define MEMORY_TAG_ALL MEMORY_TAG_COUNT

/** @brief Computes current global number of unfreed allocations. */
#define MEMORY_ALLOCATION_COUNT \
    ( memory_allocation_count () - memory_free_count () )

/**
 * @brief Initializes the memory subsystem.
 * 
 * Requests a large sandbox of memory from the host platform and runs all
 * dynamic memory allocation and release processes from within it.
 * 
 * When the application no longer needs a dynamic memory management subsystem,
 * the host platform should be signaled to free the sandbox via memory_shutdown.
 * 
 * @param capacity The amount of memory to request from the host platform.
 * @return true on success; false otherwise.
 */
bool
memory_startup
(   u64 capacity
);

/**
 * @brief Terminates the memory subsystem.
 */
void
memory_shutdown
( void );

/**
 * @brief Allocates a block of memory.
 * 
 * @param size The number of bytes to allocate.
 * @param tag The block tag.
 * @return The allocated block.
 */
void*
memory_allocate
(   u64         size
,   MEMORY_TAG  tag
);

/**
 * @brief Allocates a block of memory.
 * 
 * @param size The number of bytes to allocate.
 * @param alignment Memory alignment.
 * @param tag The block tag.
 * @return The allocated block.
 */
void*
memory_allocate_aligned
(   u64         size
,   u16         alignment
,   MEMORY_TAG  tag
);

/**
 * @brief Frees a block of memory.
 * 
 * @param memory The block to free. Must be non-zero.
 * @param size The block size in bytes.
 * @param tag The block tag.
 */
void
memory_free
(   void*       memory
,   u64         size
,   MEMORY_TAG  tag
);

/**
 * @brief Frees a block of memory.
 * 
 * @param memory The block to free. Must be non-zero.
 * @param size The block size in bytes.
 * @param alignment Memory alignment.
 * @param tag The block tag.
 */
void
memory_free_aligned
(   void*       memory
,   u64         size
,   u16         alignment
,   MEMORY_TAG  tag
);

/**
 * @brief Clears a block of memory.
 * 
 * @param memory The block to clear. Must be non-zero.
 * @param size The block size in bytes.
 * @return memory.
 */
void*
memory_clear
(   void*   memory
,   u64     size
);

/**
 * @brief Sets a block of memory.
 * 
 * @param memory The block to set. Must be non-zero.
 * @param size The block size in bytes.
 * @return memory.
 */
void*
memory_set
(   void*   dst
,   i32     value
,   u64     size
);

/**
 * @brief Copies a specified number of bytes of memory from a source block to a
 * destination block.
 * 
 * The size of both blocks should be adequate for the specified size parameter.
 * The source and destination blocks may not overlap.
 * 
 * @param dst The destination block. Must be non-zero.
 * @param src The source block. Must be non-zero.
 * @param size The number of bytes to copy.
 * @return dst.
 */
void*
memory_copy
(   void*       dst
,   const void* src
,   u64         size
);

/**
 * @brief Moves a specified number of bytes of memory from a source block to a
 * destination block.
 * 
 * The size of both blocks should be adequate for the specified size parameter.
 * The source and destination blocks may overlap.
 * 
 * @param dst The destination block. Must be non-zero.
 * @param src The source block. Must be non-zero.
 * @param size The number of bytes to move.
 * @return dst.
 */
void*
memory_move
(   void*       dst
,   const void* src
,   u64         size
);

/**
 * @brief Fixed-length string equality test predicate.
 * 
 * @param s1 A string. Must be non-zero.
 * @param s2 A string. Must be non-zero.
 * @param size The number of bytes to compare.
 * @return true if strings are equal; false otherwise.
 */
bool
memory_equal
(   const void* s1
,   const void* s2
,   u64         size
);

/**
 * @brief Stringify utility.
 * 
 * Uses dynamic memory allocation. Call string_destroy to free.
 * 
 * @return A resizable string representation of memory usage statistics.
 * (see container/string.h)
 */
char*
memory_stat
( void );

/**
 * @brief Queries the global allocation count.
 * 
 * @return The global allocation count.
 */
u64
memory_allocation_count
( void );

/**
 * @brief Queries the global free count.
 * 
 * @return The global free count.
 */
u64
memory_free_count
( void );

/**
 * @brief Queries the total number of bytes allocated with a particular tag.
 * 
 * To query the total number of bytes allocated, pass MEMORY_TAG_ALL.
 * 
 * @param tag The memory tag to query.
 * @return The total number of bytes allocated with tag.
 */
u64
memory_amount_allocated
(   MEMORY_TAG tag
);

#endif  // MEMORY_H