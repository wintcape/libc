/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/test_array.c
 * @brief Implementation of the container/test_array header.
 * (see container/test_array.h for additional details)
 */
#include "container/test_array.h"

#include "test/expect.h"

#include "core/memory.h"

/** @brief Computes current global number of unfreed allocations. */
#define GLOBAL_ALLOCATION_COUNT \
    ( memory_allocation_count () - memory_free_count () )

u8
test_array_create_and_destroy
( void )
{
    u64 global_amount_allocated;
    u64 array_amount_allocated;
    u64 global_allocation_count;

    u64 global_amount_allocated_;
    u64 array_amount_allocated_;
    u64 global_allocation_count_;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    array_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ARRAY );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: array_create_new.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    array_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ARRAY );
    global_allocation_count_ = GLOBAL_ALLOCATION_COUNT;

    u16* array = array_create_new ( u16 );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , array );

    // TEST 1.1: array_create_new performed one memory allocation.
    EXPECT_EQ ( global_allocation_count_ + 1 , GLOBAL_ALLOCATION_COUNT );

    // TEST 1.2: array_create_new allocated the correct number of bytes with the correct memory tag.
    EXPECT_EQ ( global_amount_allocated_ + array_size ( array ) , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated_ + array_size ( array ) , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );

    // TEST 1.3: Array created via array_create_new has the default capacity.
    EXPECT_EQ ( ARRAY_DEFAULT_CAPACITY , array_capacity ( array ) );

    // TEST 1.4: Array created via array_create_new has 0 length.
    EXPECT_EQ ( 0 , array_length ( array ) );

    // TEST 1.5: Array created via array_create_new has correct stride for type.
    EXPECT_EQ ( sizeof ( u16 ) , array_stride ( array ) );

    // TEST 1.6: Array created via array_create_new initializes all bytes in its capacity to 0.
    for ( u64 i = 0; i < ARRAY_DEFAULT_CAPACITY; ++i )
    {
        EXPECT_EQ ( 0 , array[ i ] );
    }

    // TEST 1.7: array_destroy restores the global allocator state.
    array_destroy ( array );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( global_allocation_count_ , GLOBAL_ALLOCATION_COUNT );

    // TEST 2: array_create.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    array_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ARRAY );
    global_allocation_count_ = GLOBAL_ALLOCATION_COUNT;

    const u64 initial_capacity = 4 * ARRAY_DEFAULT_CAPACITY + 3;
    array = array_create ( u16 , initial_capacity );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , array );

    // TEST 2.1: array_create_new performed one memory allocation.
    EXPECT_EQ ( global_allocation_count_ + 1 , GLOBAL_ALLOCATION_COUNT );

    // TEST 2.2: array_create_new allocated the correct number of bytes with the correct memory tag.
    EXPECT_EQ ( global_amount_allocated_ + array_size ( array ) , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated_ + array_size ( array ) , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );

    // TEST 2.3: Array created via array_create has the correct capacity.
    EXPECT_EQ ( initial_capacity , array_capacity ( array ) );

    // TEST 2.4: Array created via array_create has 0 length.
    EXPECT_EQ ( 0 , array_length ( array ) );

    // TEST 2.5: Array created via array_create has correct stride for type.
    EXPECT_EQ ( sizeof ( u16 ) , array_stride ( array ) );

    // TEST 2.6: Array created via array_create initializes all bytes in its capacity to 0.
    for ( u64 i = 0; i < initial_capacity; ++i )
    {
        EXPECT_EQ ( 0 , array[ i ] );
    }

    // TEST 2.7: array_destroy restores the global allocator state.
    array_destroy ( array );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( global_allocation_count_ , GLOBAL_ALLOCATION_COUNT );

    // TEST 3: array_destroy handles invalid argument.

    // TEST: array_destroy does not modify the global allocator state if no array is provided.
    array_destroy ( 0 );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( global_allocation_count_ , GLOBAL_ALLOCATION_COUNT );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

    return true;
}

