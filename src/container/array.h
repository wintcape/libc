/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/array.h
 * @brief Provides an interface for a resizable array data structure.
 */
#ifndef ARRAY_H
#define ARRAY_H

#include "common.h"

/** @brief Type and instance definitions for array fields. */
typedef enum
{
    ARRAY_FIELD_CAPACITY
,   ARRAY_FIELD_LENGTH
,   ARRAY_FIELD_STRIDE

,   ARRAY_FIELD_COUNT
}
ARRAY_FIELD;

/** @brief Array default capacity. */
#define ARRAY_DEFAULT_CAPACITY 10

/** @brief Array rescale factor. */
#define ARRAY_SCALE_FACTOR(capacity) \
    ( ( (capacity) * 3 ) >> 1 )

/**
 * @brief Allocates memory for a resizable array.
 * 
 * Use array_create to explicitly specify an initial capacity, or
 * array_create_new to use a default.
 * 
 * Uses dynamic memory allocation. Call array_destroy to free.
 * 
 * @param initial_capacity The initial capacity. Must be non-zero.
 * @param stride The fixed element size in bytes. Must be non-zero.
 * @return A resizable array.
 */
void*
_array_create
(   ARRAY_FIELD initial_capacity
,   ARRAY_FIELD stride
);

/** @param type C data type of the array. */
#define array_create(type,initial_capacity) \
    _array_create ( (initial_capacity) , sizeof ( type ) )

/** @param type C data type of the array. */
#define array_create_new(type) \
    _array_create ( ARRAY_DEFAULT_CAPACITY , sizeof ( type ) )

/**
 * @brief Creates a resizable array by copying an existing array. O(n).
 *
 * Uses dynamic memory allocation. Call array_destroy to free.
 * 
 * @param array The array to copy.
 * @param stride The fixed element size in bytes. Must be non-zero.
 * @param length The number of elements contained by array.
 * @return A resizable copy of array.
*/
void*
_array_create_from
(   void*       array
,   ARRAY_FIELD stride
,   ARRAY_FIELD length
);

/** @param type C data type of the array. */
#define array_create_from(type,array,length) \
    _array_create_from ( (array) , sizeof ( type ) , (length) )

/**
 * @brief Copies a resizable array. O(n).
 * 
 * Uses dynamic memory allocation. Call array_destroy to free.
 * 
 * @param array The array to copy.
 * @return A copy of array.
 */
void*
_array_copy
(   void* array
);

#define array_copy(array) \
    _array_copy ( array )

/**
 * @brief Frees the memory used by a resizable array.
 * 
 * @param array The array to free.
 */
void
_array_destroy
(   void* array
);

#define array_destroy(array) \
    _array_destroy ( array )

/**
 * @brief Obtains the value of an array field. O(1).
 * 
 * @param array The array to query. Must be non-zero.
 * @param field The field to read.
 * @return The value of the array field.
 */
u64
_array_field_get
(   const void* array
,   ARRAY_FIELD field
);

/** @brief Get array field: capacity. */
#define array_capacity(array) \
    _array_field_get ( (array) , ARRAY_FIELD_CAPACITY )

/** @brief Get array field: length. */
#define array_length(array) \
    _array_field_get ( (array) , ARRAY_FIELD_LENGTH )

/** @brief Get array field: stride. */
#define array_stride(array) \
    _array_field_get ( (array) , ARRAY_FIELD_STRIDE )

/**
 * @brief Sets the value of an array field. O(1).
 * 
 * @param array The array to mutate. Must be non-zero.
 * @param field The field to set.
 * @param value The value to set.
 */
void
_array_field_set
(   void*       array
,   ARRAY_FIELD field
,   u64         value
);

/**
 * @brief Computes the size (in bytes) of an array data structure. O(1).
 * 
 * @param array The array to query. Must be non-zero.
 */
u64
_array_size
(   const void* array
);

#define array_size(array) \
    _array_size ( array )

/**
 * @brief Resizes an existing array. O(n).
 * 
 * @param array The array to resize. Must be non-zero.
 * @param minimum_capacity The minimum capacity of the new array.
 * @return A copy of the original array content with an expanded capacity.
 */
void*
_array_resize
(   void*   array
,   u64     minimum_capacity
);

