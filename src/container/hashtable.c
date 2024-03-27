/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file container/hashtable.c
 * @brief Implementation of the container/hashtable header.
 * (see container/hashtable.h for additional detailist)
 */
#include "container/hashtable.h"

#include "core/logger.h"
#include "core/memory.h"

/** @brief Type definition for internal state. */
typedef struct
{
    u64     stride;
    u32     capacity;
    bool    pointer;
    bool    owns_memory;
    void*   content;
}
state_t;

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
,   u64*            memory_requirement_
,   void*           memory_
,   hashtable_t**   hashtable
)
{
    if ( pointer )
    {
        stride = sizeof ( void* );
    }
    if ( !capacity || !stride )
    {
        if ( !capacity )
        {
            LOGERROR ( "hashtable_create: Value of capacity argument must be non-zero." );
        }
        if ( !stride )
        {
            LOGERROR ( "hashtable_create: Value of stride argument must be non-zero." );
        }
        return false;
    }

    const u64 memory_requirement = sizeof ( state_t ) + capacity * stride;
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
        memory = memory_allocate ( memory_requirement , MEMORY_TAG_HASHTABLE );
    }

    if ( !hashtable )
    {
        LOGERROR ( "hashtable_create: Missing argument: hashtable (output buffer)." );
        if ( !memory_ )
        {
            memory_free ( memory , memory_requirement , MEMORY_TAG_HASHTABLE );
        }
        return false;
    }

    memory_clear ( memory , memory_requirement );

    state_t* state = memory;
    ( *state ).capacity = capacity;
    ( *state ).stride = stride;
    ( *state ).pointer = pointer;
    ( *state ).owns_memory = !memory_;
    ( *state ).content = ( void* )( ( ( u64 ) memory ) + sizeof ( state_t ) );

    *hashtable = state;
    return true;
}

void
hashtable_destroy
(   hashtable_t** hashtable
)
{
    if ( !hashtable )
    {
        return;
    }

    state_t* state = *hashtable;
    if ( !state )
    {
        return;
    }

    const u64 memory_requirement = sizeof ( state_t ) + ( *state ).capacity
                                                      * ( *state ).stride
                                                      ;
    if ( ( *state ).owns_memory )
    {
        memory_free ( state , memory_requirement , MEMORY_TAG_HASHTABLE );
    }
    else
    {
        memory_clear ( state , memory_requirement );
    }

    *hashtable = 0;
}

u64
hashtable_stride
(   const hashtable_t* hashtable
)
{
    return ( *( ( state_t* ) hashtable ) ).stride;
}

u64
hashtable_capacity
(   const hashtable_t* hashtable
)
{
    return ( *( ( state_t* ) hashtable ) ).capacity;
}

bool
hashtable_pointer
(   const hashtable_t* hashtable
)
{
    return ( *( ( state_t* ) hashtable ) ).pointer;
}

bool
hashtable_owns_memory
(   const hashtable_t* hashtable
)
{
    return ( *( ( state_t* ) hashtable ) ).owns_memory;
}

bool
hashtable_set
(   hashtable_t*    hashtable
,   const char*     key
,   const void*     value
)
{
    state_t* state = hashtable;
    if ( !value && !( *state ).pointer )
    {
        LOGERROR ( "hashtable_set: Missing argument: value." );
        return false;
    }

    const u64 index = hashtable_key_hash ( key , ( *state ).capacity );
    if ( ( *state ).pointer )
    {
        memory_copy ( ( void* )( ( ( u64 )( ( *state ).content ) )
                               + index * ( *state ).stride
                               )
                    , &value
                    , ( *state ).stride
                    );
    }
    else
    {
        memory_copy ( ( void* )( ( ( u64 )( ( *state ).content ) )
                               + index * ( *state ).stride
                               )
                    , value
                    , ( *state ).stride
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
    state_t* state = hashtable;
    const u64 index = hashtable_key_hash ( key , ( *state ).capacity );
    memory_copy ( value
                , ( void* )( ( ( u64 )( ( *state ).content ) )
                           + index * ( *state ).stride
                           )
                , ( *state ).stride
                );
    return true;
}

bool
hashtable_fill
(   hashtable_t*    hashtable
,   void*           value
)
{
    state_t* state = hashtable;

    if ( ( *state ).pointer )
    {
        LOGERROR ( "hashtable_fill: May not be used on a pointer-valued hashtable." );
        return false;
    }

    for ( u64 i = 0; i < ( *state ).capacity; ++i )
    {
        memory_copy ( ( void* )( ( ( u64 )( ( *state ).content ) )
                               + i * ( *state ).stride
                               )
                    , value
                    , ( *state ).stride
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
