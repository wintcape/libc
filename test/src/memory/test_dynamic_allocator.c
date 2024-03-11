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

/** @brief Computes current global number of unfreed allocations. */
#define GLOBAL_ALLOCATION_COUNT \
    ( memory_allocation_count () - memory_free_count () )

u8
test_dynamic_allocator_init_and_clear
( void )
{
    u64 global_amount_allocated;
    u64 allocator_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

    dynamic_allocator_t allocator;
    u64 memory_requirement = 0;
    void* memory;

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: dynamic_allocator_init handles invalid arguments.

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 1.1: dynamic_allocator_init logs an error and fails if no output buffer for memory requirement is provided.
    EXPECT_NOT ( dynamic_allocator_init ( 1024 , 0 , 0 , 0 ) );

    // TEST 1.2: dynamic_allocator_init logs an error and fails if provided capacity is invalid.
    EXPECT_NOT ( dynamic_allocator_init ( 0 , &memory_requirement , 0 , 0 ) );

    // TEST 1.3: dynamic_allocator_init logs an error and fails if no output buffer is provided for the allocator.
    memory = memory_allocate ( 1 , MEMORY_TAG_STRING );
    EXPECT_NEQ ( 0 , memory ); // Verify there was no memory error prior to the test.
    EXPECT_NOT ( dynamic_allocator_init ( 1024 , &memory_requirement , memory , 0 ) );
    memory_free ( memory , 1 , MEMORY_TAG_STRING );

    // TEST 2: dynamic_allocator_init.

    // TEST 2.1: dynamic_allocator_init succeeds.
    EXPECT ( dynamic_allocator_init ( 1024 , &memory_requirement , 0 , 0 ) );
    memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT_NEQ ( 0 , memory ); // Verify there was no memory error prior to the test.
    EXPECT ( dynamic_allocator_init ( 1024 , &memory_requirement , memory , &allocator ) );
    
    // TEST 2.2: Dynamic allocator created via dynamic_allocator_init has access to a valid memory buffer.
    EXPECT_NEQ ( 0 , allocator.memory );

    // TEST 2.3: Dynamic allocator created via dynamic_allocator_init is initialized with all of its memory free.
    EXPECT_EQ ( 1024 , dynamic_allocator_query_free ( &allocator ) );

    // TEST 3: dynamic_allocator_clear.

    // TEST 3.1: dynamic_allocator_destroy clears all dynamic allocator data structure fields.
    dynamic_allocator_clear ( &allocator );
    EXPECT_EQ ( 0 , allocator.memory );

    // TEST 4: dynamic_allocator_clear handles invalid arguments.

    // TEST 4.1: dynamic_allocator_clear does not fail if no dynamic allocator is provided.
    dynamic_allocator_clear ( 0 );

    // TEST 4.2: dynamic_allocator_clear does not fail if provided dynamic allocator is not initialized.
    dynamic_allocator_clear ( &allocator );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

    return true;
}

