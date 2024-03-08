/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file container/freelist.c
 * @brief Implementation of the container/freelist header.
 * (see container/freelist.h for additional detailist)
 */
#include "container/freelist.h"

#include "core/logger.h"
#include "core/memory.h"
#include "core/string.h"

#include "math/math.h"

/** @brief Type definition for a freelist node. */
typedef struct node_t
{
    u64             offset;
    u64             size;
    struct node_t*  next;

}
node_t;

/** @brief Type definition for internal state. */
typedef struct
{
    u64     capacity;
    u64     max_entries;
    node_t* head;
    node_t* freelist;
}
state_t;

/**
 * @brief Obtains a handle to the first empty node within a freelist.
 * 
 * @param freelist The freelist to query.  Must be non-zero.
 * @return A handle to the first empty block within freelist.
 */
node_t*
freelist_get_node
(   freelist_t* freelist
);

/**
 * @brief Clears a node within a freelist.
 * 
 * @param node The freelist node to clear. Must be non-zero.
 */
void
freelist_return_node
(   node_t* node
);

bool
freelist_init
(   u64         capacity
,   u64*        memory_requirement
,   void*       memory
,   freelist_t* freelist
)
{
    if ( !memory_requirement )
    {
        LOGERROR ( "freelist_init: Missing argument: memory_requirement." );
        return false;
    }

    const u64 max_entries = MAX ( 20U
                                , capacity / ( sizeof ( void* ) * sizeof ( node_t ) )
                                );

    *memory_requirement = sizeof ( state_t ) + sizeof ( node_t ) * max_entries;

    if ( !memory )
    {
        return true;
    }

    if ( !freelist )
    {
        LOGERROR ( "freelist_init: Missing argument: freelist (output buffer)." );
        return false;
    }

    u64 minimum_recommended_capacity = ( sizeof ( state_t ) + sizeof ( node_t ) ) * 8;
    if ( capacity < minimum_recommended_capacity )
    {
        f64 arg_amount;
        f64 min_amount;
        const char* arg_unit = string_bytesize ( capacity , &arg_amount );
        const char* min_unit = string_bytesize ( minimum_recommended_capacity , &min_amount );
        LOGWARN ( "Requested freelist with capacity of %.2f %s."
                  "\n\tNOTE:  Freelist is inefficient when handling less than %.2f %s."
                , &arg_amount , arg_unit
                , &min_amount , min_unit
                );
    }

    ( *freelist ).memory = memory;

    memory_clear ( ( *freelist ).memory , *memory_requirement );
    
    state_t* state = ( *freelist ).memory;
    ( *state ).freelist = ( *freelist ).memory + sizeof ( state_t );
    ( *state ).max_entries = max_entries;
    ( *state ).capacity = capacity;

    memory_clear ( ( *state ).freelist
                 , sizeof ( node_t ) * ( *state ).max_entries
                 );
    ( *state ).head = &( *state ).freelist[ 0 ];
    ( *( ( *state ).head ) ).offset = 0;
    ( *( ( *state ).head ) ).size = capacity;
    ( *( ( *state ).head ) ).next = 0;

    return true;
}

void
freelist_clear
(   freelist_t* freelist
)
{
    if ( !freelist || !( *freelist ).memory )
    {
        return;
    }

    state_t* state = ( *freelist ).memory;
    memory_clear ( ( *freelist ).memory
                 , sizeof ( state_t ) + sizeof ( node_t ) * ( *state ).max_entries
                 );
    ( *freelist ).memory = 0;
}

