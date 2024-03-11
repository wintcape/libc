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

/** @brief Computes current global number of unfreed allocations. */
#define GLOBAL_ALLOCATION_COUNT \
    ( memory_allocation_count () - memory_free_count () )

u8
test_hashtable_create_and_destroy
( void )
{
    u64 global_amount_allocated;
    u64 hashtable_amount_allocated;
    u64 global_allocation_count;

    u64 global_amount_allocated_;
    u64 hashtable_amount_allocated_;
    u64 global_allocation_count_;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    hashtable_amount_allocated = memory_amount_allocated ( MEMORY_TAG_HASHTABLE );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

    const u64 stride = 8;
    const u32 capacity = 64;
    hashtable_t hashtable;

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: hashtable_create handles invalid arguments.

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 1.1: hashtable_create fails if no output buffer for hashtable is provided.
    EXPECT_NOT ( hashtable_create ( true , stride , capacity , 0 , 0 ) );

    // TEST 1.2: hashtable_create fails for a data-valued hashtable if provided stride is invalid.
    EXPECT_NOT ( hashtable_create ( false , 0 , capacity , 0 , &hashtable ) );

    // TEST 1.3: hashtable_create fails if provided capacity is invalid.
    EXPECT_NOT ( hashtable_create ( true , 0 , 0 , 0 , &hashtable ) );

    // TEST 2: Pointer-valued, auto-allocated hashtable.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    hashtable_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_HASHTABLE );
    global_allocation_count_ = GLOBAL_ALLOCATION_COUNT;

    // TEST 2.1: hashtable_create succeeds.
    EXPECT ( hashtable_create ( true , stride , capacity , 0 , &hashtable ) );

    // TEST 2.2: hashtable_create performed one memory allocation.
    EXPECT_EQ ( global_allocation_count_ + 1 , GLOBAL_ALLOCATION_COUNT );

    // TEST 2.3: hashtable_create allocated the correct number of bytes with the correct memory tag.
    EXPECT_EQ ( global_amount_allocated_ + capacity * stride , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ + capacity * stride , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );

    // TEST 2.4: Hashtable created via hashtable_create owns its own memory.
    EXPECT ( hashtable.owns_memory );

    // TEST 2.5: Hashtable created via hashtable_create has access to a valid memory buffer.
    EXPECT_NEQ ( 0 , hashtable.memory );

    // TEST 2.6: Hashtable created via hashtable_create has correct value of boolean 'pointer' field.
    EXPECT ( hashtable.pointer );

    // TEST 2.7: Hashtable created via hashtable_create has stride equal to the size of a pointer (ignores the provided value of the 'stride' argument).
    EXPECT_EQ ( sizeof ( void* ) , hashtable.stride );

    // TEST 2.8: Hashtable created via hashtable_create has correct capacity.
    EXPECT_EQ ( capacity , hashtable.capacity );

    hashtable_destroy ( &hashtable );

    // TEST 2.9: hashtable_destroy restores the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count_ , GLOBAL_ALLOCATION_COUNT );
 
    // TEST 2.10: hashtable_destroy clears all hashtable data structure fields.
    EXPECT_EQ ( 0 , hashtable.memory );
    EXPECT_EQ ( 0 , hashtable.owns_memory );
    EXPECT_EQ ( 0 , hashtable.pointer );
    EXPECT_EQ ( 0 , hashtable.stride );
    EXPECT_EQ ( 0 , hashtable.capacity );

    // TEST 3: Data-valued, auto-allocated hashtable.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    hashtable_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_HASHTABLE );
    global_allocation_count_ = GLOBAL_ALLOCATION_COUNT;

    // TEST 3.1: hashtable_create succeeds with valid arguments.
    EXPECT ( hashtable_create ( false , stride , capacity , 0 , &hashtable ) );

    // TEST 3.2: hashtable_create performed one memory allocation.
    EXPECT_EQ ( global_allocation_count_ + 1 , GLOBAL_ALLOCATION_COUNT );

    // TEST 3.3: hashtable_create allocated the correct number of bytes with the correct memory tag.
    EXPECT_EQ ( global_amount_allocated_ + capacity * stride , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ + capacity * stride , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );

    // TEST 3.4: Hashtable created via hashtable_create owns its own memory.
    EXPECT ( hashtable.owns_memory );

    // TEST 3.5: Hashtable created via hashtable_create has access to a valid memory buffer.
    EXPECT_NEQ ( 0 , hashtable.memory );

    // TEST 3.6: Hashtable created via hashtable_create data structure has correct value of boolean 'pointer' field.
    EXPECT_NOT ( hashtable.pointer );

    // TEST 3.7: Hashtable created via hashtable_create data structure has correct stride.
    EXPECT_EQ ( stride , hashtable.stride );

    // TEST 3.8: Hashtable created via hashtable_create data structure has correct capacity.
    EXPECT_EQ ( capacity , hashtable.capacity );

    hashtable_destroy ( &hashtable );

    // TEST 3.9: hashtable_destroy restores the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count_ , GLOBAL_ALLOCATION_COUNT );

    // TEST 3.10: hashtable_destroy clears all hashtable data structure fields.
    EXPECT_EQ ( 0 , hashtable.memory );
    EXPECT_EQ ( 0 , hashtable.owns_memory );
    EXPECT_EQ ( 0 , hashtable.pointer );
    EXPECT_EQ ( 0 , hashtable.stride );
    EXPECT_EQ ( 0 , hashtable.capacity );

    // TEST 4: Pointer-valued, pre-allocated hashtable.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    hashtable_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_HASHTABLE );
    global_allocation_count_ = GLOBAL_ALLOCATION_COUNT;

    void* memory = memory_allocate ( capacity * stride , MEMORY_TAG_HASHTABLE );

    // Verify the memory allocation was successful prior to the test.
    EXPECT_NEQ ( 0 , memory );
    EXPECT_EQ ( global_allocation_count_ + 1 , GLOBAL_ALLOCATION_COUNT );
    EXPECT_EQ ( global_amount_allocated_ + capacity * stride , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ + capacity * stride , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    hashtable_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_HASHTABLE );
    global_allocation_count_ = GLOBAL_ALLOCATION_COUNT;

    // TEST 4.1: hashtable_create succeeds.
    EXPECT ( hashtable_create ( true , stride , capacity , memory , &hashtable ) );

    // TEST 4.2: hashtable_create does not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count_ , GLOBAL_ALLOCATION_COUNT );

    // TEST 4.3: Hashtable created via hashtable_create does not own its own memory.
    EXPECT_NOT ( hashtable.owns_memory );

    // TEST 4.4: Hashtable created via hashtable_create has access to the provided memory buffer.
    EXPECT_EQ ( memory , hashtable.memory );

    // TEST 4.5: Hashtable created via hashtable_create has correct value of boolean 'pointer' field.
    EXPECT ( hashtable.pointer );

    // TEST 4.6: Hashtable created via hashtable_create has stride equal to the size of a pointer (ignores the provided value of the 'stride' argument).
    EXPECT_EQ ( sizeof ( void* ) , hashtable.stride );

    // TEST 4.7: Hashtable created via hashtable_create has correct capacity.
    EXPECT_EQ ( capacity , hashtable.capacity );

    hashtable_destroy ( &hashtable );

    // TEST 4.8: hashtable_destroy does not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count_ , GLOBAL_ALLOCATION_COUNT );

    // TEST 4.9: hashtable_destroy clears all hashtable data structure fields.
    EXPECT_EQ ( 0 , hashtable.memory );
    EXPECT_EQ ( 0 , hashtable.owns_memory );
    EXPECT_EQ ( 0 , hashtable.pointer );
    EXPECT_EQ ( 0 , hashtable.stride );
    EXPECT_EQ ( 0 , hashtable.capacity );

    // TEST 5: Data-valued, pre-allocated hashtable.

    // TEST 5.1: hashtable_create succeeds.
    EXPECT ( hashtable_create ( false , stride , capacity , memory , &hashtable ) );

    // TEST 5.2: hashtable_create does not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count_ , GLOBAL_ALLOCATION_COUNT );

    // TEST 5.3: Hashtable created via hashtable_create does not own its own memory.
    EXPECT_NOT ( hashtable.owns_memory );

    // TEST 5.4: Hashtable created via hashtable_create has access to the provided memory buffer.
    EXPECT_EQ ( memory , hashtable.memory );

    // TEST 5.5: Hashtable created via hashtable_create has correct value of boolean 'pointer' field.
    EXPECT_NOT ( hashtable.pointer );

    // TEST 5.6: Hashtable created via hashtable_create has correct stride.
    EXPECT_EQ ( stride , hashtable.stride );

    // TEST 5.7: Hashtable created via hashtable_create has correct capacity.
    EXPECT_EQ ( capacity , hashtable.capacity );

    hashtable_destroy ( &hashtable );

    // TEST 5.8: hashtable_destroy does not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count_ , GLOBAL_ALLOCATION_COUNT );

    // TEST 5.9: hashtable_destroy clears all hashtable data structure fields.
    EXPECT_EQ ( 0 , hashtable.memory );
    EXPECT_EQ ( 0 , hashtable.owns_memory );
    EXPECT_EQ ( 0 , hashtable.pointer );
    EXPECT_EQ ( 0 , hashtable.stride );
    EXPECT_EQ ( 0 , hashtable.capacity );

    // TEST 6: hashtable_destroy handles invalid argument.

    // TEST 6.1: hashtable_destroy does not modify the global allocator state if no hashtable is provided.
    hashtable_destroy ( 0 );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count_ , GLOBAL_ALLOCATION_COUNT );

    // TEST 6.2: hashtable_destroy does not modify the global allocator state if the provided hashtable is uninitializes.
    hashtable_destroy ( &hashtable );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count_ , GLOBAL_ALLOCATION_COUNT );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    memory_free ( memory , capacity * stride , MEMORY_TAG_HASHTABLE );
    
    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

    return true;
}

