/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file platform/mutex.h
 * @brief Defines an interface for performing synchronous mutual exclusion
 * operations.
 */
#ifndef MUTEX_H
#define MUTEX_H

#include "common.h"

/** @brief Type defintion for a mutex. */
typedef struct
{
    void* internal;
}
mutex_t;

/**
 * @brief Initializes a mutex.
 *
 * Uses dynamic memory allocation. Call mutex_destroy to free.
 * 
 * @param mutex Output buffer.
 * @return true on success; false otherwise.
 */
bool
mutex_create
(   mutex_t* mutex
);

/**
 * @brief Frees the memory used by a mutex.
 * 
 * @param mutex The mutex to free.
 */
void
mutex_destroy
(   mutex_t* mutex
);

/**
 * @brief Locks a mutex.
 * 
 * @param mutex The mutex to lock.
 * @return true on success; false otherwise.
 */
bool
mutex_lock
(   mutex_t* mutex
);

/**
 * @brief Unlocks a mutex.
 * 
 * @param mutex The mutex to unlock.
 * @return true on success; false otherwise.
 */
bool
mutex_unlock
(   mutex_t* mutex
);

#endif  // MUTEX_H