/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file memory/test_linear_allocator.c
 * @brief Implementation of the memory/test_linear_allocator header.
 * (see memory/test_linear_allocator.h for additional details)
 */
#include "memory/test_linear_allocator.h"

#include "test/expect.h"

u8
test_linear_allocator_create_and_destroy
( void )
{
    linear_allocator_t allocator;

    // TEST: linear_allocator_create fails if no output buffer is provided for the linear allocator.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( linear_allocator_create ( sizeof ( u64 ) , 0 , 0 ) );

    // TEST: linear_allocator_create succeeds with valid arguments.
    EXPECT ( linear_allocator_create ( sizeof ( u64 ) , 0 , &allocator ) );

    // TEST: Linear allocator created via linear_allocator_create has access to a valid memory buffer.
    EXPECT_NEQ ( 0 , allocator.memory );

    // TEST: Linear allocator created via linear_allocator_create has the correct capacity.
    EXPECT_EQ ( sizeof ( u64 ) , allocator.capacity );

    // TEST: Linear allocator created via linear_allocator_create is initialized with all of its memory free.
    EXPECT_EQ ( 0 , allocator.allocated );

    // TEST: linear_allocator_destroy clears all linear allocator data structure fields.
    linear_allocator_destroy ( &allocator );
    EXPECT_EQ ( 0 , allocator.memory );
    EXPECT_EQ ( 0 , allocator.capacity );
    EXPECT_EQ ( 0 , allocator.allocated );

    return true;
}

u8
test_linear_allocator_allocate
( void )
{
    linear_allocator_t allocator;
    void* blk;

    EXPECT ( linear_allocator_create ( sizeof ( u64 ) , 0 , &allocator ) );

    // Verify there was no memory error prior to testing.
    EXPECT_NEQ ( 0 , allocator.memory );

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: linear_allocator_allocate logs an error and does not perform memory allocation if no allocator is provided.
    blk = 0;
    blk = linear_allocator_allocate ( 0 , 1 );
    EXPECT_EQ ( 0 , blk );
    EXPECT_EQ ( 0 , allocator.allocated );

    // TEST: linear_allocator_allocate logs an error and does not perform memory allocation if the provided size is invalid.
    blk = 0;
    blk = linear_allocator_allocate ( &allocator , 0 );
    EXPECT_EQ ( 0 , blk );
    EXPECT_EQ ( 0 , allocator.allocated );

    blk = 0;
    blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );

    // TEST: If passed valid arguments, linear_allocator_allocate returns a handle to an allocated block of memory. 
    EXPECT_NEQ ( 0 , blk );

    // TEST: After linear_allocator_allocate is completed, the allocator has allocated the correct amount of its total memory. 
    EXPECT_EQ ( sizeof ( u64 ) , allocator.allocated );

    linear_allocator_destroy ( &allocator );

    // TEST: linear_allocator_allocate logs an error and does not perform memory allocation if the provided allocator is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    blk = 0;
    blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );
    EXPECT_EQ ( 0 , blk );
    EXPECT_EQ ( 0 , allocator.allocated );

    return true;
}

u8
test_linear_allocator_max_allocation_count
( void )
{
    const u64 op_count = 1024;
    linear_allocator_t allocator;
    void* blk;

    EXPECT ( linear_allocator_create ( sizeof ( u64 ) * op_count , 0 , &allocator ) );

    // Verify there was no memory error prior to testing.
    EXPECT_NEQ ( 0 , allocator.memory );

    for ( u64 i = 0; i < op_count; ++i )
    {
        blk = 0;
        blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );

        // TEST: If passed valid arguments, linear_allocator_allocate returns a handle to an allocated block of memory. 
        EXPECT_NEQ ( 0 , blk );

        // TEST: After linear_allocator_allocate is completed, the allocator has allocated the correct amount of its total memory. 
        EXPECT_EQ ( sizeof ( u64 ) * ( i + 1 ) , allocator.allocated );
    }

    linear_allocator_destroy ( &allocator );

    return true;
}

u8
test_linear_allocator_max_allocation_size
( void )
{
    linear_allocator_t allocator;
    void* blk;

    EXPECT ( linear_allocator_create ( sizeof ( u64 ) , 0 , &allocator ) );

    // Verify there was no memory error prior to testing.
    EXPECT_NEQ ( 0 , allocator.memory );

    blk = 0;
    blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );

    // TEST: If passed valid arguments, linear_allocator_allocate returns a handle to an allocated block of memory.
    EXPECT_NEQ ( 0 , blk );

    // TEST: After linear_allocator_allocate is completed, the allocator has allocated the correct amount of its total memory (all of it).
    EXPECT_EQ ( sizeof ( u64 ) , allocator.allocated );

    linear_allocator_destroy ( &allocator );

    return true;
}

u8
test_linear_allocator_overflow
( void )
{
    const u64 op_count = 1024;
    linear_allocator_t allocator;
    void* blk;

    EXPECT ( linear_allocator_create ( sizeof ( u64 ) * op_count , 0 , &allocator ) );

    // Verify there was no memory error prior to testing.
    EXPECT_NEQ ( 0 , allocator.memory );
    
    for ( u64 i = 0; i < op_count; ++i )
    {
        blk = 0;
        blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );

        // TEST: If passed valid arguments, linear_allocator_allocate returns a handle to an allocated block of memory. 
        EXPECT_NEQ ( 0 , blk );

        // TEST: After linear_allocator_allocate is completed, the allocator has allocated the correct amount of its total memory. 
        EXPECT_EQ ( sizeof ( u64 ) * ( i + 1 ) , allocator.allocated );
    }

    // TEST: linear_allocator_allocate logs an error and does not perform memory allocation if the provided allocator is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    blk = 0;
    blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );
    EXPECT_EQ ( 0 , blk );
    EXPECT_EQ ( sizeof ( u64 ) * op_count , allocator.allocated );
    linear_allocator_destroy ( &allocator );

    return true;
}

u8
test_linear_allocator_free
( void )
{
    const u64 max_allocations = 1024;
    linear_allocator_t allocator;
    void* blk;

    EXPECT ( linear_allocator_create ( sizeof ( u64 ) * max_allocations , 0 , &allocator ) );
    
    // Verify there was no memory error prior to testing.
    EXPECT_NEQ ( 0 , allocator.memory );

    for ( u64 i = 0; i < max_allocations; ++i )
    {
        blk = 0;
        blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );

        // TEST: If passed valid arguments, linear_allocator_allocate returns a handle to an allocated block of memory. 
        EXPECT_NEQ ( 0 , blk );

        // TEST: After linear_allocator_allocate is completed, the allocator has allocated the correct amount of its total memory. 
        EXPECT_EQ ( sizeof ( u64 ) * ( i + 1 ) , allocator.allocated );
    }

    // TEST: linear_allocator_free succeeds with valid argument.
    EXPECT ( linear_allocator_free ( &allocator ) );

    // TEST: linear_allocator_free frees all memory currently allocated by the allocator.
    EXPECT_EQ ( 0 , allocator.allocated );
    
    linear_allocator_destroy ( &allocator );

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: linear_allocator_free fails if no allocator is provided.
    EXPECT_NOT ( linear_allocator_free ( 0 ) );

    // TEST: linear_allocator_free fails if the provided allocator is uninitialized.
    EXPECT_NOT ( linear_allocator_free ( &allocator ) );

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