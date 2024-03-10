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

    // TEST: hashtable_create fails if no output buffer for hashtable is provided.
    EXPECT_NOT ( hashtable_create ( true , stride , capacity , 0 , 0 ) );

    // TEST: hashtable_create fails for a data-valued hashtable if provided stride is invalid.
    EXPECT_NOT ( hashtable_create ( false , 0 , capacity , 0 , &hashtable ) );

    // TEST: hashtable_create fails if provided capacity is invalid.
    EXPECT_NOT ( hashtable_create ( true , 0 , 0 , 0 , &hashtable ) );

    // TEST: hashtable_create (pointer-valued, auto-allocated) succeeds with valid arguments.
    EXPECT ( hashtable_create ( true , stride , capacity , 0 , &hashtable ) );

    // TEST: Pointer-valued, auto-allocated hashtable created via hashtable_create owns its own memory.
    EXPECT ( hashtable.owns_memory );

    // TEST: Pointer-valued, auto-allocated hashtable created via hashtable_create has access to a valid memory buffer.
    EXPECT_NEQ ( 0 , hashtable.memory );

    // TEST: Pointer-valued, auto-allocated hashtable created via hashtable_create has correct value of boolean 'pointer' field.
    EXPECT ( hashtable.pointer );

    // TEST: Pointer-valued, auto-allocated hashtable created via hashtable_create has stride equal to the size of a pointer (ignores the provided value of the 'stride' argument).
    EXPECT_EQ ( sizeof ( void* ) , hashtable.stride );

    // TEST: Pointer-valued, auto-allocated hashtable created via hashtable_create has correct capacity.
    EXPECT_EQ ( capacity , hashtable.capacity );

    // TEST: hashtable_destroy clears all hashtable data structure fields for a pointer-valued, auto-allocated hashtable.
    hashtable_destroy ( &hashtable );
    EXPECT_EQ ( 0 , hashtable.memory );
    EXPECT_EQ ( 0 , hashtable.owns_memory );
    EXPECT_EQ ( 0 , hashtable.pointer );
    EXPECT_EQ ( 0 , hashtable.stride );
    EXPECT_EQ ( 0 , hashtable.capacity );

    // TEST: hashtable_create (data-valued, auto-allocated) succeeds with valid arguments.
    EXPECT ( hashtable_create ( false , stride , capacity , 0 , &hashtable ) );

    // TEST: Data-valued, auto-allocated hashtable owns its own memory.
    EXPECT ( hashtable.owns_memory );

    // TEST: Data-valued, auto-allocated hashtable has access to a valid memory buffer following hashtable_create.
    EXPECT_NEQ ( 0 , hashtable.memory );

    // TEST: Data-valued, auto-allocated hashtable data structure has correct value of boolean 'pointer' field following hashtable_create.
    EXPECT_NOT ( hashtable.pointer );

    // TEST: Data-valued, auto-allocated hashtable data structure has correct stride following hashtable_create.
    EXPECT_EQ ( stride , hashtable.stride );

    // TEST: Data-valued, auto-allocated hashtable data structure has correct capacity following hashtable_create.
    EXPECT_EQ ( capacity , hashtable.capacity );

    // TEST: hashtable_destroy clears all hashtable data structure fields for a pointer-valued, auto-allocated hashtable.
    hashtable_destroy ( &hashtable );
    EXPECT_EQ ( 0 , hashtable.memory );
    EXPECT_EQ ( 0 , hashtable.owns_memory );
    EXPECT_EQ ( 0 , hashtable.pointer );
    EXPECT_EQ ( 0 , hashtable.stride );
    EXPECT_EQ ( 0 , hashtable.capacity );

    void* memory = memory_allocate ( capacity * stride , MEMORY_TAG_HASHTABLE );

    // TEST: hashtable_create (pointer-valued, pre-allocated) succeeds with valid arguments.
    EXPECT ( hashtable_create ( true , stride , capacity , memory , &hashtable ) );

    // TEST: Pointer-valued, pre-allocated hashtable does not own its own memory.
    EXPECT_NOT ( hashtable.owns_memory );

    // TEST: Pointer-valued, pre-allocated hashtable has access to the provided memory buffer following hashtable_create.
    EXPECT_EQ ( memory , hashtable.memory );

    // TEST: Pointer-valued, pre-allocated hashtable data structure has correct value of boolean 'pointer' field following hashtable_create.
    EXPECT ( hashtable.pointer );

    // TEST: Pointer-valued, pre-allocated hashtable data structure has stride equal to the size of a pointer following hashtable_create (ignores the provided value of the 'stride' argument).
    EXPECT_EQ ( sizeof ( void* ) , hashtable.stride );

    // TEST: Pointer-valued, pre-allocated hashtable data structure has correct capacity following hashtable_create.
    EXPECT_EQ ( capacity , hashtable.capacity );

    // TEST: hashtable_destroy clears all hashtable data structure fields for a pointer-valued, pre-allocated hashtable.
    hashtable_destroy ( &hashtable );
    EXPECT_EQ ( 0 , hashtable.memory );
    EXPECT_EQ ( 0 , hashtable.owns_memory );
    EXPECT_EQ ( 0 , hashtable.pointer );
    EXPECT_EQ ( 0 , hashtable.stride );
    EXPECT_EQ ( 0 , hashtable.capacity );

    // TEST: hashtable_create (data-valued, pre-allocated) succeeds with valid arguments.
    EXPECT ( hashtable_create ( false , stride , capacity , memory , &hashtable ) );
    
    // TEST: Data-valued, pre-allocated hashtable does not own its own memory.
    EXPECT_NOT ( hashtable.owns_memory );

    // TEST: Data-valued, pre-allocated hashtable has access to the provided memory buffer following hashtable_create.
    EXPECT_EQ ( memory , hashtable.memory );

    // TEST: Data-valued, pre-allocated hashtable data structure has correct value of boolean 'pointer' field following hashtable_create.
    EXPECT_NOT ( hashtable.pointer );

    // TEST: Data-valued, pre-allocated hashtable data structure has correct stride following hashtable_create.
    EXPECT_EQ ( stride , hashtable.stride );

    // TEST: Data-valued, pre-allocated hashtable data structure has correct capacity following hashtable_create.
    EXPECT_EQ ( capacity , hashtable.capacity );

    // TEST: hashtable_destroy clears all hashtable data structure fields for a data-valued, pre-allocated hashtable.
    hashtable_destroy ( &hashtable );
    EXPECT_EQ ( 0 , hashtable.memory );
    EXPECT_EQ ( 0 , hashtable.owns_memory );
    EXPECT_EQ ( 0 , hashtable.pointer );
    EXPECT_EQ ( 0 , hashtable.stride );
    EXPECT_EQ ( 0 , hashtable.capacity );

    memory_free ( memory , capacity * stride , MEMORY_TAG_HASHTABLE );

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

    // Verify there was no memory error prior to testing.
    EXPECT_NEQ ( 0 , hashtable.memory );

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: hashtable_set logs an error and fails if no hashtable is provided.
    EXPECT_NOT ( hashtable_set ( 0 , "key" , &values[ 0 ] ) );

    // TEST: hashtable_set logs an error and fails on a data-valued hashtable if no key is provided.
    EXPECT_NOT ( hashtable_set ( &hashtable , 0 , &values[ 0 ] ) );

    // TEST: hashtable_set logs an error and fails on a data-valued hashtable if no handle to a value is provided.
    EXPECT_NOT ( hashtable_set ( &hashtable , "key0" , 0 ) );

    // TEST: hashtable_set succeeds on a data-valued hashtable given valid arguments.
    EXPECT ( hashtable_set ( &hashtable , "key0" , &values[ 0 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key1" , &values[ 1 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key2" , &values[ 2 ] ) );

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: hashtable_get logs an error and fails if no hashtable is provided.
    EXPECT_NOT ( hashtable_get ( 0 , "key" , &get ) );

    // TEST: hashtable_get logs an error and fails on a data-valued hashtable if no key is provided.
    EXPECT_NOT ( hashtable_get ( &hashtable , 0 , &get ) );

    // TEST: hashtable_get logs an error and fails on a data-valued hashtable if no output buffer is provided.
    EXPECT_NOT ( hashtable_get ( &hashtable , "key0" , 0 ) );

    // TEST: hashtable_get succeeds on a data-valued hashtable given valid arguments.
    EXPECT ( hashtable_get ( &hashtable , "key0" , &get ) );

    // TEST: hashtable_get writes the correct data into the provided output buffer.
    EXPECT_EQ ( values[ 0 ] , get );

    // TEST: hashtable_get succeeds on a data-valued hashtable given valid arguments.
    EXPECT ( hashtable_get ( &hashtable , "key1" , &get ) );

    // TEST: hashtable_get writes the correct data into the provided output buffer.
    EXPECT_EQ ( values[ 1 ] , get );

    // TEST: hashtable_get succeeds on a data-valued hashtable given valid arguments.
    EXPECT ( hashtable_get ( &hashtable , "key2" , &get ) );

    // TEST: hashtable_get writes the correct data into the provided output buffer.
    EXPECT_EQ ( values[ 2 ] , get );

    hashtable_destroy ( &hashtable );

    // TEST: hashtable_set logs an error and fails if the provided data-valued hashtable is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( hashtable_set ( &hashtable , "key" , &values[ 0 ] ) );
    
    // TEST: hashtable_get logs an error and fails if the provided data-valued hashtable is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
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

    // Verify there was no memory error prior to testing.
    EXPECT_NEQ ( 0 , hashtable.memory );

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: hashtable_set logs an error and fails if no hashtable is provided.
    EXPECT_NOT ( hashtable_set ( 0 , "key" , &values[ 0 ] ) );

    // TEST: hashtable_set logs an error and fails on a pointer-valued hashtable if no key is provided.
    EXPECT_NOT ( hashtable_set ( &hashtable , 0 , &values[ 0 ] ) );

    // TEST: hashtable_set succeeds on a pointer-valued hashtable given valid arguments.
    EXPECT ( hashtable_set ( &hashtable , "key0" , &values[ 0 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key1" , &values[ 1 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key2" , &values[ 2 ] ) );

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: hashtable_get logs an error and fails if no hashtable is provided.
    EXPECT_NOT ( hashtable_get ( 0 , "key" , &get ) );

    // TEST: hashtable_get logs an error and fails on a pointer-valued hashtable if no key is provided.
    EXPECT_NOT ( hashtable_get ( &hashtable , 0 , &get ) );

    // TEST: hashtable_get logs an error and fails on a pointer-valued hashtable if no output buffer is provided.
    EXPECT_NOT ( hashtable_get ( &hashtable , "key0" , 0 ) );

    // TEST: hashtable_get succeeds on a pointer-valued hashtable given valid arguments.
    EXPECT ( hashtable_get ( &hashtable , "key0" , &get ) );

    // TEST: hashtable_get writes the correct pointer into the output buffer.
    EXPECT_EQ ( &values[ 0 ] , get );
    EXPECT_EQ ( values[ 0 ] , *get );

    // TEST: hashtable_get succeeds on a pointer-valued hashtable given valid arguments.
    EXPECT ( hashtable_get ( &hashtable , "key1" , &get ) );

    // TEST: hashtable_get writes the correct pointer into the output buffer.
    EXPECT_EQ ( &values[ 1 ] , get );
    EXPECT_EQ ( values[ 1 ] , *get );

    // TEST: hashtable_get succeeds on a pointer-valued hashtable given valid arguments.
    EXPECT ( hashtable_get ( &hashtable , "key2" , &get ) );

    // TEST: hashtable_get writes the correct pointer into the output buffer.
    EXPECT_EQ ( &values[ 2 ] , get );
    EXPECT_EQ ( values[ 2 ] , *get );

    // Update the values at the addresses the hashtable pointers reference, then run the same tests again.
    values[ 0 ] = values[ 0 ] + 1;
    values[ 1 ] = values[ 1 ] + 1;
    values[ 2 ] = values[ 2 ] + 1;

    // TEST: hashtable_get succeeds on a pointer-valued hashtable given valid arguments.
    EXPECT ( hashtable_get ( &hashtable , "key0" , &get ) );

    // TEST: hashtable_get writes the correct pointer into the output buffer.
    EXPECT_EQ ( &values[ 0 ] , get );
    EXPECT_EQ ( values[ 0 ] , *get );

    // TEST: hashtable_get succeeds on a pointer-valued hashtable given valid arguments.
    EXPECT ( hashtable_get ( &hashtable , "key1" , &get ) );

    // TEST: hashtable_get writes the correct pointer into the output buffer.
    EXPECT_EQ ( &values[ 1 ] , get );
    EXPECT_EQ ( values[ 1 ] , *get );

    // TEST: hashtable_get succeeds on a pointer-valued hashtable given valid arguments.
    EXPECT ( hashtable_get ( &hashtable , "key2" , &get ) );

    // TEST: hashtable_get writes the correct pointer into the output buffer.
    EXPECT_EQ ( &values[ 2 ] , get );
    EXPECT_EQ ( values[ 2 ] , *get );

    hashtable_destroy ( &hashtable );

    // TEST: hashtable_set logs an error and fails if the provided pointer-valued hashtable is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( hashtable_set ( &hashtable , "key" , &values[ 0 ] ) );
    
    // TEST: hashtable_get logs an error and fails if the provided pointer-valued hashtable is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
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

    // Set a key-value pair within a hashtable.
    EXPECT ( hashtable_create ( false , element_size , element_count , memory , &hashtable ) );
    EXPECT_NEQ ( 0 , hashtable.memory ); // Verify there was no memory error prior to testing.
    EXPECT ( hashtable_set ( &hashtable , "key0" , &values[ 0 ] ) );

    // TEST: hashtable_get succeeds even when the provided key cannot be found within the hashtable.
    get = 0;
    EXPECT ( hashtable_get ( &hashtable , "key1" , &get ) );

    // TEST: hashtable_get does not modify the output buffer if the provided key could not be found.
    EXPECT_EQ ( 0 , get );

    // TEST: hashtable_get succeeds even when the provided key cannot be found within the hashtable.
    get = 0;
    EXPECT ( hashtable_get ( &hashtable , "key2" , &get ) );

    // TEST: hashtable_get does not modify the output buffer if the provided key could not be found.
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

    // Populate a hashtable with key-value pairs.
    EXPECT ( hashtable_create ( true , 0 , element_count , memory , &hashtable ) );
    EXPECT_NEQ ( 0 , hashtable.memory ); // Verify there was no memory error prior to testing.
    EXPECT ( hashtable_set ( &hashtable , "key0" , &values[ 0 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key1" , &values[ 1 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key2" , &values[ 2 ] ) );

    // Verify all the key-value pairs prior to testing.
    get = 0;
    EXPECT ( hashtable_get ( &hashtable , "key0" , &get ) );
    EXPECT_EQ ( &values[ 0 ] , get );
    EXPECT_EQ ( values[ 0 ] , *get );
    get = 0;
    EXPECT ( hashtable_get ( &hashtable , "key1" , &get ) );
    EXPECT_EQ ( &values[ 1 ] , get );
    EXPECT_EQ ( values[ 1 ] , *get );
    get = 0;
    EXPECT ( hashtable_get ( &hashtable , "key2" , &get ) );
    EXPECT_EQ ( &values[ 2 ] , get );
    EXPECT_EQ ( values[ 2 ] , *get );

    // TEST: hashtable_set succeeds on a pointer-valued hashtable even when no value handle is provided.
    EXPECT ( hashtable_set ( &hashtable , "key0" , 0 ) );
    EXPECT ( hashtable_set ( &hashtable , "key1" , 0 ) );
    EXPECT ( hashtable_set ( &hashtable , "key2" , 0 ) );

    // TEST: The pointer values set for a key which was referenced by a hashtable_set call on a pointer-valued hashtable without a value handle should be null.
    get = ( ( void* ) -1 );
    EXPECT ( hashtable_get ( &hashtable , "key0" , &get ) );
    EXPECT_EQ ( 0 , get );

    // TEST: The pointer values set for a key which was referenced by a hashtable_set call on a pointer-valued hashtable without a value handle should be null.
    get = ( ( void* ) -1 );
    EXPECT ( hashtable_get ( &hashtable , "key1" , &get ) );
    EXPECT_EQ ( 0 , get );

    // TEST: The pointer values set for a key which was referenced by a hashtable_set call on a pointer-valued hashtable without a value handle should be null.
    get = ( ( void* ) -1 );
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