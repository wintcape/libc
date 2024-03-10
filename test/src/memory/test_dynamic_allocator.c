/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file memory/test_dynamic_allocator.c

 * @brief Implementation of the memory/test_dynamic_allocator header.
 * (see memory/test_dynamic_allocator.h for additional details)
 */
#include "memory/test_dynamic_allocator.h"

#include "test/expect.h"

#include "core/memory.h"
#include "core/string.h"

// Type definition for a container to hold allocation info.
typedef struct
{
    void*   block;
    u16     alignment;
    u64     size;
}
alloc_t;

u8
test_dynamic_allocator_init_and_clear
( void )
{
    dynamic_allocator_t allocator;
    u64 memory_requirement = 0;
    void* memory;

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: dynamic_allocator_init logs an error and fails if no output buffer for memory requirement is provided.
    EXPECT_NOT ( dynamic_allocator_init ( 1024 , 0 , 0 , 0 ) );

    // TEST: dynamic_allocator_init logs an error and fails if provided capacity is invalid.
    EXPECT_NOT ( dynamic_allocator_init ( 0 , &memory_requirement , 0 , 0 ) );

    // TEST: dynamic_allocator_init logs an error and fails if no output buffer is provided for the allocator.
    memory = memory_allocate ( 1 , MEMORY_TAG_STRING );
    EXPECT_NEQ ( 0 , memory ); // Verify there was no memory error prior to testing.
    EXPECT_NOT ( dynamic_allocator_init ( 1024 , &memory_requirement , memory , 0 ) );
    memory_free ( memory , 1 , MEMORY_TAG_STRING );

    // TEST: dynamic_allocator_init succeeds with valid arguments.
    EXPECT ( dynamic_allocator_init ( 1024 , &memory_requirement , 0 , 0 ) );
    memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT_NEQ ( 0 , memory ); // Verify there was no memory error prior to testing.
    EXPECT ( dynamic_allocator_init ( 1024 , &memory_requirement , memory , &allocator ) );
    
    // TEST: Dynamic allocator created via dynamic_allocator_init has access to a valid memory buffer.
    EXPECT_NEQ ( 0 , allocator.memory );

    // TEST: Linear allocator created via linear_allocator_create is initialized with all of its memory free.
    EXPECT_EQ ( 1024 , dynamic_allocator_query_free ( &allocator ) );

    // TEST: linear_allocator_destroy clears all linear allocator data structure fields.
    dynamic_allocator_clear ( &allocator );
    EXPECT_EQ ( 0 , allocator.memory );

    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    return true;
}

