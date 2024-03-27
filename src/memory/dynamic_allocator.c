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

/** @brief Type definition for internal state. */
typedef struct
{
    u64         capacity;

    u64         freelist_memory_requirement;
    freelist_t* freelist;

    bool        owns_memory;
    void*       memory;
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
    GiB ( 4 )

bool
dynamic_allocator_create
(   u64                     capacity
,   u64*                    memory_requirement_
,   void*                   memory_
,   dynamic_allocator_t**   allocator
)
{
    if ( !capacity )
    {
        LOGERROR ( "dynamic_allocator_init: Value of capacity argument must be non-zero." );
        return false;
    }

    u64 freelist_memory_requirement = 0;
    freelist_create ( capacity , &freelist_memory_requirement , 0 , 0 );
    const u64 memory_requirement = freelist_memory_requirement
                                 + capacity
                                 + sizeof ( state_t )
                                 ;
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
                                 , MEMORY_TAG_DYNAMIC_ALLOCATOR
                                 );
    }

    if ( !allocator )
    {
        LOGERROR ( "dynamic_allocator_create: Missing argument: allocator (output buffer)." );
        if ( !memory_ )
        {
            memory_free ( memory
                        , memory_requirement
                        , MEMORY_TAG_DYNAMIC_ALLOCATOR
                        );
        }
        return false;
    }

    state_t* state = memory;

    ( *state ).capacity = capacity;
    ( *state ).owns_memory = !memory_;

    ( *state ).freelist_memory_requirement = freelist_memory_requirement;
    if ( !freelist_create ( capacity
                          , 0
                          , ( void* )( ( ( u64 ) memory ) + sizeof ( state_t ) )
                          , &( *state ).freelist
                          ))
    {
        LOGERROR ( "dynamic_allocator_create: Failed to initialize backend freelist." );
        if ( ( *state ).owns_memory )
        {
            memory_free ( memory
                        , memory_requirement
                        , MEMORY_TAG_DYNAMIC_ALLOCATOR
                        );
        }
        return false;
    }

    ( *state ).memory = ( void* )( ( ( u64 ) memory )
                                 + sizeof ( state_t )
                                 + ( *state ).freelist_memory_requirement
                                 );
    memory_clear ( ( *state ).memory , capacity );

    *allocator = state;
    return true;
}

void
dynamic_allocator_destroy
(   dynamic_allocator_t** allocator
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

    freelist_destroy ( &( *state ).freelist );

    const u64 memory_requirement = ( *state ).freelist_memory_requirement
                                 + ( *state ).capacity
                                 + sizeof ( state_t )
                                 ;
    if ( ( *state ).owns_memory )
    {
        memory_free ( state
                    , memory_requirement
                    , MEMORY_TAG_DYNAMIC_ALLOCATOR
                    );
    }
    else
    {
        memory_clear ( state , memory_requirement );
    }

    *allocator = 0;
}

u64
dynamic_allocator_capacity
(   const dynamic_allocator_t* allocator
)
{
    return ( *( ( state_t* ) allocator ) ).capacity;
}

bool
dynamic_allocator_owns_memory
(   const dynamic_allocator_t* allocator
)
{
    return ( *( ( state_t* ) allocator ) ).owns_memory;
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
    state_t* state = allocator;

    const u64 header_size = sizeof ( header_t );
    const u64 storage_size = SIZE_STORAGE;
    const u64 required_size = alignment + header_size + storage_size + size;

    if ( required_size >= MAX_SINGLE_ALLOCATION_SIZE )
    {
        f64 max_amount;
        f64 req_amount;
        const char* max_unit = string_bytesize ( MAX_SINGLE_ALLOCATION_SIZE , &max_amount );
        const char* req_unit = string_bytesize ( required_size , &req_amount );
        LOGERROR ( "dynamic_allocator_allocate: Cannot request block size larger than MAX_SINGLE_ALLOCATION_SIZE (maximum: %.2f %s, requested: %.2f %s)."
                 , &max_amount , max_unit
                 , &req_amount , req_unit
                 );
        return 0;
    }

    u64 base_offset = 0;
    if ( !freelist_allocate ( ( *state ).freelist
                            , required_size
                            , &base_offset
                            ))
    {
        f64 req_amount;
        f64 rem_amount;
        const char* req_unit = string_bytesize ( size , &req_amount );
        const char* rem_unit = string_bytesize ( freelist_query_free ( ( *state ).freelist )
                                               , &rem_amount
                                               );
        LOGERROR ( "dynamic_allocator_allocate: No blocks of memory large enough to allocate from."
                   "\n\tRequested size: %.2f %s (Available: %.2f %s)"
                 , &req_amount , req_unit
                 , &rem_amount , rem_unit
                 );
        return 0;
    }

    void* memory = ( void* )( ( ( u64 )( ( *state ).memory ) ) + base_offset );
    u64 memory_offset = aligned ( ( ( u64 ) memory ) + SIZE_STORAGE , alignment );
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
    state_t* state = allocator;
    const void* const memory_end = ( *state ).memory + ( *state ).capacity;

    if (   memory < ( *state ).memory
        || memory > memory_end
       )
    {
        LOGWARN ( "dynamic_allocator_free: Trying to release block [%@] outside of allocator range [%@ .. %@]."
                , memory
                , ( *state ).memory
                , memory_end
                );
        return false;
    }

    u32* size = ( u32* )( ( ( u64 ) memory ) - SIZE_STORAGE );
    header_t* header = ( header_t* )( ( ( u64 ) memory ) + *size );
    u64 offset = ( ( u64 )( ( *header ).start ) )
               - ( ( u64 )( ( *state ).memory ) );
    u64 required_size = ( *header ).alignment
                      + sizeof ( header_t )
                      + SIZE_STORAGE
                      + *size
                      ;

    if ( !freelist_free ( ( *state ).freelist
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
    return freelist_query_free ( ( *( ( state_t* ) allocator ) ).freelist );
}

u64
dynamic_allocator_header_size
( void ) 
{
    return sizeof ( header_t ) + SIZE_STORAGE;
}