/**
 * @brief Appends an element to an array. O(1), on average.
 * 
 * @param array The array to append to. Must be non-zero.
 * @param src The element to append. Must be non-zero.
 * @return The array (possibly with new address).
 */
void*
_array_push
(   void*       array
,   const void* src
);

#define array_push(array,value)                  \
    do                                           \
    {                                            \
        __typeof__ ( (value) ) tmp = (value);    \
       (array) = _array_push ( (array) , &tmp ); \
    }                                            \
    while ( 0 )

/**
 * @brief Removes the last element from an array. O(1).
 * 
 * @param array The array to remove from. Must be non-zero.
 * @param dst A destination buffer to store the element that was removed. Pass 0
 * to retrieve nothing.
 */
void
_array_pop
(   void* array
,   void* dst
);

#define array_pop(array,dst) \
    _array_pop ( (array) , (dst) )

/**
 * @brief Inserts an element into an array at a specified index. O(n).
 * 
 * @param array The array to append to. Must be non-zero.
 * @param index The index to insert at.
 * @param src The element to insert. Must be non-zero.
 * @return The array (possibly with new address).
 */
void*
_array_insert
(   void*       array
,   u64         index
,   const void* src
);

#define array_insert(array,index,value)                      \
    do                                                       \
    {                                                        \
        __typeof__ ( (value) ) tmp = (value);                \
       (array) = _array_insert ( (array) , (index) , &tmp ); \
    }                                                        \
    while ( 0 )

/**
 * @brief Removes an element from an array at a specified index. O(n).
 * 
 * @param array The array to mutate. Must be non-zero.
 * @param index The index of the element to remove.
 * @param dst A destination buffer to store the element that was removed. Pass 0
 * to retrieve nothing.
 * @return The array (possibly with new address).
 */
void*
_array_remove
(   void*   array
,   u64     index
,   void*   dst
);

#define array_remove(array,index,dst) \
    _array_remove ( (array) , (index) , (dst) )

/**
 * @brief Reverses an array. O(n). In-place.
 * 
 * Use _array_reverse to explicitly specify fixed array stride and length, or
 * array_reverse (for arrays created with the _array_create class of functions)
 * to fetch these fields using array_field_get prior to passing them to
 * _array_reverse.
 * 
 * @param array The array to reverse. Must be non-zero.
 * @param array_stride The array stride.
 * @param array_length The number of elements contained by the array.
 * @return The array after reversal.
 */
void*
_array_reverse
(   void*       array
,   const u64   array_stride
,   const u64   array_length
);

#define array_reverse(array) \
    _array_reverse ( (array) , array_stride ( array ) , array_length ( array ) )

/**
 * @brief Shuffles the elements of an array. O(n).
 * 
 * Use _array_shuffle to explicitly specify fixed array stride and length, or
 * array_shuffle (for arrays created with the _array_create class of functions)
 * to fetch these fields using array_field_get prior to passing them to
 * _array_shuffle.
 * 
 * @param array The array to shuffle. Must be non-zero.
 * @param array_stride The array stride.
 * @param array_length The number of elements contained by the array.
 * @return The array with its elements shuffled.
 */
void*
_array_shuffle
(   void*   array
,   u64     array_stride
,   u64     array_length
);

#define array_shuffle(array)                \
    _array_shuffle ( (array)                \
                   , array_stride ( array ) \
                   , array_length ( array ) \
                   )

/**
 * @brief Sorts an array in-place.
 * 
 * Current implementation uses quicksort algorithm.
 * AVERAGE CASE TIME COMPLEXITY : O(n log(n))
 * WORST CASE TIME COMPLEXITY   : O(n²)
 * 
 * Use _array_sort to explicitly specify fixed array stride and length, or
 * array_sort (for arrays created with the _array_create class of functions)
 * to fetch these fields using array_field_get prior to passing them to
 * _array_sort.
 * 
 * @param array The array to sort. Must be non-zero.
 * @param array_stride The array stride.
 * @param array_length The number of elements contained by the array.
 * @param comparator A function which compares two array elements.
 * @return The array with all elements sorted according to the comparator.
 */
void*
_array_sort
(   void*                   array
,   u64                     array_stride
,   u64                     array_length
,   comparator_function_t   comparator
);

#define array_sort(array)                \
    _array_sort ( (array)                \
                , array_stride ( array ) \
                , array_length ( array ) \
                , (comparator)           \
                )

#endif  // ARRAY_H