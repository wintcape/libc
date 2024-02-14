/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file memory/linear_allocator.c
 * @brief Implementation of the memory/linear_allocator header.
 * (see memory/linear_allocator.h for additional details)
 */

#include "memory/linear_allocator.h"

#include "core/logger.h"
#include "core/memory.h"
#include "core/string.h"

bool
linear_allocator_create
(   u64                 capacity
,   void*               memory
,   linear_allocator_t* allocator
)
{
    if ( !allocator )
    {
        LOGERROR ( "linear_allocator_create: Missing argument: allocator." );
        return false;
    }

    ( *allocator ).capacity = capacity;
    ( *allocator ).allocated = 0;
    ( *allocator ).owns_memory = !memory;

    if ( memory )
    {
        ( *allocator ).memory = memory;
    }
    else
    {
        ( *allocator ).memory = memory_allocate ( capacity
                                                , MEMORY_TAG_LINEAR_ALLOCATOR
                                                );
    }

    memory_clear ( ( *allocator ).memory , capacity );

    return true;
}

void
linear_allocator_destroy
(   linear_allocator_t* allocator
)
{
    if ( !allocator )
    {
        return;
    }
    if ( ( *allocator ).owns_memory && ( *allocator ).memory )
    {
        memory_free ( ( *allocator ).memory
                    , ( *allocator ).capacity
                    , MEMORY_TAG_LINEAR_ALLOCATOR
                    );
    }
    memory_clear ( allocator , sizeof ( linear_allocator_t ) );
}

void*
linear_allocator_allocate
(   linear_allocator_t* allocator
,   u64                 size
)
{
    if ( !allocator || !size || !( *allocator ).memory )
    {
        if ( !allocator )
        {
            LOGERROR ( "linear_allocator_allocate: Missing argument: allocator." );
        }
        if ( !size )
        {
            LOGERROR ( "linear_allocator_allocate: Cannot allocate block of size 0." );
        }
        if ( allocator && !( *allocator ).memory )
        {
            LOGERROR ( "linear_allocator_allocate: The provided allocator is uninitialized (%@)."
                     , allocator
                     );
        }
        return 0;
    }

    if ( ( *allocator ).allocated + size > ( *allocator ).capacity )
    {
        f64 req_amount;
        f64 rem_amount;
        const char* req_unit = string_bytesize ( ( *allocator ).capacity - ( *allocator ).allocated
                                               , &req_amount
                                               );
        const char* rem_unit = string_bytesize ( ( *allocator ).capacity - ( *allocator ).allocated
                                               , &rem_amount
                                               );
        LOGERROR ( "linear_allocator_allocate: Cannot allocate %.2f %s, only %.2f %s remaining."
                 , &req_amount , req_unit
                 , &rem_amount , rem_unit
                 );
        return 0;
    }

    void* blk = ( *allocator ).memory + ( *allocator ).allocated;
    ( *allocator ).allocated += size;
    return blk;
}

bool
linear_allocator_free
(   linear_allocator_t* allocator
)
{
    if ( !allocator || !( *allocator ).memory )
    {
        if ( !allocator )
        {
            LOGERROR ( "linear_allocator_free: Missing argument: allocator." );
        }
        if ( allocator && !( *allocator ).memory )
        {
            LOGERROR ( "linear_alloctor_free: The provided allocator is uninitialized (%@)."
                     , allocator
                     );
        }
        return false;
    }
    
    ( *allocator ).allocated = 0;
    memory_clear ( ( *allocator ).memory , ( *allocator ).capacity );
    return true;
}