bool
freelist_allocate
(   freelist_t* freelist
,   u64         size
,   u64*        offset
)
{
    if ( !freelist || !offset || !( *freelist ).memory )
    {
        if ( !freelist )
        {
            LOGERROR ( "freelist_allocate: Missing argument: freelist." );
        }
        if ( !offset )
        {
            LOGERROR ( "freelist_allocate: Missing argument: offset." );
        }
        if ( freelist && ( *freelist ).memory )
        {
            LOGERROR ( "freelist_allocate: The provided freelist is uninitialized." );
        }
        return false;
    }

    state_t* state = ( *freelist ).memory;
    node_t* current_node = ( *state ).head;
    node_t* previous_node = 0;

    while ( current_node )
    {
        if ( ( *current_node ).size == size )
        {
            *offset = ( *current_node ).offset;
            
            node_t* output_node = 0;
            if ( previous_node )
            {
                ( *previous_node ).next = ( *current_node ).next;
                output_node = current_node;
            }
            else
            {
                output_node = ( *state ).head;
                ( *state ).head = ( *current_node ).next;
            }

            freelist_return_node ( output_node );
            return true;
        }
        else if ( ( *current_node ).size > size )
        {
            *offset = ( *current_node ).offset;
            ( *current_node ).size -= size;
            ( *current_node ).offset += size;
            return true;
        }

        previous_node = current_node;
        current_node = ( *current_node ).next;
    }

    f64 req_amount;    
    f64 rem_amount;
    const char* req_unit = string_bytesize ( size , &req_amount );
    const char* rem_unit = string_bytesize ( freelist_query_free ( freelist ) , &rem_amount );
    LOGWARN ( "freelist_allocate: No block with enough free space found (requested: %.2f %s, available: %.2f %s)."
            , &req_amount , req_unit
            , &rem_amount , rem_unit
            );

    return false;
}

bool
freelist_free
(   freelist_t* freelist
,   u64         size
,   u64         offset
)
{
    if ( !freelist || !size || !( *freelist ).memory )
    {
        if ( !freelist )
        {
            LOGERROR ( "freelist_allocate: Missing argument: freelist." );
        }
        if ( !size )
        {
            LOGERROR ( "freelist_allocate: Value of size argument must be non-zero." );
        }
        if ( freelist && ( *freelist ).memory )
        {
            LOGERROR ( "freelist_allocate: The provided freelist is uninitialized." );
        }
        return false;
    }

    state_t* state = ( *freelist ).memory;
    node_t* current_node = ( *state ).head;
    node_t* previous_node = 0;

    if ( !current_node )
    {
        node_t* new_node = freelist_get_node ( freelist );
        ( *new_node ).offset = offset;
        ( *new_node ).size = size;
        ( *new_node ).next = 0;
        ( *state ).head = new_node;
        return true;
    }

    while ( current_node )
    {
        if ( ( *current_node ).offset + ( *current_node ).size == offset )
        {
            ( *current_node ).size += size;
            if (   ( *current_node ).next
                && ( *current_node ).offset + ( *current_node ).size == ( *( ( *current_node ).next ) ).offset
               )
            {
                ( *current_node ).size += ( *( ( *current_node ).next ) ).size;
                node_t* output_node = ( *current_node ).next;
                ( *current_node ).next = ( *( ( *current_node ).next ) ).next;
                freelist_return_node ( output_node );
            }
            return true;
        }
        else if ( ( *current_node ).offset == offset )
        {
            LOGERROR ( "freelist_free: Double free occurred at memory offset %@."
                     , ( *current_node ).offset
                     );
            return false;
        }
        else if ( ( *current_node ).offset > offset )
        {
            node_t* new_node = freelist_get_node ( freelist );
            ( *new_node ).offset = offset;
            ( *new_node ).size = size;

            if ( previous_node )
            {
                ( *previous_node ).next = new_node;
                ( *new_node ).next = current_node;
            }
            else
            {
                ( *new_node ).next = current_node;
                ( *state ).head = new_node;
            }

            if (    ( *new_node ).next
                 && ( *new_node ).offset + ( *new_node ).size == ( *( ( *new_node ).next ) ).offset
               )
            {
                ( *new_node ).size += ( *( ( *new_node ).next ) ).size;
                node_t* output_node = ( *new_node ).next;
                ( *new_node ).next = ( *output_node ).next;
                freelist_return_node ( output_node );
            }
            if (    previous_node
                 && ( *previous_node ).offset + ( *previous_node ).size == ( *new_node ).offset
               )
            {
                ( *previous_node ).size += ( *new_node ).size;
                node_t* output_node = new_node;
                ( *previous_node ).next = ( *output_node ).next;
                freelist_return_node ( output_node );
            }

            return true;
        }

        if (    !( *current_node ).next
             && ( *current_node ).offset + ( *current_node ).size < offset
           )
        {
            node_t* new_node = freelist_get_node ( freelist );
            ( *new_node ).offset = offset;
            ( *new_node ).size = size;
            ( *new_node ).next = 0;
            ( *current_node ).next = new_node;
            return true;
        }

        previous_node = current_node;
        current_node = ( *current_node ).next;

    }// End while.

    LOGWARN ( "freelist_free: Did not find a block to free. Memory corruption probable." );
    return false;
}