u8
test_dynamic_allocator_allocate_and_free
( void )
{
    const u64 allocator_size = 1024;
    const u16 alignment = 1;
    const u64 total_allocator_size = allocator_size + dynamic_allocator_header_size () + alignment;
    dynamic_allocator_t allocator;
    u64 memory_requirement = 0;
    void* memory;
    void* blk;

    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , 0 , 0 ) );
    memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , memory , &allocator ) );
    
    // Verify there was no memory error prior to testing.
    EXPECT_NEQ ( 0 , allocator.memory );

    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );
    
    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: dynamic_allocator_allocate logs an error and returns 0 if no allocator is provided.
    blk = dynamic_allocator_allocate ( 0 , 10 );
    EXPECT_EQ ( 0 , blk );

    // TEST: dynamic_allocator_allocate logs an error and returns 0 if size is invalid.
    blk = dynamic_allocator_allocate ( &allocator , 0 );
    EXPECT_EQ ( 0 , blk );

    // TEST: dynamic_allocator_allocate does not allocate if size is invalid.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );
    
    // TEST: dynamic_allocator_allocate returns a handle to a valid memory buffer when given valid arguments.
    blk = dynamic_allocator_allocate ( &allocator , 10 );
    EXPECT_NEQ ( 0 , blk );

    // TEST: Allocator has correct amount of free space following dynamic_allocator_allocate.
    EXPECT_EQ ( allocator_size - 10 , dynamic_allocator_query_free ( &allocator ) );

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: dynamic_allocator_free logs an error and fails if no allocator is provided.
    EXPECT_NOT ( dynamic_allocator_free ( 0 , blk ) );

    // TEST: dynamic_allocator_free logs an error and fail if no memory block is provided.
    EXPECT_NOT ( dynamic_allocator_free ( &allocator , 0 ) );

    // TEST: dynamic_allocator_free does not allocate if no memory block is provided.
    EXPECT_EQ ( allocator_size - 10 , dynamic_allocator_query_free ( &allocator ) );

    // TEST: dynamic_allocator_free succeeds given valid arguments.
    EXPECT ( dynamic_allocator_free ( &allocator , blk ) );

    // TEST: Allocator has correct amount of free space following dynamic_allocator_free.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: dynamic_allocator_allocate_aligned logs an error and returns 0 if no allocator is provided.
    blk = dynamic_allocator_allocate_aligned ( 0 , 10 , alignment );
    EXPECT_EQ ( 0 , blk );

    // TEST: dynamic_allocator_allocate_aligned logs an error and returns 0 if size is invalid.
    blk = dynamic_allocator_allocate_aligned ( &allocator , 0 , alignment );
    EXPECT_EQ ( 0 , blk );

    // TEST: dynamic_allocator_allocate_aligned does not allocate if size is invalid.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    // TEST: dynamic_allocator_allocate_aligned logs an error and returns 0 if alignment is invalid.
    blk = dynamic_allocator_allocate_aligned ( &allocator , 10 , 0 );
    EXPECT_EQ ( 0 , blk );

    // TEST: dynamic_allocator_allocate_aligned does not allocate if alignment is invalid.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    // TEST: dynamic_allocator_allocate_aligned returns a handle to a valid memory buffer when given valid arguments.
    blk = dynamic_allocator_allocate_aligned ( &allocator , 10 , alignment );
    EXPECT_NEQ ( 0 , blk );

    // TEST: Allocator has correct amount of free space following dynamic_allocator_allocate_aligned.
    EXPECT_EQ ( allocator_size - 10 , dynamic_allocator_query_free ( &allocator ) );

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: dynamic_allocator_free logs an error and fails if no allocator is provided.
    EXPECT_NOT ( dynamic_allocator_free_aligned ( 0 , blk ) );

    // TEST: dynamic_allocator_free logs an error and fail if no memory block is provided.
    EXPECT_NOT ( dynamic_allocator_free_aligned ( &allocator , 0 ) );

    // TEST: dynamic_allocator_free does not allocate if no memory block is provided.
    EXPECT_EQ ( allocator_size - 10 , dynamic_allocator_query_free ( &allocator ) );

    // TEST: dynamic_allocator_free succeeds given valid arguments.
    EXPECT ( dynamic_allocator_free_aligned ( &allocator , blk ) );

    // TEST: Allocator has correct amount of free space following dynamic_allocator_free_aligned.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    dynamic_allocator_clear ( &allocator );

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: dynamic_allocator_allocate logs an error and returns 0 if the provided allocator is uninitialized.
    blk = dynamic_allocator_allocate ( &allocator , 10 );
    EXPECT_EQ ( 0 , blk );

    // TEST: dynamic_allocator_free logs an error and fails if the provided allocator is uninitialized.
    blk = ( ( void* ) -1 );
    EXPECT_NOT ( dynamic_allocator_free ( &allocator , blk ) );

    // TEST: dynamic_allocator_allocate_aligned logs an error and returns 0 if the provided allocator is uninitialized.
    blk = dynamic_allocator_allocate_aligned ( &allocator , 10 , alignment );
    EXPECT_EQ ( 0 , blk );

    // TEST: dynamic_allocator_free_aligned logs an error and fails if the provided allocator is uninitialized.
    blk = ( ( void* ) -1 );
    EXPECT_NOT ( dynamic_allocator_free_aligned ( &allocator , blk ) );

    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    return true;
}