u8
test_array_push_and_pop
( void )
{
    u64 global_amount_allocated;
    u64 array_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    array_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ARRAY );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

    const i16 to_push = 437;
    const u64 max_op = 100000;

    i16* array = array_create_new ( i16 );
    i16* old_memory = memory_allocate ( sizeof ( i16 ) * max_op , MEMORY_TAG_ARRAY );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , array );
    EXPECT_NEQ ( 0 , old_memory );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: array_push.

    LOGDEBUG ( "Appending %i elements to an array. . ." , max_op );

    for ( u64 i = 0; i < max_op; ++i )
    {
        // Copy the array state prior to the operation being tested.
        const u64 old_length = array_length ( array );
        memory_copy ( old_memory , array , old_length * array_stride ( array ) );

        array_push ( array , to_push );

        // Verify there was no memory error prior to the test.
        EXPECT_NEQ ( 0 , array );

        // TEST 1.1: array_push increases the length of the array by 1.
        EXPECT_EQ ( old_length + 1 , array_length ( array ) );

        // TEST 1.2: array_push appends the correct element to the end of the array.
        EXPECT_EQ ( to_push , array[ old_length ] );

        // TEST 1.3: array_push leaves the rest of the array unmodified.
        EXPECT ( memory_equal ( array , old_memory , old_length ) );
    }

    LOGDEBUG ( "  Done." );

    // TEST 2: array_pop.

    LOGDEBUG ( "Removing them. . ." );

    u64 old_length;
    i16 popped;

    while ( array_length ( array ) > 1 )
    {
        // Copy the array state prior to the operation being tested.
        old_length = array_length ( array );
        memory_copy ( old_memory , array , old_length );

        popped = 0;
        array_pop ( array , &popped );

        // Verify there was no memory error prior to the test.
        EXPECT_NEQ ( 0 , array );

        // TEST 2.1: array_pop decreases the length of the array by 1.
        EXPECT_EQ ( old_length - 1 , array_length ( array ) );

        // TEST 2.2: array_pop copies the last element of the array into the provided output buffer.
        EXPECT_EQ ( to_push , popped );

        // TEST 2.3: array_pop leaves the rest of the array unmodified.
        EXPECT ( memory_equal ( array , old_memory , array_length ( array ) ) );
    }

    // TEST 2.4: array_pop succeeds when no output buffer is provided.
    array_pop ( array , 0 );

    LOGDEBUG ( "  Done." );

    // TEST 3: array_pop succeeds on empty array.

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , array );

    // Verify the array is empty prior to the test.
    EXPECT_EQ ( 0 , array_length ( array ) );

    // TEST 3.1: array_pop warns when the array is empty.
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    popped = 0;
    i16* array_ = array;
    array_pop ( array , &popped );

    // TEST 3.2: array_pop does not perform memory allocation if the array is empty* (current implementation doesn't allocate in general either, but I don't need to test for that).
    EXPECT_EQ ( array_ , array );
    
    // TEST 3.3: array_pop does not modify array length if the array is empty.
    EXPECT_EQ ( 0 , array_length ( array ) );

    // TEST 3.4: array_pop writes nothing to the output buffer if the array is empty.
    EXPECT_EQ ( 0 , popped );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    memory_free ( old_memory , sizeof ( i16 ) * max_op , MEMORY_TAG_ARRAY );
    array_destroy ( array );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

    return true;
}