u8
test_hashtable_set_and_get_data
( void )
{
    u64 global_amount_allocated;
    u64 hashtable_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    hashtable_amount_allocated = memory_amount_allocated ( MEMORY_TAG_HASHTABLE );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

    const u64 element_size = sizeof ( u64 );
    const u64 element_count = 3;
    const u64 values[ 3 ] = { random () , random () , random () };
    u64 memory[ 3 ];
    u64 get;
    hashtable_t hashtable;

    EXPECT ( hashtable_create ( false , element_size , element_count , memory , &hashtable ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , hashtable.memory );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: hashtable_set (on a data-valued hashtable) handles invalid arguments (1).

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 1.1: hashtable_set logs an error and fails if no hashtable is provided.
    EXPECT_NOT ( hashtable_set ( 0 , "key" , &values[ 0 ] ) );

    // TEST 1.2: hashtable_set logs an error and fails on a data-valued hashtable if no key is provided.
    EXPECT_NOT ( hashtable_set ( &hashtable , 0 , &values[ 0 ] ) );

    // TEST 1.3: hashtable_set logs an error and fails on a data-valued hashtable if no handle to a value is provided.
    EXPECT_NOT ( hashtable_set ( &hashtable , "key0" , 0 ) );

    // TEST 2: hashtable_set (on a data-valued hashtable).

    // TEST 2.1: hashtable_set succeeds.
    EXPECT ( hashtable_set ( &hashtable , "key0" , &values[ 0 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key1" , &values[ 1 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key2" , &values[ 2 ] ) );

    // TEST 3: hashtable_get (on a data-valued hashtable) handles invalid arguments (1).

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 3.1: hashtable_get logs an error and fails if no hashtable is provided.
    EXPECT_NOT ( hashtable_get ( 0 , "key" , &get ) );

    // TEST 3.2: hashtable_get logs an error and fails on a data-valued hashtable if no key is provided.
    EXPECT_NOT ( hashtable_get ( &hashtable , 0 , &get ) );

    // TEST 3.3: hashtable_get logs an error and fails on a data-valued hashtable if no output buffer is provided.
    EXPECT_NOT ( hashtable_get ( &hashtable , "key0" , 0 ) );

    // TEST 4: hashtable_get (on a data-valued hashtable).

    // TEST 4.1: hashtable_get succeeds on a data-valued hashtable given a valid key.
    EXPECT ( hashtable_get ( &hashtable , "key0" , &get ) );

    // TEST 4.2: hashtable_get writes the correct data into the provided output buffer.
    EXPECT_EQ ( values[ 0 ] , get );

    // TEST 4.3: hashtable_get succeeds on a data-valued hashtable given a valid key.
    EXPECT ( hashtable_get ( &hashtable , "key1" , &get ) );

    // TEST 4.4: hashtable_get writes the correct data into the provided output buffer.
    EXPECT_EQ ( values[ 1 ] , get );

    // TEST 4.5: hashtable_get succeeds on a data-valued hashtable given a valid key.
    EXPECT ( hashtable_get ( &hashtable , "key2" , &get ) );

    // TEST 4.6: hashtable_get writes the correct data into the provided output buffer.
    EXPECT_EQ ( values[ 2 ] , get );

    hashtable_destroy ( &hashtable );

    // TEST 5: hashtable_set (on a data-valued hashtable) handles invalid arguments (2).

    // TEST 5.1: hashtable_set logs an error and fails if the provided data-valued hashtable is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( hashtable_set ( &hashtable , "key" , &values[ 0 ] ) );

    // TEST 6: hashtable_get (on a data-valued hashtable) handles invalid arguments (2).
    
    // TEST 6.1: hashtable_get logs an error and fails if the provided data-valued hashtable is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( hashtable_get ( &hashtable , "key" , &get ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////
    
    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

    return true;
}

u8
test_hashtable_set_and_get_pointer
( void )
{
    u64 global_amount_allocated;
    u64 hashtable_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    hashtable_amount_allocated = memory_amount_allocated ( MEMORY_TAG_HASHTABLE );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

    const u64 element_count = 3;
    u64 values[ 3 ] = { random () , random () , random () };
    u64* memory[ 3 ];
    u64* get;
    hashtable_t hashtable;

    EXPECT ( hashtable_create ( true , 0 , element_count , memory , &hashtable ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , hashtable.memory );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: hashtable_set (on a pointer-valued hashtable) handles invalid arguments (1).

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 1.1: hashtable_set logs an error and fails if no hashtable is provided.
    EXPECT_NOT ( hashtable_set ( 0 , "key" , &values[ 0 ] ) );

    // TEST 1.2: hashtable_set logs an error and fails on a pointer-valued hashtable if no key is provided.
    EXPECT_NOT ( hashtable_set ( &hashtable , 0 , &values[ 0 ] ) );

    // TEST 2: hashtable_set (on a pointer-valued hashtable).

    // TEST 2.1: hashtable_set succeeds on a pointer-valued hashtable given a valid key.
    EXPECT ( hashtable_set ( &hashtable , "key0" , &values[ 0 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key1" , &values[ 1 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key2" , &values[ 2 ] ) );

    // TEST 3: hashtable_get (on a pointer-valued hashtable) handles invalid arguments (1).

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 3.1: hashtable_get logs an error and fails if no hashtable is provided.
    EXPECT_NOT ( hashtable_get ( 0 , "key" , &get ) );

    // TEST 3.2: hashtable_get logs an error and fails on a pointer-valued hashtable if no key is provided.
    EXPECT_NOT ( hashtable_get ( &hashtable , 0 , &get ) );

    // TEST 3.3: hashtable_get logs an error and fails on a pointer-valued hashtable if no output buffer is provided.
    EXPECT_NOT ( hashtable_get ( &hashtable , "key0" , 0 ) );

    // TEST 4: hashtable_get (on a pointer-valued hashtable).

    // TEST 4.1: hashtable_get succeeds on a pointer-valued hashtable given a valid key.
    EXPECT ( hashtable_get ( &hashtable , "key0" , &get ) );

    // TEST 4.2: hashtable_get writes the correct pointer into the output buffer.
    EXPECT_EQ ( &values[ 0 ] , get );
    EXPECT_EQ ( values[ 0 ] , *get );

    // TEST 4.3: hashtable_get succeeds on a pointer-valued hashtable given a valid key.
    EXPECT ( hashtable_get ( &hashtable , "key1" , &get ) );

    // TEST 4.4: hashtable_get writes the correct pointer into the output buffer.
    EXPECT_EQ ( &values[ 1 ] , get );
    EXPECT_EQ ( values[ 1 ] , *get );

    // TEST 4.5: hashtable_get succeeds on a pointer-valued hashtable given a valid key.
    EXPECT ( hashtable_get ( &hashtable , "key2" , &get ) );

    // TEST 4.6: hashtable_get writes the correct pointer into the output buffer.
    EXPECT_EQ ( &values[ 2 ] , get );
    EXPECT_EQ ( values[ 2 ] , *get );

    // Update the values at the addresses the hashtable pointers reference, then run the same tests again.
    values[ 0 ] = values[ 0 ] + 1;
    values[ 1 ] = values[ 1 ] + 1;
    values[ 2 ] = values[ 2 ] + 1;

    // TEST 4.7: hashtable_get succeeds on a pointer-valued hashtable given a valid key.
    EXPECT ( hashtable_get ( &hashtable , "key0" , &get ) );

    // TEST 4.8: hashtable_get writes the correct pointer into the output buffer.
    EXPECT_EQ ( &values[ 0 ] , get );
    EXPECT_EQ ( values[ 0 ] , *get );

    // TEST 4.9: hashtable_get succeeds on a pointer-valued hashtable given a valid key.
    EXPECT ( hashtable_get ( &hashtable , "key1" , &get ) );

    // TEST 4.10: hashtable_get writes the correct pointer into the output buffer.
    EXPECT_EQ ( &values[ 1 ] , get );
    EXPECT_EQ ( values[ 1 ] , *get );

    // TEST 4.11: hashtable_get succeeds on a pointer-valued hashtable given a valid key.
    EXPECT ( hashtable_get ( &hashtable , "key2" , &get ) );

    // TEST 4.12: hashtable_get writes the correct pointer into the output buffer.
    EXPECT_EQ ( &values[ 2 ] , get );
    EXPECT_EQ ( values[ 2 ] , *get );

    hashtable_destroy ( &hashtable );

    // TEST 5: hashtable_set (on a pointer-valued hashtable) handles invalid arguments (2).

    // TEST 5.1: hashtable_set logs an error and fails if the provided pointer-valued hashtable is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( hashtable_set ( &hashtable , "key" , &values[ 0 ] ) );

    // TEST 6: hashtable_get (on a pointer-valued hashtable) handles invalid arguments (2).
    
    // TEST 6.1: hashtable_get logs an error and fails if the provided pointer-valued hashtable is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( hashtable_get ( &hashtable , "key" , &get ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////
    
    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

    return true;
}

u8
test_hashtable_get_nonexistent
( void )
{
    u64 global_amount_allocated;
    u64 hashtable_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    hashtable_amount_allocated = memory_amount_allocated ( MEMORY_TAG_HASHTABLE );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

    const u64 element_size = sizeof ( u64 );
    const u64 element_count = 3;
    const u64 values[ 3 ] = { random () , random () , random () };
    u64 memory[ 3 ];
    u64 get;
    hashtable_t hashtable;

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // Set a key-value pair within a hashtable.
    EXPECT ( hashtable_create ( false , element_size , element_count , memory , &hashtable ) );
    EXPECT_NEQ ( 0 , hashtable.memory ); // Verify there was no memory error prior to the test.
    EXPECT ( hashtable_set ( &hashtable , "key0" , &values[ 0 ] ) );

    // TEST 1: hashtable_get succeeds even when the provided key cannot be found within the hashtable.
    get = 0;
    EXPECT ( hashtable_get ( &hashtable , "key1" , &get ) );

    // TEST 2: hashtable_get does not modify the output buffer if the provided key could not be found.
    EXPECT_EQ ( 0 , get );

    // TEST 3: hashtable_get succeeds even when the provided key cannot be found within the hashtable.
    get = 0;
    EXPECT ( hashtable_get ( &hashtable , "key2" , &get ) );

    // TEST 4: hashtable_get does not modify the output buffer if the provided key could not be found.
    EXPECT_EQ ( 0 , get );

    // End test.
    ////////////////////////////////////////////////////////////////////////////
    
    hashtable_destroy ( &hashtable );
    
    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

    return true;
}

u8
test_hashtable_remove_pointer
( void )
{
    u64 global_amount_allocated;
    u64 hashtable_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    hashtable_amount_allocated = memory_amount_allocated ( MEMORY_TAG_HASHTABLE );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

    const u64 element_count = 3;
    u64 values[ 3 ] = { random () , random () , random () };
    u64* memory[ 3 ];
    u64* get;
    hashtable_t hashtable;

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // Populate a hashtable with key-value pairs.
    EXPECT ( hashtable_create ( true , 0 , element_count , memory , &hashtable ) );
    EXPECT_NEQ ( 0 , hashtable.memory ); // Verify there was no memory error prior to the test.
    EXPECT ( hashtable_set ( &hashtable , "key0" , &values[ 0 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key1" , &values[ 1 ] ) );
    EXPECT ( hashtable_set ( &hashtable , "key2" , &values[ 2 ] ) );

    // Verify all the key-value pairs prior to the test.
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

    // TEST 1: hashtable_set succeeds on a pointer-valued hashtable even when no value handle is provided.
    EXPECT ( hashtable_set ( &hashtable , "key0" , 0 ) );
    EXPECT ( hashtable_set ( &hashtable , "key1" , 0 ) );
    EXPECT ( hashtable_set ( &hashtable , "key2" , 0 ) );

    // TEST 2: The pointer values set for a key which was referenced by a hashtable_set call on a pointer-valued hashtable without a value handle should be null.
    get = ( ( void* ) -1 );
    EXPECT ( hashtable_get ( &hashtable , "key0" , &get ) );
    EXPECT_EQ ( 0 , get );

    // TEST 3: The pointer values set for a key which was referenced by a hashtable_set call on a pointer-valued hashtable without a value handle should be null.
    get = ( ( void* ) -1 );
    EXPECT ( hashtable_get ( &hashtable , "key1" , &get ) );
    EXPECT_EQ ( 0 , get );

    // TEST 4: The pointer values set for a key which was referenced by a hashtable_set call on a pointer-valued hashtable without a value handle should be null.
    get = ( ( void* ) -1 );
    EXPECT ( hashtable_get ( &hashtable , "key2" , &get ) );
    EXPECT_EQ ( 0 , get );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    hashtable_destroy ( &hashtable );
    
    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

    return true;
}

void
test_register_hashtable
( void )
{
    test_register ( test_hashtable_create_and_destroy , "Creating or destroying a hashtable." );
    test_register ( test_hashtable_set_and_get_data , "Testing 'set' and 'get' operations on a data-valued hashtable." );
    test_register ( test_hashtable_set_and_get_pointer , "Testing 'set' and 'get' operations on a pointer-valued hashtable." );
    test_register ( test_hashtable_get_nonexistent , "Testing hashtable 'get' operation with an argument that cannot be found within the hashtable." );
    test_register ( test_hashtable_remove_pointer , "Testing the ability to remove a pointer value within a pointer-valued hashtable." );
}