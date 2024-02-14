/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/test_hashtable.c
 * @brief Implementation of the container/test_hashtable header.
 * (see container/test_hashtable.h for additional details)
 */
#include "container/test_hashtable.h"

#include "test/expect.h"

#include "core/memory.h"

#include "math/math.h"

u8
test_hashtable_create_and_destroy
( void )
{
    const u64 stride = 8;
    const u32 capacity = 64;
    hashtable_t hashtable;
    LOGWARN ( "The following errors are intentionally triggered by a test:" );
    EXPECT_NOT ( hashtable_create ( true , 0 , capacity , 0 , 0 ) );
    EXPECT_NOT ( hashtable_create ( true , 0 , 0 , 0 , &hashtable ) );
    EXPECT_NOT ( hashtable_create ( false , stride , capacity , 0 , 0 ) );
    EXPECT_NOT ( hashtable_create ( false , stride , 0 , 0 , &hashtable ) );
    EXPECT_NOT ( hashtable_create ( false , 0 , capacity , 0 , &hashtable ) );
    EXPECT ( hashtable_create ( true , 0 , capacity , 0 , &hashtable ) );
    EXPECT_NEQ ( 0 , hashtable.memory );
    EXPECT ( hashtable.owns_memory );
    EXPECT ( hashtable.pointer );
    EXPECT_EQ ( sizeof ( void* ) , hashtable.stride );
    EXPECT_EQ ( capacity , hashtable.capacity );
    hashtable_destroy ( &hashtable );
    EXPECT_EQ ( 0 , hashtable.memory );
    EXPECT_EQ ( 0 , hashtable.owns_memory );
    EXPECT_EQ ( 0 , hashtable.pointer );
    EXPECT_EQ ( 0 , hashtable.stride );
    EXPECT_EQ ( 0 , hashtable.capacity );
    EXPECT ( hashtable_create ( false , stride , capacity , 0 , &hashtable ) );
    EXPECT_NEQ ( 0 , hashtable.memory );
    EXPECT ( hashtable.owns_memory );
    EXPECT_NOT ( hashtable.pointer );
    EXPECT_EQ ( stride , hashtable.stride );
    EXPECT_EQ ( capacity , hashtable.capacity );
    hashtable_destroy ( &hashtable );
    EXPECT_EQ ( 0 , hashtable.memory );
    EXPECT_EQ ( 0 , hashtable.owns_memory );
    EXPECT_EQ ( 0 , hashtable.pointer );
    EXPECT_EQ ( 0 , hashtable.stride );
    EXPECT_EQ ( 0 , hashtable.capacity );
    void* memory = memory_allocate ( capacity , MEMORY_TAG_QUEUE );
    EXPECT ( hashtable_create ( true , 0 , capacity , memory , &hashtable ) );
    EXPECT_EQ ( memory , hashtable.memory );
    EXPECT_NOT ( hashtable.owns_memory );
    EXPECT ( hashtable.pointer );
    EXPECT_EQ ( sizeof ( void* ) , hashtable.stride );
    EXPECT_EQ ( capacity , hashtable.capacity );
    hashtable_destroy ( &hashtable );
    EXPECT_EQ ( 0 , hashtable.memory );
    EXPECT_EQ ( 0 , hashtable.owns_memory );
    EXPECT_EQ ( 0 , hashtable.pointer );
    EXPECT_EQ ( 0 , hashtable.stride );
    EXPECT_EQ ( 0 , hashtable.capacity );
    EXPECT ( hashtable_create ( false , stride , capacity , memory , &hashtable ) );
    EXPECT_EQ ( memory , hashtable.memory );
    EXPECT_NOT ( hashtable.owns_memory );
    EXPECT_NOT ( hashtable.pointer );
    EXPECT_EQ ( stride , hashtable.stride );
    EXPECT_EQ ( capacity , hashtable.capacity );
    hashtable_destroy ( &hashtable );
    EXPECT_EQ ( 0 , hashtable.memory );
    EXPECT_EQ ( 0 , hashtable.owns_memory );
    EXPECT_EQ ( 0 , hashtable.pointer );
    EXPECT_EQ ( 0 , hashtable.stride );
    EXPECT_EQ ( 0 , hashtable.capacity );
    memory_free ( memory , capacity , MEMORY_TAG_QUEUE );
    return true;
}

