/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file core/memory.c
 * @brief Implementation of the core/memory header.
 * (see core/memory.h for additional details)
 */
#include "core/memory.h"

#include "container/string.h"

#include "core/logger.h"
#include "core/mutex.h"

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
    bool                initialized;

    stat_t              stat;
    
    u64                 allocator_capacity;
    dynamic_allocator_t allocator;
    void*               allocator_start;

    u64                 capacity;
    void*               memory;

    mutex_t             allocation_mutex;
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
        LOGERROR ( "memory_startup: Called more than once." );
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
        LOGFATAL ( "memory_startup: Failed to initialize internal allocator." );
        return false;
    }

    if ( !mutex_create ( &( *state ).allocation_mutex ) )
    {
        LOGFATAL ( "memory_startup: Failed to initialize the mutex data structure employed by thread-safe memory operations." );
        return false;
    }

    ( *state ).initialized = true;

    LOGDEBUG ( "  Success." );

    return true;
}

void
memory_shutdown
( void )
{
    if ( !state || !( *state ).initialized )
    {
        return;
    }

    ( *state ).initialized = false;

    mutex_destroy ( &( *state ).allocation_mutex );

    dynamic_allocator_clear ( &( *state ).allocator );

    if ( ( *state ).stat.allocation_count != ( *state ).stat.free_count )
    {
        LOGDEBUG ( "memory_shutdown: Noticed allocation count (%i) != free count (%i) when shutting down memory subsystem."
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
    if ( tag == MEMORY_TAG_UNKNOWN )
    {
        LOGWARN ( "memory_allocate: Called with MEMORY_TAG_UNKNOWN." );
    }

    void* memory;
    if ( state && ( *state ).initialized )
    {
        if ( !mutex_lock ( &( *state ).allocation_mutex ) )
        {
            LOGFATAL ( "memory_allocate: Could not obtain mutex lock. Aborting operation." );
            return 0;
        }
        memory = dynamic_allocator_allocate_aligned ( &( *state ).allocator
                                                    , size
                                                    , alignment
                                                    );
        if ( memory )
        {
            ( *state ).stat.allocated += size;
            ( *state ).stat.tagged_allocations[ tag ] += size;
            ( *state ).stat.allocation_count += 1;
        }
        if ( !mutex_unlock ( &( *state ).allocation_mutex ) )
        {
            LOGERROR ( "memory_allocate: Failed to release the mutex lock." );
        }
    }
    else
    {   // Failsafe for if memory subsystem is not initialized.
        memory = platform_memory_allocate ( size );
    }
    if ( memory )
    {
        memory_clear ( memory , size );
    }
    else
    {
        LOGFATAL ( "memory_allocate: Failed to allocate memory." );
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
    if ( tag == MEMORY_TAG_UNKNOWN )
    {
        LOGWARN ( "memory_free: Called with MEMORY_TAG_UNKNOWN." );
    }

    if ( state && ( *state ).initialized )
    {
        if ( !mutex_lock ( &( *state ).allocation_mutex ) )
        {
            LOGFATAL ( "memory_free: Could not obtain mutex lock. Heap corruption is likely." );
            return;
        }
        bool success = dynamic_allocator_free_aligned ( &( *state ).allocator
                                                      , memory
                                                      );
        if ( success )
        {
            ( *state ).stat.allocated -= size;
            ( *state ).stat.tagged_allocations[ tag ] -= size;
            ( *state ).stat.free_count += 1;
        }
        if ( !mutex_unlock ( &( *state ).allocation_mutex ) )
        {
            LOGERROR ( "memory_free: Failed to release mutex lock." );
        }
        if ( !success )
        {
            /**
             * If the free operation failed, try freeing the memory on the
             * platform level as a failsafe.
             */
            platform_memory_free ( memory );
            // . . .gee, I sure hope that worked. . .
        }
    }
    else
    {   // Failsafe for if memory subsystem is not initialized.
        platform_memory_free ( memory );
    }
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
    return platform_memory_equal ( s1 , s2 , size );
}

char*
memory_stat
( void )
{
    if ( !state || !( *state ).initialized )
    {
        LOGERROR ( "memory_stat: Called before the memory subsystem was initialized." );
        return 0;
    }

    char* lines[ MEMORY_TAG_COUNT + 2 ];
    char* string = string_create_from ( "System memory usage:\n" );
    const char* unit;
    f64 amount;
    u64 i = 0;
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
    if ( !state )
    {
        return 0;
    }
    return ( *state ).stat.allocation_count;
}

u64
memory_free_count
( void )
{
    if ( !state )
    {
        return 0;
    }
    return ( *state ).stat.free_count;
}
