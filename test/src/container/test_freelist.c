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
    freelist_t freelist;
    u64 memory_requirement = 0;
    u64 size = 40;
    void* memory;

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: freelist_init logs an error and fails if no output buffer for memory requirement is provided.
    EXPECT_NOT ( freelist_init ( size , 0 , 0 , 0 ) );

    // TEST: freelist_init logs an error and fails if no output buffer for freelist is provided.
    memory = 0;
    memory = memory_allocate ( 1 , MEMORY_TAG_STRING );
    EXPECT_NEQ ( 0 , memory ); // Verify there was no memory error prior to testing.
    EXPECT_NOT ( freelist_init ( size , &memory_requirement , memory , 0 ) );
    memory_free ( memory , 1 , MEMORY_TAG_STRING );

    // TEST: freelist_init warns but succeeds when requested capacity is very small.
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    EXPECT ( freelist_init ( size , &memory_requirement , 0 , 0 ) );
    memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( freelist_init ( size , &memory_requirement , memory , &freelist ) );

    // TEST: Freelist created via freelist_init has access to a valid memory buffer.
    EXPECT_NEQ ( 0 , freelist.memory );

    // TEST: freelist_init initializes all memory blocks to free.
    EXPECT_EQ ( size , freelist_query_free ( &freelist ) );

    freelist_clear ( &freelist );

    // TEST: freelist_clear clears the memory buffer handle.
    EXPECT_EQ ( 0 , freelist.memory );

    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    return true;
}

u8
test_freelist_allocate_one_and_free_one
( void )
{
    freelist_t freelist;
    u64 memory_requirement = 0;
    u64 size = 512;
    u64 offset = INVALID_ID; // This is purposefully NOT a valid offset.

    EXPECT ( freelist_init ( size , &memory_requirement , 0 , 0 ) );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( freelist_init ( size , &memory_requirement, memory , &freelist ) );

    // Verify there was no memory error prior to testing.
    EXPECT_NEQ ( 0 , freelist.memory );

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: freelist_allocate logs an error and fails if no freelist is provided.
    EXPECT_NOT ( freelist_allocate ( 0 , 0 , &offset ) );

    // TEST: freelist_allocate logs an error and fails if no output buffer is provided.
    EXPECT_NOT ( freelist_allocate ( &freelist , 0 , 0 ) );

    // TEST: freelist_allocate succeeds with bad offset.
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset ) );

    // TEST: freelist_allocate with bad offset sets the offset to 0 and writes it to the output buffer.
    EXPECT_EQ ( 0 , offset );

    // TEST: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 64 , freelist_query_free ( &freelist ) );

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: freelist_free logs an error and fails if no freelist is provided.
    EXPECT_NOT ( freelist_free ( 0 , 64 , 0 ) );

    // TEST: freelist_free logs an error and fails if provided size is invalid.
    EXPECT_NOT ( freelist_free ( &freelist , 0 , 0 ) );

    // TEST: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( &freelist , 64 , offset ) );

    // TEST: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size , freelist_query_free ( &freelist ) );

    freelist_clear ( &freelist );

    // TEST: freelist_allocate logs an error and fails if the provided freelist is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( freelist_allocate ( &freelist , 64 , &offset ) );
    
    // TEST: freelist_free logs an error and fails if the provided freelist is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_NOT ( freelist_free ( &freelist , 64 , offset ) );

    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    return true;
}

u8
test_freelist_allocate_one_and_free_multiple
( void )
{
    freelist_t freelist;
    u64 memory_requirement = 0;
    u64 size = 512;

    EXPECT ( freelist_init ( size , &memory_requirement , 0 , 0 ) );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( freelist_init ( size , &memory_requirement , memory , &freelist ) );

    // Verify there was no memory error prior to testing.
    EXPECT_NEQ ( 0 , freelist.memory );
    
    // TEST: freelist_allocate succeeds with bad offset.
    u64 offset1 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset1 ) );

    // TEST: First freelist_allocate with bad offset sets the offset to 0 and writes it to the output buffer.
    EXPECT_EQ ( 0 , offset1 );

    // TEST: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 64 , freelist_query_free ( &freelist ) );
    
    // TEST: freelist_allocate succeeds with bad offset.
    u64 offset2 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset2 ) );

    // TEST: Subsequent freelist_allocate with bad offset sets the offset to the current offset + the size of the previous allocation and writes it to the output buffer.
    EXPECT_EQ ( 64 , offset2 );

    // TEST: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 128 , freelist_query_free ( &freelist ) );

    // TEST: freelist_allocate succeeds with bad offset.
    u64 offset3 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset3 ) );

    // TEST: freelist_allocate with bad offset sets the offset to the current offset + the size of the previous allocation and writes it to the output buffer.
    EXPECT_EQ ( 128 , offset3 );

    // TEST: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 192 , freelist_query_free ( &freelist ) );

    // TEST: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( &freelist , 64 , offset2 ) );

    // TEST: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size - 128, freelist_query_free ( &freelist ) );
    
    // TEST: freelist_allocate (with bad offset) succeeds in filling the space freed by previous freelist_free call.
    u64 offset4 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset4 ) );

    // TEST: Subsequent freelist_allocate with bad offset sets the offset to the current offset + the size of the previous allocation and writes it to the output buffer.
    EXPECT_EQ ( offset2 , offset4 );

    // TEST: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 192 , freelist_query_free ( &freelist ) );

    // TEST: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( &freelist , 64 , offset1 ) );

    // TEST: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size - 128 , freelist_query_free ( &freelist ) );

    // TEST: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( &freelist , 64 , offset3 ) );

    // TEST: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size - 64 , freelist_query_free ( &freelist ) );

    // TEST: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( &freelist , 64 , offset4 ) );

    // TEST: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size , freelist_query_free ( &freelist ) );

    freelist_clear ( &freelist );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    return true;
}

