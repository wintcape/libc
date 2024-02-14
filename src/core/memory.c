/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file core/memory.c
 * @brief Implementation of the core/memory header.
 * (see core/memory.h for additional details)
 */
#include "core/memory.h"

#include "container/string.h"

#include "core/logger.h"

#include "memory/dynamic_allocator.h"

#include "platform/platform.h"

/** @brief Memory tag strings. */
static const char* memory_tags[ MEMORY_TAG_COUNT ] = { "UNKNOWN           "
                                                     , "ARRAY             "
                                                     , "STRING            "
                                                     , "HASHTABLE         "
                                                     , "QUEUE             "
                                                     , "LINEAR_ALLOCATOR  "
                                                     , "DYNAMIC_ALLOCATOR "
                                                     , "THREAD            "
                                                     , "MUTEX             "
                                                     , "SEMAPHORE         "
                                                     };

/** @brief Type definition for a container to hold global statistics. */
typedef struct
{
    u64     allocation_count;
    u64     free_count;

    u64     allocated;
    u64     tagged_allocations[ MEMORY_TAG_COUNT ];
}
stat_t;

/** @brief Type definition for memory subsystem state. */
typedef struct
{
    stat_t              stat;
    
    u64                 allocator_capacity;
    dynamic_allocator_t allocator;
    void*               allocator_start;

    u64                 capacity;
    void*               memory;
}
state_t;

/** @brief Memory subsystem state. */
static state_t* state = 0;

bool
memory_startup
(   const u64 capacity
)
{
    if ( state )
    {
        LOGFATAL ( "memory_startup: Called more than once." );
        return false;
    }

    const u64 state_memory_requirement = sizeof ( state_t );
    u64 allocator_memory_requirement = 0;
    dynamic_allocator_init ( capacity , &allocator_memory_requirement , 0 , 0 );
    const u64 memory_requirement = state_memory_requirement + allocator_memory_requirement;

    f64 amount;
    const char* unit = string_bytesize ( memory_requirement , &amount );
    LOGDEBUG ( "Requesting %.2f %s of memory from "PLATFORM_STRING" for the entire runtime environment. . ."
             , &amount , unit
             );

    void* memory = platform_memory_allocate ( memory_requirement );
    if ( !memory )
    {
        return false;
    }

    state = memory;
    ( *state ).allocator_capacity = allocator_memory_requirement;
    ( *state ).allocator_start = memory + state_memory_requirement;
    ( *state ).capacity = capacity;
    memory_clear ( &( *state ).stat , sizeof ( stat_t ) );

    if ( !dynamic_allocator_init ( capacity
                                 , &( *state ).allocator_capacity
                                 , ( *state ).allocator_start
                                 , &( *state ).allocator
                                 ))
    {
        LOGFATAL ( "Memory subsystem failed to initialize internal allocator." );
        return false;
    }

    LOGDEBUG ( "  Success." );

    return true;
}

void
memory_shutdown
( void )
{
    if ( !state )
    {
        return;
    }

    dynamic_allocator_clear ( &( *state ).allocator );

    if ( ( *state ).stat.allocation_count != ( *state ).stat.free_count )
    {
        LOGDEBUG ( "Noticed allocation count (%i) != free count (%i) when shutting down memory subsystem."
                 , ( *state ).stat.allocation_count
                 , ( *state ).stat.free_count
                 );
    }
    
    platform_memory_free ( state );

    state = 0;
}

void*
memory_allocate
(   u64         size
,   MEMORY_TAG  tag
)
{
    return memory_allocate_aligned ( size , 1 , tag );
}

void*
memory_allocate_aligned
(   u64         size
,   u16         alignment
,   MEMORY_TAG  tag
)
{
    // Warn if tag unknown.
    if ( tag == MEMORY_TAG_UNKNOWN )
    {
        LOGWARN ( "memory_allocate: called with MEMORY_TAG_UNKNOWN." );
    }

    // Perform the memory allocation.
    void* memory;
    if ( state )
    {
        memory = dynamic_allocator_allocate_aligned ( &( *state ).allocator
                                                    , size
                                                    , alignment
                                                    );
    }
    else
    {   // Failsafe for if memory subsystem is not yet initialized.
        memory = platform_memory_allocate ( size );
    }

    if ( !memory )
    {
        LOGFATAL ( "memory_allocate: Failed to allocate memory." );
        return 0;
    }
    
    memory_clear ( memory , size );

    // Update statistics.
    if ( state )
    {
        ( *state ).stat.allocated += size;
        ( *state ).stat.tagged_allocations[ tag ] += size;
        ( *state ).stat.allocation_count += 1;
    }

    return memory;
}

