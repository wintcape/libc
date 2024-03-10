/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/queue.h
 * @brief Provides an interface for a FIFO queue data structure.
 */
#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"

/** @brief Type definition for a queue data structure. */
typedef struct
{
    u64     stride;
    u64     length;
    u64     allocated;
    void*   memory;
}
queue_t;

/** @brief Queue default capacity. */
#define QUEUE_DEFAULT_CAPACITY 1

/**
 * @brief Initializes a queue.
 * 
 * Uses dynamic memory allocation; call queue_destroy to free.
 * 
 * @param stride The size of each element in bytes.
 * @param queue Output buffer.
 * @return true on success; false otherwise.
 */
bool
queue_create
(   u64         stride
,   queue_t*    queue
);

/**
 * @brief Frees the memory used by a queue.
 * 
 * @param queue The queue to free.
 */
void
queue_destroy
(   queue_t* queue
);

/**
 * @brief Appends an element to a queue. O(1), on average.
 *
 * @param queue The queue to append to.
 * @param src The element to append.
 * @return true on success; false otherwise
 */
bool
queue_push
(   queue_t*    queue
,   const void* src
);

/**
 * @brief Retrieves the head of the queue. O(1).
 *
 * @param queue The queue to query.
 * @param dst Output buffer for the head, if present.
 * @return true on success; false if queue empty.
 */
bool
queue_peek
(   const queue_t*  queue
,   void*           dst
);

/**
 * @brief Removes the head of the queue. O(n).
 *
 * @param queue The queue to remove from.
 * @param dst Output buffer for the head, if present.
 * @return true on success; false if queue empty.
 */
bool
queue_pop
(   queue_t*    queue
,   void*       dst
);

#endif  // QUEUE_H