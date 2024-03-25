/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file memory/test_linear_allocator.c
 * @brief Implementation of the memory/test_linear_allocator header.
 * (see memory/test_linear_allocator.h for additional details)
 */
#include "memory/test_linear_allocator.h"

#include "test/expect.h"

#include "core/memory.h"

u8
test_linear_allocator_create_and_destroy
( void )
{
    u64 global_amount_allocated;
    u64 allocator_amount_allocated;
    u64 global_allocation_count;

    u64 global_amount_allocated_;
    u64 allocator_amount_allocated_;
    u64 global_allocation_count_;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated = memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 capacity = sizeof ( u64 ) * 8;
    linear_allocator_t allocator;

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: linear_allocator_create handles invalid arguments.

    // TEST 1.1: linear_allocator_create fails if no output buffer is provided for the linear allocator.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( linear_allocator_create ( capacity , 0 , 0 ) );

    // TEST 2: Auto-allocated linear allocator.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // TEST 2.1: linear_allocator_create succeeds.
    EXPECT ( linear_allocator_create ( capacity , 0 , &allocator ) );

    // TEST 2.2: linear_allocator_create performed one memory allocation.
    EXPECT_EQ ( global_allocation_count_ + 1 , MEMORY_ALLOCATION_COUNT );

    // TEST 2.3: linear_allocator_create allocated the correct number of bytes with the correct memory tag.
    EXPECT_EQ ( global_amount_allocated_ + allocator.capacity , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated_ + allocator.capacity , memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR ) );

    // TEST 2.4: Linear allocator created via linear_allocator_create owns its own memory.
    EXPECT ( allocator.owns_memory );

    // TEST 2.5: Linear allocator created via linear_allocator_create has access to a valid memory buffer.
    EXPECT_NEQ ( 0 , allocator.memory );

    // TEST 2.6: Linear allocator created via linear_allocator_create has the correct capacity.
    EXPECT_EQ ( capacity , allocator.capacity );

    // TEST 2.7: Linear allocator created via linear_allocator_create is initialized with all of its memory free.
    EXPECT_EQ ( 0 , allocator.allocated );

    linear_allocator_destroy ( &allocator );

    // TEST 2.8: linear_allocator_destroy restores the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 2.9: linear_allocator_destroy clears all linear allocator data structure fields.
    EXPECT_EQ ( 0 , allocator.allocated );
    EXPECT_EQ ( 0 , allocator.capacity );
    EXPECT_EQ ( 0 , allocator.memory );
    EXPECT_EQ ( 0 , allocator.owns_memory );

    // TEST 3: Pre-allocated linear allocator.

    // Copy the current global allocator state prior to the allocation.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    void* memory = memory_allocate ( capacity , MEMORY_TAG_LINEAR_ALLOCATOR );

    // Verify the memory allocation was successful prior to the test.
    EXPECT_NEQ ( 0 , memory );
    EXPECT_EQ ( global_allocation_count_ + 1 , MEMORY_ALLOCATION_COUNT );
    EXPECT_EQ ( allocator_amount_allocated_ + capacity , memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR ) );

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // TEST 3.1: linear_allocator_create succeeds.
    EXPECT ( linear_allocator_create ( capacity , memory , &allocator ) );

    // TEST 3.2: linear_allocator_create does not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 3.3: Linear allocator created via linear_allocator_create does not own its own memory.
    EXPECT_NOT ( allocator.owns_memory );

    // TEST 3.4: Linear allocator created via linear_allocator_create has access to the provided memory buffer.
    EXPECT_EQ ( memory , allocator.memory );

    // TEST 3.5: Linear allocator created via linear_allocator_create has the correct capacity.
    EXPECT_EQ ( capacity , allocator.capacity );

    // TEST 3.6: Linear allocator created via linear_allocator_create is initialized with all of its memory free.
    EXPECT_EQ ( 0 , allocator.allocated );

    linear_allocator_destroy ( &allocator );

    // TEST 3.7: linear_allocator_destroy does not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 3.8: linear_allocator_destroy clears all linear allocator data structure fields.
    EXPECT_EQ ( 0 , allocator.allocated );
    EXPECT_EQ ( 0 , allocator.capacity );
    EXPECT_EQ ( 0 , allocator.memory );
    EXPECT_EQ ( 0 , allocator.owns_memory );

    // TEST 4: linear_allocator_destroy handles invalid arguments.

    // TEST 4.1: linear_allocator_destroy does not modify the global allocator state if no linear allocator is provided.
    linear_allocator_destroy ( 0 );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 4.2: linear_allocator_destroy does not modify the global allocator state if the provided linear allocator is uninitialized.
    linear_allocator_destroy ( &allocator );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    memory_free ( memory , capacity , MEMORY_TAG_LINEAR_ALLOCATOR );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_linear_allocator_allocate
