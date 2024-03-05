/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/test_array.c
 * @brief Implementation of the container/test_array header.
 * (see container/test_array.h for additional details)
 */
#include "container/test_array.h"

#include "test/expect.h"

#include "core/memory.h"

u8
test_array_create_and_destroy
( void )
{
    u16* array = array_create_new ( u16 );
    EXPECT_NEQ ( 0 , array );
    EXPECT_EQ ( ARRAY_DEFAULT_CAPACITY , array_capacity ( array ) );
    EXPECT_EQ ( 0 , array_length ( array ) );
    EXPECT_EQ ( sizeof ( u16 ) , array_stride ( array ) );
    for ( u64 i = 0; i < ARRAY_DEFAULT_CAPACITY; ++i )
    {
        EXPECT_EQ ( 0 , array[ i ] );
    }
    array_destroy ( array );
    const u64 initial_capacity = 4 * ARRAY_DEFAULT_CAPACITY + 3;
    array = array_create ( u16 , initial_capacity );
    EXPECT_NEQ ( 0 , array );
    EXPECT_EQ ( initial_capacity , array_capacity ( array ) );
    EXPECT_EQ ( 0 , array_length ( array ) );
    EXPECT_EQ ( sizeof ( u16 ) , array_stride ( array ) );
    for ( u64 i = 0; i < initial_capacity; ++i )
    {
        EXPECT_EQ ( 0 , array[ i ] );
    }
    array_destroy ( array );
    return true;
}

u8
test_array_push_and_pop
( void )
{
    const i16 to_push = 437;
    const u64 max_op = 100000;
    i16* array = array_create_new ( i16 );
    LOGDEBUG ( "Appending %i elements to an array. . ." , max_op );
    i16* old_memory = memory_allocate ( sizeof ( i16 ) * max_op , MEMORY_TAG_ARRAY );
    for ( u64 i = 0; i < max_op; ++i )
    {
        const u64 old_length = array_length ( array );
        memory_copy ( old_memory , array , old_length * array_stride ( array ) );
        array_push ( array , to_push );
        EXPECT_NEQ ( 0 , array );
        EXPECT_EQ ( old_length + 1 , array_length ( array ) );
        EXPECT_EQ ( to_push , array[ old_length ] );
        EXPECT ( memory_equal ( array , old_memory , old_length ) );
    }
    LOGDEBUG ( "  Done." );
    LOGDEBUG ( "Removing them. . ." );
    u64 old_length;
    i16 popped;
    while ( array_length ( array ) > 1 )
    {
        old_length = array_length ( array );
        memory_copy ( old_memory , array , old_length );
        popped = 0;
        array_pop ( array , &popped );
        EXPECT_NEQ ( 0 , array );
        EXPECT_EQ ( old_length - 1 , array_length ( array ) );
        EXPECT_EQ ( to_push , popped );
        EXPECT ( memory_equal ( array , old_memory , array_length ( array ) ) );
    }
    array_pop ( array , 0 );
    EXPECT_NEQ ( 0 , array );
    EXPECT_EQ ( 0 , array_length ( array ) );
    popped = 0;
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    array_pop ( array , &popped );
    EXPECT_NEQ ( 0 , array );
    EXPECT_EQ ( 0 , array_length ( array ) );
    EXPECT_EQ ( 0 , popped );
    memory_free ( old_memory , sizeof ( i16 ) * max_op , MEMORY_TAG_ARRAY );
    LOGDEBUG ( "  Done." );
    array_destroy ( array );
    return true;
}

