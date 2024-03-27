/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/hashtable.h
 * @brief Provides an interface for a hashtable data structure.
 */
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "common.h"

/** @brief Type declaration for a hashtable. */
typedef void hashtable_t;

/**
 * @brief Initializes a hashtable.
 * 
 * If pre-allocating a memory buffer:
 *   Call once to get the memory requirement; call a second time passing in a
 *   valid memory buffer of the required size.
 * 
 * If using implicit memory allocation:
 *   Uses dynamic memory allocation (see core/memory.h). Call hashtable_destroy
 *   to free.
 * 
 * Supports the construction of both pointer-valued and data-valued hashtables.
 * A pointer-valued hashtable consists of key-value pairs whose values are to be
 * interpreted as the memory addresses of externally-stored data; if the data
 * structures referenced by these pointers need to be allocated and freed, that
 * needs to be handled externally. A data-valued hashtable consists of key-value
 * pairs whose values are to be interpreted as raw data.
 * 
 * @param pointer Pass true if the output should be a pointer-valued hashtable;
 * pass false if it should be data-valued.
 * @param stride The size of each element in bytes (only applicable for data-
 * valued hashtables).
 * @param capacity The maximum number of elements the hashtable may hold. Cannot
 * be resized.
 * @param memory_requirement Output buffer to hold the actual number of bytes
 * required to operate the dynamic allocator. Only applicable if pre-allocating
 * a memory buffer of the required size. Pass 0 to use implicit memory
 * allocation.
 * @param memory Optional pre-allocated memory buffer. Only applicable if
 * memory is being pre-allocated. Pass 0 to read memory requirement; otherwise,
 * pass a pre-allocated buffer of the required size.
 * @param hashtable Output buffer for hashtable.
 * @return true on success; false otherwise.
 */
bool
hashtable_create
(   bool            pointer
,   u64             stride
,   u32             capacity
,   u64*            memory_requirement
,   void*           memory
,   hashtable_t**   hashtable
);

/**
 * @brief Frees the memory used by a hashtable.
 * 
 * If the hashtable was not pre-allocated, this function will free the memory
 * implicitly (see core/memory.h).
 * 
 * @param hashtable Handle to the hashtable to free.
 */
void
hashtable_destroy
(   hashtable_t** hashtable
);

/**
 * @brief Obtains the value of a hashtable 'stride' field.
 * 
 * @param hashtable The hashtable to query. Must be non-zero.
 * @return The value of the hashtable 'stride' field.
 */
u64
hashtable_stride
(   const hashtable_t* hashtable
);

/**
 * @brief Queries the maximum capacity of a hashtable.
 * 
 * @param hashtable The hashtable to query. Must be non-zero.
 * @return Tthe maximum capacity of hashtable.
 */
u64
hashtable_capacity
(   const hashtable_t* hashtable
);

/**
 * @brief Queries whether a hashtable is pointer-valued or data-valued.
 * 
 * @param hashtable The hashtable to query. Must be non-zero.
 * @return true if hashtable is pointer-valued; false otherwise.
 */
bool
hashtable_pointer
(   const hashtable_t* hashtable
);

/**
 * @brief Queries whether a hashtable was created with implicit memory
 * allocation.
 * 
 * @param hashtable The hashtable to query. Must be non-zero.
 * @return true if a hashtable was created with implicit memory allocation;
 * false otherwise.
 */
bool
hashtable_owns_memory
(   const hashtable_t* hashtable
);

/**
 * @brief Sets a hashtable value. O(1).
 * 
 * @param hashtable The hashtable to mutate. Must be non-zero.
 * @param key The key whose value will be set. Must be a null-terminated string.
 * @param value Handle to the data to set as the key's value. If hashtable is
 * data-valued, this must be set to the address of the data to copy in; if
 * hashtable is pointer-valued, 0 may be passed to set the value of the key to
 * a null pointer (effectively removing the pointer value from the hashtable).
 * @return true on success; false otherwise.
 */
bool
hashtable_set
(   hashtable_t*    hashtable
,   const char*     key
,   const void*     value
);

/**
 * @brief Queries a hashtable value. O(1).
 * 
 * @param hashtable The hashtable to query. Must be non-zero.
 * @param key The key whose value will be read. Must be a null-terminated
 * string.
 * @param value Output buffer for the value. Must be non-zero.
 * @return true on success; false otherwise.
 */
bool
hashtable_get
(   hashtable_t*    hashtable
,   const char*     key
,   void*           value
);

/**
 * @brief Sets the values for all keys in the hashtable to a default value.
 * O(n).
 * 
 * Only for data-valued hashtables.
 * 
 * @param hashtable The hashtable to mutate. Must be non-zero.
 * @param value Handle to the data to set as every key's default value.
 * Must be non-zero.
 * @return true on success; false otherwise.
 */
bool
hashtable_fill
(   hashtable_t*    hashtable
,   void*           value
);

#endif  // HASHTABLE_H