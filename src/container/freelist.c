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
    bool    owns_memory;
    node_t* head;
    node_t* content;
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
freelist_create
(   u64             capacity
,   u64*            memory_requirement_
,   void*           memory_
,   freelist_t**    freelist
)
{
    if ( !capacity )
    {
        LOGERROR ( "freelist_create: Value of capacity argument must be non-zero." );
        return false;
    }

    const u64 max_entries = MAX ( 20U
                                , capacity / ( sizeof ( void* ) * sizeof ( node_t ) )
                                );
    const u64 memory_requirement = sizeof ( state_t ) + max_entries
                                                      * sizeof ( node_t )
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
        memory = memory_allocate ( memory_requirement , MEMORY_TAG_FREELIST );
    }

    if ( !freelist )
    {
        LOGERROR ( "freelist_create: Missing argument: freelist (output buffer)." );
        if ( !memory_ )
        {
            memory_free ( memory , memory_requirement , MEMORY_TAG_FREELIST );
        }
        return false;
    }

    u64 minimum_recommended_capacity = 8 * ( sizeof ( state_t ) + sizeof ( node_t ) );
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

    memory_clear ( memory , memory_requirement );
    
    state_t* state = memory;
    ( *state ).owns_memory = !memory_;
    ( *state ).content = ( void* )( ( ( u64 ) memory ) + sizeof ( state_t ) );
    ( *state ).max_entries = max_entries;
    ( *state ).capacity = capacity;
    ( *state ).head = &( *state ).content[ 0 ];
    ( *( ( *state ).head ) ).offset = 0;
    ( *( ( *state ).head ) ).size = capacity;
    ( *( ( *state ).head ) ).next = 0;

    *freelist = state;
    return true;
}

void
freelist_destroy
(   freelist_t** freelist
)
{
    if ( !freelist )
    {
        return;
    }

    state_t* state = *freelist;
    if ( !state )
    {
        return;
    }

    const u64 memory_requirement = sizeof ( state_t ) + ( *state ).max_entries
                                                      * sizeof ( node_t )
                                                      ;
    if ( ( *state ).owns_memory )
    {
        memory_free ( state , memory_requirement , MEMORY_TAG_FREELIST );
    }
    else
    {
        memory_clear ( state , memory_requirement );
    }

    *freelist = 0;
}

bool
freelist_owns_memory
(   const freelist_t* freelist
)
{
    return ( *( ( state_t* ) freelist ) ).owns_memory;
}

bool
freelist_allocate
(   freelist_t* freelist
,   u64         size
,   u64*        offset
)
{
    state_t* state = freelist;
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
    state_t* state = freelist;
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
(   freelist_t**    freelist
,   u64             minimum_capacity
,   u64*            memory_requirement_
,   void*           new_memory_
,   void**          old_memory_
)
{
    if ( !freelist )
    {
        LOGERROR ( "freelist_resize: Missing argument: freelist." );
        return false;
    }

    state_t* state = *freelist;
    if ( ( *state ).capacity >= minimum_capacity )
    {
        return false;
    }

    const u64 max_entries = MAX ( 20U
                                , minimum_capacity / ( sizeof ( void* ) )
                                );
    const u64 memory_requirement = sizeof ( state_t ) + sizeof ( node_t )
                                                      * max_entries
                                                      ;

    void* old_memory = state;
    void* new_memory;
    if ( memory_requirement_ )
    {
        *memory_requirement_ = memory_requirement;
        if ( !new_memory_ )
        {
            return true;
        }
        new_memory = new_memory_;
        if ( !old_memory_ )
        {
            LOGERROR ( "freelist_resize: Missing argument: old_memory (output buffer)." );
            return false;
        }
        *old_memory_ = old_memory;
    }
    else
    {
        new_memory = memory_allocate ( memory_requirement
                                     , MEMORY_TAG_FREELIST
                                     );
    }

    state_t* old_state = old_memory;
    const u64 capacity_difference = minimum_capacity - ( *old_state ).capacity;

    state = new_memory;
    memory_clear ( state , memory_requirement );
    ( *state ).owns_memory = !new_memory_;
    ( *state ).content = ( void* )( ( ( u64 ) new_memory_ )
                                  + sizeof ( state_t )
                                  );
    ( *state ).max_entries = max_entries;
    ( *state ).capacity = minimum_capacity;
    ( *state ).head = &( *state ).content[ 0 ];

    node_t* current_node_new = ( *state ).head;
    node_t* current_node_old = ( *old_state ).head;

    if ( !current_node_old )
    {
        ( *( ( *state ).head ) ).offset = ( *old_state ).capacity;
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
            if ( ( *current_node_old ).offset + ( *current_node_old ).size == ( *old_state ).capacity )
            {
                ( *new_node ).size += capacity_difference;
            }
            else
            {
                node_t* new_tail = freelist_get_node ( freelist );
                ( *new_tail ).offset = ( *old_state ).capacity;
                ( *new_tail ).size = capacity_difference;
                ( *new_tail ).next = 0;
                ( *new_node ).next = new_tail;
            }

            break;
        }
    }

    *freelist = state;
    return true;
}

void
freelist_reset
(   freelist_t* freelist
)
{
    state_t* state = freelist;
    memory_clear ( ( *state ).content
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
    state_t* state = freelist;
    u64 sum = 0;
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
    state_t* state = freelist;
    for ( u64 i = 1; i < ( *state ).max_entries; ++i )
    {
        if ( ( *state ).content[ i ].size == 0 )
        {
            ( *state ).content[ i ].next = 0;
            ( *state ).content[ i ].offset = 0;
            return &( *state ).content[ i ];
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