void
memory_free
(   void*       memory
,   u64         size
,   MEMORY_TAG  tag
)
{
    memory_free_aligned ( memory , size , 1 , tag );
}

void
memory_free_aligned
(   void*       memory
,   u64         size
,   u16         alignment
,   MEMORY_TAG  tag
)
{
    // Warn if tag unknown.
    if ( tag == MEMORY_TAG_UNKNOWN )
    {
        LOGWARN ( "memory_free: called with MEMORY_TAG_UNKNOWN." );
    }

    // Perform the memory free.
    if ( state && dynamic_allocator_free_aligned ( &( *state ).allocator
                                                 , memory
                                                 ))
    {
        // Update statistics.
        ( *state ).stat.allocated -= size;
        ( *state ).stat.tagged_allocations[ tag ] -= size;
        ( *state ).stat.free_count += 1;
    }
    else
    {
        /**
         * If the call failed or the subsystem is not yet initialized,
         * try freeing the memory on the platform level as a failsafe.
         */
        platform_memory_free ( memory );
    }   // . . .gee, I sure hope that worked. . .
}

void*
memory_clear
(   void*   memory
,   u64     size
)
{
    return platform_memory_clear ( memory , size );
}

void*
memory_set
(   void*   memory
,   i32     value
,   u64     size
)
{
    return platform_memory_set ( memory , value , size );
}

void*
memory_copy
(   void*       dst
,   const void* src
,   u64         size
)
{
    return platform_memory_copy ( dst , src , size );    
}

void*
memory_move
(   void*       dst
,   const void* src
,   u64         size
)
{
    return platform_memory_move ( dst , src , size );    
}

bool
memory_equal
(   const void* s1
,   const void* s2
,   u64         size
)
{
    if ( !s1 || !s2 )
    {
        return false;
    }
    
    const u64* s1a;
    const u64* s2a;
    const u8*  s1b;
    const u8*  s2b;
    for ( s1a = ( u64* ) s1 , s2a = ( u64* ) s2
        ; size >= sizeof ( u64 )
        ; ++s1a , ++s2a , size -= sizeof ( u64 )
        )
    {
        if ( *s1a != *s2a )
        {
            return false;
        }
    }
    for ( s1b = ( u8* ) s1a , s2b = ( u8* ) s2a
        ; size
        ; ++s1b , ++s2b , size -= sizeof ( u8 )
        )
    {
        if ( *s1b != *s2b )
        {
            return false;
        }
    }
    return true;
}

char*
memory_stat
( void )
{
    char* lines[ MEMORY_TAG_COUNT+2 ];
    char* string = string_create_from ( "System memory usage:\n" );
    const char* unit;
    f64 amount;
    u32 i = 0;
    while ( i < MEMORY_TAG_COUNT )
    {
        unit = string_bytesize ( ( *state ).stat.tagged_allocations[ i ]
                               , &amount
                               );
        lines[ i ] = string_format ( "\t  %s: %.2f %s\n"
                                   , memory_tags[ i ] , &amount , unit
                                   );
        string_push ( string
                    , lines[ i ]
                    , string_length ( lines[ i ] )
                    );
        i += 1;
    }

    unit = string_bytesize ( ( *state ).stat.allocated
                           , &amount
                           );
    lines[ i ] = string_format ( "\t  ------------------------------\n"
                                 "\t  TOTAL            : %.2f %s\n"
                               , &amount , unit
                               );
    string_push ( string
                , lines[ i ]
                , string_length ( lines[ i ] )
                );
    i += 1;

    unit = string_bytesize ( ( *state ).capacity
                           , &amount
                           );
    lines[ i ] = string_format ( "\t                    (%.2f %s reserved)"
                               , &amount , unit
                               );
    string_push ( string
                , lines[ i ]
                , string_length ( lines[ i ] )
                );
    i += 1;

    while ( i )
    {
        string_destroy ( lines[ i - 1 ] );
        i -= 1;
    }

    return string;
}

u64
memory_allocation_count
( void )
{
    if ( state )
    {
        return ( *state ).stat.allocation_count;
    }
    return 0;
}

u64
memory_free_count
( void )
{
    if ( state )
    {
        return ( *state ).stat.free_count;
    }
    return 0;
}
