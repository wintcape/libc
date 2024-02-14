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
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( linear_allocator_create ( sizeof ( u64 ) , 0 , 0 ) );
    EXPECT ( linear_allocator_create ( sizeof ( u64 ) , 0 , &allocator ) );
    EXPECT_NEQ ( 0 , allocator.memory );
    EXPECT_EQ ( sizeof ( u64 ) , allocator.capacity );
    EXPECT_EQ ( 0 , allocator.allocated );
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
    EXPECT ( linear_allocator_create ( sizeof ( u64 ) , 0 , &allocator ) );
    void* blk;
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    blk = linear_allocator_allocate ( &allocator , 0 );
    EXPECT_EQ ( 0 , blk );
    blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );
    linear_allocator_destroy ( &allocator );
    LOGWARN ( "The following errors are intentionally triggered by a test:" );
    blk = linear_allocator_allocate ( 0 , sizeof ( u64 ) );
    EXPECT_EQ ( 0 , blk );
    blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );
    EXPECT_EQ ( 0 , blk );
    return true;
}

u8
test_linear_allocator_max_allocation_count
( void )
{
    const u64 max_allocations = 1024;
    linear_allocator_t allocator;
    EXPECT ( linear_allocator_create ( sizeof ( u64 ) * max_allocations , 0 , &allocator ) );
    void* blk;
    for ( u64 i = 0; i < max_allocations; ++i )
    {
        blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );
        EXPECT_NEQ ( 0 , blk );
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
    EXPECT ( linear_allocator_create ( sizeof ( u64 ) , 0 , &allocator ) );
    void* blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );
    EXPECT_NEQ ( 0 , blk );
    EXPECT_EQ ( sizeof ( u64 ) , allocator.allocated );
    linear_allocator_destroy ( &allocator );
    return true;
}

u8
test_linear_allocator_overflow
( void )
{
    const u64 max_allocations = 1024;
    linear_allocator_t allocator;
    EXPECT ( linear_allocator_create ( sizeof ( u64 ) * max_allocations , 0 , &allocator ) );
    void* blk;
    for ( u64 i = 0; i < max_allocations; ++i )
    {
        blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );
        EXPECT_NEQ ( 0 , blk );
        EXPECT_EQ ( sizeof ( u64 ) * ( i + 1 ) , allocator.allocated );
    }
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );
    EXPECT_EQ ( 0 , blk );
    EXPECT_EQ ( sizeof ( u64 ) * max_allocations , allocator.allocated );
    linear_allocator_destroy ( &allocator );
    return true;
}

u8
test_linear_allocator_free
( void )
{
    const u64 max_allocations = 1024;
    linear_allocator_t allocator;
    EXPECT ( linear_allocator_create ( sizeof ( u64 ) * max_allocations , 0 , &allocator ) );
    void* blk;
    for ( u64 i = 0; i < max_allocations; ++i )
    {
        blk = linear_allocator_allocate ( &allocator , sizeof ( u64 ) );
        EXPECT_NEQ ( 0 , blk );
        EXPECT_EQ ( sizeof ( u64 ) * ( i + 1 ) , allocator.allocated );
    }
    EXPECT ( linear_allocator_free ( &allocator ) );
    EXPECT_EQ ( 0 , allocator.allocated );
    linear_allocator_destroy ( &allocator );
    linear_allocator_destroy ( &allocator );
    LOGWARN ( "The following errors are intentionally triggered by a test:" );
    EXPECT_NOT ( linear_allocator_free ( 0 ) );
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