u8
test_dynamic_allocator_allocate_and_free
( void )
{
    u64 global_amount_allocated;
    u64 allocator_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

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
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator.memory );

    // Verify the entire allocator memory buffer is free prior to the test.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );
    
    // TEST 1: dynamic_allocator_allocate handles invalid arguments (1).

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 1.1: dynamic_allocator_allocate logs an error and returns 0 if no allocator is provided.
    blk = dynamic_allocator_allocate ( 0 , 10 );
    EXPECT_EQ ( 0 , blk );

    // TEST 1.2: dynamic_allocator_allocate logs an error and returns 0 if size is invalid.
    blk = dynamic_allocator_allocate ( &allocator , 0 );
    EXPECT_EQ ( 0 , blk );

    // TEST 1.3: dynamic_allocator_allocate does not allocate if size is invalid.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    // TEST 2: dynamic_allocator_allocate.
    
    // TEST 2.1: dynamic_allocator_allocate returns a handle to a valid memory buffer.
    blk = dynamic_allocator_allocate ( &allocator , 10 );
    EXPECT_NEQ ( 0 , blk );

    // TEST 2.2: Allocator has correct amount of free space.
    EXPECT_EQ ( allocator_size - 10 , dynamic_allocator_query_free ( &allocator ) );

    // TEST 3: dynamic_allocator_free handles invalid arguments (1).

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 3.1: dynamic_allocator_free logs an error and fails if no allocator is provided.
    EXPECT_NOT ( dynamic_allocator_free ( 0 , blk ) );

    // TEST 3.2: dynamic_allocator_free logs an error and fails if no memory block is provided.
    EXPECT_NOT ( dynamic_allocator_free ( &allocator , 0 ) );

    // TEST 3.3: dynamic_allocator_free does not allocate if no memory block is provided.
    EXPECT_EQ ( allocator_size - 10 , dynamic_allocator_query_free ( &allocator ) );

    // TEST 4: dynamic_allocator_free.

    // TEST 4.1: dynamic_allocator_free succeeds.
    EXPECT ( dynamic_allocator_free ( &allocator , blk ) );

    // TEST 4.2: Allocator has correct amount of free space.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    // TEST 5: dynamic_allocator_allocate_aligned handles invalid arguments (1).

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 5.1: dynamic_allocator_allocate_aligned logs an error and returns 0 if no allocator is provided.
    blk = dynamic_allocator_allocate_aligned ( 0 , 10 , alignment );
    EXPECT_EQ ( 0 , blk );

    // TEST 5.2: dynamic_allocator_allocate_aligned logs an error and returns 0 if size is invalid.
    blk = dynamic_allocator_allocate_aligned ( &allocator , 0 , alignment );
    EXPECT_EQ ( 0 , blk );

    // TEST 5.3: dynamic_allocator_allocate_aligned does not allocate if size is invalid.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    // TEST 5.4: dynamic_allocator_allocate_aligned logs an error and returns 0 if alignment is invalid.
    blk = dynamic_allocator_allocate_aligned ( &allocator , 10 , 0 );
    EXPECT_EQ ( 0 , blk );

    // TEST 5.5: dynamic_allocator_allocate_aligned does not allocate if alignment is invalid.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    // TEST 6: dynamic_allocator_allocate_aligned.

    // TEST 6.1: dynamic_allocator_allocate_aligned returns a handle to a valid memory buffer.
    blk = dynamic_allocator_allocate_aligned ( &allocator , 10 , alignment );
    EXPECT_NEQ ( 0 , blk );

    // TEST 6.2: Allocator has correct amount of free space.
    EXPECT_EQ ( allocator_size - 10 , dynamic_allocator_query_free ( &allocator ) );

    // TEST 7: dynamic_allocator_free_aligned handles invalid arguments (1).

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 7.1: dynamic_allocator_free_aligned logs an error and fails if no allocator is provided.
    EXPECT_NOT ( dynamic_allocator_free_aligned ( 0 , blk ) );

    // TEST 7.2: dynamic_allocator_free_aligned logs an error and fail if no memory block is provided.
    EXPECT_NOT ( dynamic_allocator_free_aligned ( &allocator , 0 ) );

    // TEST 7.3: dynamic_allocator_free_aligned does not allocate if no memory block is provided.
    EXPECT_EQ ( allocator_size - 10 , dynamic_allocator_query_free ( &allocator ) );

    // TEST 8: dynamic_allocator_free_aligned.

    // TEST 8.1: dynamic_allocator_free_aligned succeeds.
    EXPECT ( dynamic_allocator_free_aligned ( &allocator , blk ) );

    // TEST 8.2: Allocator has correct amount of free space.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    dynamic_allocator_clear ( &allocator );

    // TEST 9: dynamic_allocator_allocate handles invalid arguments (2).

    // TEST 9.1: dynamic_allocator_allocate logs an error and returns 0 if the provided allocator is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    blk = dynamic_allocator_allocate ( &allocator , 10 );
    EXPECT_EQ ( 0 , blk );

    // TEST 10: dynamic_allocator_free handles invalid arguments (2).

    // TEST 10.1: dynamic_allocator_free logs an error and fails if the provided allocator is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    blk = ( ( void* ) -1 );
    EXPECT_NOT ( dynamic_allocator_free ( &allocator , blk ) );

    // TEST 11: dynamic_allocator_allocate_aligned handles invalid arguments (2).

    // TEST 11.1: dynamic_allocator_allocate_aligned logs an error and returns 0 if the provided allocator is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    blk = dynamic_allocator_allocate_aligned ( &allocator , 10 , alignment );
    EXPECT_EQ ( 0 , blk );

    // TEST 12: dynamic_allocator_free_aligned handles invalid arguments (2).

    // TEST 12.1: dynamic_allocator_free_aligned logs an error and fails if the provided allocator is uninitialized.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    blk = ( ( void* ) -1 );
    EXPECT_NOT ( dynamic_allocator_free_aligned ( &allocator , blk ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

    return true;
}

u8
test_dynamic_allocator_single_allocation_all_space
( void )
{
    u64 global_amount_allocated;
    u64 allocator_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

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
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator.memory );

    // Verify the entire allocator memory buffer is free prior to the test.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: dynamic_allocator_allocate returns a handle to a valid output buffer.
    blk = dynamic_allocator_allocate ( &allocator , 1024 );
    EXPECT_NEQ ( 0 , blk );

    // TEST 2: Allocator has correct amount of free space.
    EXPECT_EQ ( 0 , dynamic_allocator_query_free ( &allocator ) );

    // TEST 3: dynamic_allocator_free succeeds.
    EXPECT ( dynamic_allocator_free ( &allocator , blk ) );

    // TEST 4: Allocator has correct amount of free space.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    dynamic_allocator_clear ( &allocator );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

    return true;
}

