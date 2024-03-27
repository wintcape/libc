/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/test_queue.c
 * @brief Implementation of the container/test_queue header.
 * (see container/test_queue.h for additional details)
 */
#include "container/test_queue.h"

#include "test/expect.h"

#include "core/memory.h"

#include "math/math.h"

u8
test_queue_create_and_destroy
( void )
{
    u64 global_amount_allocated;
    u64 queue_amount_allocated;
    u64 global_allocation_count;

    u64 global_amount_allocated_;
    u64 queue_amount_allocated_;
    u64 global_allocation_count_;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    queue_amount_allocated = memory_amount_allocated ( MEMORY_TAG_QUEUE );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    queue_t* queue;

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: queue_create.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    queue_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_QUEUE );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    queue = queue_create ( i16 );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , queue );

    // TEST 1.1: queue_create performed one memory allocation.
    EXPECT_EQ ( global_allocation_count_ + 1 , MEMORY_ALLOCATION_COUNT );

    // TEST 1.2: queue_create allocated the correct number of bytes with the correct memory tag.
    EXPECT_EQ ( global_amount_allocated_ + queue_size ( queue ) , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( queue_amount_allocated_ + queue_size ( queue ) , memory_amount_allocated ( MEMORY_TAG_QUEUE ) );

    // TEST 1.3: Queue created via queue_create has correct stride.
    EXPECT_EQ ( sizeof ( i16 ) , queue_stride ( queue ) );

    // TEST 1.4: Queue created via queue_create has default capacity.
    EXPECT_EQ ( QUEUE_DEFAULT_CAPACITY * queue_stride ( queue ) , queue_allocated ( queue ) );

    // TEST 1.5: Queue created via queue_create has 0 length.
    EXPECT_EQ ( 0 , queue_length ( queue ) );

    // TEST 2: queue_destroy.

    // TEST 2.1: queue_destroy restores the global allocator state.
    queue_destroy ( queue );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( queue_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_QUEUE ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 3: queue_create handles invalid arguments.

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    queue_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_QUEUE );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // TEST 3.1: queue_create logs an error and fails if provided capacity is invalid.
    EXPECT_EQ ( 0 , _queue_create ( 0 , 1 ) );

    // TEST 3.2: queue_create logs an error and fails if provided stride is invalid.
    EXPECT_EQ ( 0 , _queue_create ( 1 , 0 ) );

    // TEST 3.3: queue_create does not allocate memory on failure.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( queue_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_QUEUE ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 4: queue_destroy handles invalid argument.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    queue_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_QUEUE );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // TEST 4.1: queue_destroy does not free memory if no queue is provided.
    queue_destroy ( 0 );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( queue_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_QUEUE ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( queue_amount_allocated , memory_amount_allocated ( MEMORY_TAG_QUEUE ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_queue_push_and_pop
( void )
{
    u64 global_amount_allocated;
    u64 queue_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    queue_amount_allocated = memory_amount_allocated ( MEMORY_TAG_QUEUE );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 op_count = 65536;
    const u32 to_push = random ();
    queue_t* queue = queue_create ( u32 );
    queue_t* queue_;
    u32 popped;

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , queue );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: queue_push.

    for ( u64 i = 0; i < op_count; ++i )
    {
        // Verify there was no memory error prior to the test.
        EXPECT_NEQ ( 0 , queue_push ( queue , &to_push ) );

        // TEST 1.2: queue_push does not modify the queue stride.
        EXPECT_EQ ( sizeof ( to_push ) , queue_stride ( queue ) );

        // TEST 1.3: queue_push increases the length of the queue by 1.
        EXPECT_EQ ( i + 1 , queue_length ( queue ) );

        // TEST 1.4: Queue has access to a memory buffer of adequate size.
        EXPECT ( queue_allocated ( queue ) >= queue_length ( queue ) * queue_stride ( queue ) );

        // TEST 1.5: queue_push appends the correct element to the end of the queue.
        EXPECT ( memory_equal ( ( void* )( ( ( u64 ) queue ) + queue_stride ( queue ) * ( queue_length ( queue ) - 1 ) ) , &to_push , queue_stride ( queue ) ) );
    }

    // TEST 1.6: Multiple subsequent queue_push invocations result in the correct output queue.
    for ( u64 i = 0; i < queue_length ( queue ); ++i )
    {
        EXPECT ( memory_equal ( ( void* )( ( ( u64 ) queue ) + queue_stride ( queue ) * i ) , &to_push , sizeof ( to_push ) ) );
    }

    // TEST 2: queue_pop.

    for ( u64 i = op_count; i > 1; --i )
    {
        // TEST 2.1: queue_pop succeeds.
        queue_ = queue;
        EXPECT ( queue_pop ( queue , &popped ) );

        // TEST 2.2: queue_pop does not modify the queue address.
        EXPECT_EQ ( queue_ , queue );

        // TEST 2.3: queue_push decreases the length of the queue by 1.
        EXPECT_EQ ( i - 1 , queue_length ( queue ) );

        // TEST 2.4: queue_push writes the correct element into the output buffer.
        EXPECT_EQ ( to_push , popped );
    }

    // TEST 2.5: queue_pop succeeds when no output buffer is provided.
    queue_ = queue;
    EXPECT ( queue_pop ( queue , 0 ) );

    // TEST 2.6: queue_pop does not modify the queue address.
    EXPECT_EQ ( queue_ , queue );

    // Verify the queue is empty prior to the test.
    EXPECT_EQ ( 0 , queue_length ( queue ) );

    // TEST 2.7: queue_pop warns and fails when the queue is empty.
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    popped = 0;
    queue_ = queue;
    EXPECT_NOT ( queue_pop ( queue , &popped ) );

    // TEST 2.8: queue_pop does not modify the queue address.
    EXPECT_EQ ( queue_ , queue );
    
    // TEST 2.9: queue_pop does not modify queue length if the queue is empty.
    EXPECT_EQ ( 0 , queue_length ( queue ) );

    // TEST 2.10: queue_pop writes nothing to the output buffer if the queue is empty.
    EXPECT_EQ ( 0 , popped );

    queue_destroy ( queue );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( queue_amount_allocated , memory_amount_allocated ( MEMORY_TAG_QUEUE ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_queue_peek
( void )
{
    u64 global_amount_allocated;
    u64 queue_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    queue_amount_allocated = memory_amount_allocated ( MEMORY_TAG_QUEUE );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;
    
    const u64 op_count = 10000;
    queue_t* queue = queue_create ( u32 );
    u32 to_push;
    u32 popped;

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , queue );

    // Push random elements onto the queue.
    for ( u64 i = 0; i < op_count; ++i )
    {
        to_push = random ();
        EXPECT_EQ ( i , queue_length ( queue ) );

        // Verify there was no memory error before continuing.
        EXPECT_NEQ ( 0 , queue_push ( queue , &to_push ) );
    }

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    for ( u64 i = op_count; i; --i )
    {
        // TEST 1: queue_peek with valid arguments.
        popped = 0;
        EXPECT ( queue_peek ( queue , &popped ) );

        // TEST 2: queue_peek writes the correct value into the output buffer.
        EXPECT ( memory_equal ( queue , &popped , queue_stride ( queue ) ) );

        // Pop the element from the queue.
        EXPECT ( queue_pop ( queue , &popped ) );

        // Verify there was no memory error before continuing.
        EXPECT_NEQ ( 0 , queue );
    }

    // TEST 3: queue_peek warns when the queue is empty.
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    popped = 0;
    queue_peek ( queue , &popped );

    // TEST 4: queue_peek writes nothing to the output buffer if the queue is empty.
    EXPECT_EQ ( 0 , popped );

    queue_destroy ( queue );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( queue_amount_allocated , memory_amount_allocated ( MEMORY_TAG_QUEUE ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

void
test_register_queue
( void )
{
    test_register ( test_queue_create_and_destroy , "Creating or destroying a queue." );
    test_register ( test_queue_push_and_pop , "Testing queue 'push' and 'pop' operations." );
    test_register ( test_queue_peek , "Testing queue 'peek' operation." );
}