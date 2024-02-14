/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file memory/dynamic_allocator.c
 * @brief Implementation of the memory/dynamic_allocator header.
 * (see memory/dynamic_allocator.h for additional details)
 */
#include "memory/dynamic_allocator.h"

#include "core/logger.h"
#include "core/memory.h"
#include "core/string.h"

#include "container/freelist.h"

/** @brief Type definition for an internal state. */
typedef struct
{
    u64         capacity;
    freelist_t  freelist;
    void*       freelist_start;
    void*       memory_start;
}
state_t;

/** @brief Type definition for a container to hold memory allocation header info. */
typedef struct
{
    void*   start;
    u16     alignment;
}
header_t;

/** @brief Storage size (in bytes) of a node's user memory block. */
#define SIZE_STORAGE \
    sizeof ( u32 )

/** @brief Maximum single allocation size. */
#define MAX_SINGLE_ALLOCATION_SIZE \
    GIBIBYTES ( 4 )

bool
dynamic_allocator_init
(   u64                     capacity
,   u64*                    memory_requirement
,   void*                   memory
,   dynamic_allocator_t*    allocator
)
{
    if ( capacity < 1 )
    {
        LOGERROR ( "dynamic_allocator_init: Attempted to initialize allocator with size 0." );
        return false;
    }

    if ( !memory_requirement )
    {
        LOGERROR ( "dynamic_allocator_init: Missing argument: memory_requirement." );
        return false;
    }

    u64 freelist_memory_requirement = 0;
    freelist_init ( capacity , &freelist_memory_requirement , 0 , 0 );
    
    *memory_requirement = freelist_memory_requirement
                        + sizeof ( state_t )
                        + capacity
                        ;

    if ( !memory )
    {
        return true;
    }

    if ( !allocator )
    {
        LOGERROR ( "dynamic_allocator_init: Missing argument: allocator (output buffer)." );
        return false;
    }

    ( *allocator ).memory = memory;
    state_t* state = ( *allocator ).memory;
    ( *state ).capacity = capacity;
    ( *state ).freelist_start = ( *allocator ).memory + sizeof ( state_t );
    ( *state ).memory_start = ( *state ).freelist_start
                            + freelist_memory_requirement
                            ;

    freelist_init ( capacity
                  , &freelist_memory_requirement
                  , ( *state ).freelist_start
                  , &( *state ).freelist
                  );

    memory_clear ( ( *state ).memory_start , capacity );

    return true;
}

void
dynamic_allocator_clear
(   dynamic_allocator_t* allocator
)
{
    if ( !allocator )
    {
        return;
    }

    state_t* state = ( *allocator ).memory;
    freelist_clear ( &( *state ).freelist );
    memory_clear ( ( *state ).memory_start , ( *state ).capacity );
    ( *state ).capacity = 0;
    ( *allocator ).memory = 0;
}

void*
dynamic_allocator_allocate
(   dynamic_allocator_t*    allocator
,   u64                     size
)
{
    return dynamic_allocator_allocate_aligned ( allocator , size , 1 );
}

