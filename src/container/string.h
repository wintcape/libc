/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/string.h
 * @brief Provides an interface for a resizable string data structure.
 */
#ifndef STRING_H
#define STRING_H

#include "common.h"

#include "container/array.h"
#include "container/string/format.h"

#include "core/string.h"

/** @brief Defines resizable string default capacity. */
#define STRING_DEFAULT_CAPACITY 64

/**
 * @brief Generates a handle to an empty resizable string.
 * 
 * Use _string_create to explicitly specify initial capacity, or string_create
 * to use the default.
 * 
 * Uses dynamic memory allocation. Call string_destroy to free.
 * 
 * @param initial_capacity The initial capacity for the string backend array.
 * @return An empty resizable string with the specified backend array capacity.
 */
char*
__string_create
(   ARRAY_FIELD initial_capacity
);

#define string_create() \
    __string_create ( STRING_DEFAULT_CAPACITY )

#define _string_create(initial_capacity) \
    __string_create ( initial_capacity )

/**
 * @brief Creates a resizable copy of an existing string. O(n).
 * 
 * Use _string_copy to explicitly specify string length, or string_create_from
 * to compute the length of a null-terminated string ( O(n) ) before passing it
 * to _string_copy. If the string being copied is itself a resizable string
 * (i.e. a string itself created via the string_create class of functions),
 * string_copy may be used to implicitly fetch the current length of the
 * resizable string ( O(1) ) before passing it to __string_copy.
 * 
 * Uses dynamic memory allocation. Call string_destroy to free.
 * 
 * @param src The string to copy. Must be non-zero.
 * @param src_length The number of characters to copy from src.
 * @return A resizable copy of s.
 */
char*
__string_copy
(   const char* src
,   const u64   src_length
);

#define string_copy(string)                                        \
    ({                                                             \
        const char* string__ = (string);                           \
        __string_copy ( (string__) , string_length ( string__ ) ); \
    })

#define string_create_from(string)                                  \
    ({                                                              \
        const char* string__ = (string);                            \
        __string_copy ( (string__) , _string_length ( string__ ) ); \
    })

#define _string_copy(string,length) \
    __string_copy ( (string) , (length) )

/**
 * @brief Frees the memory used by a provided resizable string.
 * 
 * @param string The resizable string to free.
 */
void
string_destroy
(   char* string
);

/**
 * @brief Reads the current length of a resizable string. O(1).
 * 
 * @param string A resizable string. Must be non-zero.
 * @return The number of characters currently contained by string.
 */
u64
string_length
(   const char* string
);

/**
 * @brief Appends to a resizable string. O(1), on average.
 * 
 * Use string_push to explicitly specify string length, or _string_push
 * to compute the length of a null-terminated string before passing it to
 * __string_push.
 * 
 * @param string The resizable string to append to. Must be non-zero.
 * @param src The string to append. Must be non-zero.
 * @param src_length The number of characters to copy from src.
 * @return The resizable string (possibly with new address).
 */
char*
__string_push
(   char*       string
,   const char* src
,   const u64   src_length
);

#define string_push(string,src,length) \
    ( (string) = __string_push ( (string) , (src) , (length) ) )

#define _string_push(string,src)                            \
    do                                                      \
    {                                                       \
        const char* src__ = (src);                          \
        (string) = __string_push ( (string)                 \
                                 , src__                    \
                                 , _string_length ( src__ ) \
                                 );                         \
    }                                                       \
    while ( 0 )

/**
 * @brief Inserts into a resizable string. O(n).
 * 
 * Use string_insert to explicitly specify string length, or _string_insert
 * to compute the length of a null-terminated string before passing it to
 * __string_insert.
 * 
 * @param string The resizable string to append to. Must be non-zero.
 * @param index The index to insert at.
 * @param src The string to insert. Must be non-zero.
 * @param src_length The number of characters to copy from src.
 * @return The resizable string (possibly with new address).
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

#define _string_insert(string,index,src)                      \
    do                                                        \
    {                                                         \
        const char* src__ = (src);                            \
        (string) = __string_insert ( (string)                 \
                                   , (index)                  \
                                   , src__                    \
                                   , _string_length ( src__ ) \
                                   );                         \
    }                                                         \
    while ( 0 )

/**
 * @brief Removes a substring from a resizable string. O(n).
 * 
 * @param string The resizable string to remove from. Must be non-zero.
 * @param index The starting index of the substring to remove.
 * @param count The number of characters to remove.
 * @return The resizable string with the substring removed.
 */
char*
__string_remove
(   void*   string
,   u64     index
,   u64     count
);

#define string_remove(string,index,count) \
    __string_remove ( (string) , (index) , (count) )

/**
 * @brief **Effectively** clears a resizable string. O(1).
 * 
 * @param string The resizable string to clear. Must be non-zero.
 * @return The resizable string set to empty.
 */
char*
__string_clear
(   char* string
);

#define string_clear(string) \
    __string_clear ( string )

/**
 * @brief Trims whitespace off front and back of a string. O(n). In-place.
 * 
 * @param string The resizable string to trim. Must be non-zero.
 * @return The resizable string with whitespace trimmed off the front and back.
 */
char*
__string_trim
(   char* string
);

#define string_trim(string) \
    __string_trim ( string )

/**
 * @brief Replaces all instances of a substring within a string with a different
 * substring. O(n).
 * 
 * Use string_replace to explicitly specify string length, or _string_replace
 * to compute the lengths of null-terminated strings before passing them to
 * __string_replace.
 * 
 * @param string The resizable string to mutate. Must be non-zero.
 * @param remove The substring to remove. Must be non-zero.
 * @param replace The substring to replace the removed substring with.
 * Must be non-zero.
 * @return The resizable string with all instances of the removed substring
 * replaced by the replacement substring (possibly with new address).
 */
char*
__string_replace
(   char*       string
,   const char* remove
,   const u64   remove_length
,   const char* replace
,   const u64   replace_length
);

#define string_replace(string,remove,remove_length,replace,replace_length) \
    ( (string) = __string_replace ( (string)                               \
                                  , (remove)                               \
                                  , (remove_length)                        \
                                  , (replace)                              \
                                  , (replace_length)                       \
                                  ) )

#define _string_replace(string,remove,replace)                     \
    do                                                             \
    {                                                              \
        const char* remove__ = (remove);                           \
        const char* replace__ = (replace);                         \
        (string) = __string_replace ( (string)                     \
                                    , remove__                     \
                                    , _string_length ( remove__ )  \
                                    , replace__                    \
                                    , _string_length ( replace__ ) \
                                    );                             \
    }                                                              \
    while ( 0 )

/**
 * @brief Strips a string of ANSI formatting codes. O(n). In-place.
 * 
 * @param string The resizable string to strip. Must be non-zero.
 * @return The resizable string stripped of ANSI formatting codes.
 */
char*
__string_strip_ansi
(   char* string
);

#define string_strip_ansi(string) \
    __string_strip_ansi ( string )

#endif  // STRING_H