/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/test_freelist.c
 * @brief Implementation of the container/test_freelist header.
 * (see container/test_freelist.h for additional details)
 */
#include "container/test_freelist.h"

#include "test/expect.h"

#include "core/memory.h"

/** @brief Type definition for a container to hold allocation info. */
typedef struct
{
    u64 size;
    u64 offset;
}
alloc_t;

u8
test_freelist_create_and_clear
( void )
{
    u64 global_amount_allocated;
    u64 freelist_amount_allocated;
    u64 global_allocation_count;

    u64 global_amount_allocated_;
    u64 freelist_amount_allocated_;
    u64 global_allocation_count_;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    freelist_amount_allocated = memory_amount_allocated ( MEMORY_TAG_FREELIST );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    freelist_t* freelist;
    u64 memory_requirement = 0;
    u64 size = 40;
    void* memory;

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: freelist_create handles invalid arguments.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    freelist_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_FREELIST );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 1.1: freelist_create logs an error and fails if invalid capacity is provided.
    EXPECT_NOT ( freelist_create ( 0 , 0 , 0 , &freelist ) );

    // TEST 1.2: freelist_create logs an error and fails if no output buffer for freelist is provided.
    EXPECT_NOT ( freelist_create ( size , 0 , 0 , 0 ) );

    // TEST 1.3: freelist_create did not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_FREELIST ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 2: freelist_create can be used to query the memory requirement for a freelist.
    
    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    freelist_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_FREELIST );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // TEST 2.1: freelist_create succeeds.
    memory_requirement = 0;
    EXPECT ( freelist_create ( size , &memory_requirement , 0 , 0 ) );

    // TEST 2.2: freelist_create writes the memory requirement to the output buffer.
    EXPECT_NEQ ( 0 , memory_requirement );

    // TEST 2.3: freelist_create did not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_FREELIST ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 3: Auto-allocated freelist.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    freelist_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_FREELIST );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // Query the memory requirement prior to the test.
    EXPECT ( freelist_create ( size , &memory_requirement , 0 , 0 ) );

    // TEST 3.1: freelist_create warns but succeeds when requested capacity is very small.
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    freelist = 0;
    EXPECT ( freelist_create ( size , 0 , 0 , &freelist ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , freelist );

    // TEST 3.2: freelist_create without a pre-allocated buffer performed one memory allocation.
    EXPECT_EQ ( global_allocation_count_ + 1 , MEMORY_ALLOCATION_COUNT );

    // TEST 3.3: freelist_create without a pre-allocated buffer allocated the correct number of bytes with the correct memory tag.
    EXPECT_EQ ( global_amount_allocated_ + memory_requirement , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated_ + memory_requirement , memory_amount_allocated ( MEMORY_TAG_FREELIST ) );

    // TEST 3.4: freelist_create initializes all memory blocks to free.
    EXPECT_EQ ( size , freelist_query_free ( freelist ) );

    // TEST 3.5: freelist_destroy nullifies the freelist handle.
    freelist_destroy ( &freelist );
    EXPECT_EQ ( 0 , freelist );

    // TEST 3.6: freelist_destroy without a pre-allocated buffer restores the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_FREELIST ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 4: Pre-allocated freelist.

    // Allocate the correct amount of memory prior to the test.
    EXPECT ( freelist_create ( size , &memory_requirement , 0 , 0 ) );
    memory = memory_allocate ( memory_requirement , MEMORY_TAG_FREELIST );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , memory );

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    freelist_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_FREELIST );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // TEST 4.1: freelist_create warns but succeeds when requested capacity is very small.
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    freelist = 0;
    EXPECT ( freelist_create ( size , 0 , memory , &freelist ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , freelist );

    // TEST 4.2: freelist_create with a pre-allocated buffer did not allocate memory.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_FREELIST ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 4.3: freelist_create initializes all memory blocks to free.
    EXPECT_EQ ( size , freelist_query_free ( freelist ) );

    // TEST 4.4: freelist_destroy nullifies the freelist handle.
    freelist_destroy ( &freelist );
    EXPECT_EQ ( 0 , freelist );

    // TEST 4.5: freelist_destroy with a pre-allocated buffer did not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_FREELIST ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    memory_free ( memory , memory_requirement , MEMORY_TAG_FREELIST );

    // TEST 5: freelist_destroy handles invalid argument.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    freelist_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_FREELIST );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // TEST 5.1: freelist_destroy does not modify the global allocator state if null handle is provided.
    freelist_destroy ( 0 );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_FREELIST ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 5.2: freelist_destroy does not modify the global allocator state if provided freelist is null.
    freelist = 0;
    freelist_destroy ( &freelist );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_FREELIST ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated , memory_amount_allocated ( MEMORY_TAG_FREELIST ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_freelist_allocate_one_and_free_one
( void )
{
    u64 global_amount_allocated;
    u64 freelist_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    freelist_amount_allocated = memory_amount_allocated ( MEMORY_TAG_FREELIST );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    freelist_t* freelist = 0;
    u64 size = 512;
    u64 offset = INVALID_ID; // This is purposefully NOT a valid offset.

    EXPECT ( freelist_create ( size , 0, 0 , &freelist ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , freelist );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: freelist_allocate.

    // TEST 1.1: freelist_allocate succeeds with bad offset.
    EXPECT ( freelist_allocate ( freelist , 64 , &offset ) );

    // TEST 1.2: freelist_allocate with bad offset sets the offset to 0 and writes it to the output buffer.
    EXPECT_EQ ( 0 , offset );

    // TEST 1.3: Freelist has correct amount of free space.
    EXPECT_EQ ( size - 64 , freelist_query_free ( freelist ) );

    // TEST 2: freelist_free.

    // TEST 2.1: freelist_free succeeds.
    EXPECT ( freelist_free ( freelist , 64 , offset ) );

    // TEST 2.2: Freelist has correct amount of free space.
    EXPECT_EQ ( size , freelist_query_free ( freelist ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    freelist_destroy ( &freelist );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated , memory_amount_allocated ( MEMORY_TAG_FREELIST ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_freelist_allocate_one_and_free_multiple
( void )
{
    u64 global_amount_allocated;
    u64 freelist_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    freelist_amount_allocated = memory_amount_allocated ( MEMORY_TAG_FREELIST );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    freelist_t* freelist;
    u64 size = 512;

    EXPECT ( freelist_create ( size , 0 , 0 , &freelist ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , freelist );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.
    
    // TEST 1: freelist_allocate succeeds with bad offset.
    u64 offset1 = INVALID_ID;
    EXPECT ( freelist_allocate ( freelist , 64 , &offset1 ) );

    // TEST 2: First freelist_allocate with bad offset sets the offset to 0 and writes it to the output buffer.
    EXPECT_EQ ( 0 , offset1 );

    // TEST 3: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 64 , freelist_query_free ( freelist ) );
    
    // TEST 4: freelist_allocate succeeds with bad offset.
    u64 offset2 = INVALID_ID;
    EXPECT ( freelist_allocate ( freelist , 64 , &offset2 ) );

    // TEST 5: Subsequent freelist_allocate with bad offset sets the offset to the current offset + the size of the previous allocation and writes it to the output buffer.
    EXPECT_EQ ( 64 , offset2 );

    // TEST 6: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 128 , freelist_query_free ( freelist ) );

    // TEST 7: freelist_allocate succeeds with bad offset.
    u64 offset3 = INVALID_ID;
    EXPECT ( freelist_allocate ( freelist , 64 , &offset3 ) );

    // TEST 8: freelist_allocate with bad offset sets the offset to the current offset + the size of the previous allocation and writes it to the output buffer.
    EXPECT_EQ ( 128 , offset3 );

    // TEST 9: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 192 , freelist_query_free ( freelist ) );

    // TEST 10: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( freelist , 64 , offset2 ) );

    // TEST 11: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size - 128, freelist_query_free ( freelist ) );
    
    // TEST 12: freelist_allocate (with bad offset) succeeds in filling the space freed by previous freelist_free call.
    u64 offset4 = INVALID_ID;
    EXPECT ( freelist_allocate ( freelist , 64 , &offset4 ) );

    // TEST 13: Subsequent freelist_allocate with bad offset sets the offset to the current offset + the size of the previous allocation and writes it to the output buffer.
    EXPECT_EQ ( offset2 , offset4 );

    // TEST 14: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 192 , freelist_query_free ( freelist ) );

    // TEST 15: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( freelist , 64 , offset1 ) );

    // TEST 16: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size - 128 , freelist_query_free ( freelist ) );

    // TEST 17: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( freelist , 64 , offset3 ) );

    // TEST 18: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size - 64 , freelist_query_free ( freelist ) );

    // TEST 19: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( freelist , 64 , offset4 ) );

    // TEST 20: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size , freelist_query_free ( freelist ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    freelist_destroy ( &freelist );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated , memory_amount_allocated ( MEMORY_TAG_FREELIST ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_freelist_allocate_one_and_free_multiple_varying_sizes
( void )
{
    u64 global_amount_allocated;
    u64 freelist_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    freelist_amount_allocated = memory_amount_allocated ( MEMORY_TAG_FREELIST );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    freelist_t* freelist;
    u64 size = 512;

    EXPECT ( freelist_create ( size , 0 , 0 , &freelist ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , freelist );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: freelist_allocate succeeds with bad offset.
    u64 offset1 = INVALID_ID;
    EXPECT ( freelist_allocate ( freelist , 64 , &offset1 ) );

    // TEST 2: First freelist_allocate with bad offset sets the offset to 0 and writes it to the output buffer.
    EXPECT_EQ ( 0 , offset1 );

    // TEST 3: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 64 , freelist_query_free ( freelist ) );

    // TEST 4: freelist_allocate succeeds with bad offset.
    u64 offset2 = INVALID_ID;
    EXPECT ( freelist_allocate ( freelist , 64 , &offset2 ) );

    // TEST 5: Subsequent freelist_allocate with bad offset sets the offset to the current offset + the size of the previous allocation and writes it to the output buffer.
    EXPECT_EQ ( 64 , offset2 );

    // TEST 6: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 128 , freelist_query_free ( freelist ) );
    
    // TEST 7: freelist_allocate succeeds with bad offset.
    u64 offset3 = INVALID_ID;
    EXPECT ( freelist_allocate ( freelist , 64 , &offset3 ) );

    // TEST 8: Subsequent freelist_allocate with bad offset sets the offset to the current offset + the size of the previous allocation and writes it to the output buffer.
    EXPECT_EQ ( 128 , offset3 );

    // TEST 9: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 192 , freelist_query_free( freelist ) );

    // TEST 10: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( freelist , 64 , offset2 ) );

    // TEST 11: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size - 128 , freelist_query_free ( freelist ) );

    // TEST 12: freelist_allocate (with bad offset) succeeds in filling the space freed by previous freelist_free call.
    u64 offset4 = INVALID_ID;
    EXPECT ( freelist_allocate ( freelist , 64 , &offset4 ) );

    // TEST 13: Subsequent freelist_allocate with bad offset sets the offset to the offset of the removed block (since the new one is occupying its spot) and writes it to the output buffer.
    EXPECT_EQ ( offset2 , offset4 );

    // TEST 14: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 192 , freelist_query_free ( freelist ) );

    // TEST 15: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( freelist , 64 , offset1 ) );

    // TEST 16: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size - 128 , freelist_query_free ( freelist ) );

    // TEST 17: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( freelist , 64 , offset3 ) );

    // TEST 18: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size - 64 , freelist_query_free ( freelist ) );

    // TEST 19: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( freelist , 64 , offset4 ) );

    // TEST 20: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size , freelist_query_free ( freelist ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    freelist_destroy ( &freelist );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated , memory_amount_allocated ( MEMORY_TAG_FREELIST ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_freelist_allocate_until_full_and_fail_to_allocate_more
( void )
{
    u64 global_amount_allocated;
    u64 freelist_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    freelist_amount_allocated = memory_amount_allocated ( MEMORY_TAG_FREELIST );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    freelist_t* freelist;
    u64 size = 512;

    EXPECT ( freelist_create ( size , 0, 0 , &freelist ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , freelist );

    // Fill freelist prior to the test.
    u64 offset = INVALID_ID;
    EXPECT ( freelist_allocate ( freelist , 512 , &offset ) );
    EXPECT_EQ ( 0 , offset );
    EXPECT_EQ ( 0 , freelist_query_free ( freelist ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: freelist_allocate warns and fails when there is no free space remaining.
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    u64 offset2 = INVALID_ID;
    EXPECT_NOT ( freelist_allocate ( freelist , 64 , &offset2 ) );

    // TEST 2: Freelist has correct amount of free space (0) following failed freelist_allocate call.
    EXPECT_EQ ( 0 , freelist_query_free ( freelist ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    freelist_destroy ( &freelist );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated , memory_amount_allocated ( MEMORY_TAG_FREELIST ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_freelist_util_allocate
(   freelist_t* freelist
,   alloc_t*    data
,   u64*        allocated
,   u64         size
)
{
    ( *data ).offset = INVALID_ID;
    EXPECT ( freelist_allocate ( freelist , ( *data ).size , &( *data ).offset ) );
    EXPECT_NEQ ( ( *data ).offset , INVALID_ID );
    *allocated += ( *data ).size;
    EXPECT_EQ ( size - *allocated , freelist_query_free ( freelist ) );
    return true;
}

u8
test_freelist_util_free
(   freelist_t* freelist
,   alloc_t*    data
,   u64*        allocated
,   u64         size
)
{
    EXPECT ( freelist_free ( freelist, ( *data ).size, ( *data ).offset ) );
    *allocated -= ( *data ).size;
    EXPECT_EQ ( size - *allocated , freelist_query_free ( freelist ) );
    ( *data ).offset = INVALID_ID;
    return true;
}

u8
test_freelist_multiple_allocate_and_free_random
( void )
{
    u64 global_amount_allocated;
    u64 freelist_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    freelist_amount_allocated = memory_amount_allocated ( MEMORY_TAG_FREELIST );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u32 alloc_count = 65556;
    const u32 max_op = 100000;

    freelist_t* freelist;
    u32 alloc = 0;
    u64 allocated = 0;
    u32 op = 0;

    // Randomize the allocation sizes. Initialize them all with bad offsets so it can be determined later whether or not they have been freed.
    alloc_t allocs[ 65556 ];
    memory_clear ( allocs , sizeof ( alloc_t ) * alloc_count );
    for ( u64 i = 0; i < alloc_count; ++i )
    {
        allocs[ i ].size = random2 ( 1 , 65536 );
        allocs[ i ].offset = INVALID_ID;
    }
    u64 size = 0;
    for (u64 i = 0; i < alloc_count; ++i )
    {
        size += allocs[ i ].size;
    }

    EXPECT ( freelist_create ( size , 0 , 0 , &freelist ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , freelist );

    // Verify entire freelist is free space prior to the test.
    EXPECT_EQ ( size , freelist_query_free ( freelist ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // Allocate and free at random, until the maximum number of allowed operations has been reached.
    while ( op < max_op )
    {
        if ( !alloc || random2 ( 0 , 99 ) > 50 )
        {
            for (;;)
            {
                u32 i = random2 ( 0 , alloc_count - 1 );
                if ( allocs[ i ].offset == INVALID_ID )
                {
                    // TEST 1: freelist_allocate succeeds with random size.
                    if ( !test_freelist_util_allocate ( freelist , &allocs[ i ] , &allocated , size ) )
                    {
                        LOGERROR ( "test_freelist_multiple_alloc_and_free_random:  test_freelist_util_allocate failed on index: %i." , i );
                        return false;
                    }
                    alloc += 1;
                    break;
                }
            }
            op += 1;
        }
        else
        {
            for (;;)
            {
                u32 i = random2 ( 0 , alloc_count - 1 );
                if ( allocs[ i ].offset != INVALID_ID )
                {
                    // TEST 2: freelist_free succeeds with random block.
                    if ( !test_freelist_util_free ( freelist , &allocs[ i ] , &allocated , size ) )
                    {
                        LOGERROR ( "test_freelist_multiple_alloc_and_free_random:  test_freelist_util_free failed on index: %i." , i );
                        return false;
                    }
                    alloc -= 1;
                    break;
                }
            }
            op += 1;
        }
    }

    // Free any remaining blocks.
    for ( u64 i = 0; i < alloc_count; ++i )
    {
        if ( allocs[ i ].offset != INVALID_ID )
        {
            if ( !test_freelist_util_free ( freelist , &allocs[ i ] , &allocated , size ) )
            {
                LOGERROR ( "test_freelist_multiple_alloc_and_free_random:  test_freelist_util_free failed on index: %i." , i );
                return false;
            }
        }
    }

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    freelist_destroy ( &freelist );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated , memory_amount_allocated ( MEMORY_TAG_FREELIST ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );
    
    return true;
}

void
test_register_freelist
( void )
{
    test_register ( test_freelist_create_and_clear , "Initializing or clearing a freelist." );
    test_register ( test_freelist_allocate_one_and_free_one , "Testing freelist with a single allocation and free." );
    test_register ( test_freelist_allocate_one_and_free_multiple , "Testing freelist with a single allocation and multiple frees." );
    test_register ( test_freelist_allocate_one_and_free_multiple_varying_sizes , "Testing freelist with multiple allocations and frees of varying sizes." );
    test_register ( test_freelist_allocate_until_full_and_fail_to_allocate_more , "Testing freelist overflow handling." );
    test_register ( test_freelist_multiple_allocate_and_free_random , "Testing freelist with multiple random-sized allocations, each freed in random order." );
}