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
    u64 global_amount_allocated;
    u64 hashtable_amount_allocated;
    u64 global_allocation_count;

    u64 global_amount_allocated_;
    u64 hashtable_amount_allocated_;
    u64 global_allocation_count_;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    hashtable_amount_allocated = memory_amount_allocated ( MEMORY_TAG_HASHTABLE );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 stride = 8;
    const u32 capacity = 64;

    hashtable_t* hashtable;
    u64 memory_requirement;
    void* memory;

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: hashtable_create handles invalid arguments.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    hashtable_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_HASHTABLE );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 1.1: hashtable_create fails if no output buffer for hashtable is provided.
    EXPECT_NOT ( hashtable_create ( true , stride , capacity , 0 , 0 , 0 ) );

    // TEST 1.2: hashtable_create fails for a data-valued hashtable if provided stride is invalid.
    EXPECT_NOT ( hashtable_create ( false , 0 , capacity , 0 , 0 , &hashtable ) );

    // TEST 1.3: hashtable_create fails if provided capacity is invalid.
    EXPECT_NOT ( hashtable_create ( true , 0 , 0 , 0 , 0 , &hashtable ) );

    // TEST 1.4: hashtable_create did not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 2: hashtable_create can be used to query the memory requirement for a hashtable.
    
    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    hashtable_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_HASHTABLE );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;
    
    // TEST 2.1: hashtable_create succeeds.
    memory_requirement = 0;
    EXPECT ( hashtable_create ( true , stride , capacity , &memory_requirement , 0 , 0 ) );

    // TEST 2.2: hashtable_create writes the memory requirement to the output buffer.
    EXPECT_NEQ ( 0 , memory_requirement );

    // TEST 2.3: hashtable_create did not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 3: Pointer-valued, auto-allocated hashtable.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    hashtable_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_HASHTABLE );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // Query the memory_requirement prior to the test.
    EXPECT ( hashtable_create ( true , stride , capacity , &memory_requirement , 0 , 0 ) );

    // TEST 3.1: hashtable_create succeeds.
    hashtable = 0;
    EXPECT ( hashtable_create ( true , stride , capacity , 0 , 0 , &hashtable ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , hashtable );

    // TEST 3.2: hashtable_create performed one memory allocation.
    EXPECT_EQ ( global_allocation_count_ + 1 , MEMORY_ALLOCATION_COUNT );

    // TEST 3.3: hashtable_create allocated the correct number of bytes with the correct memory tag.
    EXPECT_EQ ( global_amount_allocated_ + memory_requirement , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ + memory_requirement , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );

    // TEST 3.4: Hashtable created via hashtable_create owns its own memory.
    EXPECT ( hashtable_owns_memory ( hashtable ) );

    // TEST 3.5: Hashtable created via hashtable_create has correct value of boolean 'pointer' field.
    EXPECT ( hashtable_pointer ( hashtable ) );

    // TEST 3.6: Hashtable created via hashtable_create has stride equal to the size of a pointer (ignores the provided value of the 'stride' argument).
    EXPECT_EQ ( sizeof ( void* ) , hashtable_stride ( hashtable ) );

    // TEST 3.7: Hashtable created via hashtable_create has correct capacity.
    EXPECT_EQ ( capacity , hashtable_capacity ( hashtable ) );

    // TEST 3.8: hashtable_destroy nullifies the hashtable handle.
    hashtable_destroy ( &hashtable );
    EXPECT_EQ ( 0 , hashtable );

    // TEST 3.9: hashtable_destroy restores the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 4: Data-valued, auto-allocated hashtable.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    hashtable_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_HASHTABLE );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // Query the memory_requirement prior to the test.
    EXPECT ( hashtable_create ( true , stride , capacity , &memory_requirement , 0 , 0 ) );

    // TEST 4.1: hashtable_create succeeds.
    hashtable = 0;
    EXPECT ( hashtable_create ( false , stride , capacity , 0 , 0 , &hashtable ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , hashtable );

    // TEST 4.2: hashtable_create performed one memory allocation.
    EXPECT_EQ ( global_allocation_count_ + 1 , MEMORY_ALLOCATION_COUNT );

    // TEST 4.3: hashtable_create allocated the correct number of bytes with the correct memory tag.
    EXPECT_EQ ( global_amount_allocated_ + memory_requirement , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ + memory_requirement , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );

    // TEST 4.4: Hashtable created via hashtable_create owns its own memory.
    EXPECT ( hashtable_owns_memory ( hashtable )  );

    // TEST 4.5: Hashtable created via hashtable_create data structure has correct value of boolean 'pointer' field.
    EXPECT_NOT ( hashtable_pointer ( hashtable )  );

    // TEST 4.6: Hashtable created via hashtable_create data structure has correct stride.
    EXPECT_EQ ( stride , hashtable_stride ( hashtable )  );

    // TEST 4.7: Hashtable created via hashtable_create data structure has correct capacity.
    EXPECT_EQ ( capacity , hashtable_capacity ( hashtable ) );

    // TEST 4.8: hashtable_destroy nullifies the hashtable handle.
    hashtable_destroy ( &hashtable );
    EXPECT_EQ ( 0 , hashtable );

    // TEST 4.9: hashtable_destroy restores the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 5: Pointer-valued, pre-allocated hashtable.

    // Allocate the correct amount of memory prior to the test.
    EXPECT ( hashtable_create ( true , stride , capacity , &memory_requirement , 0 , 0 ) );
    memory = memory_allocate ( memory_requirement , MEMORY_TAG_HASHTABLE );
    
    // Verify no memory error occurred prior to the test.
    EXPECT_NEQ ( 0 , memory );

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    hashtable_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_HASHTABLE );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // TEST 5.1: hashtable_create succeeds.
    hashtable = 0;
    EXPECT ( hashtable_create ( true , stride , capacity , 0 , memory , &hashtable ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , hashtable );

    // TEST 5.2: hashtable_create does not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 5.3: Hashtable created via hashtable_create does not own its own memory.
    EXPECT_NOT ( hashtable_owns_memory ( hashtable ) );

    // TEST 5.4: Hashtable created via hashtable_create has correct value of boolean 'pointer' field.
    EXPECT ( hashtable_pointer ( hashtable ) );

    // TEST 5.5: Hashtable created via hashtable_create has stride equal to the size of a pointer (ignores the provided value of the 'stride' argument).
    EXPECT_EQ ( sizeof ( void* ) , hashtable_stride ( hashtable ) );

    // TEST 5.6: Hashtable created via hashtable_create has correct capacity.
    EXPECT_EQ ( capacity , hashtable_capacity ( hashtable ) );

    // TEST 5.7: hashtable_destroy nullifies the hashtable handle.
    hashtable_destroy ( &hashtable );
    EXPECT_EQ ( 0 , hashtable );

    // TEST 5.8: hashtable_destroy does not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    memory_free ( memory , memory_requirement , MEMORY_TAG_HASHTABLE );

    // TEST 6: Data-valued, pre-allocated hashtable.

    // Allocate the correct amount of memory prior to the test.
    EXPECT ( hashtable_create ( false , stride , capacity , &memory_requirement , 0 , 0 ) );
    memory = memory_allocate ( memory_requirement , MEMORY_TAG_HASHTABLE );
    
    // Verify no memory error occurred prior to the test.
    EXPECT_NEQ ( 0 , memory );

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    hashtable_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_HASHTABLE );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // TEST 6.1: hashtable_create succeeds.
    hashtable = 0;
    EXPECT ( hashtable_create ( false , stride , capacity , 0 , memory , &hashtable ) );

    // TEST 6.2: hashtable_create does not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 6.3: Hashtable created via hashtable_create does not own its own memory.
    EXPECT_NOT ( hashtable_owns_memory ( hashtable ) );

    // TEST 6.4: Hashtable created via hashtable_create has correct value of boolean 'pointer' field.
    EXPECT_NOT ( hashtable_pointer ( hashtable ) );

    // TEST 6.5: Hashtable created via hashtable_create has correct stride.
    EXPECT_EQ ( stride , hashtable_stride ( hashtable ) );

    // TEST 6.6: Hashtable created via hashtable_create has correct capacity.
    EXPECT_EQ ( capacity , hashtable_capacity ( hashtable ) );

    // TEST 6.7: hashtable_destroy nullifies the hashtable handle.
    hashtable_destroy ( &hashtable );
    EXPECT_EQ ( 0 , hashtable );

    // TEST 6.8: hashtable_destroy does not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    memory_free ( memory , memory_requirement , MEMORY_TAG_HASHTABLE );

    // TEST 7: hashtable_destroy handles invalid argument.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    hashtable_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_HASHTABLE );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // TEST 7.1: hashtable_destroy does not modify the global allocator state if null handle is provided.
    hashtable_destroy ( 0 );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 7.2: hashtable_destroy does not modify the global allocator state if provided hashtable is null.
    hashtable = 0;
    hashtable_destroy ( &hashtable );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // End test.
    ////////////////////////////////////////////////////////////////////////////
    
    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

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
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 element_size = sizeof ( u64 );
    const u64 element_count = 3;
    const u64 values[] = { random () , random () , random () };

    hashtable_t* hashtable = 0;
    u64 get;

    EXPECT ( hashtable_create ( false , element_size , element_count , 0 , 0 , &hashtable ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , hashtable );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: hashtable_set (on a data-valued hashtable).

    // TEST 1.1: hashtable_set logs an error and fails if no handle to a value is provided.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( hashtable_set ( hashtable , "key0" , 0 ) );

    // TEST 1.2: hashtable_set succeeds.
    EXPECT ( hashtable_set ( hashtable , "key0" , &values[ 0 ] ) );
    EXPECT ( hashtable_set ( hashtable , "key1" , &values[ 1 ] ) );
    EXPECT ( hashtable_set ( hashtable , "key2" , &values[ 2 ] ) );

    // TEST 2: hashtable_get (on a data-valued hashtable).

    // TEST 2.1: hashtable_get succeeds.
    EXPECT ( hashtable_get ( hashtable , "key0" , &get ) );

    // TEST 2.2: hashtable_get writes the correct data into the provided output buffer.
    EXPECT_EQ ( values[ 0 ] , get );

    // TEST 2.3: hashtable_get succeeds.
    EXPECT ( hashtable_get ( hashtable , "key1" , &get ) );

    // TEST 2.4: hashtable_get writes the correct data into the provided output buffer.
    EXPECT_EQ ( values[ 1 ] , get );

    // TEST 2.5: hashtable_get succeeds.
    EXPECT ( hashtable_get ( hashtable , "key2" , &get ) );

    // TEST 2.6: hashtable_get writes the correct data into the provided output buffer.
    EXPECT_EQ ( values[ 2 ] , get );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    hashtable_destroy ( &hashtable );
    
    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

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
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 element_count = 3;
    u64 values[ 3 ] = { random () , random () , random () };

    hashtable_t* hashtable = 0;
    u64* get;

    EXPECT ( hashtable_create ( true , 0 , element_count , 0 , 0 , &hashtable ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , hashtable );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: hashtable_set (on a pointer-valued hashtable).

    // TEST 1.1: hashtable_set succeeds.
    EXPECT ( hashtable_set ( hashtable , "key0" , &values[ 0 ] ) );
    EXPECT ( hashtable_set ( hashtable , "key1" , &values[ 1 ] ) );
    EXPECT ( hashtable_set ( hashtable , "key2" , &values[ 2 ] ) );

    // TEST 2: hashtable_get (on a pointer-valued hashtable).

    // TEST 2.1: hashtable_get succeeds.
    EXPECT ( hashtable_get ( hashtable , "key0" , &get ) );

    // TEST 2.2: hashtable_get writes the correct pointer into the output buffer.
    EXPECT_EQ ( &values[ 0 ] , get );
    EXPECT_EQ ( values[ 0 ] , *get );

    // TEST 2.3: hashtable_get succeeds.
    EXPECT ( hashtable_get ( hashtable , "key1" , &get ) );

    // TEST 2.4: hashtable_get writes the correct pointer into the output buffer.
    EXPECT_EQ ( &values[ 1 ] , get );
    EXPECT_EQ ( values[ 1 ] , *get );

    // TEST 2.5: hashtable_get succeeds.
    EXPECT ( hashtable_get ( hashtable , "key2" , &get ) );

    // TEST 2.6: hashtable_get writes the correct pointer into the output buffer.
    EXPECT_EQ ( &values[ 2 ] , get );
    EXPECT_EQ ( values[ 2 ] , *get );

    // Update the values at the addresses the hashtable pointers reference, then run the same tests again.
    values[ 0 ] = values[ 0 ] + 1;
    values[ 1 ] = values[ 1 ] + 1;
    values[ 2 ] = values[ 2 ] + 1;

    // TEST 2.7: hashtable_get succeeds.
    EXPECT ( hashtable_get ( hashtable , "key0" , &get ) );

    // TEST 2.8: hashtable_get writes the correct pointer into the output buffer.
    EXPECT_EQ ( &values[ 0 ] , get );
    EXPECT_EQ ( values[ 0 ] , *get );

    // TEST 2.9: hashtable_get succeeds.
    EXPECT ( hashtable_get ( hashtable , "key1" , &get ) );

    // TEST 2.10: hashtable_get writes the correct pointer into the output buffer.
    EXPECT_EQ ( &values[ 1 ] , get );
    EXPECT_EQ ( values[ 1 ] , *get );

    // TEST 2.11: hashtable_get succeeds.
    EXPECT ( hashtable_get ( hashtable , "key2" , &get ) );

    // TEST 2.12: hashtable_get writes the correct pointer into the output buffer.
    EXPECT_EQ ( &values[ 2 ] , get );
    EXPECT_EQ ( values[ 2 ] , *get );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    hashtable_destroy ( &hashtable );
    
    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

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
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 element_size = sizeof ( u64 );
    const u64 element_count = 3;
    const u64 values[ 3 ] = { random () , random () , random () };

    hashtable_t* hashtable = 0;
    u64 get;

    EXPECT ( hashtable_create ( false , element_size , element_count , 0 , 0 , &hashtable ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , hashtable );
    
    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // Set a key-value pair within a hashtable.
    EXPECT ( hashtable_set ( hashtable , "key0" , &values[ 0 ] ) );

    // TEST 1: hashtable_get succeeds even when the provided key cannot be found within the hashtable.
    get = 0;
    EXPECT ( hashtable_get ( hashtable , "key1" , &get ) );

    // TEST 2: hashtable_get does not modify the output buffer if the provided key could not be found.
    EXPECT_EQ ( 0 , get );

    // TEST 3: hashtable_get succeeds even when the provided key cannot be found within the hashtable.
    get = 0;
    EXPECT ( hashtable_get ( hashtable , "key2" , &get ) );

    // TEST 4: hashtable_get does not modify the output buffer if the provided key could not be found.
    EXPECT_EQ ( 0 , get );

    // End test.
    ////////////////////////////////////////////////////////////////////////////
    
    hashtable_destroy ( &hashtable );
    
    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

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
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 element_count = 3;
    u64 values[ 3 ] = { random () , random () , random () };

    hashtable_t* hashtable = 0;
    u64* get;

    EXPECT ( hashtable_create ( true , 0 , element_count , 0 , 0 , &hashtable ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , hashtable );

    // Populate a hashtable with key-value pairs.
    EXPECT ( hashtable_set ( hashtable , "key0" , &values[ 0 ] ) );
    EXPECT ( hashtable_set ( hashtable , "key1" , &values[ 1 ] ) );
    EXPECT ( hashtable_set ( hashtable , "key2" , &values[ 2 ] ) );

    // Verify all the key-value pairs prior to the test.
    get = 0;
    EXPECT ( hashtable_get ( hashtable , "key0" , &get ) );
    EXPECT_EQ ( &values[ 0 ] , get );
    EXPECT_EQ ( values[ 0 ] , *get );
    get = 0;
    EXPECT ( hashtable_get ( hashtable , "key1" , &get ) );
    EXPECT_EQ ( &values[ 1 ] , get );
    EXPECT_EQ ( values[ 1 ] , *get );
    get = 0;
    EXPECT ( hashtable_get ( hashtable , "key2" , &get ) );
    EXPECT_EQ ( &values[ 2 ] , get );
    EXPECT_EQ ( values[ 2 ] , *get );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: hashtable_set succeeds on a pointer-valued hashtable even when no value handle is provided.
    EXPECT ( hashtable_set ( hashtable , "key0" , 0 ) );
    EXPECT ( hashtable_set ( hashtable , "key1" , 0 ) );
    EXPECT ( hashtable_set ( hashtable , "key2" , 0 ) );

    // TEST 2: The pointer values set for a key which was referenced by a hashtable_set call on a pointer-valued hashtable without a value handle should be null.
    get = ( ( void* ) -1 );
    EXPECT ( hashtable_get ( hashtable , "key0" , &get ) );
    EXPECT_EQ ( 0 , get );

    // TEST 3: The pointer values set for a key which was referenced by a hashtable_set call on a pointer-valued hashtable without a value handle should be null.
    get = ( ( void* ) -1 );
    EXPECT ( hashtable_get ( hashtable , "key1" , &get ) );
    EXPECT_EQ ( 0 , get );

    // TEST 4: The pointer values set for a key which was referenced by a hashtable_set call on a pointer-valued hashtable without a value handle should be null.
    get = ( ( void* ) -1 );
    EXPECT ( hashtable_get ( hashtable , "key2" , &get ) );
    EXPECT_EQ ( 0 , get );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    hashtable_destroy ( &hashtable );
    
    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( hashtable_amount_allocated , memory_amount_allocated ( MEMORY_TAG_HASHTABLE ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

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