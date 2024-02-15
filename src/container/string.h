/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/string.h
 * @brief Provides an interface for a mutable string data structure.
 */
#ifndef STRING_H
#define STRING_H

#include "common.h"

#include "container/array.h"
#include "container/string/format.h"

#include "core/string.h"

/** @brief Defines mutable string default capacity. */
#define STRING_DEFAULT_CAPACITY 64

/**
 * @brief Creates a mutable string.
 * 
 * Uses dynamic memory allocation. Call string_destroy to free.
 * 
 * @param initial_capacity The initial capacity for the string backend array.
 * @return An empty mutable string with the specified backend array capacity.
 */
char*
_string_create
(   ARRAY_FIELD initial_capacity
);

#define string_create() \
    _string_create ( STRING_DEFAULT_CAPACITY )

/**
 * @brief Creates a mutable string from an existing string of precalculated
 * length.
 * 
 * Use string_copy to explicitly specify string length, or string_create_from
 * to calculate the length of a null-terminated string before passing it to
 * _string_copy.
 * 
 * Uses dynamic memory allocation. Call string_destroy to free.
 * 
 * @param src The string to copy.
 * @param src_length The amount to copy.
 * @return A mutable copy of s.
 */
char*
_string_copy
(   const char* src
,   const u64   src_length
);

#define string_copy(string) \
    _string_copy ( (string) , string_length ( string ) )

#define string_create_from(string) \
    _string_copy ( (string) , _string_length ( string ) )

/**
 * @brief Frees the memory used by a provided mutable string.
 * 
 * @param string The mutable string to free.
 */
void
string_destroy
(   char* string
);

/**
 * @brief Reads the length of a mutable string. O(1).
 * 
 * @param string A mutable string.
 * @return The number of characters currently contained by string.
 */
u64
string_length
(   const char* string
);

/**
 * @brief Appends to a mutable string.
 * 
 * Use string_push to explicitly specify string length, or _string_push
 * to calculate the length of a null-terminated string before passing it to
 * __string_push.
 * 
 * @param string The mutable string to append to.
 * @param src The string to append.
 * @return The mutable string (possibly with new address).
 */
char*
__string_push
(   char*       string
,   const char* src
,   const u64   src_length
);

#define string_push(string,src,length) \
    ( (string) = __string_push ( (string) , (src) , (length) ) )

#define _string_push(string,src_)                                                \
    do                                                                           \
    {                                                                            \
        const char* src__ = (src_);                                              \
        (string) = __string_push ( (string)                                      \
                                 , src__                                         \
                                 , _string_length ( src__ )                      \
                                 );                                              \
    }                                                                            \
    while ( 0 )

/**
 * @brief Inserts into a mutable string.
 * 
 * Use string_insert to explicitly specify string length, or _string_insert
 * to calculate the length of a null-terminated string before passing it to
 * __string_insert.
 * 
 * @param string The mutable string to append to.
 * @param index The index to insert at.
 * @param src The string to insert.
 * @return The mutable string (possibly with new address).
 */
char*
__string_insert
(   char*       string
,   u64         index
,   const char* src
,   const u64   src_length
);

#define string_insert(string,index,src,length) \
    ( (string) = __string_insert ( (string) , (index) , (src) , (length) ) )

#define _string_insert(string,index,src_)                     \
    do                                                        \
    {                                                         \
        const char* src__ = (src_);                           \
        (string) = __string_insert ( (string)                 \
                                   , (index)                  \
                                   , src__                    \
                                   , _string_length ( src__ ) \
                                   );                         \
    }                                                         \
    while ( 0 )

/**
 * @brief Removes a substring from a mutable string.
 * 
 * @param string The mutable string to remove from.
 * @param index The starting index of the substring to remove.
 * @param count The number of characters to remove.
 * @return The mutable string with the substring removed.
 */
char*
_string_remove
(   void*   string
,   u64     index
,   u64     count
);

#define string_remove(string,index,count) \
    _string_remove ( (string) , (index) , (count) )

/**
 * @brief **Effectively** clears a mutable string.
 * 
 * @param string The mutable string to clear.
 * @return The mutable string set to empty.
 */
char*
_string_clear
(   char* string
);

#define string_clear(string) \
    _string_clear ( string )

/**
 * @brief Trims whitespace off front and back of a string. In-place.
 * 
 * @param string The mutable string to trim.
 * @return The mutable string with whitespace trimmed off the front and back.
 */
char*
_string_trim
(   char* string
);

#define string_trim(string) \
    _string_trim ( string )

#endif  // STRING_H