u8
test_array_insert_and_remove
( void )
{
    u64 global_amount_allocated;
    u64 array_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    array_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ARRAY );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

    const i64 to_insert[] = { 437 , 24 , -179253478 , -15799 };
    const i64 remove1[]   = { 437 , 24 , -15799 };
    const i64 remove2[]   = { 437 , -15799 };
    const i64 remove3[]   = { -15799 };
    i64* array1 = array_create_new ( i64 );
    i64* array2 = array_create_new ( i64 );
    i64* array;

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , array1 );
    EXPECT_NEQ ( 0 , array2 );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: array_insert handles invalid arguments.

    // TEST 1.1: array_insert logs an error if the index is invalid.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    array = array1;
    array_insert ( array1 , array_length ( array1 ) + 1 , to_insert[ 0 ] );

    // TEST 1.2: array_insert does not perform memory allocation if the index is invalid.
    EXPECT_EQ ( array , array1 );

    // TEST 1.3: array_insert does not modify array length if the index is invalid.
    EXPECT_EQ ( 0 , array_length ( array1 ) );

    // TEST 2: array_insert.

    array_insert ( array1 , array_length ( array1 ) , to_insert[ 0 ] );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , array1 );

    // TEST 2.1: array_insert increases array length by 1.
    EXPECT_EQ ( 1 , array_length ( array1 ) );

    // TEST 2.2: array_insert inserts the correct element at the correct index.
    EXPECT_EQ ( to_insert[ 0 ] , array1[ 0 ] );
    
    array_insert ( array1 , array_length ( array1 ) , to_insert[ 2 ] );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , array1 );

    // TEST 2.3: array_insert increases array length by 1.
    EXPECT_EQ ( 2 , array_length ( array1 ) );

    // TEST 2.4: array_insert inserts the correct element at the correct index.
    EXPECT_EQ ( to_insert[ 2 ] , array1[ 1 ] );

    array_insert ( array1 , array_length ( array1 ) , to_insert[ 3 ] );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , array1 );

    // TEST 2.5: array_insert increases array length by 1.
    EXPECT_EQ ( 3 , array_length ( array1 ) );

    // TEST 2.6: array_insert inserts the correct element at the correct index.
    EXPECT_EQ ( to_insert[ 3 ] , array1[ 2 ] );

    array_insert ( array1 , 1 , to_insert[ 1 ] );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , array1 );

    // TEST 2.7: Multiple subsequent invocations of array_insert result in the correct output array data and length.
    EXPECT_EQ ( 4 , array_length ( array1 ) );
    EXPECT ( memory_equal ( array1 , to_insert , sizeof ( to_insert ) ) );

    // TEST 2.8: array_insert yields the same results as array_push when inserting at the end of the array.
    array_push ( array2 , to_insert[ 0 ] );
    array_push ( array2 , to_insert[ 1 ] );
    array_push ( array2 , to_insert[ 2 ] );
    array_push ( array2 , to_insert[ 3 ] );
    EXPECT_EQ ( array_stride ( array2 ) , array_stride ( array1 ) );
    EXPECT_EQ ( array_length ( array2 ) , array_length ( array1 ) );
    EXPECT ( memory_equal ( array1 , array2 , array_stride ( array1 ) * array_length ( array1 ) ) );
    
    // TEST 3: array_remove handles invalid arguments.

    i64 removed1;
    i64 removed2;

    // TEST 3.1: array_remove logs an error if the index is invalid.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    array = array1;
    removed1 = 0;
    array_remove ( array1 , array_length ( array1 ) , &removed1 );

    // TEST 3.2: array_remove does not perform memory allocation if the index is invalid* (current implementation doesn't allocate in general either, but I don't need to test for that).
    EXPECT_EQ ( array , array1 );

    // TEST 3.3: array_remove does not modify array length if the index is invalid.
    EXPECT_EQ ( 4 , array_length ( array1 ) );

    // TEST 3.4: array_remove writes nothing to the output buffer if the index is invalid.
    EXPECT_EQ ( 0 , removed1 );
    
    // TEST 3.5: array_remove does not modify any array data if the index is invalid.
    EXPECT ( memory_equal ( array1 , array2 , array_stride ( array1 ) * array_length ( array1 ) ) );
    
    array_remove ( array1 , array_length ( array1 ) - 1 , &removed1 );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , array1 );

    // TEST 3.6: array_remove decreases array length by 1.
    EXPECT_EQ ( 3 , array_length ( array1 ) );

    // TEST 3.7: array_remove copies the correct element into the output buffer.
    EXPECT_EQ ( to_insert[ array_length ( array1 ) - 1 + 1 ] , removed1 );

    // TEST 3.8: array_remove removes the array element and leaves the remaining array data unmodified.
    EXPECT ( memory_equal ( array1 , to_insert , 3 * sizeof ( i64 ) ) );

    // TEST 3.9: array_remove yields the same results as array_pop when removing from the end of the array.
    array_pop ( array2 , &removed2 );
    EXPECT_EQ ( array_length ( array2 ) , array_length ( array1 ) );
    EXPECT ( memory_equal ( array1 , array2 , array_stride ( array1 ) * array_length ( array1 ) ) );

    // Add the removed element back to the array again prior to the test.
    array_push ( array1 , removed1 );
    EXPECT_NEQ ( 0 , array1 );
    EXPECT_EQ ( 4 , array_length ( array1 ) );
    EXPECT ( memory_equal ( array1 , to_insert , 4 * sizeof ( i64 ) ) );

    array_remove ( array1 , 2 , &removed1 );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , array1 );

    // TEST 3.10: array_remove decreases array length by 1.
    EXPECT_EQ ( 3 , array_length ( array1 ) );

    // TEST 3.11: array_remove copies the correct element into the output buffer.
    EXPECT_EQ ( to_insert[ 2 ] , removed1 );

    // TEST 3.12: array_remove removes the array element and leaves the remaining array data unmodified.
    EXPECT ( memory_equal ( array1 , remove1 , array_stride ( array1 ) * array_length ( array1 ) ) );
    
    array_remove ( array1 , 1 , &removed1 );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , array1 );

    // TEST 3.13: array_remove decreases array length by 1.
    EXPECT_EQ ( 2 , array_length ( array1 ) );

    // TEST 3.14: array_remove copies the correct element into the output buffer.
    EXPECT_EQ ( to_insert[ 1 ] , removed1 );

    // TEST 3.15: array_remove removes the array element and leaves the remaining array data unmodified.
    EXPECT ( memory_equal ( array1 , remove2 , array_stride ( array1 ) * array_length ( array1 ) ) );
    
    array_remove ( array1 , 0 , &removed1 );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , array1 );

    // TEST 3.16: array_remove decreases array length by 1.
    EXPECT_EQ ( 1 , array_length ( array1 ) );

    // TEST 3.17: array_remove copies the correct element into the output buffer.
    EXPECT_EQ ( to_insert[ 0 ] , removed1 );

    // TEST 3.18: array_remove removes the array element and leaves the remaining array data unmodified.
    EXPECT ( memory_equal ( array1 , remove3 , array_stride ( array1 ) * array_length ( array1 ) ) );
    
    array_remove ( array1 , 0 , 0 );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , array1 );

    // TEST 3.19: array_remove decreases array length by 1.
    EXPECT_EQ ( 0 , array_length ( array1 ) );

    // TEST 4: array_remove handles invalid aruments.

    // TEST 4.1: array_remove warns when the array is empty.
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    removed1 = 0;
    array = array1;
    array_remove ( array1 , 0 , &removed1 );

    // TEST 4.2: array_remove does not perform memory allocation if the array is empty* (current implementation doesn't allocate in general either, but I don't need to test for that).
    EXPECT_EQ ( array , array1 );

    // TEST 4.3: array_remove does not modify array length if the array is empty.
    EXPECT_EQ ( 0 , array_length ( array1 ) );

    // TEST 4.4: array_remove writes nothing to the output buffer if the array is empty.
    EXPECT_EQ ( 0 , removed1 );

    array_destroy ( array1 );
    array_destroy ( array2 );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

    return true;
}