( void )
{
    u64 global_amount_allocated;
    u64 allocator_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated = memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 capacity = sizeof ( u64 ) * 8;
    linear_allocator_t allocator;
    void* blk;

    EXPECT ( linear_allocator_create ( capacity , 0 , &allocator ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator.memory );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: linear_allocator_allocate handles invalid arguments (1).

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 1.1: linear_allocator_allocate logs an error and returns 0 if no allocator is provided.
    blk = linear_allocator_allocate ( 0 , 1 );
    EXPECT_EQ ( 0 , blk );

    // TEST 1.2: linear_allocator_allocate logs an error and does not perform memory allocation if the provided size is invalid.
    blk = linear_allocator_allocate ( &allocator , 0 );
    EXPECT_EQ ( 0 , blk );
    EXPECT_EQ ( 0 , allocator.allocated );

    // TEST 2: linear_allocator_allocate succeeds with valid arguments.

    blk = linear_allocator_allocate ( &allocator , capacity );

    // TEST 2.1: linear_allocator_allocate returns a handle to an allocated block of memory. 
    EXPECT_NEQ ( 0 , blk );

    // TEST 2.2: The allocator has allocated the correct amount of its total memory. 
    EXPECT_EQ ( capacity , allocator.allocated );

    // TEST 3: linear_allocator_allocate handles invalid arguments (2).

    linear_allocator_destroy ( &allocator );

    // TEST 3.1: linear_allocator_allocate logs an error and does not perform memory allocation if the provided allocator is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    blk = linear_allocator_allocate ( &allocator , capacity );
    EXPECT_EQ ( 0 , blk );
    EXPECT_EQ ( 0 , allocator.allocated );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_linear_allocator_max_allocation_count
( void )
{
    u64 global_amount_allocated;
    u64 allocator_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated = memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 op_count = 1024;
    linear_allocator_t allocator;
    void* blk;

    EXPECT ( linear_allocator_create ( sizeof ( u64 ) * op_count , 0 , &allocator ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator.memory );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    for ( u64 i = 0; i < op_count; ++i )
    {
        blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );

        // TEST 1: linear_allocator_allocate returns a handle to an allocated block of memory.
        EXPECT_NEQ ( 0 , blk );

        // TEST 2: The allocator has allocated the correct amount of its total memory. 
        EXPECT_EQ ( sizeof ( u64 ) * ( i + 1 ) , allocator.allocated );
    }

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    linear_allocator_destroy ( &allocator );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_linear_allocator_max_allocation_size
( void )
{
    u64 global_amount_allocated;
    u64 allocator_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated = memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    linear_allocator_t allocator;
    void* blk;

    EXPECT ( linear_allocator_create ( sizeof ( u64 ) , 0 , &allocator ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator.memory );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );

    // TEST 1: linear_allocator_allocate returns a handle to an allocated block of memory.
    EXPECT_NEQ ( 0 , blk );

    // TEST 2: The allocator has allocated the correct amount of its total memory (all of it).
    EXPECT_EQ ( sizeof ( u64 ) , allocator.allocated );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    linear_allocator_destroy ( &allocator );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_linear_allocator_overflow
( void )
{
    u64 global_amount_allocated;
    u64 allocator_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated = memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 op_count = 1024;
    linear_allocator_t allocator;
    void* blk;

    EXPECT ( linear_allocator_create ( sizeof ( u64 ) * op_count , 0 , &allocator ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator.memory );

    // Fill the allocator prior to the test.
    for ( u64 i = 0; i < op_count; ++i )
    {
        blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );
        EXPECT_NEQ ( 0 , blk );
        EXPECT_EQ ( sizeof ( u64 ) * ( i + 1 ) , allocator.allocated );
    }

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: linear_allocator_allocate logs an error and does not perform memory allocation if the provided allocator is full.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );
    EXPECT_EQ ( 0 , blk );
    EXPECT_EQ ( sizeof ( u64 ) * op_count , allocator.allocated );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    linear_allocator_destroy ( &allocator );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_linear_allocator_free
( void )
{
    u64 global_amount_allocated;
    u64 allocator_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated = memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 max_allocations = 1024;
    linear_allocator_t allocator;
    void* blk;

    EXPECT ( linear_allocator_create ( sizeof ( u64 ) * max_allocations , 0 , &allocator ) );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator.memory );

    // Fill the allocator first prior to the test.
    for ( u64 i = 0; i < max_allocations; ++i )
    {
        blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );
        EXPECT_NEQ ( 0 , blk );
        EXPECT_EQ ( sizeof ( u64 ) * ( i + 1 ) , allocator.allocated );
    }

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: linear_allocator_free succeeds with valid argument.

    // TEST 1.1: linear_allocator_free succeeds.
    EXPECT ( linear_allocator_free ( &allocator ) );

    // TEST 1.2: linear_allocator_free frees all memory currently allocated by the allocator.
    EXPECT_EQ ( 0 , allocator.allocated );

    // TEST 2: linear_allocator_free handles invalid arguments.
    
    linear_allocator_destroy ( &allocator );

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 2.1: linear_allocator_free fails if no allocator is provided.
    EXPECT_NOT ( linear_allocator_free ( 0 ) );

    // TEST 2.2: linear_allocator_free fails if the provided allocator is uninitialized.
    EXPECT_NOT ( linear_allocator_free ( &allocator ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

void
test_register_linear_allocator
( void )
{
    test_register ( test_linear_allocator_create_and_destroy , "Creating or destroying a linear allocator." );
    test_register ( test_linear_allocator_allocate , "Allocating memory managed by a linear allocator." );
    test_register ( test_linear_allocator_max_allocation_count , "Testing linear allocator max allocation count." );
    test_register ( test_linear_allocator_max_allocation_size , "Testing linear allocator max allocation size." );
    test_register ( test_linear_allocator_overflow , "Testing linear allocator overflow handling." );
    test_register ( test_linear_allocator_free , "Freeing memory managed by a linear allocator." );
}