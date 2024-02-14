/**
 * @file platform/platform.h
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @brief Interface to abstract away the host platform layer.
 */
#ifndef PLATFORM_H
#define PLATFORM_H

#include "common.h"

#include "core/mutex.h"
#include "core/thread.h"

/**
 * @brief Platform-independent memory allocation function (see core/memory.h).
 * 
 * @param size The number of bytes to allocate.
 */
void*
platform_memory_allocate
(   u64 size
);

/**
 * @brief Platform-independent memory free function (see core/memory.h).
 * 
 * @param blk The block to free.
 */
void
platform_memory_free
(   void* blk
);

/**
 * @brief Platform-independent memory clear function (see core/memory.h).
 * 
 * @param blk The block to initialize.
 * @param size The number of bytes to set.
 * @return blk.
 */
void*
platform_memory_clear
(   void*   blk
,   u64     size
);

/**
 * @brief Platform-independent memory set function (see core/memory.h).
 * 
 * @param blk The block to set.
 * @param value The value to set.
 * @param size The number of bytes to set.
 * @return blk.
 */
void*
platform_memory_set
(   void*       blk
,   const i32   value
,   u64         size
);

/**
 * @brief Platform-independent memory copy function (see core/memory.h).
 * 
 * @param dst The destination block.
 * @param src The source block.
 * @param size The number of bytes to copy.
 * @return dst.
 */
void*
platform_memory_copy
(   void*       dst
,   const void* src
,   u64         size
);

/**
 * @brief Platform-independent memory move function (see core/memory.h).
 * 
 * @param dst The destination block.
 * @param src The source block.
 * @param size The number of bytes to move.
 * @return dst.
 */
void*
platform_memory_move
(   void*       dst
,   const void* src
,   u64         size
);

/**
 * @brief Platform-independent function to compute the length of a null-
 * terminated string (see core/string.h).
 * 
 * String is assumed to terminate within an accessible memory range.
 * For potentially unsafe strings, use platform_string_length_clamped instead.
 * 
 * @param string The null-terminated string to read.
 * @return The number of characters in string.
 */
u64
platform_string_length
(   const char* string
);


/**
 * @brief Clamped variant of platform_string_length for unsafe strings.
 * 
 * @param string The string to read.
 * @param limit The maximum number of characters to read from string.
 * @return The number of characters in string, or limit if string contains more
 * than limit characters.
 */
u64
platform_string_length_clamped
(   const char* string
,   const u64   limit
);

/**
 * @brief Platform-independent function to query the system time
 * (see core/clock.h).
 * 
 * @return The system time.
 */
f64
platform_get_absolute_time
( void );

/**
 * @brief Platform-independent sleep function.
 * 
 * @param ms The number of ms to sleep on the current thread for.
 */
void
platform_sleep
(   u64 ms
);

/**
 * @brief Platform-independent 'thread create' function (see core/thread.h).
 * 
 * Uses dynamic memory allocation; call platform_thread_destroy to free.
 * 
 * @param function The callback function to run threaded.
 * @param args Internal state arguments.
 * @param auto_detach Thread should immediately release resources when work is
 * complete? Y/N. If true, the output buffer will be unset.
 * @param thread Output buffer (only set if auto_detach is false).
 * @returns true if successfully created; otherwise false.
 */
bool
platform_thread_create
(   PFN_thread_start    function
,   void*               args
,   bool                auto_detach
,   thread_t*           thread
);

/**
 * @brief Platform-independent 'thread destroy' function (see core/thread.h).
 * 
 * @param thread The thread to free.
 */
void
platform_thread_destroy
(   thread_t* thread
);

/**
 * @brief Platform-independent 'thread detach' function (see core/thread.h).
 * 
 * @param thread The thread to detach.
 */
void
platform_thread_detach
(   thread_t* thread
);

/**
 * @brief Platform-independent 'thread cancel' function (see core/thread.h).
 * 
 * @param thread The thread to cancel.
 */
void
platform_thread_cancel
(   thread_t* thread
);

/**
 * @brief Platform-independent 'thread wait' function (see core/thread.h).
 * 
 * @param thread The thread to wait for.
 * @return true on success; false otherwise.
 */
bool
platform_thread_wait
(   thread_t* thread
);

/**
 * @brief Variant of platform_thread_wait which accepts an explicit timeout as
 * an argument.
 * 
 * @param thread A pointer to the thread to wait for.
 * @param timeout_ms The number of milliseconds to wait prior to timeout.
 * @return true on success; false on timeout or error.
 */
bool
platform_thread_wait_timeout
(   thread_t*   thread
,   const u64   timeout_ms
);

/**
 * @brief Platform-independent 'thread active' function (see core/thread.h).
 * 
 * @param thread The thread to query.
 * @returns true if thread currently active; otherwise false.
 */
bool
platform_thread_active
(   thread_t* thread
);

/**
 * @brief Platform-independent 'thread sleep' function (see core/thread.h).
 * 
 * @param thread The thread to sleep on.
 * @param ms The time to sleep, in milliseconds.
 */
void
platform_thread_sleep
(   thread_t*   thread
,   const u64   ms
);

/**
 * @brief Platform-independent function to obtain the identifier for the current
 * thread (see core/thread.h).
 * 
 * @return The identifier for the current thread.
 */
u64
platform_thread_id
( void );

/**
 * @brief Platform-independent 'mutex create' function (see core/mutex.h).
 *
 * Uses dynamic memory allocation; call platform_mutex_destroy to free.
 * 
 * @param mutex Output buffer.
 * @return true on success; false otherwise.
 */
bool
platform_mutex_create
(   mutex_t* mutex
);

/**
 * @brief Platform-independent 'mutex destroy' function (see core/mutex.h).
 * 
 * @param mutex The mutex to free.
 */
void
platform_mutex_destroy
(   mutex_t* mutex
);

/**
 * @brief Platform-independent 'mutex lock' function (see core/mutex.h).
 * 
 * @param mutex The mutex to lock.
 * @return true on success; false otherwise.
 */
bool
platform_mutex_lock
(   mutex_t* mutex
);

/**
 * @brief Platform-independent 'mutex unlock' function (see core/mutex.h).
 * 
 * @param mutex The mutex to unlock.
 * @return true on success; false otherwise.
 */
bool
platform_mutex_unlock
(   mutex_t* mutex
);

#endif  // PLATFORM_H
