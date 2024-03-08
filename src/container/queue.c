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
 * @brief Ensures that an existing queue has a capacity greater than or equal to
 * some minimum number of elements. The queue is resized, if needed.
 * 
 * @param queue The queue to (possibly) resize. Must be non-zero.
 * @param min_capacity The number of elements the queue needs to be able to
 * hold.
 */
void
queue_resize_if_needed
(   queue_t*    queue
,   const u64   min_capacity
);

bool
queue_create
(   const u64   stride
,   queue_t*    queue
)
{
    if ( !queue || !stride )
    {
        if ( !queue )
        {
            LOGERROR ( "queue_create: Missing argument: queue (output buffer)." );
        }
        if ( !stride )
        {
            LOGERROR ( "queue_create: Value of stride argument must be non-zero." );
        }
        return false;
    }

    ( *queue ).stride = stride;
    ( *queue ).length = 0;
    ( *queue ).allocated = 0;
    ( *queue ).memory = 0;
    queue_resize_if_needed ( queue , QUEUE_DEFAULT_CAPACITY );

    return true;
}

void
queue_destroy
(   queue_t* queue
)
{
    if ( !queue )
    {
        return;
    }
    if ( ( *queue ).memory )
    {
        memory_free ( ( *queue ).memory
                    , ( *queue ).allocated
                    , MEMORY_TAG_QUEUE
                    );
    }
    memory_clear ( queue , sizeof ( queue_t ) );
}

bool
queue_push
(   queue_t*    queue
,   const void* src
)
{
    if ( !queue || !src )
    {
        if ( !queue )
        {
            LOGERROR ( "queue_push: Missing argument: queue." );
        }
        if ( !src )
        {
            LOGERROR ( "queue_push: Missing argument: src." );
        }
        return false;
    }

    queue_resize_if_needed ( queue , ( *queue ).length + 1 );
    memory_copy ( ( void* )(  ( ( u64 )( ( *queue ).memory ) )
                            + ( *queue ).stride * ( *queue ).length
                           )
                , src
                , ( *queue ).stride
                );
    ( *queue ).length += 1;

    return true;
}

bool
queue_peek
(   const queue_t*  queue
,   void*           dst
)
{
    if ( !queue || !dst )
    {
        if ( !queue )
        {
            LOGERROR ( "queue_peek: Missing argument: queue." );
        }
        if ( !dst )
        {
            LOGERROR ( "queue_peek: Missing argument: dst." );
        }
        return false;
    }

    if ( !( *queue ).length )
    {
        LOGWARN ( "queue_peek: Queue is empty." );
        return false;
    }

    memory_copy ( dst , ( *queue ).memory , ( *queue ).stride );

    return true;
}

bool
queue_pop
(   queue_t*    queue
,   void*       dst
)
{
    if ( !queue || !dst )
    {
        if ( !queue )
        {
            LOGERROR ( "queue_pop: Missing argument: queue." );
        }
        if ( !dst )
        {
            LOGERROR ( "queue_pop: Missing argument: dst." );
        }
        return false;
    }

    if ( !( *queue ).length )
    {
        LOGWARN ( "queue_pop: Queue is empty." );
        return false;
    }

    memory_copy ( dst , ( *queue ).memory , ( *queue ).stride );
    memory_copy ( ( *queue ).memory
                , ( void* )(  ( ( u64 )( ( *queue ).memory ) )
                            + ( *queue ).stride
                           )
                , ( *queue ).stride * ( *queue ).length
                );
    ( *queue ).length -= 1;

    return true;
}

void
queue_resize_if_needed
(   queue_t*    queue
,   const u64   min_capacity
)
{
    const u64 new_size = ( *queue ).stride * min_capacity;
    if ( ( *queue ).allocated >= new_size )
    {
        return;
    }
    
    void* new_memory = memory_allocate ( new_size , MEMORY_TAG_QUEUE );
    if ( ( *queue ).memory )
    {
        memory_copy ( new_memory , ( *queue ).memory , ( *queue ).allocated );
        memory_free ( ( *queue ).memory
                    , ( *queue ).allocated
                    , MEMORY_TAG_QUEUE
                    );
    }
    ( *queue ).memory = new_memory;
    ( *queue ).allocated = new_size;
}