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

/**
 * @brief Primary implementation of string_i64 and string_u64
 * (see string_i64 and string_u64).
 * 
 * After return, the string at dst will be written in least-significant-bit
 * first order. Since the standard method for printing Arabic-numeral integers
 * is in most-significant-bit first order, the output of this function should be
 * reversed for the purpose of display.
 * 
 * @param value A 64-bit value.
 * @param radix Integer radix in the range [2..36] (inclusive).
 * @param dst Output buffer for string.
 * @return The number of characters written to dst.
 */
u64
_string_u64
(   u64     value
,   u8      radix
,   char*   dst
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
    return ( s1_length == s2_length )
        && ( s1 == s2 || memory_equal ( s1 , s2 , s1_length ) )
        ;
}

bool
string_contains
(   const char* search
,   const u64   search_length
,   const char* find
,   const u64   find_length
,   bool        reverse
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

char*
string_reverse
(   char*       string
,   const u64   string_length
)
{
    if ( string_length < 2 )
    {
        return string;
    }
    
    u64 i;
    u64 j;
    char swap;
    for ( i = 0 , j = string_length - 1; i < j; ++i , --j )
    {
        swap = string[ i ];
        string[ i ] = string[ j ];
        string[ j ] = swap;
    }
    return string;
}

u64
string_i64
(   i64     value
,   u8      radix
,   char*   dst
)
{
    if ( radix < STRING_INTEGER_MIN_RADIX || radix > STRING_INTEGER_MAX_RADIX )
    {
        LOGWARN ( "string_i64: Illegal value for radix argument: %u. Clamping to range [%u..%u]."
                , radix
                , STRING_INTEGER_MIN_RADIX
                , STRING_INTEGER_MAX_RADIX
                );
        radix = CLAMP ( radix
                      , STRING_INTEGER_MIN_RADIX
                      , STRING_INTEGER_MAX_RADIX
                      );
    }

    const bool negative = radix == 10 && value < 0;
    if ( negative )
    {
        *dst = '-';
        value = -value;
    }

    const u64 length = _string_u64 ( value , radix , dst + negative );
    string_reverse ( dst + negative , length );
    return length + negative;
}

u64
string_u64
(   u64     value
,   u8      radix
,   char*   dst
)
{
    if ( radix < STRING_INTEGER_MIN_RADIX || radix > STRING_INTEGER_MAX_RADIX )
    {
        LOGWARN ( "string_u64: Illegal value for radix argument: %u. Clamping to range [%u..%u]."
                , radix
                , STRING_INTEGER_MIN_RADIX
                , STRING_INTEGER_MAX_RADIX
                );
        radix = CLAMP ( radix
                      , STRING_INTEGER_MIN_RADIX
                      , STRING_INTEGER_MAX_RADIX
                      );
    }

    const u64 length = _string_u64 ( value , radix , dst );
    string_reverse ( dst , length );
    return length;
}

u64
string_f64
(   f64     value
,   u8      precision
,   bool    abbreviated
,   char*   dst
)
{   // TODO: Implement this.
    return 0;
}

const char*
string_bytesize
(   u64     size
,   f64*    amount
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
(   u64 content_size
)
{
    const u64 header_size = sizeof ( u64 );
    const u64 size = header_size + content_size;
    char* string = memory_allocate ( size , MEMORY_TAG_STRING );
    *( ( u64* ) string ) = size;
    return ( char* )( ( ( u64 ) string ) + header_size );
}

char*
string_allocate_from
(   const char* string
)
{
    const u64 length = _string_length ( string );
    char* copy = string_allocate ( length + 1 );
    memory_copy ( copy , string , length );
    return copy;
}

void
string_free
(   void* string
)
{
    if ( !string )
    {
        return;
    }
    const u64 header_size = sizeof ( u64 );
    string = ( void* )( ( ( u64 ) string ) - header_size );
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
        *index = search_length - 1;
        return true;
    }
    for ( u64 i = search_length - find_length + 1; i; --i )
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

u64
_string_u64
(   u64     value
,   u8      radix
,   char*   dst
)
{
    char* i = dst;
    do
    {
        const u8 digit = value % radix;
        value /= radix;
        if ( digit < 10 )
        {
            *i = '0' + digit;
        }
        else
        {
            *i = 'A' + digit - 10;
        }
        i += 1;
    }
    while ( value );
    return i - dst;
}