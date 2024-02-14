/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file core/string.h
 * @brief Defines common string operations.
 */
#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include "common.h"

/** @brief Defines the maximum size (in bytes) for a local string buffer. */
#define STACK_STRING_MAX_SIZE \
    KIBIBYTES ( 32 )

/**
 * @brief Computes the number of characters in a null-terminated string. O(n).
 * 
 * String is assumed to terminate within an accessible memory range.
 * For potentially unsafe strings, use _string_length_clamped instead.
 * 
 * For an O(1) alternative, use string_length instead (see container/string.h).
 * 
 * @param string The null-terminated string to read.
 * @return The number of characters in string.
 */
u64
_string_length
(   const char* string
);

/**
 * @brief Clamped variant of _string_length for unsafe strings.
 * 
 * @param string The string to read.
 * @param limit The maximum number of characters to read from string.
 * @return The number of characters in string, or limit if string contains more
 * than limit characters.
 */
u64
_string_length_clamped
(   const char* string
,   const u64   limit
);

/**
 * @brief String equality test predicate.
 * 
 * Use string_equal to explicitly specify string length, or _string_equal
 * to calculate the lengths of null-terminated strings before passing them to
 * string_equal.
 * 
 * @param s1 A string.
 * @param s1_length The number of characters in s1.
 * @param s2 A string.
 * @param s2_length The number of characters in s2.
 * @return true if strings are equal; false otherwise.
 */
bool
string_equal
(   const char* s1
,   const u64   s1_length
,   const char* s2
,   const u64   s2_length
);

#define _string_equal (s1,s2) \
    string_equal ( s1 , _string_length ( s1 ) , s2 , _string_length ( s2 ) )

/**
 * @brief Empty string test predicate.
 * 
 * @param string The string to check.
 * @return true if string is empty; false otherwise.
 */
bool
string_empty
(   const char* string
);

/**
 * @brief Searches a string for a substring.
 * 
 * Use string_contains to explicitly specify string length, or _string_contains
 * to calculate the lengths of null-terminated strings before passing them to
 * string_contains.
 * 
 * @param search The string to search.
 * @param search_length The number of characters in search.
 * @param find The string to find.
 * @param find_length The number of characters in find.
 * @param reverse Search in reverse? Y/N
 * @param index Output buffer to hold the index in search at which find was
 * found.
 * @return true if search contains find, or if find_length is zero and
 * search_length is non-zero; false if search does not contain find, or if
 * find_length is greater than search_length.
*/
bool
string_contains
(   const char* search
,   const u64   search_length
,   const char* find
,   const u64   find_length
,   const bool  reverse
,   u64*        index
);

#define _string_contains (search,find,reverse,index)     \
    string_contains ( search , _string_length ( search ) \
                    , find , _string_length ( find )     \
                    , reverse                            \
                    , index                              \
                    )

/**
 * @brief Character stringify utility.
 * 
 * @param c The character.
 * @return A string copy of c. 
 */
#define string_char(c) \
    ( ( char[] ){ (c) , 0 } )

/**
 * @brief Bytesize to string.
 * 
 * Converts size into appropriate units. The unit abbreviation string is
 * returned, and the converted size is written into the output buffer.
 * 
 * @param size Size in bytes.
 * @param amount Output buffer for size (after conversion to appropriate units).
 * @return "GiB" | "MiB" | "KiB" | "B"
 */
const char*
string_bytesize
(   const u64   size
,   f64*        amount
);

/**
 * @brief Allocates memory for a string of the provided size.
 * 
 * Uses dynamic memory allocation. Call string_free to free.
 * 
 * @param size The number of bytes of memory to allocate.
 * @return An empty string of the provided size.
 */
char*
string_allocate
(   const u64 size
);

/**
 * @brief Generates a copy of a null-terminated string.
 * 
 * Uses dynamic memory allocation. Call string_free to free.
 * 
 * @param string The null-terminated string to copy.
 * @return A copy of string.
 */
char*
string_allocate_from
(   const char* string
);

/**
 * @brief Frees the memory used by a provided string.
 * 
 * @param string The string to free.
 */
void
string_free
(   void* string
);

#endif  // STRING_UTIL_H