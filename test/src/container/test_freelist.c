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
test_freelist_init_and_clear
( void )
{
    u64 global_amount_allocated;
    u64 freelist_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    freelist_amount_allocated = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    freelist_t freelist;
    u64 memory_requirement = 0;
    u64 size = 40;
    void* memory;

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: freelist_init handles invalid arguments.

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 1.1: freelist_init logs an error and fails if no output buffer for memory requirement is provided.
    EXPECT_NOT ( freelist_init ( size , 0 , 0 , 0 ) );

    // TEST 1.1: freelist_init logs an error and fails if no output buffer for freelist is provided.
    memory = memory_allocate ( 1 , MEMORY_TAG_STRING );
    EXPECT_NEQ ( 0 , memory ); // Verify there was no memory error prior to the test.
    EXPECT_NOT ( freelist_init ( size , &memory_requirement , memory , 0 ) );
    memory_free ( memory , 1 , MEMORY_TAG_STRING );

    // TEST 2: freelist_init succeeds with valid arguments.

    // TEST 2.1: freelist_init warns but succeeds when requested capacity is very small.
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    EXPECT ( freelist_init ( size , &memory_requirement , 0 , 0 ) );
    memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( freelist_init ( size , &memory_requirement , memory , &freelist ) );

    // TEST 2.2: Freelist created via freelist_init has access to a valid memory buffer.
    EXPECT_NEQ ( 0 , freelist.memory );

    // TEST 2.3: freelist_init initializes all memory blocks to free.
    EXPECT_EQ ( size , freelist_query_free ( &freelist ) );

    // TEST 3: freelist_clear succeeds with valid argument.

    freelist_clear ( &freelist );

    // TEST 3.1: freelist_clear clears the memory buffer handle.
    EXPECT_EQ ( 0 , freelist.memory );

    // TEST 4: freelist_clear handles invalid argument.

    freelist_clear ( 0 );

    // TEST 4.1: freelist_clear does not fail if provided freelist is not initialized.
    freelist_clear ( &freelist );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
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
    freelist_amount_allocated = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    freelist_t freelist;
    u64 memory_requirement = 0;
    u64 size = 512;
    u64 offset = INVALID_ID; // This is purposefully NOT a valid offset.

    EXPECT ( freelist_init ( size , &memory_requirement , 0 , 0 ) );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( freelist_init ( size , &memory_requirement, memory , &freelist ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , freelist.memory );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: freelist_allocate handles invalid arguments..

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 1.1: freelist_allocate logs an error and fails if no freelist is provided.
    EXPECT_NOT ( freelist_allocate ( 0 , 0 , &offset ) );

    // TEST 1.2: freelist_allocate logs an error and fails if no output buffer is provided.
    EXPECT_NOT ( freelist_allocate ( &freelist , 0 , 0 ) );

    // TEST 2: freelist_allocate with valid argument.

    // TEST 2.1: freelist_allocate succeeds with bad offset.
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset ) );

    // TEST 2.2: freelist_allocate with bad offset sets the offset to 0 and writes it to the output buffer.
    EXPECT_EQ ( 0 , offset );

    // TEST 2.3: Freelist has correct amount of free space.
    EXPECT_EQ ( size - 64 , freelist_query_free ( &freelist ) );

    // TEST 3: freelist_free handles invalid arguments.

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 3.1: freelist_free logs an error and fails if no freelist is provided.
    EXPECT_NOT ( freelist_free ( 0 , 64 , 0 ) );

    // TEST 3.2: freelist_free logs an error and fails if provided size is invalid.
    EXPECT_NOT ( freelist_free ( &freelist , 0 , 0 ) );

    // TEST 4: freelist_free succeeds with valid arguments.

    // TEST 4.1: freelist_free succeeds.
    EXPECT ( freelist_free ( &freelist , 64 , offset ) );

    // TEST 4.2: Freelist has correct amount of free space.
    EXPECT_EQ ( size , freelist_query_free ( &freelist ) );

    // TEST 5: freelist_allocate and freelist_free handle invalid arguments (2).

    freelist_clear ( &freelist );

    // TEST 5.1: freelist_allocate logs an error and fails if the provided freelist is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( freelist_allocate ( &freelist , 64 , &offset ) );
    
    // TEST 5.2: freelist_free logs an error and fails if the provided freelist is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( freelist_free ( &freelist , 64 , offset ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
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
    freelist_amount_allocated = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    freelist_t freelist;
    u64 memory_requirement = 0;
    u64 size = 512;

    EXPECT ( freelist_init ( size , &memory_requirement , 0 , 0 ) );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( freelist_init ( size , &memory_requirement , memory , &freelist ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , freelist.memory );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.
    
    // TEST 1: freelist_allocate succeeds with bad offset.
    u64 offset1 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset1 ) );

    // TEST 2: First freelist_allocate with bad offset sets the offset to 0 and writes it to the output buffer.
    EXPECT_EQ ( 0 , offset1 );

    // TEST 3: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 64 , freelist_query_free ( &freelist ) );
    
    // TEST 4: freelist_allocate succeeds with bad offset.
    u64 offset2 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset2 ) );

    // TEST 5: Subsequent freelist_allocate with bad offset sets the offset to the current offset + the size of the previous allocation and writes it to the output buffer.
    EXPECT_EQ ( 64 , offset2 );

    // TEST 6: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 128 , freelist_query_free ( &freelist ) );

    // TEST 7: freelist_allocate succeeds with bad offset.
    u64 offset3 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset3 ) );

    // TEST 8: freelist_allocate with bad offset sets the offset to the current offset + the size of the previous allocation and writes it to the output buffer.
    EXPECT_EQ ( 128 , offset3 );

    // TEST 9: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 192 , freelist_query_free ( &freelist ) );

    // TEST 10: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( &freelist , 64 , offset2 ) );

    // TEST 11: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size - 128, freelist_query_free ( &freelist ) );
    
    // TEST 12: freelist_allocate (with bad offset) succeeds in filling the space freed by previous freelist_free call.
    u64 offset4 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset4 ) );

    // TEST 13: Subsequent freelist_allocate with bad offset sets the offset to the current offset + the size of the previous allocation and writes it to the output buffer.
    EXPECT_EQ ( offset2 , offset4 );

    // TEST 14: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 192 , freelist_query_free ( &freelist ) );

    // TEST 15: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( &freelist , 64 , offset1 ) );

    // TEST 16: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size - 128 , freelist_query_free ( &freelist ) );

    // TEST 17: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( &freelist , 64 , offset3 ) );

    // TEST 18: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size - 64 , freelist_query_free ( &freelist ) );

    // TEST 19: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( &freelist , 64 , offset4 ) );

    // TEST 20: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size , freelist_query_free ( &freelist ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    freelist_clear ( &freelist );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
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
    freelist_amount_allocated = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    freelist_t freelist;
    u64 memory_requirement = 0;
    u64 size = 512;

    EXPECT ( freelist_init ( size , &memory_requirement , 0 , 0 ) );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( freelist_init ( size , &memory_requirement , memory , &freelist ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , freelist.memory );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: freelist_allocate succeeds with bad offset.
    u64 offset1 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset1 ) );

    // TEST 2: First freelist_allocate with bad offset sets the offset to 0 and writes it to the output buffer.
    EXPECT_EQ ( 0 , offset1 );

    // TEST 3: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 64 , freelist_query_free ( &freelist ) );

    // TEST 4: freelist_allocate succeeds with bad offset.
    u64 offset2 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset2 ) );

    // TEST 5: Subsequent freelist_allocate with bad offset sets the offset to the current offset + the size of the previous allocation and writes it to the output buffer.
    EXPECT_EQ ( 64 , offset2 );

    // TEST 6: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 128 , freelist_query_free ( &freelist ) );
    
    // TEST 7: freelist_allocate succeeds with bad offset.
    u64 offset3 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset3 ) );

    // TEST 8: Subsequent freelist_allocate with bad offset sets the offset to the current offset + the size of the previous allocation and writes it to the output buffer.
    EXPECT_EQ ( 128 , offset3 );

    // TEST 9: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 192 , freelist_query_free( &freelist ) );

    // TEST 10: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( &freelist , 64 , offset2 ) );

    // TEST 11: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size - 128 , freelist_query_free ( &freelist ) );

    // TEST 12: freelist_allocate (with bad offset) succeeds in filling the space freed by previous freelist_free call.
    u64 offset4 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset4 ) );

    // TEST 13: Subsequent freelist_allocate with bad offset sets the offset to the offset of the removed block (since the new one is occupying its spot) and writes it to the output buffer.
    EXPECT_EQ ( offset2 , offset4 );

    // TEST 14: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 192 , freelist_query_free ( &freelist ) );

    // TEST 15: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( &freelist , 64 , offset1 ) );

    // TEST 16: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size - 128 , freelist_query_free ( &freelist ) );

    // TEST 17: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( &freelist , 64 , offset3 ) );

    // TEST 18: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size - 64 , freelist_query_free ( &freelist ) );

    // TEST 19: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( &freelist , 64 , offset4 ) );

    // TEST 20: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size , freelist_query_free ( &freelist ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    freelist_clear ( &freelist );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
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
    freelist_amount_allocated = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    freelist_t freelist;
    u64 memory_requirement = 0;
    u64 size = 512;

    EXPECT ( freelist_init ( size , &memory_requirement , 0 , 0 ) );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( freelist_init ( size , &memory_requirement, memory , &freelist ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , freelist.memory );

    // Fill freelist prior to the test.
    u64 offset = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 512 , &offset ) );
    EXPECT_EQ ( 0 , offset );
    EXPECT_EQ ( 0 , freelist_query_free ( &freelist ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: freelist_allocate warns and fails when there is no free space remaining.
    u64 offset2 = INVALID_ID;
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    EXPECT_NOT ( freelist_allocate ( &freelist , 64 , &offset2 ) );

    // TEST 2: Freelist has correct amount of free space (0) following failed freelist_allocate call.
    EXPECT_EQ ( 0 , freelist_query_free ( &freelist ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    freelist_clear ( &freelist );
    memory_free ( memory  , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
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
    freelist_amount_allocated = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u32 alloc_count = 65556;
    const u32 max_op = 100000;
    u32 alloc = 0;
    u64 allocated = 0;
    u32 op = 0;
    freelist_t freelist;
    alloc_t allocs[ 65556 ];
    memory_clear ( allocs , sizeof ( alloc_t ) * alloc_count );

    // Randomize the allocation sizes. Initialize them all with bad offsets so it can be determined later whether or not they have been freed.
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

    u64 memory_requirement = 0;
    EXPECT ( freelist_init( size  , &memory_requirement , 0 , 0 ) );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( freelist_init ( size , &memory_requirement , memory , &freelist ) );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , freelist.memory );

    // Verify entire freelist is free space prior to the test.
    EXPECT_EQ ( size , freelist_query_free ( &freelist ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // Allocate and free at random, until the maximum number of allowed operations has been reached.
    LOGDEBUG ( "Performing %i random allocate and free operations. . ." , max_op );
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
                    if ( !test_freelist_util_allocate ( &freelist , &allocs[ i ] , &allocated , size ) )
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
                    if ( !test_freelist_util_free ( &freelist , &allocs[ i ] , &allocated , size ) )
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
    LOGDEBUG ( "  Done." );

    // Free any remaining blocks.
    LOGDEBUG ( "Freeing remaining allocations. . ." );
    for ( u64 i = 0; i < alloc_count; ++i )
    {
        if ( allocs[ i ].offset != INVALID_ID )
        {
            if ( !test_freelist_util_free ( &freelist , &allocs[ i ] , &allocated , size ) )
            {
                LOGERROR ( "test_freelist_multiple_alloc_and_free_random:  test_freelist_util_free failed on index: %i." , i );
                return false;
            }
        }
    }
    LOGDEBUG ( "  Done." );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    freelist_clear ( &freelist );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( freelist_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );
    
    return true;
}

void
test_register_freelist
( void )
{
    test_register ( test_freelist_init_and_clear , "Initializing or clearing a freelist." );
    test_register ( test_freelist_allocate_one_and_free_one , "Testing freelist with a single allocation and free." );
    test_register ( test_freelist_allocate_one_and_free_multiple , "Testing freelist with a single allocation and multiple frees." );
    test_register ( test_freelist_allocate_one_and_free_multiple_varying_sizes , "Testing freelist with multiple allocations and frees of varying sizes." );
    test_register ( test_freelist_allocate_until_full_and_fail_to_allocate_more , "Testing freelist overflow handling." );
    test_register ( test_freelist_multiple_allocate_and_free_random , "Testing freelist with multiple random-sized allocations, each freed in random order." );
}