u8
test_dynamic_allocator_multiple_allocation_all_space
( void )
{
    u64 global_amount_allocated;
    u64 allocator_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

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
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator.memory );

    // Verify that all the allocator memory is currently free prior to the test.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: dynamic_allocator_allocate returns a handle to a valid output buffer.
    void* blk0 = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_NEQ ( 0 , blk0 );

    // TEST 2: Allocator has correct amount of free space.
    EXPECT_EQ ( 768 + 2 * header_size , dynamic_allocator_query_free ( &allocator ) );
    
    // TEST 3: dynamic_allocator_allocate returns a handle to a valid output buffer.
    void* blk1 = dynamic_allocator_allocate ( &allocator , 512 );
    EXPECT_NEQ ( 0 , blk1 );

    // TEST 4: Allocator has correct amount of free space.
    EXPECT_EQ ( 256 + header_size , dynamic_allocator_query_free ( &allocator ) );

    // TEST 5: dynamic_allocator_allocate returns a handle to a valid output buffer.
    void* blk2 = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_NEQ ( 0 , blk2 );

    // TEST 6: Allocator has correct amount of free space.
    EXPECT_EQ ( 0 , dynamic_allocator_query_free ( &allocator ) );

    // TEST 7: dynamic_allocator_free succeeds.
    EXPECT ( dynamic_allocator_free ( &allocator , blk2 ) );

    // TEST 8: Allocator has correct amount of free space.
    EXPECT_EQ ( 256 + header_size , dynamic_allocator_query_free ( &allocator ) );

    // TEST 9: dynamic_allocator_free succeeds.
    EXPECT ( dynamic_allocator_free ( &allocator , blk0 ) );

    // TEST 10: Allocator has correct amount of free space.
    EXPECT_EQ ( 512 + 2 * header_size , dynamic_allocator_query_free ( &allocator ) );

    // TEST 11: dynamic_allocator_free succeeds.
    EXPECT ( dynamic_allocator_free ( &allocator , blk1 ) );

    // TEST 12: Allocator has correct amount of free space.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    dynamic_allocator_clear ( &allocator );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

    return true;
}
u8
test_dynamic_allocator_multiple_requests_too_many
( void )
{
    u64 global_amount_allocated;
    u64 allocator_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

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
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator.memory );

    // Verify that all the allocator memory is currently free prior to the test.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    // Fill allocator prior to the test.
    void* blk0 = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_NEQ ( 0 , blk0 );
    EXPECT_EQ ( 768 + 2 * header_size , dynamic_allocator_query_free ( &allocator ) );
    void* blk1 = dynamic_allocator_allocate ( &allocator , 512 );
    EXPECT_NEQ ( 0 , blk1 );
    EXPECT_EQ ( 256 + header_size , dynamic_allocator_query_free ( &allocator ) );
    void* blk2 = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_NEQ ( 0 , blk2 );
    EXPECT_EQ ( 0 , dynamic_allocator_query_free ( &allocator ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: dynamic_allocator_allocate logs an error and fails when inadequate free space remains within the backing freelist data structure.
    LOGWARN ( "The following warning and error are intentionally triggered by a test:" );
    void* blk_fail = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_EQ ( 0 , blk_fail );

    // TEST 2: dynamic_allocator_allocate has the same amount of free space remaining.
    EXPECT_EQ ( 0 , dynamic_allocator_query_free ( &allocator ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    dynamic_allocator_clear ( &allocator );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

    return true; 
}

u8
test_dynamic_allocator_single_request_too_large
( void )
{
    u64 global_amount_allocated;
    u64 allocator_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

    const u64 allocator_size = 1024;
    const u64 alignment = 1;
    const u64 header_size = dynamic_allocator_header_size () + alignment;
    const u64 total_allocator_size = allocator_size + 3 * header_size;
    dynamic_allocator_t allocator;
    u64 memory_requirement = 0;
    
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , 0 , 0 ) );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , memory , &allocator ) );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator.memory );

    // Verify that all the allocator memory is currently free prior to the test.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    // Fill allocator prior to the test.
    void* blk0 = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_NEQ ( 0 , blk0 );
    EXPECT_EQ ( 768 + 2 * header_size , dynamic_allocator_query_free ( &allocator ) );
    void* blk1 = dynamic_allocator_allocate ( &allocator , 512 );
    EXPECT_NEQ ( 0 , blk1 );
    EXPECT_EQ ( 256 + header_size , dynamic_allocator_query_free ( &allocator ) );
    void* blk2 = dynamic_allocator_allocate ( &allocator , 128 );
    EXPECT_NEQ ( 0 , blk2 );
    EXPECT_EQ ( 128 , dynamic_allocator_query_free ( &allocator ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: dynamic_allocator_allocate logs an error and fails when inadequate free space remains within the backing freelist data structure.
    LOGWARN ( "The following warning and error are intentionally triggered by a test:" );
    void* blk_fail = dynamic_allocator_allocate ( &allocator , 256 );
    EXPECT_EQ ( 0 , blk_fail );

    // TEST 2: dynamic_allocator_allocate has the same amount of free space remaining.
    EXPECT_EQ ( 128 , dynamic_allocator_query_free ( &allocator ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    dynamic_allocator_clear ( &allocator );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

    return true; 
}

u8
test_dynamic_allocator_single_allocation_aligned
( void )
{
    u64 global_amount_allocated;
    u64 allocator_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

    const u64 allocator_size = 1024;
    const u64 alignment = 16;
    const u64 total_allocator_size = allocator_size + dynamic_allocator_header_size () + alignment;
    dynamic_allocator_t allocator;
    u64 memory_requirement = 0;

    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , 0 , 0 ) );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , memory , &allocator ) );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator.memory );

    // Verify that all the allocator memory is currently free prior to the test.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: dynamic_allocator_allocate_aligned returns a handle to a valid output buffer.
    void* blk = dynamic_allocator_allocate_aligned ( &allocator , 1024 , alignment );
    EXPECT_NEQ ( 0 , blk );
    
    u64 blk_size;
    u16 blk_alignment;
    EXPECT ( dynamic_allocator_size_alignment ( blk , &blk_size , &blk_alignment ) );

    // TEST 2: dynamic_allocator_allocate_aligned allocates the block with the correct alignment.
    EXPECT_EQ ( alignment , blk_alignment );

    // TEST 3: dynamic_allocator_allocate_aligned allocates the correct number of bytes.
    EXPECT_EQ ( 1024 , blk_size );

    // TEST 4: Dynamic allocator has correct amount of free space remaining.
    EXPECT_EQ ( 0 , dynamic_allocator_query_free ( &allocator ) );

    // TEST 5: dynamic_allocator_free_aligned succeeds.
    EXPECT ( dynamic_allocator_free_aligned ( &allocator , blk ) );

    // TEST 6: Dynamic allocator has correct amount of free space remaining.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    dynamic_allocator_clear ( &allocator );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

    return true;
}