u8
test_freelist_allocate_one_and_free_multiple_varying_sizes
( void )
{
    freelist_t freelist;
    u64 memory_requirement = 0;
    u64 size = 512;

    EXPECT ( freelist_init ( size , &memory_requirement , 0 , 0 ) );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( freelist_init ( size , &memory_requirement , memory , &freelist ) );

    // Verify there was no memory error prior to testing.
    EXPECT_NEQ ( 0 , freelist.memory );

    // TEST: freelist_allocate succeeds with bad offset.
    u64 offset1 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset1 ) );

    // TEST: First freelist_allocate with bad offset sets the offset to 0 and writes it to the output buffer.
    EXPECT_EQ ( 0 , offset1 );

    // TEST: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 64 , freelist_query_free ( &freelist ) );

    // TEST: freelist_allocate succeeds with bad offset.
    u64 offset2 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset2 ) );

    // TEST: Subsequent freelist_allocate with bad offset sets the offset to the current offset + the size of the previous allocation and writes it to the output buffer.
    EXPECT_EQ ( 64 , offset2 );

    // TEST: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 128 , freelist_query_free ( &freelist ) );
    
    // TEST: freelist_allocate succeeds with bad offset.
    u64 offset3 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset3 ) );

    // TEST: Subsequent freelist_allocate with bad offset sets the offset to the current offset + the size of the previous allocation and writes it to the output buffer.
    EXPECT_EQ ( 128 , offset3 );

    // TEST: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 192 , freelist_query_free( &freelist ) );

    // TEST: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( &freelist , 64 , offset2 ) );

    // TEST: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size - 128 , freelist_query_free ( &freelist ) );

    // TEST: freelist_allocate (with bad offset) succeeds in filling the space freed by previous freelist_free call.
    u64 offset4 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset4 ) );

    // TEST: Subsequent freelist_allocate with bad offset sets the offset to the offset of the removed block (since the new one is occupying its spot) and writes it to the output buffer.
    EXPECT_EQ ( offset2 , offset4 );

    // TEST: Freelist has correct amount of free space following freelist_allocate.
    EXPECT_EQ ( size - 192 , freelist_query_free ( &freelist ) );

    // TEST: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( &freelist , 64 , offset1 ) );

    // TEST: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size - 128 , freelist_query_free ( &freelist ) );

    // TEST: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( &freelist , 64 , offset3 ) );

    // TEST: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size - 64 , freelist_query_free ( &freelist ) );

    // TEST: freelist_free succeeds with valid block.
    EXPECT ( freelist_free ( &freelist , 64 , offset4 ) );

    // TEST: Freelist has correct amount of free space following freelist_free.
    EXPECT_EQ ( size , freelist_query_free ( &freelist ) );

    freelist_clear ( &freelist );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    return true;
}

u8
test_freelist_allocate_until_full_and_fail_to_allocate_more
( void )
{
    freelist_t freelist;
    u64 memory_requirement = 0;
    u64 size = 512;

    EXPECT ( freelist_init ( size , &memory_requirement , 0 , 0 ) );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( freelist_init ( size , &memory_requirement, memory , &freelist ) );

    // Verify there was no memory error prior to testing.
    EXPECT_NEQ ( 0 , freelist.memory );

    // TEST: freelist_allocate succeeds with bad offset.
    u64 offset = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 512 , &offset ) );

    // TEST: First freelist_allocate with bad offset sets the offset to 0 and writes it to the output buffer.
    EXPECT_EQ ( 0 , offset );

    // TEST: Freelist has correct amount of free space (0) following freelist_allocate.
    EXPECT_EQ ( 0 , freelist_query_free ( &freelist ) );

    // TEST: freelist_allocate warns and fails when there is no free space remaining.
    u64 offset2 = INVALID_ID;
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    EXPECT_NOT ( freelist_allocate ( &freelist , 64 , &offset2 ) );

    // TEST: Freelist has correct amount of free space (0) following failed freelist_allocate call.
    EXPECT_EQ ( 0 , freelist_query_free ( &freelist ) );

    freelist_clear ( &freelist );
    memory_free ( memory  , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

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
    EXPECT_EQ ( freelist_query_free ( freelist ) , size - *allocated );
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
    EXPECT_EQ ( freelist_query_free ( freelist ) , size - *allocated );
    ( *data ).offset = INVALID_ID;
    return true;
}

u8
test_freelist_multiple_allocate_and_free_random
( void )
{
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

    // Verify there was no memory error prior to testing.
    EXPECT_NEQ ( 0 , freelist.memory );

    // Verify entire freelist is free space prior to testing.
    EXPECT_EQ ( size , freelist_query_free ( &freelist ) );

    LOGDEBUG ( "Performing %i random allocate and free operations. . ." , max_op );

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
                    // TEST: freelist_allocate succeeds with random size.
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
                    // TEST: freelist_free succeeds with random block.
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

    LOGDEBUG ( "Freeing remaining allocations. . ." );

    // Free any remaining blocks.
    for ( u64 i = 0; i < alloc_count; ++i )
    {
        if ( allocs[ i ].offset != INVALID_ID )
        {
            if ( !test_freelist_util_free ( &freelist , &allocs[ i ] , &allocated , size) )
            {
                LOGERROR ( "test_freelist_multiple_alloc_and_free_random:  test_freelist_util_free failed on index: %i." , i );
                return false;
            }
        }
    }

    LOGDEBUG ( "  Done." );

    freelist_clear ( &freelist );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    
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