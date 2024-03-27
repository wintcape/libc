/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file platform/thread.h
 * @brief Defines an interface for performing multi-threaded operations.
 */
#ifndef THREAD_H
#define THREAD_H

#include "common.h"

#include "container/queue.h"

/** @brief Type definition for a thread. */
typedef struct
{
    void*       internal;
    u64         id;
    queue_t*    work;
}
thread_t;

/** @brief Type definition for a 'thread start' callback function. */
typedef u32 ( *thread_start_function_t )( void* );

/**
 * @brief Creates a new thread.
 * 
 * Uses dynamic memory allocation. Call thread_destroy to free.
 * 
 * @param function The callback function to run threaded.
 * @param args Internal state arguments.
 * @param auto_detach Thread should immediately release resources when work is
 * complete? Y/N. If true, the output buffer will be unset.
 * @param thread Output buffer (only set if auto_detach is false).
 * @return true if successfully created; otherwise false.
 */
bool
thread_create
(   thread_start_function_t function
,   void*                   args
,   bool                    auto_detach
,   thread_t*               thread
);

/**
 * @brief Frees the memory used by a thread.
 * 
 * @param thread The thread to free.
 */
void
thread_destroy
(   thread_t* thread
);

/**
 * @brief Detaches the thread, automatically releasing its resources once work
 * is completed.
 * 
 * @param thread The thread to detach.
 */
void
thread_detach
(   thread_t* thread
);

/**
 * @brief Cancels work on the thread if possible, releasing its resources at an
 * appropriate time.
 * 
 * @param thread The thread to cancel.
 */
void
thread_cancel
(   thread_t* thread
);

/**
 * @brief Waits on the thread to complete all work.
 * 
 * @param thread The thread to wait for.
 * @return true on success; false otherwise.
 */
bool
thread_wait
(   thread_t* thread
);

/**
 * @brief Variant of thread_wait which accepts a timeout parameter.
 * 
 * @param thread A pointer to the thread to wait for.
 * @param timeout_ms The number of milliseconds to wait prior to timeout.
 * @return true on success; false on timeout or error.
 */
bool
thread_wait_timeout
(   thread_t*   thread
,   u64         timeout_ms
);

/**
 * @brief Queries if a thread is currently active.
 * 
 * @param thread The thread to query.
 * @return true if thread currently active; otherwise false.
 */
bool
thread_active
(   thread_t* thread
);

/**
 * @brief Sleeps on a thread for a specified amount of time.
 * 
 * @param thread The thread to sleep on.
 * @param ms The time to sleep, in milliseconds.
 */
void
thread_sleep
(   thread_t*   thread
,   u64         ms
);

/**
 * @brief Obtains the identifier for the current thread.
 * 
 * @return The identifier for the current thread.
 */
u64
thread_id
( void );

#endif  // THREAD_H