u8
test_hashtable_set_and_get_data
( void )
{
    const u64 element_size = sizeof ( u64 );
    const u64 element_count = 3;
    const u64 values[ 3 ] = { random () , random () , random () };
    u64 memory[ 3 ];
    u64 get;
    hashtable_t hashtable;
    EXPECT ( hashtable_create ( false , element_size , element_count , memory , &hashtable ) );
    LOGWARN ( "The following errors are intentionally triggered by a test:" );
    EXPECT_NOT ( hashtable_set ( &hashtable , 0 , &values[ 0 ] ) );
    EXPECT_NOT ( hashtable_set ( &hashtable , "key0" , 0 ) );
    EXPECT ( hashtable_set ( &hashtable , "key0" , &values[ 0 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key1" , &values[ 1 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key2" , &values[ 2 ] ) );
    LOGWARN ( "The following errors are intentionally triggered by a test:" );
    EXPECT_NOT ( hashtable_get ( &hashtable , 0 , &get ) );
    EXPECT_NOT ( hashtable_get ( &hashtable , "key0" , 0 ) );
    EXPECT ( hashtable_get ( &hashtable , "key0" , &get ) );
    EXPECT_EQ ( values[ 0 ] , get );
    EXPECT ( hashtable_get ( &hashtable , "key1" , &get ) );
    EXPECT_EQ ( values[ 1 ] , get );
    EXPECT ( hashtable_get ( &hashtable , "key2" , &get ) );
    EXPECT_EQ ( values[ 2 ] , get );
    hashtable_destroy ( &hashtable );
    LOGWARN ( "The following errors are intentionally triggered by a test:" );
    EXPECT_NOT ( hashtable_set ( 0 , "key" , &values[ 0 ] ) );
    EXPECT_NOT ( hashtable_set ( &hashtable , "key" , &values[ 0 ] ) );
    EXPECT_NOT ( hashtable_get ( 0 , "key" , &get ) );
    EXPECT_NOT ( hashtable_get ( &hashtable , "key" , &get ) );
    return true;
}

u8
test_hashtable_set_and_get_pointer
( void )
{
    const u64 element_count = 3;
    u64 values[ 3 ] = { random () , random () , random () };
    u64* memory[ 3 ];
    u64* get;
    hashtable_t hashtable;
    EXPECT ( hashtable_create ( true , 0 , element_count , memory , &hashtable ) );
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( hashtable_set ( &hashtable , 0 , &values[ 0 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key0" , &values[ 0 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key1" , &values[ 1 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key2" , &values[ 2 ] ) );
    LOGWARN ( "The following errors are intentionally triggered by a test:" );
    EXPECT_NOT ( hashtable_get ( &hashtable , 0 , &get ) );
    EXPECT_NOT ( hashtable_get ( &hashtable , "key0" , 0 ) );
    EXPECT ( hashtable_get ( &hashtable , "key0" , &get ) );
    EXPECT_EQ ( &values[ 0 ] , get );
    EXPECT_EQ ( values[ 0 ] , *get );
    EXPECT ( hashtable_get ( &hashtable , "key1" , &get ) );
    EXPECT_EQ ( &values[ 1 ] , get );
    EXPECT_EQ ( values[ 1 ] , *get );
    EXPECT ( hashtable_get ( &hashtable , "key2" , &get ) );
    EXPECT_EQ ( &values[ 2 ] , get );
    EXPECT_EQ ( values[ 2 ] , *get );
    values[ 0 ] = values[ 0 ] + 1;
    values[ 1 ] = values[ 1 ] + 1;
    values[ 2 ] = values[ 2 ] + 1;
    EXPECT ( hashtable_get ( &hashtable , "key0" , &get ) );
    EXPECT_EQ ( &values[ 0 ] , get );
    EXPECT_EQ ( values[ 0 ] , *get );
    EXPECT ( hashtable_get ( &hashtable , "key1" , &get ) );
    EXPECT_EQ ( &values[ 1 ] , get );
    EXPECT_EQ ( values[ 1 ] , *get );
    EXPECT ( hashtable_get ( &hashtable , "key2" , &get ) );
    EXPECT_EQ ( &values[ 2 ] , get );
    EXPECT_EQ ( values[ 2 ] , *get );
    hashtable_destroy ( &hashtable );
    LOGWARN ( "The following errors are intentionally triggered by a test:" );
    EXPECT_NOT ( hashtable_set ( 0 , "key" , &values[ 0 ] ) );
    EXPECT_NOT ( hashtable_set ( &hashtable , "key" , &values[ 0 ] ) );
    EXPECT_NOT ( hashtable_get ( 0 , "key" , &get ) );
    EXPECT_NOT ( hashtable_get ( &hashtable , "key" , &get ) );
    return true;
}

u8
test_hashtable_get_nonexistent
( void )
{
    const u64 element_size = sizeof ( u64 );
    const u64 element_count = 3;
    const u64 values[ 3 ] = { random () , random () , random () };
    u64 memory[ 3 ];
    u64 get;
    hashtable_t hashtable;
    EXPECT ( hashtable_create ( false , element_size , element_count , memory , &hashtable ) );
    EXPECT ( hashtable_set ( &hashtable , "key0" , &values[ 0 ] ) );
    get = 0;
    EXPECT ( hashtable_get ( &hashtable , "key1" , &get ) );
    EXPECT_EQ ( 0 , get );
    EXPECT ( hashtable_get ( &hashtable , "key2" , &get ) );
    EXPECT_EQ ( 0 , get );
    hashtable_destroy ( &hashtable );
    return true;
}

u8
test_hashtable_remove_pointer
( void )
{
    const u64 element_count = 3;
    u64 values[ 3 ] = { random () , random () , random () };
    u64* memory[ 3 ];
    u64* get;
    hashtable_t hashtable;
    EXPECT ( hashtable_create ( true , 0 , element_count , memory , &hashtable ) );
    EXPECT ( hashtable_set ( &hashtable , "key0" , &values[ 0 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key1" , &values[ 1 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key2" , &values[ 2 ] ) );
    EXPECT ( hashtable_get ( &hashtable , "key0" , &get ) );
    EXPECT_EQ ( &values[ 0 ] , get );
    EXPECT_EQ ( values[ 0 ] , *get );
    EXPECT ( hashtable_get ( &hashtable , "key1" , &get ) );
    EXPECT_EQ ( &values[ 1 ] , get );
    EXPECT_EQ ( values[ 1 ] , *get );
    EXPECT ( hashtable_get ( &hashtable , "key2" , &get ) );
    EXPECT_EQ ( &values[ 2 ] , get );
    EXPECT_EQ ( values[ 2 ] , *get );
    EXPECT ( hashtable_set ( &hashtable , "key0" , 0 ) );
    EXPECT ( hashtable_set ( &hashtable , "key1" , 0 ) );
    EXPECT ( hashtable_set ( &hashtable , "key2" , 0 ) );
    get = 0;
    EXPECT ( hashtable_get ( &hashtable , "key0" , &get ) );
    EXPECT_EQ ( 0 , get );
    EXPECT ( hashtable_get ( &hashtable , "key1" , &get ) );
    EXPECT_EQ ( 0 , get );
    EXPECT ( hashtable_get ( &hashtable , "key2" , &get ) );
    EXPECT_EQ ( 0 , get );
    hashtable_destroy ( &hashtable );
    return true;
}

void
test_register_hashtable
( void )
{
    test_register ( test_hashtable_create_and_destroy , "Creating or destroying a hashtable." );
    test_register ( test_hashtable_set_and_get_data , "Testing 'set' and 'get' operations for a data-valued hashtable." );
    test_register ( test_hashtable_set_and_get_pointer , "Testing 'set' and 'get' operations for a pointer-valued hashtable." );
    test_register ( test_hashtable_get_nonexistent , "Testing hashtable 'get' operation with an argument that cannot be found within the hashtable." );
    test_register ( test_hashtable_remove_pointer , "Testing the ability to remove a pointer value within a pointer-valued hashtable." );
}