void*
dynamic_allocator_allocate_aligned
(   dynamic_allocator_t*    allocator
,   u64                     size
,   u16                     alignment
)
{
    if ( !allocator || !size || !alignment || !( *allocator ).memory )
    {
        if ( !allocator )
        {
            LOGERROR ( "dynamic_allocator_allocate: Missing argument: allocator." );
        }
        if ( !size )
        {
            LOGERROR ( "dynamic_allocator_allocate: Cannot allocate block of size 0." );
        }
        if ( !alignment )
        {
            LOGERROR ( "dynamic_allocator_allocate: Cannot allocate block with alignment 0." );
        }
        if ( allocator && !( *allocator ).memory )
        {
            LOGERROR ( "dynamic_allocator_allocate: The provided allocator is uninitialized (%@)."
                     , allocator
                     );
        }
        return 0;
    }

    state_t* state = ( *allocator ).memory;

    const u64 header_size = sizeof ( header_t );
    const u64 storage_size = SIZE_STORAGE;
    const u64 required_size = alignment + header_size + storage_size + size;

    if ( required_size >= MAX_SINGLE_ALLOCATION_SIZE )
    {
        f64 amount;
        const char* unit = string_bytesize ( MAX_SINGLE_ALLOCATION_SIZE , &amount );
        LOGERROR ( "dynamic_allocator_allocate: Cannot request block size larger than MAX_SINGLE_ALLOCATION_SIZE (%.2f %s)."
                 , amount
                 , unit
                 );
        return 0;
    }

    u64 base_offset = 0;
    if ( !freelist_allocate ( &( *state ).freelist
                            , required_size
                            , &base_offset
                            ))
    {
        f64 req_amount;
        f64 rem_amount;
        const char* req_unit = string_bytesize ( size , &req_amount );
        const char* rem_unit = string_bytesize ( freelist_query_free ( &( *state ).freelist )
                                               , &rem_amount
                                               );
        LOGERROR ( "dynamic_allocator_allocate: No blocks of memory large enough to allocate from."
                   "\n\tRequested size: %.2f %s (Available: %.2f %s)"
                 , &req_amount , req_unit
                 , &rem_amount , rem_unit
                 );
        return 0;
    }

    void* memory = ( void* )( ( u64 )( ( *state ).memory_start ) + base_offset );
    u64 memory_offset = aligned ( ( u64 ) memory + SIZE_STORAGE , alignment );
    u32* memory_size = ( u32* )( memory_offset - SIZE_STORAGE );
    *memory_size = ( u32 ) size;
    header_t* header = ( header_t* )( memory_offset + size );
    ( *header ).start = memory;
    ( *header ).alignment = alignment;
    return ( void* ) memory_offset;
}

bool
dynamic_allocator_free
(   dynamic_allocator_t*    allocator
,   void*                   memory
)
{
    return dynamic_allocator_free_aligned ( allocator , memory );
}

bool
dynamic_allocator_free_aligned
(   dynamic_allocator_t*    allocator
,   void*                   memory
)
{
    if ( !allocator || !memory || !( *allocator ).memory )
    {
        if ( !allocator )
        {
            LOGERROR ( "dynamic_allocator_free: Missing argument: allocator." );
        }
        if ( !memory )
        {
            LOGERROR ( "dynamic_allocator_free: Missing argument: memory." );
        }
        if ( allocator && !( *allocator ).memory )
        {
            LOGERROR ( "dynamic_allocator_free: The provided allocator is uninitialized (%@)."
                     , allocator
                     );
        }
        return false;
    }
    
    state_t* state = ( *allocator ).memory;
    const void* const memory_end = ( *state ).memory_start + ( *state ).capacity;

    if (   memory < ( *state ).memory_start
        || memory > memory_end
       )
    {
        LOGWARN ( "dynamic_allocator_free: Trying to release block [%@] outside of allocator range [%@ .. %@]."
                , memory
                , ( *state ).memory_start
                , memory_end
                );
        return false;
    }

    u32* size = ( u32* )( ( u64 ) memory - SIZE_STORAGE );
    header_t* header = ( header_t* )( ( u64 ) memory + *size );
    u64 offset = ( u64 ) ( *header ).start - ( u64 ) ( *state ).memory_start;
    u64 required_size = ( *header ).alignment
                      + sizeof ( header_t )
                      + SIZE_STORAGE
                      + *size
                      ;

    if ( !freelist_free ( &( *state ).freelist
                        , required_size
                        , offset
                        ))
    {
        LOGERROR ( "dynamic_allocator_free: Failed to free memory." );
        return false;
    }

    return true;
}

bool
dynamic_allocator_size_alignment
(   void*   memory
,   u64*    size
,   u16*    alignment
)
{
    *size = *( ( u32* )( ( ( u64 ) memory ) - SIZE_STORAGE ) );
    header_t* header = ( header_t* )( ( ( u64 ) memory ) + *size );
    *alignment = ( *header ).alignment;
    return true;
}

// Expensive!
u64
dynamic_allocator_query_free
(   const dynamic_allocator_t* allocator
)
{
    state_t* state = ( *allocator ).memory;
    return freelist_query_free ( &( *state ).freelist );
}

u64
dynamic_allocator_header_size
( void ) 
{
    return sizeof ( header_t ) + SIZE_STORAGE;
}
