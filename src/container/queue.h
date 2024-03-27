/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/queue.h
 * @brief Provides an interface for a FIFO queue data structure.
 */
#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"

/** @brief Type declaration for a queue. */
typedef void queue_t;

/** @brief Type and instance definitions for queue fields. */
typedef enum
{
    QUEUE_FIELD_ALLOCATED
,   QUEUE_FIELD_LENGTH
,   QUEUE_FIELD_STRIDE

,   QUEUE_FIELD_COUNT
}
QUEUE_FIELD;

/** @brief Queue default capacity. */
#define QUEUE_DEFAULT_CAPACITY 1

/**
 * @brief Allocates memory for a queue.
 * 
 * Use _queue_create to explicitly specify an initial capacity, or
 * queue_create to use a default.
 * 
 * Uses dynamic memory allocation. Call queue_destroy to free.
 * 
 * @param initial_capacity The initial capacity. Must be non-zero.
 * @param stride The fixed element size in bytes. Must be non-zero.
 * @return An empty queue.
 */
queue_t*
_queue_create
(   QUEUE_FIELD initial_capacity
,   QUEUE_FIELD stride
);

/** @param type C data type of the queue. */
#define queue_create(type) \
    _queue_create ( QUEUE_DEFAULT_CAPACITY , sizeof ( type ) )

/**
 * @brief Frees the memory used by a queue.
 * 
 * @param queue The queue to free.
 */
void
_queue_destroy
(   queue_t* queue
);

#define queue_destroy(queue) \
    _queue_destroy ( queue )

/**
 * @brief Obtains the value of a resizable queue field. O(1).
 * 
 * @param queue The queue to query. Must be non-zero.
 * @param field The field to read.
 * @return The value of the queue field.
 */
u64
_queue_field_get
(   const queue_t*  queue
,   QUEUE_FIELD     field
);

/** @brief Get queue field: allocated. */
#define queue_allocated(queue) \
    _queue_field_get ( (queue) , QUEUE_FIELD_ALLOCATED )

/** @brief Get queue field: length. */
#define queue_length(queue) \
    _queue_field_get ( (queue) , QUEUE_FIELD_LENGTH )

/** @brief Get queue field: stride. */
#define queue_stride(queue) \
    _queue_field_get ( (queue) , QUEUE_FIELD_STRIDE )

/**
 * @brief Computes the size in bytes of a queue data structure. O(1).
 * 
 * @param queue The queue to query. Must be non-zero.
 * @return The size in bytes of a queue data structure.
 */
u64
_queue_size
(   const queue_t* queue
);

#define queue_size(queue) \
    _queue_size ( queue )

/**
 * @brief Appends an element to a queue. O(1), on average.
 *
 * @param queue The queue to append to. Must be non-zero.
 * @param src The element to append. Must be non-zero.
 * @return The queue (possibly with new address).
 */
queue_t*
_queue_push
(   queue_t*    queue
,   const void* src
);

#define queue_push(queue,src) \
    ( (queue) = _queue_push ( (queue) , (src) ) )

/**
 * @brief Retrieves the head of the queue. O(1).
 *
 * @param queue The queue to query. Must be non-zero.
 * @param dst Output buffer for the head, if present. Must be non-zero.
 * @return true on success; false if queue empty.
 */
bool
_queue_peek
(   const queue_t*  queue
,   void*           dst
);

#define queue_peek(queue,dst) \
    _queue_peek ( (queue) , (dst) )

/**
 * @brief Removes the head of the queue. O(n).
 *
 * @param queue The queue to remove from. Must be non-zero.
 * @param dst Optional output buffer for the head, if present.
 * @return true on success; false if queue empty.
 */
bool
_queue_pop
(   queue_t*    queue
,   void*       dst
);

#define queue_pop(queue,dst) \
    _queue_pop ( (queue) , (dst) )

#endif  // QUEUE_H