u8
test_dynamic_allocator_multiple_allocation_aligned_different_alignments
( void )
{
    u64 global_amount_allocated;
    u64 allocator_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;

    const u64 header_size = dynamic_allocator_header_size ();
    const u32 alloc_count = 4;
    dynamic_allocator_t allocator;
    u64 memory_requirement = 0;
    u64 currently_allocated = 0;
    
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
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator.memory );

    // Verify that all the allocator memory is currently free prior to the test.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    {
        u64 i = 0;

        // TEST 1: dynamic_allocator_allocate_aligned returns a handle to a valid output buffer.
        allocs[ i ].block = dynamic_allocator_allocate_aligned ( &allocator , allocs[ i ].size , allocs[ i ].alignment );
        EXPECT_NEQ ( 0 , allocs[ i ].block );

        u64 block_size;
        u16 block_alignment;
        EXPECT ( dynamic_allocator_size_alignment ( allocs[ i ].block , &block_size , &block_alignment ) );
        
        // TEST 2: dynamic_allocator_allocate_aligned allocates the block with the correct alignment.
        EXPECT_EQ ( allocs[ i ].alignment , block_alignment );

        // TEST 3: dynamic_allocator_allocate_aligned allocates the correct number of bytes.
        EXPECT_EQ ( allocs[ i ].size , block_size );

        // TEST 4: Dynamic allocator has correct amount of free space remaining.
        currently_allocated += allocs[ i ].size + header_size + allocs[ i ].alignment;
        EXPECT_EQ ( total_allocator_size - currently_allocated , dynamic_allocator_query_free ( &allocator ) );
    }
    {
        u64 i = 1;

        // TEST 5: dynamic_allocator_allocate_aligned returns a handle to a valid output buffer.
        allocs[ i ].block = dynamic_allocator_allocate_aligned ( &allocator , allocs[ i ].size , allocs[ i ].alignment );
        EXPECT_NEQ ( 0 , allocs[ i ].block );

        u64 block_size;
        u16 block_alignment;
        EXPECT ( dynamic_allocator_size_alignment ( allocs[ i ].block , &block_size , &block_alignment ) );
        
        // TEST 6: dynamic_allocator_allocate_aligned allocates the block with the correct alignment.
        EXPECT_EQ ( allocs[ i ].alignment , block_alignment );

        // TEST 7: dynamic_allocator_allocate_aligned allocates the correct number of bytes.
        EXPECT_EQ ( allocs[ i ].size , block_size );

        // TEST 8: Dynamic allocator has correct amount of free space remaining.
        currently_allocated += allocs[ i ].size + header_size + allocs[ i ].alignment;
        EXPECT_EQ ( total_allocator_size - currently_allocated , dynamic_allocator_query_free ( &allocator ) );
    }
    {
        u64 i = 2;

        // TEST 9: dynamic_allocator_allocate_aligned returns a handle to a valid output buffer.
        allocs[ i ].block = dynamic_allocator_allocate_aligned ( &allocator , allocs[ i ].size , allocs[ i ].alignment );
        EXPECT_NEQ ( 0 , allocs[ i ].block );

        u64 block_size;
        u16 block_alignment;
        EXPECT ( dynamic_allocator_size_alignment ( allocs[ i ].block , &block_size , &block_alignment ) );
        
        // TEST 10: dynamic_allocator_allocate_aligned allocates the block with the correct alignment.
        EXPECT_EQ ( allocs[ i ].alignment , block_alignment );

        // TEST 11: dynamic_allocator_allocate_aligned allocates the correct number of bytes.
        EXPECT_EQ ( allocs[ i ].size , block_size );

        // TEST 12: Dynamic allocator has correct amount of free space remaining.
        currently_allocated += allocs[ i ].size + header_size + allocs[ i ].alignment;
        EXPECT_EQ ( total_allocator_size - currently_allocated , dynamic_allocator_query_free ( &allocator ) );
    }
    {
        u64 i = 3;

        // TEST 13: dynamic_allocator_allocate_aligned returns a handle to a valid output buffer.
        allocs[ i ].block = dynamic_allocator_allocate_aligned ( &allocator , allocs[ i ].size , allocs[ i ].alignment );
        EXPECT_NEQ ( 0 , allocs[ i ].block );

        u64 block_size;
        u16 block_alignment;
        EXPECT ( dynamic_allocator_size_alignment ( allocs[ i ].block , &block_size , &block_alignment ) );
        
        // TEST 14: dynamic_allocator_allocate_aligned allocates the block with the correct alignment.
        EXPECT_EQ ( allocs[ i ].alignment , block_alignment );

        // TEST 15: dynamic_allocator_allocate_aligned allocates the correct number of bytes.
        EXPECT_EQ ( allocs[ i ].size , block_size );

        // TEST 16: Dynamic allocator has correct amount of free space remaining.
        currently_allocated += allocs[ i ].size + header_size + allocs[ i ].alignment;
        EXPECT_EQ ( total_allocator_size - currently_allocated , dynamic_allocator_query_free ( &allocator ) );
    }
    {
        u32 i = 1;

        // TEST 17: dynamic_allocator_free_aligned succeeds.
        EXPECT ( dynamic_allocator_free_aligned ( &allocator , allocs[ i ].block ) );

        allocs[ i ].block = 0; // Ensure block is null prior to continuing the test.

        // TEST 18: Dynamic allocator has correct amount of free space remaining.
        currently_allocated -= allocs[ i ].size + header_size + allocs[ i ].alignment;
        EXPECT_EQ ( total_allocator_size - currently_allocated , dynamic_allocator_query_free ( &allocator ) );
    }
    {
        u32 i = 3;

        // TEST 19: dynamic_allocator_free_aligned succeeds.
        EXPECT ( dynamic_allocator_free_aligned ( &allocator , allocs[ i ].block ) );

        allocs[ i ].block = 0; // Ensure block is null prior to continuing the test.

        // TEST 20: Dynamic allocator has correct amount of free space remaining.
        currently_allocated -= allocs[ i ].size + header_size + allocs[ i ].alignment;
        EXPECT_EQ ( total_allocator_size - currently_allocated , dynamic_allocator_query_free ( &allocator ) );
    }
    {
        u32 i = 2;

        // TEST 21: dynamic_allocator_free_aligned succeeds.
        EXPECT ( dynamic_allocator_free_aligned ( &allocator , allocs[ i ].block ) );

        allocs[ i ].block = 0; // Ensure block is null prior to continuing the test.

        // TEST 22: Dynamic allocator has correct amount of free space remaining.
        currently_allocated -= allocs[ i ].size + header_size + allocs[ i ].alignment;
        EXPECT_EQ ( total_allocator_size - currently_allocated , dynamic_allocator_query_free ( &allocator ) );
    }
    {
        u32 i = 0;

        // TEST 23: dynamic_allocator_free_aligned succeeds.
        EXPECT ( dynamic_allocator_free_aligned ( &allocator , allocs[ i ].block ) );

        allocs[ i ].block = 0; // Ensure block is null prior to continuing the test.

        // TEST 24: Dynamic allocator has correct amount of free space remaining.
        currently_allocated -= allocs[ i ].size + header_size + allocs[ i ].alignment;
        EXPECT_EQ ( total_allocator_size - currently_allocated , dynamic_allocator_query_free ( &allocator ) );
    }

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    dynamic_allocator_clear ( &allocator );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

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
    EXPECT_EQ ( total_allocator_size - *currently_allocated , dynamic_allocator_query_free ( allocator ) );
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
    EXPECT_EQ ( total_allocator_size - *currently_allocated , dynamic_allocator_query_free ( allocator ) );
    return true;
}

