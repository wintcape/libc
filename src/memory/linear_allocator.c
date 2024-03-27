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

/** @brief Type definition for internal state. */
typedef struct
{
    u64     capacity;
    u64     allocated;
    
    bool    owns_memory;
    void*   memory;
}
state_t;

bool
linear_allocator_create
(   u64                     capacity
,   u64*                    memory_requirement_
,   void*                   memory_
,   linear_allocator_t**    allocator
)
{
    if ( !capacity )
    {
        LOGERROR ( "linear_allocator_create: Value of capacity argument must be non-zero." );
        return false;
    }

    const u64 memory_requirement = sizeof ( state_t ) + capacity;
    if ( memory_requirement_ )
    {
        *memory_requirement_ = memory_requirement;
        if ( !memory_ )
        {
            return true;
        }
    }

    void* memory;
    if ( memory_ )
    {
        memory = memory_;
    }
    else
    {
        memory = memory_allocate ( memory_requirement
                                 , MEMORY_TAG_LINEAR_ALLOCATOR
                                 );
    }

    if ( !allocator )
    {
        LOGERROR ( "linear_allocator_create: Missing argument: allocator (output buffer)." );
        if ( !memory_ )
        {
            memory_free ( memory
                        , memory_requirement
                        , MEMORY_TAG_LINEAR_ALLOCATOR
                        );
        }
        return false;
    }

    memory_clear ( memory , memory_requirement );

    state_t* state = memory;
    ( *state ).capacity = capacity;
    ( *state ).allocated = 0;
    ( *state ).owns_memory = !memory_;
    ( *state ).memory = ( void* )( ( ( u64 ) state ) + sizeof ( state_t ) );

    *allocator = state;
    return true;
}

void
linear_allocator_destroy
(   linear_allocator_t** allocator
)
{
    if ( !allocator )
    {
        return;
    }

    state_t* state = *allocator;
    if ( !state )
    {
        return;
    }

    const u64 memory_requirement = sizeof ( state_t ) + ( *state ).capacity;
    if ( ( *state ).owns_memory )
    {
        memory_free ( state
                    , memory_requirement
                    , MEMORY_TAG_LINEAR_ALLOCATOR
                    );
    }
    else
    {
        memory_clear ( state , memory_requirement );
    }

    *allocator = 0;
}

u64
linear_allocator_allocated
(   const linear_allocator_t* allocator
)
{
    return ( *( ( state_t* ) allocator ) ).allocated;
}

u64
linear_allocator_capacity
(   const linear_allocator_t* allocator
)
{
    return ( *( ( state_t* ) allocator ) ).capacity;
}

bool
linear_allocator_owns_memory
(   const linear_allocator_t* allocator
)
{
    return ( *( ( state_t* ) allocator ) ).owns_memory;
}

void*
linear_allocator_allocate
(   linear_allocator_t* allocator
,   u64                 size
)
{
    state_t* state = allocator;

    if ( ( *state ).allocated + size > ( *state ).capacity )
    {
        f64 req_amount;
        f64 rem_amount;
        const char* req_unit = string_bytesize ( ( *state ).capacity - ( *state ).allocated
                                               , &req_amount
                                               );
        const char* rem_unit = string_bytesize ( ( *state ).capacity - ( *state ).allocated
                                               , &rem_amount
                                               );
        LOGERROR ( "linear_allocator_allocate: Cannot allocate %.2f %s, only %.2f %s remaining."
                 , &req_amount , req_unit
                 , &rem_amount , rem_unit
                 );
        return 0;
    }

    void* blk = ( *state ).memory + ( *state ).allocated;
    ( *state ).allocated += size;
    return blk;
}

bool
linear_allocator_free
(   linear_allocator_t* allocator
)
{
    state_t* state = allocator;    
    ( *state ).allocated = 0;
    memory_clear ( ( *state ).memory , ( *state ).capacity );
    return true;
}