u8
test_dynamic_allocator_single_allocation_all_space
( void )
{
    const u64 allocator_size = 1024;
    const u64 alignment = 1;
    const u64 total_allocator_size = allocator_size + dynamic_allocator_header_size () + alignment;
    dynamic_allocator_t allocator;
    u64 memory_requirement = 0;
    void* memory;
    void* blk;

    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , 0 , 0 ) );
    memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , memory , &allocator ) );
    
    // Verify there was no memory error prior to testing.
    EXPECT_NEQ ( 0 , allocator.memory );

    // Verify that all the allocator memory is currently free prior to testing.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    // TEST: dynamic_allocator_allocate returns a handle to a valid output buffer given valid arguments.
    blk = dynamic_allocator_allocate ( &allocator , 1024 );
    EXPECT_NEQ ( 0 , blk );

    // TEST: Allocator has correct amount of free space following dynamic_allocator_allocate.
    EXPECT_EQ ( 0 , dynamic_allocator_query_free ( &allocator ) );

    // TEST: dynamic_allocator_free succeeds given valid arguments.
    dynamic_allocator_free ( &allocator , blk );

    // TEST: Allocator has correct amount of free space following dynamic_allocator_free.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    dynamic_allocator_clear ( &allocator );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    return true;
}

u8
test_dynamic_allocator_multiple_allocation_all_space
( void )
{
    const u64 allocator_size = 1024;
    const u64 alignment = 1;
    const u64 header_size = dynamic_allocator_header_size () + alignment;
    const u64 total_allocator_size = allocator_size + 3 * header_size;
    dynamic_allocator_t allocator;
    u64 memory_requirement = 0;
    void* memory;

    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , 0 , 0 ) );
    memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , memory , &allocator ) );
    
    // Verify there was no memory error prior to testing.
    EXPECT_NEQ ( 0 , allocator.memory );

    // Verify that all the allocator memory is currently free prior to testing.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    // TEST: dynamic_allocator_allocate returns a handle to a valid output buffer given valid arguments.
    void* blk0 = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_NEQ ( 0 , blk0 );

    // TEST: Allocator has correct amount of free space following dynamic_allocator_allocate.
    EXPECT_EQ ( 768 + 2 * header_size , dynamic_allocator_query_free ( &allocator ) );
    
    // TEST: dynamic_allocator_allocate returns a handle to a valid output buffer given valid arguments.
    void* blk1 = dynamic_allocator_allocate ( &allocator , 512 );
    EXPECT_NEQ ( 0 , blk1 );

    // TEST: Allocator has correct amount of free space following dynamic_allocator_allocate.
    EXPECT_EQ ( 256 + header_size , dynamic_allocator_query_free ( &allocator ) );

    // TEST: dynamic_allocator_allocate returns a handle to a valid output buffer given valid arguments.
    void* blk2 = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_NEQ ( 0 , blk2 );

    // TEST: Allocator has correct amount of free space following dynamic_allocator_allocate.
    EXPECT_EQ ( 0 , dynamic_allocator_query_free ( &allocator ) );

    // TEST: dynamic_allocator_free succeeds given valid arguments.
    dynamic_allocator_free ( &allocator , blk2 );

    // TEST: Allocator has correct amount of free space following dynamic_allocator_free.
    EXPECT_EQ ( 256 + header_size , dynamic_allocator_query_free ( &allocator ) );

    // TEST: dynamic_allocator_free succeeds given valid arguments.
    dynamic_allocator_free ( &allocator , blk0 );

    // TEST: Allocator has correct amount of free space following dynamic_allocator_free.
    EXPECT_EQ ( 512 + 2 * header_size , dynamic_allocator_query_free ( &allocator ) );

    // TEST: dynamic_allocator_free succeeds given valid arguments.
    dynamic_allocator_free ( &allocator , blk1 );

    // TEST: Allocator has correct amount of free space following dynamic_allocator_free.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    dynamic_allocator_clear ( &allocator );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    return true;
}
u8
test_dynamic_allocator_multiple_allocation_over_allocate
( void )
{
    const u64 allocator_size = 1024;
    const u64 alignment = 1;
    const u64 header_size = dynamic_allocator_header_size () + alignment;
    const u64 total_allocator_size = allocator_size + 3 * header_size;
    dynamic_allocator_t allocator;
    u64 memory_requirement = 0;
    void* memory;

    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , 0 , 0 ) );
    memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , memory , &allocator ) );
    
    // Verify there was no memory error prior to testing.
    EXPECT_NEQ ( 0 , allocator.memory );

    // Verify that all the allocator memory is currently free prior to testing.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    // TEST: dynamic_allocator_allocate returns a handle to a valid output buffer given valid arguments.
    void* blk0 = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_NEQ ( 0 , blk0 );

    // TEST: Allocator has correct amount of free space following dynamic_allocator_allocate.
    EXPECT_EQ ( 768 + 2 * header_size , dynamic_allocator_query_free ( &allocator ) );

    // TEST: dynamic_allocator_allocate returns a handle to a valid output buffer given valid arguments.
    void* blk1 = dynamic_allocator_allocate ( &allocator , 512 );
    EXPECT_NEQ ( 0 , blk1 );

    // TEST: Allocator has correct amount of free space following dynamic_allocator_allocate.
    EXPECT_EQ ( 256 + header_size , dynamic_allocator_query_free ( &allocator ) );

    // TEST: dynamic_allocator_allocate returns a handle to a valid output buffer given valid arguments.
    void* blk2 = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_NEQ ( 0 , blk2 );

    // TEST: Allocator has correct amount of free space following dynamic_allocator_allocate.
    EXPECT_EQ ( 0 , dynamic_allocator_query_free ( &allocator ) );

    // TEST: dynamic_allocator_allocate logs an error and fails when inadequate free space remains within the backing freelist data structure.
    LOGWARN ( "The following warning and error are intentionally triggered by a test:" );
    void* blk_fail = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_EQ ( 0 , blk_fail );

    // TEST: dynamic_allocator_allocate has the same amount of free space remaining after failing when inadequate free space remains within the backing freelist data structure.
    EXPECT_EQ ( 0 , dynamic_allocator_query_free ( &allocator ) );

    dynamic_allocator_clear ( &allocator );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    return true; 
}