bool
freelist_resize
(   freelist_t* freelist
,   u64*        memory_requirement
,   void*       new_memory
,   u64         new_capacity
,   void**      old_memory
)
{
    if (    !freelist
         || !memory_requirement
         || ( *( ( state_t* )( ( *freelist ).memory ) ) ).capacity > new_capacity
       )
    {
        return false;
    }

    const u64 max_entries = MAX ( 20U
                                , new_capacity / ( sizeof ( void* ) )
                                );

    *memory_requirement = sizeof ( state_t ) + sizeof ( node_t ) * max_entries;

    if ( !new_memory )
    {
        return true;
    }

    *old_memory = ( *freelist ).memory;

    state_t* state_old = ( *freelist ).memory;
    const u64 capacity_difference = new_capacity - ( *state_old ).capacity;

    ( *freelist ).memory = new_memory;

    memory_clear ( ( *freelist ).memory , *memory_requirement );

    state_t* state = ( *freelist ).memory;
    ( *state ).freelist = ( *freelist ).memory + sizeof ( state_t );
    ( *state ).max_entries = max_entries;
    ( *state ).capacity = new_capacity;

    memory_clear ( ( *state ).freelist
                 , sizeof ( node_t ) * ( *state ).max_entries
                 );

    ( *state ).head = &( *state ).freelist[ 0 ];

    node_t* current_node_new = ( *state ).head;
    node_t* current_node_old = ( *state_old ).head;

    if ( !current_node_old )
    {
        ( *( ( *state ).head ) ).offset = ( *state_old ).capacity;
        ( *( ( *state ).head ) ).size = capacity_difference;
        ( *( ( *state ).head ) ).next = 0;
        return true;
    }
    
    while ( current_node_old )
    {
        node_t* new_node = freelist_get_node ( freelist );

        ( *new_node ).offset = ( *current_node_old ).offset;
        ( *new_node ).size = ( *current_node_old ).size;
        ( *new_node ).next = 0;
        ( *current_node_new ).next = new_node;

        current_node_new = ( *current_node_new ).next;
        
        if ( ( *current_node_old ).next )
        {
            current_node_old = ( *current_node_old ).next;
        }
        else
        {
            if ( ( *current_node_old ).offset + ( *current_node_old ).size == ( *state_old ).capacity )
            {
                ( *new_node ).size += capacity_difference;
            }
            else
            {
                node_t* new_tail = freelist_get_node ( freelist );
                ( *new_tail ).offset = ( *state_old ).capacity;
                ( *new_tail ).size = capacity_difference;
                ( *new_tail ).next = 0;
                ( *new_node ).next = new_tail;
            }

            break;
        }
    }

    return true;
}

void
freelist_reset
(   freelist_t* freelist
)
{
    if ( !freelist || !( *freelist ).memory )
    {
        return;
    }

    state_t* state = ( *freelist ).memory;

    memory_clear ( ( *state ).freelist
                 , sizeof ( node_t ) * ( *state ).max_entries
                 );

    ( *( ( *state ).head ) ).offset = 0;
    ( *( ( *state ).head ) ).size = ( *state ).capacity;
    ( *( ( *state ).head ) ).next = 0;
}

// Expensive!
u64
freelist_query_free
(   freelist_t* freelist
)
{
    if ( !freelist || !( *freelist ).memory )
    {
        return 0;
    }

    u64 sum = 0;
    state_t* state = ( *freelist ).memory;
    node_t* node = ( *state ).head;

    while ( node )
    {
        sum += ( *node ).size;
        node = ( *node ).next;
    }

    return sum;
}

node_t*
freelist_get_node
(   freelist_t* freelist
)
{
    state_t* state = ( *freelist ).memory;
    
    for ( u64 i = 1; i < ( *state ).max_entries; ++i )
    {
        if ( ( *state ).freelist[ i ].size == 0 )
        {
            ( *state ).freelist[ i ].next = 0;
            ( *state ).freelist[ i ].offset = 0;
            return &( *state ).freelist[ i ];
        }
    }

    return 0;
}

void
freelist_return_node
(   node_t* node
)
{
    ( *node ).offset = 0;
    ( *node ).size = 0;
    ( *node ).next = 0;
}
