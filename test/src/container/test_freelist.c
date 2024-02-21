/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/test_freelist.c
 * @brief Implementation of the container/test_freelist header.
 * (see container/test_freelist.h for additional details)
 */
#include "container/test_freelist.h"

#include "test/expect.h"

#include "core/memory.h"

// Type definition for a container to hold allocation info.
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
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    freelist_init ( size , &memory_requirement , 0 , 0 );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    freelist_init ( size , &memory_requirement , memory , &freelist );
    EXPECT_NEQ ( 0 , freelist.memory );
    EXPECT_EQ ( size , freelist_query_free ( &freelist ) );
    freelist_clear ( &freelist );
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
    freelist_init ( size , &memory_requirement , 0 , 0 );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    freelist_init ( size , &memory_requirement, memory , &freelist );
    u64 offset = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset ) );
    EXPECT_EQ ( 0 , offset );
    EXPECT_EQ ( size - 64 , freelist_query_free ( &freelist ) );
    EXPECT ( freelist_free ( &freelist , 64 , offset ) );
    EXPECT_EQ ( size , freelist_query_free ( &freelist ) );
    freelist_clear ( &freelist );
    EXPECT_EQ ( 0 , freelist.memory );
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
    freelist_init ( size , &memory_requirement , 0 , 0 );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    freelist_init ( size , &memory_requirement , memory , &freelist );
    u64 offset = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset ) );
    EXPECT_EQ ( 0 , offset );
    u64 offset2 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset2 ) );
    EXPECT_EQ ( 64 , offset2 );
    u64 offset3 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset3 ) );
    EXPECT_EQ ( 128 , offset3 );
    EXPECT_EQ ( size - 192 , freelist_query_free ( &freelist ) );
    EXPECT ( freelist_free ( &freelist , 64 , offset2 ) );
    EXPECT_EQ ( size - 128, freelist_query_free ( &freelist ) );
    u64 offset4 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset4 ) );
    EXPECT_EQ ( offset2 , offset4 );
    EXPECT_EQ ( size - 192 , freelist_query_free ( &freelist ) );
    EXPECT ( freelist_free ( &freelist , 64 , offset ) );
    EXPECT_EQ ( size - 128 , freelist_query_free ( &freelist ) );
    EXPECT ( freelist_free ( &freelist , 64 , offset3 ) );
    EXPECT_EQ ( size - 64 , freelist_query_free ( &freelist ) );
    EXPECT ( freelist_free ( &freelist , 64 , offset4 ) );
    EXPECT_EQ ( size , freelist_query_free ( &freelist ) );
    freelist_clear ( &freelist );
    EXPECT_EQ ( 0, freelist.memory );
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
    freelist_init ( size , &memory_requirement , 0 , 0 );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    freelist_init ( size , &memory_requirement , memory , &freelist );
    u64 offset = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset ) );
    EXPECT_EQ ( 0 , offset );
    u64 offset2 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 32 , &offset2 ) );
    EXPECT_EQ ( 64 , offset2 );
    u64 offset3 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset3 ) );
    EXPECT_EQ ( 96 , offset3 );
    EXPECT_EQ ( size - 160 , freelist_query_free( &freelist ) );
    EXPECT ( freelist_free ( &freelist , 32 , offset2 ) );
    EXPECT_EQ ( size - 128 , freelist_query_free ( &freelist ) );
    u64 offset4 = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 64 , &offset4 ) );
    EXPECT_EQ ( 160 , offset4 );
    EXPECT_EQ ( size - 192 , freelist_query_free ( &freelist ) );
    EXPECT ( freelist_free ( &freelist , 64 , offset ) );
    EXPECT_EQ ( size - 128 , freelist_query_free ( &freelist ) );
    EXPECT ( freelist_free ( &freelist , 64 , offset3 ) );
    EXPECT_EQ ( size - 64 , freelist_query_free ( &freelist ) );
    EXPECT ( freelist_free ( &freelist , 64 , offset4 ) );
    EXPECT_EQ ( size , freelist_query_free ( &freelist ) );
    freelist_clear ( &freelist );
    EXPECT_EQ ( 0 , freelist.memory );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    return true;
}

u8
test_freelist_allocate_to_full_and_fail_to_allocate_more
( void )
{
    freelist_t freelist;
    u64 memory_requirement = 0;
    u64 size = 512;
    freelist_init ( size , &memory_requirement , 0 , 0 );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    freelist_init ( size , &memory_requirement, memory , &freelist );
    u64 offset = INVALID_ID;
    EXPECT ( freelist_allocate ( &freelist , 512 , &offset ) );
    EXPECT_EQ ( 0 , offset );
    EXPECT_EQ ( 0 , freelist_query_free ( &freelist ) );
    u64 offset2 = INVALID_ID;
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    EXPECT_NOT ( freelist_allocate ( &freelist , 64 , &offset2 ) );
    EXPECT_EQ ( 0 , freelist_query_free ( &freelist ) );
    freelist_clear ( &freelist );
    EXPECT_EQ ( 0 , freelist.memory );
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
    freelist_t freelist;
    const u32 alloc_count = 65556;
    alloc_t allocs[ 65556 ] = { 0 };
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
    freelist_init( size  , &memory_requirement , 0 , 0 );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    freelist_init ( size , &memory_requirement , memory , &freelist );
    EXPECT_EQ ( size , freelist_query_free ( &freelist ) );
    u64 allocated = 0;
    u32 op = 0;
    const u32 max_op = 100000;
    u32 alloc = 0;
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
    EXPECT_EQ ( 0 , freelist.memory );
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
    test_register ( test_freelist_allocate_to_full_and_fail_to_allocate_more , "Testing freelist overflow handling." );
    test_register ( test_freelist_multiple_allocate_and_free_random , "Testing freelist with multiple random-sized allocations, each freed in random order." );
}