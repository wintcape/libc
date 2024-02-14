/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/string.c
 * @brief Implementation of the container/string header.
 * (see container/string.h for additional details)
 */
#include "container/string.h"

#include "core/logger.h"
#include "core/memory.h"

#include "math/math.h"

char*
_string_create
(   ARRAY_FIELD initial_capacity
)
{
    char* string = array_create ( char , initial_capacity );
    _array_field_set ( string , ARRAY_FIELD_LENGTH , 1 );
    return string;//            ^ terminator
}

char*
_string_copy
(   const char* src
,   const u64   src_length
)
{//                               v terminator
    const u64 size = src_length + 1;
    char* string = array_create ( char , size );
    _array_field_set ( string , ARRAY_FIELD_LENGTH , size );
    memory_move ( string , src , size );
    return string;
}

void
string_destroy
(   char* string
)
{
    if ( !string )
    {
        return;
    }
    array_destroy ( string );
}

u64
string_length
(   const char* string
)
{
    return array_length ( string ) - 1;
}

char*
__string_push
(   char*       string
,   const char* src
,   const u64   src_length
)
{
    const u64 old_size = array_length ( string );
    const u64 new_size = old_size + src_length;
    const u64 new_length = string_length ( string ) + src_length;
    const u64 stride = array_stride ( string );

    if ( new_size >= array_capacity ( string ) )
    {
        // Artificially grow the capacity to force resize to respect the
        // number of characters being appended.
        _array_field_set ( string , ARRAY_FIELD_CAPACITY , new_size );

        string = _array_resize ( string );
    }

    memory_copy ( string + string_length ( string ) * stride
                , src
                , src_length * stride
                );
    memory_clear ( string + new_length , stride ); // Append terminator.

    _array_field_set ( string , ARRAY_FIELD_LENGTH , new_size );

    return string;
}

char*
__string_insert
(   char*       string
,   u64         index
,   const char* src
,   const u64   src_length
)
{
    const u64 old_length = string_length ( string );
    const u64 old_size = array_length ( string );
    const u64 new_size = old_size + src_length;
    const u64 stride = array_stride ( string );
    
    if ( index > old_length )
    {
        LOGERROR ( "_string_insert called with out of bounds index: %i (index) > %i (string length)."
                 , index , string_length ( string )
                 );
        return string;
    }

    if ( new_size >= array_capacity ( string ) )
    {
        // Artificially grow the capacity to force resize to respect the
        // number of characters being inserted.
        _array_field_set ( string , ARRAY_FIELD_CAPACITY , new_size );

        string = _array_resize ( string );
    }

    memory_move ( string + index + src_length
                , string + index
                , ( old_length - index ) * stride
                );
    memory_copy ( string + index , src , src_length * stride );
    memory_clear ( string + old_length + src_length , stride ); // Append terminator.
    
    _array_field_set ( string , ARRAY_FIELD_LENGTH , new_size );

    return string;
}

char*
_string_remove
(   void*   string
,   u64     index
,   u64     count
)
{
    const u64 old_length = string_length ( string );
    const u64 old_size = array_length ( string );
    const u64 new_size = old_size - count;
    const u64 stride = array_stride ( string );

    if ( index + count > old_length )
    {
        LOGERROR ( "_string_remove called with illegal index or count: (index %i + count %i) %i > %i (string length)."
                 , index , count , index + count , old_length
                 );
        return string;
    }

    memory_move ( string + index
                , string + index + count
                , ( old_length - index - count ) * stride
                );
    memory_clear ( string + ( new_size - 1 ) * stride , stride ); // Append terminator.
    
    _array_field_set ( string , ARRAY_FIELD_LENGTH , new_size );

    return string;
}

char*
_string_clear
(   char* string
)
{
    string[ 0 ] = 0; // Append terminator.
    _array_field_set ( string , ARRAY_FIELD_LENGTH , 1 );
    return string;
}

char*
_string_trim
(   char* string
)
{
    const u64 length = string_length ( string );
    u64 i;
    
    // Compute index of first non-whitespace character.
    for ( i = 0; i < length && whitespace ( string[ i ] ); ++i );
    char* const from = string + i;

    // Whitespace-only case.
    if ( i == length - 1 )
    {
        return string_clear ( string );
    }

    // Compute index of final non-whitespace character.
    for ( i = length; i && whitespace ( string[ i - 1 ] ); --i );
    char* const to = string + i;

    // Copy memory range in-place.
    const u64 size = MAX ( 0 , to - from );
    memory_move ( string , from , size );
    string[ size ] = 0; // Append terminator.

    _array_field_set ( string , ARRAY_FIELD_LENGTH , size + 1 );
    
    return string;
}