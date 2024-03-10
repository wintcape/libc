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
    queue_t queue;

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: queue_create fails if no output buffer for queue is provided.
    EXPECT_NOT ( queue_create ( 10 , 0 ) );

    // TEST: queue_create fails if provided stride is invalid.
    EXPECT_NOT ( queue_create ( 0 , &queue ) );

    // TEST: queue_create succeeds given valid arguments.
    EXPECT ( queue_create ( 10 , &queue ) );

    // TEST: Queue created via queue_create has access to a valid memory buffer.
    EXPECT_NEQ ( 0 , queue.memory );

    // TEST: Queue created via queue_create has correct stride.
    EXPECT_EQ ( 10 , queue.stride );

    // TEST: Queue created via queue_create has default capacity.
    EXPECT_EQ ( QUEUE_DEFAULT_CAPACITY * queue.stride , queue.allocated );

    // TEST: Queue created via queue_create has 0 length.
    EXPECT_EQ ( 0 , queue.length );

    // TEST: hashtable_destroy clears all queue data structure fields.
    queue_destroy ( &queue );
    EXPECT_EQ ( 0 , queue.memory );
    EXPECT_EQ ( 0 , queue.stride );
    EXPECT_EQ ( 0 , queue.allocated );
    EXPECT_EQ ( 0 , queue.length );

    return true;
}

u8
test_queue_push_and_pop
( void )
{
    const u64 op_count = 65536;
    const u32 to_push = random ();
    u32 popped;
    queue_t queue;

    EXPECT ( queue_create ( sizeof ( to_push ) , &queue ) );

    // Verify there was no memory error prior to testing.
    EXPECT_NEQ ( 0 , queue.memory );

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: queue_push fails if no queue is provided.
    EXPECT_NOT ( queue_push ( 0 , &to_push ) );

    // TEST: queue_push fails if no handle to the data to append is provided.
    EXPECT_NOT ( queue_push ( &queue , 0 ) );

    LOGDEBUG ( "Pushing %i elements onto a queue one-by-one. . ." , op_count );

    for ( u64 i = 0; i < op_count; ++i )
    {
        // TEST: queue_push succeeds given valid arguments.
        EXPECT ( queue_push ( &queue , &to_push ) );

        // Verify there was no memory error prior to testing.
        EXPECT_NEQ ( 0 , queue.memory );

        // TEST: queue_push does not modify the queue stride.
        EXPECT_EQ ( sizeof ( to_push ) , queue.stride );

        // TEST: queue_push increases the length of the queue by 1.
        EXPECT_EQ ( i + 1 , queue.length );

        // TEST: Queue has access to adequate memory following queue_push.
        EXPECT ( queue.allocated >= queue.length * queue.stride );

        // TEST: queue_push appends the correct element to the end of the queue.
        EXPECT ( memory_equal ( ( void* )( ( ( u64 )( queue.memory ) ) + queue.stride * ( queue.length - 1 ) ) , &to_push , queue.stride ) );
    }

    // TEST: Multiple subsequent queue_push invocations result in the correct output queue.
    for ( u64 i = 0; i < queue.length; ++i )
    {
        EXPECT ( memory_equal ( ( void* )( ( ( u64 )( queue.memory ) ) + queue.stride * i ) , &to_push , sizeof ( to_push ) ) );
    }

    LOGDEBUG ( "  Done." );

    // TEST: queue_pop fails if no queue is provided.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( queue_pop ( 0 , &popped ) );

    LOGDEBUG ( "Popping %i elements off the queue one-by-one. . ." , op_count );

    for ( u64 i = op_count; i > 1; --i )
    {
        // TEST: queue_pop succeeds given valid arguments.
        EXPECT ( queue_pop ( &queue , &popped ) );

        // Verify there was no memory error prior to testing.
        EXPECT_NEQ ( 0 , queue.memory );

        // TEST: queue_push decreases the length of the queue by 1.
        EXPECT_EQ ( i - 1 , queue.length );

        // TEST: queue_push writes the correct element into the output buffer.
        EXPECT_EQ ( to_push , popped );
    }

    // TEST: queue_pop succeeds when no output buffer is provided.
    queue_pop ( &queue , 0 );

    // Verify there was no memory error prior to testing.
    EXPECT_NEQ ( 0 , queue.memory );

    // Verify the queue is empty prior to testing.
    EXPECT_EQ ( 0 , queue.length );

    // TEST: queue_pop warns when the queue is empty.
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    popped = 0;
    void* queue_memory = queue.memory;
    queue_pop ( &queue , &popped );

    // TEST: queue_pop does not perform memory allocation if the queue is empty* (current implementation doesn't allocate in general either, but I don't need to test for that).
    EXPECT_EQ ( queue_memory , queue.memory );
    
    // TEST: queue_pop does not modify queue length if the queue is empty.
    EXPECT_EQ ( 0 , queue.length );

    // TEST: queue_pop writes nothing to the output buffer if the queue is empty.
    EXPECT_EQ ( 0 , popped );

    LOGDEBUG ( "  Done." );

    queue_destroy ( &queue );

    // TEST: queue_push logs an error and fails if the provided queue is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( queue_push ( &queue , &to_push ) );
    
    // TEST: queue_push logs an error and fails if the provided queue is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( queue_pop ( &queue , &popped ) );

    return true;
}

u8
test_queue_peek
( void )
{
    const u64 op_count = 10000;
    u32 to_push;
    u32 popped;
    queue_t queue;

    EXPECT ( queue_create ( sizeof ( to_push ) , &queue ) );

    // Verify there was no memory error prior to testing.
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

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: queue_peek logs an error and fails if no queue is provided.
    EXPECT_NOT ( queue_peek ( 0 , &popped ) );

    // TEST: queue_peek logs an error and fails if no output buffer is provided.
    EXPECT_NOT ( queue_peek ( &queue , 0 ) );

    for ( u64 i = op_count; i; --i )
    {
        // TEST: queue_peek with valid arguments.
        popped = 0;
        EXPECT ( queue_peek ( &queue , &popped ) );

        // TEST: queue_peek writes the correct value into the output buffer.
        EXPECT ( memory_equal ( queue.memory , &popped , queue.stride ) );

        // Pop the element from the queue.
        EXPECT ( queue_pop ( &queue , &popped ) );

        // Verify there was no memory error before continuing.
        EXPECT_NEQ ( 0 , queue.memory );
    }

    // TEST: queue_peek warns when the queue is empty.
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    popped = 0;
    void* queue_memory = queue.memory;
    queue_peek ( &queue , &popped );

    // TEST: queue_peek does not perform memory allocation if the queue is empty* (current implementation doesn't allocate in general either, but I don't need to test for that).
    EXPECT_EQ ( queue_memory , queue.memory );
    
    // TEST: queue_peek does not modify queue length if the queue is empty.
    EXPECT_EQ ( 0 , queue.length );

    // TEST: queue_peek writes nothing to the output buffer if the queue is empty.
    EXPECT_EQ ( 0 , popped );

    queue_destroy ( &queue );

    LOGWARN ( "The following error is intentionally triggered by a test:" );

    // TEST: queue_peek logs an error and fails if the provided queue is uninitialized.
    EXPECT_NOT ( queue_peek ( &queue , &popped ) );

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