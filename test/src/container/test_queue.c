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

/** @brief Computes current global number of unfreed allocations. */
#define GLOBAL_ALLOCATION_COUNT \
    ( memory_allocation_count () - memory_free_count () )

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
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

    queue_t queue;

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: queue_create handles invalid arguments.

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 1.1: queue_create fails if no output buffer for queue is provided.
    EXPECT_NOT ( queue_create ( 10 , 0 ) );

    // TEST 1.2: queue_create fails if provided stride is invalid.
    EXPECT_NOT ( queue_create ( 0 , &queue ) );

    // TEST 2: queue_create.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    queue_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_QUEUE );
    global_allocation_count_ = GLOBAL_ALLOCATION_COUNT;

    // TEST 2.1: queue_create succeeds.
    EXPECT ( queue_create ( 10 , &queue ) );

    // TEST 2.2: queue_create performed one memory allocation.
    EXPECT_EQ ( global_allocation_count_ + 1 , GLOBAL_ALLOCATION_COUNT );

    // TEST 2.3: queue_create allocated the correct number of bytes with the correct memory tag.
    EXPECT_EQ ( global_amount_allocated_ + queue.allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( queue_amount_allocated_ + queue.allocated , memory_amount_allocated ( MEMORY_TAG_QUEUE ) );

    // TEST 2.4: Queue created via queue_create has access to a valid memory buffer.
    EXPECT_NEQ ( 0 , queue.memory );

    // TEST 2.5: Queue created via queue_create has correct stride.
    EXPECT_EQ ( 10 , queue.stride );

    // TEST 2.6: Queue created via queue_create has default capacity.
    EXPECT_EQ ( QUEUE_DEFAULT_CAPACITY * queue.stride , queue.allocated );

    // TEST 2.7: Queue created via queue_create has 0 length.
    EXPECT_EQ ( 0 , queue.length );

    // TEST 3: queue_destroy.

    // TEST 3.1: queue_destroy restores the global allocator state.
    queue_destroy ( &queue );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( queue_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_QUEUE ) );
    EXPECT_EQ ( global_allocation_count_ , GLOBAL_ALLOCATION_COUNT );

    // TEST 3.2: queue_destroy clears all queue data structure fields.
    EXPECT_EQ ( 0 , queue.allocated );
    EXPECT_EQ ( 0 , queue.length );
    EXPECT_EQ ( 0 , queue.memory );
    EXPECT_EQ ( 0 , queue.stride );

    // TEST 4: queue_destroy handles invalid argument.

    // TEST: queue_destroy does not modify the global allocator state if no queue is provided.
    queue_destroy ( 0 );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( queue_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_QUEUE ) );
    EXPECT_EQ ( global_allocation_count_ , GLOBAL_ALLOCATION_COUNT );

    // TEST: queue_destroy does not modify the global allocator state if the provided queue is uninitialized.
    queue_destroy ( &queue );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( queue_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_QUEUE ) );
    EXPECT_EQ ( global_allocation_count_ , GLOBAL_ALLOCATION_COUNT );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( queue_amount_allocated , memory_amount_allocated ( MEMORY_TAG_QUEUE ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

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
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

    const u64 op_count = 65536;
    const u32 to_push = random ();
    u32 popped;
    queue_t queue;

    EXPECT ( queue_create ( sizeof ( to_push ) , &queue ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , queue.memory );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 1: queue_push fails if no queue is provided.
    EXPECT_NOT ( queue_push ( 0 , &to_push ) );

    // TEST 2: queue_push fails if no handle to the data to append is provided.
    EXPECT_NOT ( queue_push ( &queue , 0 ) );

    LOGDEBUG ( "Pushing %i elements onto a queue one-by-one. . ." , op_count );

    for ( u64 i = 0; i < op_count; ++i )
    {
        // TEST 3: queue_push succeeds given valid arguments.
        EXPECT ( queue_push ( &queue , &to_push ) );

        // Verify there was no memory error prior to the test.
        EXPECT_NEQ ( 0 , queue.memory );

        // TEST 4: queue_push does not modify the queue stride.
        EXPECT_EQ ( sizeof ( to_push ) , queue.stride );

        // TEST 5: queue_push increases the length of the queue by 1.
        EXPECT_EQ ( i + 1 , queue.length );

        // TEST 6: Queue has access to adequate memory following queue_push.
        EXPECT ( queue.allocated >= queue.length * queue.stride );

        // TEST 7: queue_push appends the correct element to the end of the queue.
        EXPECT ( memory_equal ( ( void* )( ( ( u64 )( queue.memory ) ) + queue.stride * ( queue.length - 1 ) ) , &to_push , queue.stride ) );
    }

    // TEST 8: Multiple subsequent queue_push invocations result in the correct output queue.
    for ( u64 i = 0; i < queue.length; ++i )
    {
        EXPECT ( memory_equal ( ( void* )( ( ( u64 )( queue.memory ) ) + queue.stride * i ) , &to_push , sizeof ( to_push ) ) );
    }

    LOGDEBUG ( "  Done." );

    // TEST 9: queue_pop fails if no queue is provided.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( queue_pop ( 0 , &popped ) );

    LOGDEBUG ( "Popping %i elements off the queue one-by-one. . ." , op_count );

    for ( u64 i = op_count; i > 1; --i )
    {
        // TEST 10: queue_pop succeeds given valid arguments.
        EXPECT ( queue_pop ( &queue , &popped ) );

        // Verify there was no memory error prior to the test.
        EXPECT_NEQ ( 0 , queue.memory );

        // TEST 11: queue_push decreases the length of the queue by 1.
        EXPECT_EQ ( i - 1 , queue.length );

        // TEST 12: queue_push writes the correct element into the output buffer.
        EXPECT_EQ ( to_push , popped );
    }

    // TEST 13: queue_pop succeeds when no output buffer is provided.
    queue_pop ( &queue , 0 );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , queue.memory );

    // Verify the queue is empty prior to the test.
    EXPECT_EQ ( 0 , queue.length );

    // TEST 14: queue_pop warns when the queue is empty.
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    popped = 0;
    void* queue_memory = queue.memory;
    queue_pop ( &queue , &popped );

    // TEST 15: queue_pop does not perform memory allocation if the queue is empty* (current implementation doesn't allocate in general either, but I don't need to test for that).
    EXPECT_EQ ( queue_memory , queue.memory );
    
    // TEST 16: queue_pop does not modify queue length if the queue is empty.
    EXPECT_EQ ( 0 , queue.length );

    // TEST 17: queue_pop writes nothing to the output buffer if the queue is empty.
    EXPECT_EQ ( 0 , popped );

    LOGDEBUG ( "  Done." );

    queue_destroy ( &queue );

    // TEST 18: queue_push logs an error and fails if the provided queue is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( queue_push ( &queue , &to_push ) );
    
    // TEST 19: queue_push logs an error and fails if the provided queue is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( queue_pop ( &queue , &popped ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( queue_amount_allocated , memory_amount_allocated ( MEMORY_TAG_QUEUE ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

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
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;
    
    const u64 op_count = 10000;
    u32 to_push;
    u32 popped;
    queue_t queue;

    EXPECT ( queue_create ( sizeof ( to_push ) , &queue ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , queue.memory );

    // Push random elements onto the queue.
    for ( u64 i = 0; i < op_count; ++i )
    {
        to_push = random ();
        EXPECT_EQ ( i , queue.length );
        EXPECT ( queue_push ( &queue , &to_push ) );

        // Verify there was no memory error before continuing.
        EXPECT_NEQ ( 0 , queue.memory );
    }

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 1: queue_peek logs an error and fails if no queue is provided.
    EXPECT_NOT ( queue_peek ( 0 , &popped ) );

    // TEST 2: queue_peek logs an error and fails if no output buffer is provided.
    EXPECT_NOT ( queue_peek ( &queue , 0 ) );

    for ( u64 i = op_count; i; --i )
    {
        // TEST 3: queue_peek with valid arguments.
        popped = 0;
        EXPECT ( queue_peek ( &queue , &popped ) );

        // TEST 4: queue_peek writes the correct value into the output buffer.
        EXPECT ( memory_equal ( queue.memory , &popped , queue.stride ) );

        // Pop the element from the queue.
        EXPECT ( queue_pop ( &queue , &popped ) );

        // Verify there was no memory error before continuing.
        EXPECT_NEQ ( 0 , queue.memory );
    }

    // TEST 5: queue_peek warns when the queue is empty.
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    popped = 0;
    void* queue_memory = queue.memory;
    queue_peek ( &queue , &popped );

    // TEST 6: queue_peek does not perform memory allocation if the queue is empty* (current implementation doesn't allocate in general either, but I don't need to test for that).
    EXPECT_EQ ( queue_memory , queue.memory );
    
    // TEST 7: queue_peek does not modify queue length if the queue is empty.
    EXPECT_EQ ( 0 , queue.length );

    // TEST 8: queue_peek writes nothing to the output buffer if the queue is empty.
    EXPECT_EQ ( 0 , popped );

    queue_destroy ( &queue );

    LOGWARN ( "The following error is intentionally triggered by a test:" );

    // TEST 9: queue_peek logs an error and fails if the provided queue is uninitialized.
    EXPECT_NOT ( queue_peek ( &queue , &popped ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( queue_amount_allocated , memory_amount_allocated ( MEMORY_TAG_QUEUE ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

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