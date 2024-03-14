/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file container/hashtable.c
 * @brief Implementation of the container/hashtable header.
 * (see container/hashtable.h for additional detailist)
 */
#include "container/hashtable.h"

#include "core/logger.h"
#include "core/memory.h"

/**
 * @brief Key hash generation.
 * 
 * @param name Key name string. Must be non-zero.
 * @param hashtable_capacity Hashtable capacity.
 * @return Key hashcode.
 */
u64
hashtable_key_hash
(   const char* name
,   const u32   hashtable_capacity
);

bool
hashtable_create
(   bool            pointer
,   u64             stride
,   u32             capacity
,   void*           memory
,   hashtable_t*    hashtable
)
{
    if ( !hashtable || !capacity || ( !stride && !pointer ) )
    {
        if ( !hashtable )
        {
            LOGERROR ( "hashtable_create: Missing argument: hashtable (output buffer)." );
        }
        if ( !capacity )
        {
            LOGERROR ( "hashtable_create: Value of capacity argument must be non-zero." );
        }
        if ( !stride && !pointer )
        {
            LOGERROR ( "hashtable_create: Value of stride argument must be non-zero." );
        }
        return false;
    }

    ( *hashtable ).capacity = capacity;
    ( *hashtable ).stride = pointer ? sizeof ( void* ) : stride;
    ( *hashtable ).pointer = pointer;
    ( *hashtable ).owns_memory = !memory;
    
    const u64 size = ( *hashtable ).capacity * ( *hashtable ).stride;

    if ( memory )
    {
        ( *hashtable ).memory = memory;
    }
    else
    {
        ( *hashtable ).memory = memory_allocate ( size
                                                , MEMORY_TAG_HASHTABLE
                                                );
    }
    
    memory_clear ( ( *hashtable ).memory , size );
    
    return true;
}

void
hashtable_destroy
(   hashtable_t* hashtable
)
{
    if ( !hashtable )
    {
        return;
    }
    if ( ( *hashtable ).owns_memory && ( *hashtable ).memory )
    {
        memory_free ( ( *hashtable ).memory
                    , ( *hashtable ).stride * ( *hashtable ).capacity
                    , MEMORY_TAG_HASHTABLE
                    );
    }
    memory_clear ( hashtable , sizeof ( hashtable_t ) );
}

bool
hashtable_set
(   hashtable_t*    hashtable
,   const char*     key
,   const void*     value
)
{
    if (   !hashtable
        || !key
        || !( *hashtable ).memory
        || ( !value && !( *hashtable ).pointer )
       )
    {
        if ( !hashtable )
        {
            LOGERROR ( "hashtable_set: Missing argument: hashtable." );
        }
        else if ( !( *hashtable ).memory )
        {
            LOGERROR ( "hashtable_set: The provided hashtable is uninitialized (%@)."
                     , hashtable
                     );
        }
        if ( !key )
        {
            LOGERROR ( "hashtable_set: Missing argument: key." );
        }
        if ( !value && hashtable && !( *hashtable ).pointer )
        {
            LOGERROR ( "hashtable_set: Missing argument: value." );
        }
        return false;
    }

    const u64 index = hashtable_key_hash ( key , ( *hashtable ).capacity );
    if ( ( *hashtable ).pointer )
    {
        memory_copy ( ( *hashtable ).memory + ( *hashtable ).stride * index
                    , &value
                    , ( *hashtable ).stride
                    );
    }
    else
    {
        memory_copy ( ( *hashtable ).memory + ( *hashtable ).stride * index
                    , value
                    , ( *hashtable ).stride
                    );
    }
    return true;
}

bool
hashtable_get
(   hashtable_t*    hashtable
,   const char*     key
,   void*           value
)
{
    if ( !hashtable || !key || !value || !( *hashtable ).memory )
    {
        if ( !hashtable )
        {
            LOGERROR ( "hashtable_get: Missing argument: hashtable." );
        }
        else if ( !( *hashtable ).memory )
        {
            LOGERROR ( "hashtable_get: The provided hashtable is uninitialized (%@)."
                     , hashtable
                     );
        }
        if ( !key )
        {
            LOGERROR ( "hashtable_get: Missing argument: key." );
        }
        if ( !value )
        {
            LOGERROR ( "hashtable_get: Missing argument: value (output buffer)." );
        }
        return false;
    }

    const u64 index = hashtable_key_hash ( key , ( *hashtable ).capacity );
    memory_copy ( value
                , ( *hashtable ).memory + ( *hashtable ).stride * index
                , ( *hashtable ).stride
                );
    return true;
}

bool
hashtable_fill
(   hashtable_t*    hashtable
,   void*           value
)
{
    if ( !hashtable || !value || !( *hashtable ).memory )
    {
        if ( !hashtable )
        {
            LOGERROR ( "hashtable_fill: Missing argument: hashtable." );
        }
        else if ( !( *hashtable ).memory )
        {
            LOGERROR ( "hashtable_fill: The provided hashtable is uninitialized (%@)."
                     , hashtable
                     );
        }
        if ( !value )
        {
            LOGERROR ( "hashtable_fill: Missing argument: value." );
        }
        return false;
    }

    if ( ( *hashtable ).pointer )
    {
        LOGERROR ( "hashtable_fill: May not be used on a pointer-valued hashtable." );
        return false;
    }

    for ( u64 i = 0; i < ( *hashtable ).capacity; ++i )
    {
        memory_copy ( ( *hashtable ).memory + ( ( *hashtable ).stride * i )
                    , value
                    , ( *hashtable ).stride
                    );
    }
    return true;
}

u64
hashtable_key_hash
(   const char* name
,   const u32   hashtable_capacity
)
{
    static const u64 prime = 97;
    u64 hash = 0;
    for ( const u8* i = ( const u8* ) name; *i; ++i )
    {
        hash = hash * prime + *i;
    }
    hash %= hashtable_capacity;
    return hash;
}
