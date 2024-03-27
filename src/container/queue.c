/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file container/queue.c
 * @brief Implementation of the container/queue header.
 * (see container/queue.h for additional detailist)
 */
#include "container/queue.h"

#include "core/logger.h"
#include "core/memory.h"

/**
 * @brief Sets the value of a resizable queue field. O(1).
 * 
 * @param queue The queue to mutate. Must be non-zero.
 * @param field The field to set.
 * @param value The value to set.
 */
void
_queue_field_set
(   queue_t*    queue
,   QUEUE_FIELD field
,   u64         value
);

/**
 * @brief Ensures that an existing queue has a capacity greater than or equal to
 * some minimum number of elements. The queue is resized, if needed.
 * 
 * @param queue The queue to resize. Must be non-zero.
 * @param minimum_capacity The minimum number of elements the new queue is
 * required to hold.
 * @return The queue (possibly with new address).
 */
queue_t*
queue_resize_if_needed
(   queue_t*    queue
,   QUEUE_FIELD minimum_capacity
);

queue_t*
_queue_create
(   QUEUE_FIELD initial_capacity
,   QUEUE_FIELD stride
)
{
    if ( !initial_capacity || !stride )
    {
        if ( !initial_capacity )
        {
            LOGERROR ( "_array_create: Value of initial_capacity argument must be non-zero." );
        }
        if ( !stride )
        {
            LOGERROR ( "_array_create: Value of stride argument must be non-zero." );
        }
        return 0;
    }
    
    const u64 header_size = QUEUE_FIELD_COUNT * sizeof ( u64 );
    const u64 content_size = initial_capacity * stride;
    const u64 size = header_size + content_size;

    u64* queue = memory_allocate ( size , MEMORY_TAG_QUEUE );
    memory_clear ( queue , size );

    queue[ QUEUE_FIELD_ALLOCATED ] = content_size;
    queue[ QUEUE_FIELD_LENGTH ]    = 0;
    queue[ QUEUE_FIELD_STRIDE ]    = stride;
    
    return queue + QUEUE_FIELD_COUNT;
}

void
_queue_destroy
(   queue_t* queue
)
{
    if ( !queue )
    {
        return;
    }
    memory_free ( ( ( u64* ) queue ) - QUEUE_FIELD_COUNT
                , queue_size ( queue )
                , MEMORY_TAG_QUEUE
                );
}

u64
_queue_field_get
(   const queue_t*  queue
,   QUEUE_FIELD     field
)
{
    const u64* header = ( ( u64* ) queue ) - QUEUE_FIELD_COUNT;
    return header[ field ];
}

void
_queue_field_set
(   queue_t*    queue
,   QUEUE_FIELD field
,   u64         value
)
{
    u64* header = ( ( u64* ) queue ) - QUEUE_FIELD_COUNT;
    header[ field ] = value;
}

u64
_queue_size
(   const queue_t* queue
)
{
    u64* header = ( ( u64* ) queue ) - QUEUE_FIELD_COUNT;
    const u64 header_size = QUEUE_FIELD_COUNT * sizeof ( u64 );
    const u64 content_size = header[ QUEUE_FIELD_ALLOCATED ];
    return header_size + content_size;
}

queue_t*
_queue_push
(   queue_t*    queue
,   const void* src
)
{
    const u64 stride = queue_stride ( queue );
    const u64 old_length = queue_length ( queue );
    const u64 new_length = old_length + 1;
    queue = queue_resize_if_needed ( queue , new_length );
    memory_copy ( ( void* )( ( ( u64 ) queue ) + old_length * stride )
                , src
                , stride
                );
    _queue_field_set ( queue , QUEUE_FIELD_LENGTH , new_length );
    return queue;
}

bool
_queue_peek
(   const queue_t*  queue
,   void*           dst
)
{
    if ( !queue_length ( queue ) )
    {
        LOGWARN ( "_queue_peek: Queue is empty." );
        return false;
    }
    memory_copy ( dst , queue , queue_stride ( queue ) );
    return true;
}

bool
_queue_pop
(   queue_t*    queue
,   void*       dst
)
{
    const u64 stride = queue_stride ( queue );
    const u64 old_length = queue_length ( queue );
    const u64 new_length = old_length - 1;

    if ( !old_length )
    {
        LOGWARN ( "_queue_pop: Queue is empty." );
        return false;
    }

    if ( dst )
    {
        memory_copy ( dst , queue , stride );
    }
    memory_copy ( queue
                , ( void* )( ( ( u64 ) queue ) + stride )
                , old_length * stride
                );
    _queue_field_set ( queue , QUEUE_FIELD_LENGTH , new_length );
    return true;
}

queue_t*
queue_resize_if_needed
(   queue_t*    old_queue
,   QUEUE_FIELD minimum_capacity
)
{
    const u64 stride = queue_stride ( old_queue );
    const u64 old_size = queue_allocated ( old_queue );
    const u64 new_size = stride * minimum_capacity;

    if ( old_size >= new_size )
    {
        return old_queue;
    }

    queue_t* new_queue = _queue_create ( minimum_capacity , stride );
    memory_copy ( new_queue , old_queue , old_size );
    _queue_field_set ( new_queue
                     , QUEUE_FIELD_LENGTH
                     , queue_length ( old_queue )
                     );
    _queue_destroy ( old_queue );
    return new_queue;
}