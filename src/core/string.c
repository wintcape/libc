/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file core/string.c
 * @brief Implementation of the core/string header.
 * (see core/string.h for additional details)
 */
#include "core/string.h"

#include "core/logger.h"
#include "core/memory.h"

#include "math/math.h"

#include "platform/platform.h"

/**
 * @brief Primary implementation of string_contains (see string_contains).
 * 
 * Search begins from the start of the string.
 * 
 * @param search The string to search.
 * @param search_length The number of characters in search.
 * @param find The string to find.
 * @param find_length The number of characters in find.
 * @param index Output buffer to hold the index in search at which find was
 * found.
 * @return true if search contains find; false otherwise.
 */
bool
__string_contains
(   const char* search
,   const u64   search_length
,   const char* find
,   const u64   find_length
,   u64*        index
);

/**
 * @brief Primary implementation of string_contains (see string_contains).
 * 
 * Search begins from the end of the string.
 * 
 * @param search The string to search.
 * @param search_length The number of characters in search.
 * @param find The string to find.
 * @param find_length The number of characters in find.
 * @param index Output buffer to hold the index in search at which find was
 * found.
 * @return true if search contains find; false otherwise.
 */
bool
__string_contains_reverse
(   const char* search
,   const u64   search_length
,   const char* find
,   const u64   find_length
,   u64*        index
);

u64
_string_length
(   const char* string
)
{
    return platform_string_length ( string );
}

u64
_string_length_clamped
(   const char* string
,   const u64   limit
)
{
    return platform_string_length_clamped ( string , limit );
}

bool
string_equal
(   const char* s1
,   const u64   s1_length
,   const char* s2
,   const u64   s2_length
)
{
    return s1_length == s2_length && memory_equal ( s1 , s2 , s2_length );
}

bool
string_contains
(   const char* search
,   const u64   search_length
,   const char* find
,   const u64   find_length
,   const bool  reverse
,   u64*        index
)
{
    if ( find_length > search_length )
    {
        return false;
    }
    if ( reverse )
    {
        return __string_contains_reverse ( search , search_length
                                         , find , find_length
                                         , index
                                         );
    }
    return __string_contains ( search , search_length
                             , find , find_length
                             , index
                             );
}

const char*
string_bytesize
(   const u64   size
,   f64*        amount
)
{
    if ( size >= GIBIBYTES ( 1 ) )
    {
        *amount = ( f64 ) size / GIBIBYTES ( 1 );
        return "GiB";
    }
    if ( size >= MEBIBYTES ( 1 ) )
    {
        *amount = ( f64 ) size / MEBIBYTES ( 1 );
        return "MiB";
    }
    if ( size >= KIBIBYTES ( 1 ) )
    {
        *amount = ( f64 ) size / KIBIBYTES ( 1 );
        return "KiB";
    }
    *amount = ( f64 ) size;
    return "B";
}

char*
string_allocate
(   const u64 size
)
{
    const u64 header_size = sizeof ( u64 );
    char* string = memory_allocate ( header_size + size , MEMORY_TAG_STRING );
    *( ( u64* ) string ) = size;
    string += header_size;
    return string;
}

char*
string_allocate_from
(   const char* string
)
{
    const u64 length = _string_length ( string );
    char* copy = string_allocate ( length + 1 );
    memory_copy ( copy , string , length );
    copy[ length ] = 0; // Append terminator.
    return copy;
}

void
string_free
(   void* string
)
{
    const u64 header_size = sizeof ( u64 );
    string -= header_size;
    memory_free ( string
                , *( ( u64* ) string )
                , MEMORY_TAG_STRING
                );
}

bool
__string_contains
(   const char* search
,   const u64   search_length
,   const char* find
,   const u64   find_length
,   u64*        index
)
{
    if ( !find_length )
    {
        *index = 0;
        return true;
    }
    for ( u64 i = 0; i <= search_length - find_length; ++i )
    {
        if ( search[ i ] != *find )
        {
            continue;
        }

        if ( memory_equal ( search + i + 1 , find + 1 , find_length - 1 ) )
        {
            *index = i;
            return true;
        }
    }
    return false;
}

bool
__string_contains_reverse
(   const char* search
,   const u64   search_length
,   const char* find
,   const u64   find_length
,   u64*        index
)
{
    if ( !find_length )
    {
        *index = search_length;
        return true;
    }
    for ( u64 i = search_length - find_length + 2; i; --i )
    {
        if ( search[ i - 1 ] != *find )
        {
            continue;
        }

        if ( memory_equal ( search + i , find + 1 , find_length - 1 ) )
        {
            *index = i - 1;
            return true;
        }
    }
    return false;
}