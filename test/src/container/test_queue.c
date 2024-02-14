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
    EXPECT_NOT ( queue_create ( 10 , 0 ) );
    EXPECT_NOT ( queue_create ( 0 , &queue ) );
    EXPECT ( queue_create ( 10 , &queue ) );
    EXPECT_NEQ ( 0 , queue.memory );
    EXPECT_EQ ( 10 , queue.stride );
    EXPECT_EQ ( QUEUE_DEFAULT_CAPACITY * queue.stride , queue.allocated );
    EXPECT_EQ ( 0 , queue.length );
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
    const u64 max_op = 100000;
    const u32 to_push = random ();
    u32 popped;
    queue_t queue;
    EXPECT ( queue_create ( sizeof ( to_push ) , &queue ) );
    EXPECT_NEQ ( 0 , queue.memory );
    EXPECT_EQ ( sizeof ( to_push ) , queue.stride );
    LOGWARN ( "The following errors are intentionally triggered by a test:" );
    EXPECT_NOT ( queue_push ( 0 , &to_push ) );
    EXPECT_NOT ( queue_push ( &queue , 0 ) );
    LOGDEBUG ( "Pushing %i elements onto a queue one-by-one. . ." , max_op );
    for ( u64 i = 0; i < max_op; ++i )
    {
        EXPECT_EQ ( i , queue.length );
        EXPECT ( queue_push ( &queue , &to_push ) );
        EXPECT_NEQ ( 0 , queue.memory );
        EXPECT_EQ ( sizeof ( to_push ) , queue.stride );
        EXPECT_EQ ( i + 1 , queue.length );
        EXPECT ( queue.allocated >= queue.length * queue.stride );
        EXPECT ( memory_equal ( ( void* )( ( ( u64 )( queue.memory ) ) + queue.stride * ( queue.length - 1 ) ) , &to_push , queue.stride ) );
    }
    for ( u64 i = 0; i < queue.length; ++i )
    {
        EXPECT ( memory_equal ( ( void* )( ( ( u64 )( queue.memory ) ) + queue.stride * i ) , &to_push , sizeof ( to_push ) ) );
    }
    LOGDEBUG ( "  Done." );
    LOGWARN ( "The following errors are intentionally triggered by a test:" );
    EXPECT_NOT ( queue_pop ( 0 , &popped ) );
    EXPECT_NOT ( queue_pop ( &queue , 0 ) );
    LOGDEBUG ( "Popping %i elements off the queue one-by-one. . ." , max_op );
    for ( u64 i = max_op; i > 0; --i )
    {
        EXPECT_EQ ( i , queue.length );
        EXPECT ( queue_pop ( &queue , &popped ) );
        EXPECT_NEQ ( 0 , queue.memory );
        EXPECT_EQ ( i - 1 , queue.length );
        EXPECT_EQ ( to_push , popped );
    }
    LOGDEBUG ( "  Done." );
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    EXPECT_NOT ( queue_pop ( &queue , &popped ) );
    queue_destroy ( &queue );
    return true;
}

u8
test_queue_peek
( void )
{
    const u64 max_op = 10000;
    u32 to_push;
    u32 popped;
    queue_t queue;
    EXPECT ( queue_create ( sizeof ( to_push ) , &queue ) );
    EXPECT_NEQ ( 0 , queue.memory );
    for ( u64 i = 0; i < max_op; ++i )
    {
        to_push = random ();
        EXPECT_EQ ( i , queue.length );
        EXPECT ( queue_push ( &queue , &to_push ) );
        EXPECT_NEQ ( 0 , queue.memory );
    }
    LOGWARN ( "The following errors are intentionally triggered by a test:" );
    EXPECT_NOT ( queue_peek ( 0 , &popped ) );
    EXPECT_NOT ( queue_peek ( &queue , 0 ) );
    for ( u64 i = max_op; i > 0; --i )
    {
        EXPECT_EQ ( i , queue.length );
        EXPECT ( queue_peek ( &queue , &popped ) );
        EXPECT ( memory_equal ( queue.memory , &popped , queue.stride ) );
        EXPECT ( queue_pop ( &queue , &popped ) );
        EXPECT_NEQ ( 0 , queue.memory );
    }
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    EXPECT_NOT ( queue_peek ( &queue , &popped ) );
    queue_destroy ( &queue );
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