u8
test_dynamic_allocator_multiple_allocation_aligned_different_alignments_random
( void )
{
    u64 global_amount_allocated;
    u64 allocator_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;
    
    const u64 header_size = dynamic_allocator_header_size ();
    const u32 alloc_count = 65556;
    dynamic_allocator_t allocator;
    u64 memory_requirement = 0;
    u64 currently_allocated = 0;

    // Generate random size and alignment for each block to be allocated.
    alloc_t allocs[ 65556 ] = { 0 };
    u16 po2[ 8 ] = { 1 , 2 , 4 , 8 , 16 , 32 , 64 , 128 };
    for ( u64 i = 0; i < alloc_count; ++i )
    {
        allocs[ i ].alignment = po2[ random2 ( 0 , 7 ) ];
        allocs[ i ].size = random2 ( 1 , 65536 );
    }

    // Compute expected total allocator size.
    u64 total_allocator_size = 0;
    for ( u64 i = 0; i < alloc_count; ++i )
    {
        total_allocator_size += allocs[ i ].alignment + header_size + allocs[ i ].size;
    }

    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , 0 , 0 ) );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , memory , &allocator                     ) );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator.memory );

    // Verify that all the allocator memory is currently free prior to the test.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // Allocate blocks of random size and alignment until the allocator is full.
    do
    {
        f64 amount;
        const char* unit = string_bytesize ( total_allocator_size , &amount );
        LOGDEBUG ( "Allocating %.2f %s. . ." , &amount , unit ); 
    }
    while ( 0 );
    for ( u32 i = 0; i < alloc_count; ++i )
    {
        // TEST 1: dynamic_allocator_allocate_aligned succeeds with random size and alignment.
        if ( !test_dynamic_allocator_util_allocate ( &allocator , &allocs[ i ] , &currently_allocated , header_size , total_allocator_size ) )
        {
            LOGERROR ( "test_dynamic_allocator_multiple_allocation_aligned_different_alignments_random:  test_dynamic_allocator_util_allocate failed on index: %i." , i );
            return false;
        }
    }
    LOGDEBUG ( "  Done." );

    // TEST 2: Dynamic allocator has correct amount of free space remaining.
    EXPECT_EQ ( 0 , dynamic_allocator_query_free ( &allocator ) );

    // Free blocks in random order until the allocator is empty.
    LOGDEBUG ( "Freeing %i allocator nodes. . ." , alloc_count );
    for ( u32 i = 0; i < alloc_count; ++i )
    {
        // TEST 3: dynamic_allocator_free_aligned succeeds with random size and alignment.
        if ( !test_dynamic_allocator_util_free ( &allocator , &allocs[ i ] , &currently_allocated , header_size , total_allocator_size ) )
        {
            LOGERROR ("test_dynamic_allocator_multiple_allocation_aligned_different_alignments_random:  test_dynamic_allocator_util_free failed on index: %i." , i );
            return false;
        }
    }
    LOGDEBUG ("  Done." );

    // TEST 4: Dynamic allocator has correct amount of free space remaining.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    dynamic_allocator_clear ( &allocator );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

    return true;
}