u8
test_dynamic_allocator_multiple_allocation_most_space_request_too_big
( void )
{
    dynamic_allocator_t allocator;
    u64 memory_requirement = 0;
    const u64 allocator_size = 1024;
    const u64 alignment = 1;
    u64 header_size = dynamic_allocator_header_size () + alignment;
    const u64 total_allocator_size = allocator_size + 3 * header_size;
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , 0 , 0 ) );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , memory , &allocator ) );
    EXPECT_NEQ ( 0 , allocator.memory );
    u64 free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( total_allocator_size , free );
    void* blk0 = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_NEQ ( 0 , blk0 );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 768 + 2 * header_size , free );
    void* blk1 = dynamic_allocator_allocate ( &allocator , 512 );
    EXPECT_NEQ ( 0 , blk1 );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 256 + header_size , free );
    void* blk2 = dynamic_allocator_allocate ( &allocator , 128 );
    EXPECT_NEQ ( 0 , blk2 );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 128 , free );
    LOGWARN ( "The following warning and error are intentionally triggered by a test:" );
    void* blk_fail = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_EQ ( 0 , blk_fail );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 128 , free );
    dynamic_allocator_clear ( &allocator );
    EXPECT_EQ ( 0 , allocator.memory );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    return true; 
}
u8
test_dynamic_allocator_single_allocation_aligned
( void )
{
    dynamic_allocator_t allocator;
    u64 memory_requirement = 0;
    const u64 allocator_size = 1024;
    const u64 alignment = 16;
    const u64 total_allocator_size = allocator_size + dynamic_allocator_header_size () + alignment;
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , 0 , 0 ) );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , memory , &allocator ) );
    EXPECT_NEQ ( 0 , allocator.memory );
    u64 free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( total_allocator_size , free );
    void* blk = dynamic_allocator_allocate_aligned ( &allocator , 1024 , alignment );
    EXPECT_NEQ ( 0 , blk );
    u64 blk_size;
    u16 blk_alignment;
    EXPECT ( dynamic_allocator_size_alignment ( blk , &blk_size , &blk_alignment ) );
    EXPECT_EQ ( alignment , blk_alignment );
    EXPECT_EQ ( 1024 , blk_size );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( 0 , free );
    dynamic_allocator_free_aligned ( &allocator , blk );
    free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( total_allocator_size , free );
    dynamic_allocator_clear ( &allocator );
    EXPECT_EQ ( 0 , allocator.memory );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    return true;
}
u8
test_dynamic_allocator_multiple_allocation_aligned_different_alignments
( void )
{
    dynamic_allocator_t allocator;
    u64 memory_requirement = 0;
    u64 currently_allocated = 0;
    const u64 header_size = dynamic_allocator_header_size ();
    const u32 alloc_count = 4;
    alloc_t allocs[ 4 ];
    allocs[ 0 ] = ( alloc_t ){ 0 ,  1 , 31 };    // 1-byte alignment.
    allocs[ 1 ] = ( alloc_t ){ 0 , 16 , 82 };    // 16-byte alignment.
    allocs[ 2 ] = ( alloc_t ){ 0 ,  1 , 59 };    // 1-byte alignment.
    allocs[ 3 ] = ( alloc_t ){ 0 ,  8 , 73 };    // 1-byte alignment.
    u64 total_allocator_size = 0;
    for ( u64 i = 0; i < alloc_count; ++i )
    {
        total_allocator_size += allocs[ i ].alignment + header_size + allocs[ i ].size;
    }
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , 0 , 0 ) );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , memory , &allocator ) );
    EXPECT_NEQ ( 0 , allocator.memory );
    u64 free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( total_allocator_size , free );
    {
        u64 i = 0;
        allocs[ i ].block = dynamic_allocator_allocate_aligned ( &allocator , allocs[ i ].size , allocs[ i ].alignment );
        EXPECT_NEQ ( 0 , allocs[ i ].block );
        u64 block_size;
        u16 block_alignment;
        EXPECT ( dynamic_allocator_size_alignment ( allocs[ i ].block , &block_size , &block_alignment ) );
        EXPECT_EQ ( allocs[ i ].alignment , block_alignment );
        EXPECT_EQ ( allocs[ i ].size , block_size );
        currently_allocated += allocs[ i ].size + header_size + allocs[ i ].alignment;
        free = dynamic_allocator_query_free ( &allocator );
        EXPECT_EQ ( total_allocator_size - currently_allocated , free );
    }
    {
        u32 i = 1;
        allocs[ i ].block = dynamic_allocator_allocate_aligned ( &allocator , allocs[ i ].size , allocs[ i ].alignment );
        EXPECT_NEQ ( 0 , allocs[ i ].block );
        u64 block_size;
        u16 block_alignment;
        EXPECT ( dynamic_allocator_size_alignment ( allocs[ i ].block , &block_size , &block_alignment ) );
        EXPECT_EQ ( allocs[ i ].alignment , block_alignment );
        EXPECT_EQ ( allocs[ i ].size , block_size );
        currently_allocated += allocs[ i ].size + header_size + allocs[ i ].alignment;
        free = dynamic_allocator_query_free ( &allocator );
        EXPECT_EQ ( total_allocator_size - currently_allocated , free );
    }
    {
        u32 i = 2;
        allocs[ i ].block = dynamic_allocator_allocate_aligned ( &allocator , allocs[ i ].size , allocs[ i ].alignment );
        EXPECT_NEQ ( 0 , allocs[ i ].block );
        u64 block_size;
        u16 block_alignment;
        EXPECT ( dynamic_allocator_size_alignment ( allocs[ i ].block , &block_size , &block_alignment ) );
        EXPECT_EQ ( allocs[ i ].alignment , block_alignment );
        EXPECT_EQ ( allocs[ i ].size , block_size );
        currently_allocated += allocs[ i ].size + header_size + allocs[ i ].alignment;
        free = dynamic_allocator_query_free ( &allocator );
        EXPECT_EQ ( total_allocator_size - currently_allocated , free );
    }
    {
        u32 i = 3;
        allocs[ i ].block = dynamic_allocator_allocate_aligned ( &allocator , allocs[ i ].size , allocs[ i ].alignment );
        EXPECT_NEQ ( 0 , allocs[ i ].block );
        u64 block_size;
        u16 block_alignment;
        EXPECT ( dynamic_allocator_size_alignment ( allocs[ i ].block , &block_size , &block_alignment ) );
        EXPECT_EQ ( allocs[ i ].alignment , block_alignment );
        EXPECT_EQ ( allocs[ i ].size , block_size );
        currently_allocated += allocs[ i ].size + header_size + allocs[ i ].alignment;
        free = dynamic_allocator_query_free ( &allocator );
        EXPECT_EQ ( total_allocator_size - currently_allocated , free );
    }
    {
        u32 i = 1;
        dynamic_allocator_free_aligned ( &allocator , allocs[ i ].block );
        allocs[ i ].block = 0;
        currently_allocated -= allocs[ i ].size + header_size + allocs[ i ].alignment;
        free = dynamic_allocator_query_free ( &allocator );
        EXPECT_EQ ( total_allocator_size - currently_allocated , free );
    }
    {
        u32 i = 3;
        dynamic_allocator_free_aligned ( &allocator , allocs[ i ].block );
        allocs[ i ].block = 0;
        currently_allocated -= allocs[ i ].size + header_size + allocs[ i ].alignment;
        free = dynamic_allocator_query_free ( &allocator );
        EXPECT_EQ ( total_allocator_size - currently_allocated , free );
    }
    {
        u32 i = 2;
        dynamic_allocator_free_aligned ( &allocator , allocs[ i ].block );
        allocs[ i ].block = 0;
        currently_allocated -= allocs[ i ].size + header_size + allocs[ i ].alignment;
        free = dynamic_allocator_query_free ( &allocator );
        EXPECT_EQ ( total_allocator_size - currently_allocated , free );
    }
    {
        u64 i = 0;
        dynamic_allocator_free_aligned ( &allocator , allocs[ i ].block );
        allocs[ i ].block = 0;
        currently_allocated -= allocs[ i ].size + header_size + allocs[ i ].alignment;
        free = dynamic_allocator_query_free ( &allocator );
        EXPECT_EQ ( total_allocator_size - currently_allocated , free );
    }
    dynamic_allocator_clear ( &allocator );
    EXPECT_EQ ( 0 , allocator.memory );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    return true;
}
u8
test_dynamic_allocator_util_allocate
(   dynamic_allocator_t*    allocator
,   alloc_t*                data
,   u64*                    currently_allocated
,   u64                     header_size
,   u64                     total_allocator_size
)
{
    ( *data ).block = dynamic_allocator_allocate_aligned ( allocator , ( *data ).size , ( *data ).alignment );
    EXPECT_NEQ ( 0 , ( *data ).block );
    u64 block_size;
    u16 block_alignment;
    EXPECT ( dynamic_allocator_size_alignment ( ( *data ).block , &block_size , &block_alignment ) );
    EXPECT_EQ ( ( *data ).alignment , block_alignment );
    EXPECT_EQ ( ( *data ).size , block_size );
    *currently_allocated += ( *data ).size + header_size + ( *data ).alignment;
    u64 free = dynamic_allocator_query_free ( allocator );
    EXPECT_EQ ( total_allocator_size - *currently_allocated , free );
    return true;
}