u8
test_array_insert_and_remove
( void )
{
    const i64 to_insert[] = { 437 , 24 , -179253478 , -15799 };
    const i64 remove1[]   = { 437 , 24 , -15799 };
    const i64 remove2[]   = { 437 , -15799 };
    const i64 remove3[]   = { -15799 };
    i64* array1 = array_create_new ( i64 );
    i64* array2 = array_create_new ( i64 );
    LOGWARN ( "The following warning and errors are intentionally triggered by a test:" );
    array_insert ( array1 , array_length ( array1 ) + 1 , to_insert[ 0 ] );
    EXPECT_NEQ ( 0 , array1 );
    EXPECT_EQ ( 0 , array_length ( array1 ) );
    array_insert ( array1 , array_length ( array1 ) , to_insert[ 0 ] );
    EXPECT_NEQ ( 0 , array1 );
    EXPECT_EQ ( 1 , array_length ( array1 ) );
    EXPECT_EQ ( to_insert[ 0 ] , array1[ 0 ] );
    array_insert ( array1 , array_length ( array1 ) , to_insert[ 2 ] );
    EXPECT_NEQ ( 0 , array1 );
    EXPECT_EQ ( 2 , array_length ( array1 ) );
    EXPECT_EQ ( to_insert[ 2 ] , array1[ 1 ] );
    array_insert ( array1 , array_length ( array1 ) , to_insert[ 3 ] );
    EXPECT_NEQ ( 0 , array1 );
    EXPECT_EQ ( 3 , array_length ( array1 ) );
    EXPECT_EQ ( to_insert[ 3 ] , array1[ 2 ] );
    array_insert ( array1 , 1 , to_insert[ 1 ] );
    EXPECT_NEQ ( 0 , array1 );
    EXPECT_EQ ( 4 , array_length ( array1 ) );
    EXPECT ( memory_equal ( array1 , to_insert , sizeof ( to_insert ) ) );
    array_push ( array2 , to_insert[ 0 ] );
    array_push ( array2 , to_insert[ 1 ] );
    array_push ( array2 , to_insert[ 2 ] );
    array_push ( array2 , to_insert[ 3 ] );
    EXPECT_EQ ( array_stride ( array1 ) , array_stride ( array2 ) );
    EXPECT_EQ ( array_length ( array1 ) , array_length ( array2 ) );
    EXPECT ( memory_equal ( array1 , array2 , array_stride ( array1 ) * array_length ( array1 ) ) );
    i64 removed1 = 0;
    i64 removed2 = 0;
    array_remove ( array1 , array_length ( array1 ) , &removed1 );
    EXPECT_NEQ ( 0 , array1 );
    EXPECT_EQ ( 4 , array_length ( array1 ) );
    EXPECT_EQ ( array_stride ( array1 ) , array_stride ( array2 ) );
    EXPECT_EQ ( array_length ( array1 ) , array_length ( array2 ) );
    EXPECT_EQ ( 0 , removed1 );
    EXPECT ( memory_equal ( array1 , array2 , array_stride ( array1 ) * array_length ( array1 ) ) );
    array_remove ( array1 , array_length ( array1 ) - 1 , &removed1 );
    EXPECT_NEQ ( 0 , array1 );
    EXPECT_EQ ( 3 , array_length ( array1 ) );
    EXPECT_EQ ( to_insert[ array_length ( array1 ) - 1 + 1 ] , removed1 );
    EXPECT ( memory_equal ( array1 , to_insert , 3 * sizeof ( i64 ) ) );
    array_pop ( array2 , &removed2 );
    EXPECT_EQ ( array_stride ( array1 ) , array_stride ( array2 ) );
    EXPECT_EQ ( array_length ( array1 ) , array_length ( array2 ) );
    EXPECT ( memory_equal ( array1 , array2 , array_stride ( array1 ) * array_length ( array1 ) ) );
    array_push ( array1 , removed1 );
    EXPECT_NEQ ( 0 , array1 );
    EXPECT_EQ ( 4 , array_length ( array1 ) );
    EXPECT ( memory_equal ( array1 , to_insert , 4 * sizeof ( i64 ) ) );
    array_remove ( array1 , 2 , &removed1 );
    EXPECT_NEQ ( 0 , array1 );
    EXPECT_EQ ( 3 , array_length ( array1 ) );
    EXPECT_EQ ( to_insert[ 2 ] , removed1 );
    EXPECT ( memory_equal ( array1 , remove1 , array_stride ( array1 ) * array_length ( array1 ) ) );
    array_remove ( array1 , 1 , &removed1 );
    EXPECT_NEQ ( 0 , array1 );
    EXPECT_EQ ( 2 , array_length ( array1 ) );
    EXPECT_EQ ( to_insert[ 1 ] , removed1 );
    EXPECT ( memory_equal ( array1 , remove2 , array_stride ( array1 ) * array_length ( array1 ) ) );
    array_remove ( array1 , 0 , &removed1 );
    EXPECT_NEQ ( 0 , array1 );
    EXPECT_EQ ( 1 , array_length ( array1 ) );
    EXPECT_EQ ( to_insert[ 0 ] , removed1 );
    EXPECT ( memory_equal ( array1 , remove3 , array_stride ( array1 ) * array_length ( array1 ) ) );
    array_remove ( array1 , 0 , 0 );
    EXPECT_NEQ ( 0 , array1 );
    EXPECT_EQ ( 0 , array_length ( array1 ) );
    removed1 = 0;
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    array_remove ( array1 , 0 , &removed1 );
    EXPECT_NEQ ( 0 , array1 );
    EXPECT_EQ ( 0 , array_length ( array1 ) );
    EXPECT_EQ ( 0 , removed1 );
    array_destroy ( array1 );
    array_destroy ( array2 );
    return true;
}

u8
test_array_insert_and_remove_random
( void )
{
    const u64 max_op = 10000;
    i32* array = array_create_new ( i32 );
    LOGDEBUG ( "Inserting %i elements into an array at random indices. . ." , max_op );
    i32* old_memory = memory_allocate ( sizeof ( i32 ) * max_op , MEMORY_TAG_ARRAY );
    for ( u64 i = 0; i < max_op; ++i )
    {
        const u64 old_length = array_length ( array );
        memory_copy ( old_memory , array , old_length * array_stride ( array ) );
        const i32 to_insert = random ();
        const u64 insert_index = random2 ( 0 , old_length );
        array_insert ( array , insert_index , to_insert );
        EXPECT_NEQ ( 0 , array );
        EXPECT_EQ ( old_length , array_length ( array ) - 1 );
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
        EXPECT_EQ ( to_insert , array[ insert_index ] );
    }
    LOGDEBUG ( "  Done." );
    LOGDEBUG ( "Removing them in random order. . ." );
    u64 old_length;
    i32 removed;
    while ( array_length ( array ) > 1 )
    {
        old_length = array_length ( array );
        memory_copy ( old_memory , array , old_length * array_stride ( array ) );
        removed = 0;
        const u64 remove_index = random2 ( 0 , old_length - 1 );
        array_remove ( array , remove_index , &removed );
        EXPECT_NEQ ( 0 , array );
        EXPECT_EQ ( old_length , array_length ( array ) + 1 );
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
        EXPECT_EQ ( old_memory[ remove_index ] , removed );
    }
    array_remove ( array , 0 , 0 );
    EXPECT_NEQ ( 0 , array );
    EXPECT_EQ ( 0 , array_length ( array ) );
    removed = 0;
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    array_pop ( array , &removed );
    EXPECT_NEQ ( 0 , array );
    EXPECT_EQ ( 0 , array_length ( array ) );
    EXPECT_EQ ( 0 , removed );
    memory_free ( old_memory , sizeof ( i32 ) * max_op , MEMORY_TAG_ARRAY );
    LOGDEBUG ( "  Done." );
    array_destroy ( array );
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