u8
test_array_insert_and_remove_random
( void )
{
    u64 global_amount_allocated;
    u64 array_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    array_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ARRAY );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

    const u64 max_op = 10000;

    LOGDEBUG ( "Inserting %i elements into an array at random indices. . ." , max_op );

    i32* array = array_create_new ( i32 );
    i32* old_memory = memory_allocate ( sizeof ( i32 ) * max_op , MEMORY_TAG_ARRAY );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , array );
    EXPECT_NEQ ( 0 , old_memory );
    
    for ( u64 i = 0; i < max_op; ++i )
    {
        // Copy the array state prior to performing the operation.
        const u64 old_length = array_length ( array );
        memory_copy ( old_memory , array , old_length * array_stride ( array ) );

        // Insert random element at random index.
        const i32 to_insert = random ();
        const u64 insert_index = random2 ( 0 , old_length );
        array_insert ( array , insert_index , to_insert );

        // Verify there was no memory error prior to the test.
        EXPECT_NEQ ( 0 , array );

        // TEST: array_insert increases array length by 1.
        EXPECT_EQ ( old_length , array_length ( array ) - 1 );

        // TEST: array_insert inserts the correct element at the correct array index.
        EXPECT_EQ ( to_insert , array[ insert_index ] );

        // TEST: array_insert leaves the existing array data unmodified.
        if ( !insert_index )
        {
            EXPECT ( memory_equal ( &array[ 1 ] , &old_memory[ 0 ] , old_length * array_stride ( array ) ) );
        }
        else if ( insert_index == old_length )
        {
            EXPECT ( memory_equal ( &array[ 0 ] , &old_memory[ 0 ] , old_length * array_stride ( array ) ) );
        }
        else
        {
            EXPECT ( memory_equal ( &array[ 0 ] , &old_memory[ 0 ] , ( insert_index - 1 ) * array_stride ( array ) ) );
            EXPECT ( memory_equal ( &array[ insert_index + 1 ] , &old_memory[ insert_index ] , ( old_length - insert_index ) * array_stride ( array ) ) );
        }
    }

    LOGDEBUG ( "  Done." );

    LOGDEBUG ( "Removing them in random order. . ." );

    u64 old_length;
    i32 removed;

    while ( array_length ( array ) > 1 )
    {
        // Copy the array state prior to performing the operation.
        old_length = array_length ( array );
        memory_copy ( old_memory , array , old_length * array_stride ( array ) );

        // Remove element at a random index.
        const u64 remove_index = random2 ( 0 , old_length - 1 );
        removed = 0;
        array_remove ( array , remove_index , &removed );

        // Verify there was no memory error prior to the test.
        EXPECT_NEQ ( 0 , array );

        // TEST: array_remove decreases array length by 1.
        EXPECT_EQ ( old_length , array_length ( array ) + 1 );

        // TEST: array_remove writes the correct array element into the output buffer.
        EXPECT_EQ ( old_memory[ remove_index ] , removed );

        // TEST: array_remove removes the array element and leaves the remaining array data unmodified.
        if ( !remove_index )
        {
            EXPECT ( memory_equal ( &array[ 0 ] , &old_memory[ 1 ] , array_stride ( array ) * array_length ( array ) ) );
        }
        else if ( remove_index == old_length - 1 )
        {
            EXPECT ( memory_equal ( &array[ 0 ] , &old_memory[ 0 ] , array_stride ( array ) * array_length ( array ) ) );
        }
        else
        {
            EXPECT ( memory_equal ( &array[ 0 ] , &old_memory[ 0 ] , array_stride ( array ) * ( remove_index - 1 ) ) );
            EXPECT ( memory_equal ( &array[ remove_index ] , &old_memory[ remove_index + 1 ] ,   array_stride ( array ) * ( array_length ( array ) - remove_index ) ) );
        }
    }

    // TEST: array_remove succeeds when no output buffer is provided.
    array_remove ( array , 0 , 0 );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , array );

    // TEST: array_remove decreases array length by 1.
    EXPECT_EQ ( 0 , array_length ( array ) );

    // TEST: array_remove warns when array is empty.
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    removed = 0;
    i32* array_ = array;
    array_remove ( array , 0 , &removed );

    // TEST: array_remove does not perform memory allocation if the array is empty* (current implementation doesn't allocate in general either, but I don't need to test for that).
    EXPECT_EQ ( array_ , array );

    // TEST: array_remove does not modify array length if the array is empty.
    EXPECT_EQ ( 0 , array_length ( array ) );

    // TEST: array_remove writes nothing to the output buffer if the array is empty.
    EXPECT_EQ ( 0 , removed );

    memory_free ( old_memory , sizeof ( i32 ) * max_op , MEMORY_TAG_ARRAY );
    array_destroy ( array );

    LOGDEBUG ( "  Done." );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );
    
    return true;
}

void
test_register_array
( void )
{
    test_register ( test_array_create_and_destroy , "Allocating memory for a resizable array data structure." );
    test_register ( test_array_push_and_pop , "Testing array 'push' and 'pop' operations." );
    test_register ( test_array_insert_and_remove , "Testing array 'insert' and 'remove' operations." );
    test_register ( test_array_insert_and_remove_random , "Testing array 'insert' and 'remove' operations with random indices and elements." );
}