u8
test_dynamic_allocator_util_free
(   dynamic_allocator_t*    allocator
,   alloc_t*                data
,   u64*                    currently_allocated
,   u64                     header_size
,   u64                     total_allocator_size
)
{
    if ( !dynamic_allocator_free_aligned ( allocator , ( *data ).block ) )
    {
        LOGERROR( "test_dynamic_allocator_util_free:  dynamic_allocator_free_aligned failed." );
        return false;
    }
    ( *data ).block = 0;
    *currently_allocated -= ( *data ).size + header_size + ( *data ).alignment;
    u64 free = dynamic_allocator_query_free ( allocator );
    EXPECT_EQ ( total_allocator_size - *currently_allocated , free );
    return true;
}
u8
test_dynamic_allocator_multiple_allocation_aligned_different_alignments_random
( void )
{
    dynamic_allocator_t allocator;
    u64 memory_requirement = 0;
    u64 currently_allocated = 0;
    const u64 header_size = dynamic_allocator_header_size ();
    const u32 alloc_count = 65556;
    u32 alloc;    
    alloc_t allocs[ 65556 ] = { 0 };
    u16 po2[ 8 ] = { 1 , 2 , 4 , 8 , 16 , 32 , 64 , 128 };
    for ( u64 i = 0; i < alloc_count; ++i )
    {
        allocs[ i ].alignment = po2[ random2 ( 0 , 7 ) ];
        allocs[ i ].size = random2 ( 1 , 65536 );
    }
    u64 total_allocator_size = 0;
    for ( u64 i = 0; i < alloc_count; ++i )
    {
        total_allocator_size += allocs[ i ].alignment + header_size + allocs[ i ].size;
    }
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , 0 , 0 ) );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , memory , &allocator                     ) );
    EXPECT_NEQ ( 0 , allocator.memory );
    u64 free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( total_allocator_size , free );
    f64 amount;
    const char* unit = string_bytesize ( total_allocator_size , &amount );
    LOGDEBUG ( "Allocating %.2f %s. . ." , &amount , unit ); 
    for ( alloc = 0; alloc != alloc_count; ++alloc )
    {
        u32 i = random2 ( 0 , alloc_count - 1 );
        if ( !allocs[ i ].block )
        {
            if ( !test_dynamic_allocator_util_allocate ( &allocator , &allocs[ i ] , &currently_allocated , header_size , total_allocator_size ) )
            {
                LOGERROR ( "test_dynamic_allocator_multiple_allocation_aligned_different_alignments_random:  test_dynamic_allocator_util_allocate failed on index: %i." , i );
                return false;
            }
        }
    }
    LOGDEBUG ( "  Done." );    
    LOGDEBUG ( "Freeing %i allocator nodes. . ."
 , alloc
             );
    for ( alloc = 0; alloc != alloc_count; ++alloc )
    {
        u32 i = random2 ( 0 , alloc_count - 1 );
        if ( allocs[ i ].block )
        {
            if ( !test_dynamic_allocator_util_free ( &allocator , &allocs[i] , &currently_allocated , header_size , total_allocator_size ) )
            {
                LOGERROR ("test_dynamic_allocator_multiple_allocation_aligned_different_alignments_random:  test_dynamic_allocator_util_free failed on index: %i." , i );
                return false;
            }
        }
    }
    LOGDEBUG ("  Done." );    
    dynamic_allocator_clear ( &allocator );
    EXPECT_EQ ( 0 , allocator.memory );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    return true;
}
u8
test_dynamic_allocator_multiple_allocation_and_free_aligned_different_alignments_random
( void )
{
    dynamic_allocator_t allocator;
    u64 memory_requirement = 0;
    u64 currently_allocated = 0;
    const u64 header_size = dynamic_allocator_header_size ();
    const u32 alloc_count = 65556;
    alloc_t allocs[ 65556 ] = { 0 };
    u16 po2[ 8 ] = { 1 , 2 , 4 , 8 , 16 , 32 , 64 , 128 };
    for ( u64 i = 0; i < alloc_count; ++i )
    {
        allocs[ i ].alignment = po2[ random2 ( 0 , 7 ) ];
        allocs[ i ].size = random2 ( 1 , 65536 );
    }
    u64 total_allocator_size = 0;
    for ( u64 i = 0; i < alloc_count; ++i )
    {
        total_allocator_size += allocs[ i ].alignment + header_size + allocs[ i ].size;
    }
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , 0 , 0 ) );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , memory , &allocator ) );
    EXPECT_NEQ ( 0 , allocator.memory );
    u64 free = dynamic_allocator_query_free ( &allocator );
    EXPECT_EQ ( total_allocator_size , free );
    u32 op = 0;
    const u32 max_op = 1000000;
    u32 alloc = 0;
    LOGDEBUG ( "Performing %i random allocate and free operations. . ." , max_op );
    while ( op < max_op )
    {
        if ( !alloc || random2 ( 0 , 1 ) )
        {
            for (;;)
            {
                u32 i = random2 ( 0 , alloc_count - 1 );
                if ( !allocs[ i ].block )
                {
                    if ( !test_dynamic_allocator_util_allocate ( &allocator , &allocs[ i ] , &currently_allocated , header_size , total_allocator_size ) )
                    {
                        LOGERROR ( "test_dynamic_allocator_multiple_allocation_and_free_aligned_different_alignments_random:  test_dynamic_allocator_util_allocate failed on index: %i." , i);
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
                if ( allocs[ i ].block )
                {
                    if ( !test_dynamic_allocator_util_free ( &allocator , &allocs[ i ] , &currently_allocated , header_size , total_allocator_size ) )
                    {
                        LOGERROR ( "test_dynamic_allocator_multiple_allocation_and_free_aligned_different_alignments_random:  test_dynamic_allocator_util_free failed on index: %i." , i );
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
        if ( allocs[ i ].block )
        {
            if ( !test_dynamic_allocator_util_free ( &allocator , &allocs[ i ] , &currently_allocated , header_size , total_allocator_size ) )
            {
                LOGERROR ( "test_dynamic_allocator_multiple_allocation_and_free_aligned_different_alignments_random:  test_dynamic_allocator_util_free failed on index: %i." , i );
                return false;
            }
        }
    }
    LOGDEBUG ( "  Done." );
    dynamic_allocator_clear ( &allocator );
    EXPECT_EQ ( 0 , allocator.memory );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    return true;
}
void
test_register_dynamic_allocator
( void )
{
    test_register ( test_dynamic_allocator_init_and_clear , "Initializing or clearing a dynamic allocator." ); 
    test_register ( test_dynamic_allocator_allocate_and_free , "Allocating or freeing memory managed by a dynamic allocator." );
    test_register ( test_dynamic_allocator_single_allocation_all_space , "Testing dynamic allocator with a single allocation." );
    test_register ( test_dynamic_allocator_multiple_allocation_all_space , "Testing dynamic allocator with multiple allocations." );
    test_register ( test_dynamic_allocator_multiple_allocation_over_allocate , "Testing dynamic allocator overflow handling (1)." );
    test_register ( test_dynamic_allocator_multiple_allocation_most_space_request_too_big , "Testing dynamic allocator overflow handling (2)." );
    test_register ( test_dynamic_allocator_single_allocation_aligned , "Testing dynamic allocator with a single aligned allocation." );
    test_register ( test_dynamic_allocator_multiple_allocation_aligned_different_alignments , "Testing dynamic allocator with multiple aligned allocations, each with different alignments." );
    test_register ( test_dynamic_allocator_multiple_allocation_aligned_different_alignments_random , "Testing dynamic allocator with multiple aligned allocations, each with different alignments, allocated in random order." );
    test_register ( test_dynamic_allocator_multiple_allocation_and_free_aligned_different_alignments_random , "Testing dynamic allocator with multiple aligned allocations, each with different alignments, allocated and freed in random order." );
}