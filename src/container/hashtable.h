/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/hashtable.h
 * @brief Provides an interface for a hashtable data structure.
 */
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "common.h"

/** @brief Type definition for a hashtable data structure. */
typedef struct
{
    u64                             stride;
    u32                             capacity;
    bool                            pointer;
    void*                           memory;
    bool                            owns_memory;
}
hashtable_t;

/**
 * @brief Initializes a hashtable.
 * 
 * If enough memory is preallocated, the starting memory index should be passed
 * to this function; otherwise, the function will use implicit memory allocation
 * (see core/memory.h), and the output will need to be freed via hashmap_destroy.
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
 * @param capacity The maximum capacity. Cannot be resized.
 * @param memory Starting memory index (optional). Pass 0 to use implicit memory
 * allocation.
 * @param hashtable Output buffer.
 * @return true on success; false otherwise.
 */
bool
hashtable_create
(   const bool      pointer
,   const u64       stride
,   const u32       capacity
,   void*           memory
,   hashtable_t*    hashtable
);

/**
 * @brief Frees the memory used by a hashtable.
 * 
 * If the hashtable was not preallocated, this function will free the memory
 * implicitly (see core/memory.h).
 * 
 * @param hashtable The hashtable to free.
 */
void
hashtable_destroy
(   hashtable_t* hashtable
);

/**
 * @brief Sets a hashtable value. O(1).
 * 
 * @param hashtable The hashtable to mutate.
 * @param key The key whose value will be set.
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
 * @param hashtable The hashtable to query.
 * @param key The key whose value will be read.
 * @param value Output buffer for the value.
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
 * @param hashtable The hashtable to mutate.
 * @param value Handle to the data to set as every key's default value.
 * @return true on success; false otherwise.
 */
bool
hashtable_fill
(   hashtable_t*    hashtable
,   void*           value
);

#endif  // HASHTABLE_H