u8
test_dynamic_allocator_multiple_allocation_and_free_aligned_different_alignments_random
( void )
{
    u64 global_amount_allocated;
    u64 allocator_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count = GLOBAL_ALLOCATION_COUNT;
    
    const u32 max_op = 1000000;
    const u64 header_size = dynamic_allocator_header_size ();
    const u32 alloc_count = 65556;
    dynamic_allocator_t allocator;
    u64 memory_requirement = 0;
    u64 currently_allocated = 0;

    // Generate random size and alignment for each block to be allocated.
    u32 alloc = 0;  
    alloc_t allocs[ 65556 ] = { 0 };
    u16 po2[ 8 ] = { 1 , 2 , 4 , 8 , 16 , 32 , 64 , 128 };
    for ( u64 i = 0; i < alloc_count; ++i )
    {
        allocs[ i ].alignment = po2[ random2 ( 0 , 7 ) ];
        allocs[ i ].size = random2 ( 1 , 65536 );
    }

    // Compute expected total allocator size.
    u64 total_allocator_size = 0;
    for ( u64 i = 0; i < alloc_count; ++i )
    {
        total_allocator_size += allocs[ i ].alignment + header_size + allocs[ i ].size;
    }

    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , 0 , 0 ) );
    void* memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );
    EXPECT ( dynamic_allocator_init ( total_allocator_size , &memory_requirement , memory , &allocator ) );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator.memory );

    // Verify that all the allocator memory is currently free prior to the test.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    u32 op = 0;
    
    // Allocate and free at random, until the maximum number of allowed operations has been reached.
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
                    // TEST 1: dynamic_allocator_allocate_aligned succeeds with random size and alignment.
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
                    // TEST 2: dynamic_allocator_free_aligned succeeds with random size and alignment.
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

    // Free any remaining blocks.
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

    // TEST 3: Dynamic allocator has correct amount of free space remaining.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( &allocator ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    dynamic_allocator_clear ( &allocator );
    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , GLOBAL_ALLOCATION_COUNT );

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
    test_register ( test_dynamic_allocator_multiple_requests_too_many , "Testing dynamic allocator overflow handling (1)." );
    test_register ( test_dynamic_allocator_single_request_too_large , "Testing dynamic allocator overflow handling (2)." );
    test_register ( test_dynamic_allocator_single_allocation_aligned , "Testing dynamic allocator with a single aligned allocation." );
    test_register ( test_dynamic_allocator_multiple_allocation_aligned_different_alignments , "Testing dynamic allocator with multiple aligned allocations, each with different alignments." );
    test_register ( test_dynamic_allocator_multiple_allocation_aligned_different_alignments_random , "Testing dynamic allocator with multiple aligned allocations, each with different alignments, allocated in random order." );
    test_register ( test_dynamic_allocator_multiple_allocation_and_free_aligned_different_alignments_random , "Testing dynamic allocator with multiple aligned allocations, each with different alignments, allocated and freed in random order." );
}