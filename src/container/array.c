/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/array.c
 * @brief Implementation of the container/array header.
 * (see container/array.h for additional details)
 */
#include "container/array.h"

#include "core/logger.h"
#include "core/memory.h"

void*
_array_create
(   ARRAY_FIELD initial_capacity
,   ARRAY_FIELD stride
)
{
    const u64 header_size = ARRAY_FIELD_COUNT * sizeof ( u64 );
    const u64 content_size = initial_capacity * stride;
    const u64 size = header_size + content_size;

    u64* array = memory_allocate ( size , MEMORY_TAG_ARRAY );
    memory_clear ( array , size );

    array[ ARRAY_FIELD_CAPACITY ] = initial_capacity;
    array[ ARRAY_FIELD_LENGTH ]   = 0;
    array[ ARRAY_FIELD_STRIDE ]   = stride;

    return ( void* )( array + ARRAY_FIELD_COUNT );
}

void
_array_destroy
(   void* array
)
{
    if ( !array )
    {
        return;
    }
    
    memory_free ( ( ( u64* ) array ) - ARRAY_FIELD_COUNT
                , array_size ( array )
                , MEMORY_TAG_ARRAY
                );
}

void*
array_copy
(   void* src
)
{
    const u64 length = array_length ( src );
    const u64 stride = array_stride ( src );
    void* copy = _array_create ( length , stride );
    memory_copy ( copy , src , length );
    _array_field_set ( copy , ARRAY_FIELD_LENGTH , length );
    return copy;
}

u64
_array_field_get
(   const void* array
,   ARRAY_FIELD field
)
{
    const u64* header = ( ( u64* ) array ) - ARRAY_FIELD_COUNT;
    return header[ field ];
}

void
_array_field_set
(   void*       array
,   ARRAY_FIELD field
,   u64         value
)
{
    u64* header = ( ( u64* ) array ) - ARRAY_FIELD_COUNT;
    header[ field ] = value;
}

u64
_array_size
(   const void* array
)
{
    u64* header = ( ( u64* ) array ) - ARRAY_FIELD_COUNT;
    const u64 header_size = ARRAY_FIELD_COUNT * sizeof ( u64 );
    const u64 content_size = header[ ARRAY_FIELD_STRIDE ]
                           * header[ ARRAY_FIELD_CAPACITY ]
                           ;
    return header_size + content_size;
}

void*
_array_resize
(   void*   old_array
,   u64     minimum_capacity
)
{
    const u64 length = array_length ( old_array );
    const u64 stride = array_stride ( old_array );

    void* new_array = _array_create ( ARRAY_SCALE_FACTOR ( minimum_capacity )
                                    , stride
                                    );
    memory_copy ( new_array , old_array , length * stride );
    _array_field_set ( new_array , ARRAY_FIELD_LENGTH , length );

    _array_destroy ( old_array );
    
    return new_array;
}

void*
_array_push
(   void*       array
,   const void* src
)
{
    const u64 length = array_length ( array );
    const u64 stride = array_stride ( array );

    if ( length >= array_capacity ( array ) )
    {
        array = _array_resize ( array , length );
    }

    const u64 dst = ( ( u64 ) array ) + length * stride;
    memory_copy ( ( void* ) dst , src , stride );
    _array_field_set ( array , ARRAY_FIELD_LENGTH , length + 1 );

    return array;
}

void
_array_pop
(   void* array
,   void* dst
)
{
    if ( !array_length ( array ) )
    {
        LOGWARN ( "_array_pop: Array is empty." );
        return;
    }

    const u64 length = array_length ( array ) - 1;
    const u64 stride = array_stride ( array );

    const u64 src = ( ( u64 ) array ) + length * stride;
    if ( dst )
    {
        memory_copy ( dst , ( void* ) src , stride );
    }
    _array_field_set ( array , ARRAY_FIELD_LENGTH , length );
}

void*
_array_insert
(   void*       array
,   u64         index
,   const void* src
)
{
    const u64 length = array_length ( array );
    const u64 stride = array_stride ( array );
    
    if ( index > length )
    {
        LOGERROR ( "_array_insert: Called with out of bounds index: %i (index) > %i (array length)."
                 , index , length
                 );
        return array;
    }
    
    if ( length >= array_capacity ( array ) )
    {
        array = _array_resize ( array , length );
    }
    
    const u64 dst = ( ( u64 ) array );
    memory_move ( ( void* )( dst + ( index + 1 ) * stride )
                , ( void* )( dst + index * stride )
                , ( length - index ) * stride
                );
    memory_copy ( ( void* )( dst + index * stride ) , src , stride );
    _array_field_set ( array , ARRAY_FIELD_LENGTH , length + 1 );

    return array;
}

void*
_array_remove
(   void*   array
,   u64     index
,   void*   dst
)
{
    if ( !array_length ( array ) )
    {
        LOGWARN ( "_array_remove: Array is empty." );
        return array;
    }

    const u64 length = array_length ( array ) - 1;
    const u64 stride = array_stride ( array );
    
    if ( index > length )
    {
        LOGERROR ( "_array_remove: Called with out of bounds index: %i (index) >= %i (array length)."
                 , index , length + 1
                 );
        return array;
    }

    const u64 src = ( ( u64 ) array );
    if ( dst )
    {
        memory_copy ( dst , ( void* )( src + index * stride ) , stride );
    }
    memory_move ( ( void* )( src + index * stride )
                , ( void* )( src + ( index + 1 ) * stride )
                , ( length - index ) * stride
                );
    _array_field_set ( array , ARRAY_FIELD_LENGTH , length );

    return array;
}
