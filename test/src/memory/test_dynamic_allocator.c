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
test_dynamic_allocator_create_and_destroy
( void )
{
    u64 global_amount_allocated;
    u64 allocator_amount_allocated;
    u64 global_allocation_count;

    u64 global_amount_allocated_;
    u64 allocator_amount_allocated_;
    u64 global_allocation_count_;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 capacity = 1024;
    dynamic_allocator_t* allocator;
    u64 memory_requirement = 0;
    void* memory;

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: dynamic_allocator_create handles invalid arguments.

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // TEST 1.1: dynamic_allocator_create fails if no output buffer is provided for the dynamic allocator.
    EXPECT_NOT ( dynamic_allocator_create ( capacity , 0 , 0 , 0 ) );

    // TEST 1.2: dynamic_allocator_create logs an error and fails if provided capacity is invalid.
    EXPECT_NOT ( dynamic_allocator_create ( 0 , 0 , 0 , &allocator ) );

    // TEST 1.3: dynamic_allocator_create did not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 2: dynamic_allocator_create can be used to query the memory requirement for a dynamic allocator.
    
    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;
    
    // TEST 2.1: dynamic_allocator_create succeeds.
    memory_requirement = 0;
    EXPECT ( dynamic_allocator_create ( capacity , &memory_requirement , 0 , 0 ) );

    // TEST 2.2: dynamic_allocator_create writes the memory requirement to the output buffer.
    EXPECT_NEQ ( 0 , memory_requirement );

    // TEST 2.3: dynamic_allocator_create did not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 3: Auto-allocated dynamic allocator.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // Query the memory requirement prior to the test.
    EXPECT ( dynamic_allocator_create ( capacity , &memory_requirement , 0 , 0 ) );

    // TEST 3.1: dynamic_allocator_create succeeds.
    allocator = 0;
    EXPECT ( dynamic_allocator_create ( capacity , 0 , 0 , &allocator ) );

    // Verify no memory error occurred prior to the test.
    EXPECT_NEQ ( 0 , allocator );

    // TEST 3.2: dynamic_allocator_create performed one memory allocation.
    EXPECT_EQ ( global_allocation_count_ + 1 , MEMORY_ALLOCATION_COUNT );

    // TEST 3.3: dynamic_allocator_create allocated the correct number of bytes with the correct memory tag.
    EXPECT_EQ ( global_amount_allocated_ + memory_requirement , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated_ + memory_requirement , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );

    // TEST 3.4: Dynamic allocator created via dynamic_allocator_create owns its own memory.
    EXPECT ( dynamic_allocator_owns_memory ( allocator ) );

    // TEST 3.5: Dynamic allocator created via dynamic_allocator_create has the correct capacity.
    EXPECT_EQ ( capacity , dynamic_allocator_capacity ( allocator ) );

    // TEST 3.6: Dynamic allocator created via dynamic_allocator_create is initialized with all of its memory free.
    EXPECT_EQ ( dynamic_allocator_capacity ( allocator ) , dynamic_allocator_query_free ( allocator ) );

    // TEST 3.7: dynamic_allocator_destroy nullifies the allocator handle.
    dynamic_allocator_destroy ( &allocator );
    EXPECT_EQ ( 0 , allocator );

    // TEST 3.8: dynamic_allocator_destroy restores the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 4: Pre-allocated dynamic allocator.

    // Allocate the correct amount of memory prior to the test.
    EXPECT ( dynamic_allocator_create ( capacity , &memory_requirement , 0 , 0 ) );
    memory = memory_allocate ( memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    // Verify no memory error occurred prior to the test.
    EXPECT_NEQ ( 0 , memory );

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // TEST 4.1: dynamic_allocator_create succeeds.
    allocator = 0;
    EXPECT ( dynamic_allocator_create ( capacity , 0 , memory , &allocator ) );

    // Verify no memory error occurred prior to the test.
    EXPECT_NEQ ( 0 , allocator );

    // TEST 4.2: dynamic_allocator_create does not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 4.3: Dynamic allocator created via dynamic_allocator_create does not own its own memory.
    EXPECT_NOT ( dynamic_allocator_owns_memory ( allocator ) );

    // TEST 4.4: Dynamic allocator created via dynamic_allocator_create has the correct capacity.
    EXPECT_EQ ( capacity , dynamic_allocator_capacity ( allocator ) );

    // TEST 4.5: Dynamic allocator created via dynamic_allocator_create is initialized with all of its memory free.
    EXPECT_EQ ( dynamic_allocator_capacity ( allocator ) , dynamic_allocator_query_free ( allocator ) );

    // TEST 4.7: dynamic_allocator_destroy nullifies the allocator handle.
    dynamic_allocator_destroy ( &allocator );
    EXPECT_EQ ( 0 , allocator );

    // TEST 4.8: dynamic_allocator_destroy does not modify the global allocator state.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    memory_free ( memory , memory_requirement , MEMORY_TAG_DYNAMIC_ALLOCATOR );

    // TEST 5: dynamic_allocator_destroy handles invalid arguments.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    allocator_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_LINEAR_ALLOCATOR );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // TEST 5.1: dynamic_allocator_destroy does not modify the global allocator state if null handle is provided.
    dynamic_allocator_destroy ( 0 );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 5.2: dynamic_allocator_destroy does not modify the global allocator state if provided dynamic_allocator is null.
    allocator = 0;
    dynamic_allocator_destroy ( &allocator );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

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
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 allocator_size = 1024;
    const u16 alignment = 1;
    const u64 total_allocator_size = allocator_size + dynamic_allocator_header_size () + alignment;
    
    dynamic_allocator_t* allocator = 0;
    void* blk;

    EXPECT ( dynamic_allocator_create ( total_allocator_size , 0 , 0 , &allocator ) );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator );

    // Verify the entire allocator memory buffer is free prior to the test.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( allocator ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: dynamic_allocator_allocate.
    
    // TEST 1.1: dynamic_allocator_allocate returns a handle to a valid memory buffer.
    blk = dynamic_allocator_allocate ( allocator , 10 );
    EXPECT_NEQ ( 0 , blk );

    // TEST 1.2: Allocator has correct amount of free space.
    EXPECT_EQ ( allocator_size - 10 , dynamic_allocator_query_free ( allocator ) );

    // TEST 2: dynamic_allocator_free.

    // TEST 2.1: dynamic_allocator_free succeeds.
    EXPECT ( dynamic_allocator_free ( allocator , blk ) );

    // TEST 2.2: Allocator has correct amount of free space.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( allocator ) );

    // TEST 3: dynamic_allocator_allocate_aligned.

    // TEST 3.1: dynamic_allocator_allocate_aligned returns a handle to a valid memory buffer.
    blk = dynamic_allocator_allocate_aligned ( allocator , 10 , alignment );
    EXPECT_NEQ ( 0 , blk );

    // TEST 3.2: Allocator has correct amount of free space.
    EXPECT_EQ ( allocator_size - 10 , dynamic_allocator_query_free ( allocator ) );

    // TEST 4: dynamic_allocator_free_aligned.

    // TEST 4.1: dynamic_allocator_free_aligned succeeds.
    EXPECT ( dynamic_allocator_free_aligned ( allocator , blk ) );

    // TEST 4.2: Allocator has correct amount of free space.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( allocator ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    dynamic_allocator_destroy ( &allocator );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

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
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 allocator_size = 1024;
    const u64 alignment = 1;
    const u64 total_allocator_size = allocator_size + dynamic_allocator_header_size () + alignment;
    
    dynamic_allocator_t* allocator = 0;
    void* blk;

    EXPECT ( dynamic_allocator_create ( total_allocator_size , 0 , 0 , &allocator ) );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator );

    // Verify the entire allocator memory buffer is free prior to the test.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( allocator ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: dynamic_allocator_allocate returns a handle to a valid output buffer.
    blk = dynamic_allocator_allocate ( allocator , 1024 );
    EXPECT_NEQ ( 0 , blk );

    // TEST 2: Allocator has correct amount of free space.
    EXPECT_EQ ( 0 , dynamic_allocator_query_free ( allocator ) );

    // TEST 3: dynamic_allocator_free succeeds.
    EXPECT ( dynamic_allocator_free ( allocator , blk ) );

    // TEST 4: Allocator has correct amount of free space.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( allocator ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    dynamic_allocator_destroy ( &allocator );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

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
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 allocator_size = 1024;
    const u64 alignment = 1;
    const u64 header_size = dynamic_allocator_header_size () + alignment;
    const u64 total_allocator_size = allocator_size + 3 * header_size;

    dynamic_allocator_t* allocator = 0;

    EXPECT ( dynamic_allocator_create ( total_allocator_size , 0 , 0 , &allocator ) );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator );

    // Verify that all the allocator memory is currently free prior to the test.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( allocator ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: dynamic_allocator_allocate returns a handle to a valid output buffer.
    void* blk0 = dynamic_allocator_allocate ( allocator , 256 );
    EXPECT_NEQ ( 0 , blk0 );

    // TEST 2: Allocator has correct amount of free space.
    EXPECT_EQ ( 768 + 2 * header_size , dynamic_allocator_query_free ( allocator ) );
    
    // TEST 3: dynamic_allocator_allocate returns a handle to a valid output buffer.
    void* blk1 = dynamic_allocator_allocate ( allocator , 512 );
    EXPECT_NEQ ( 0 , blk1 );

    // TEST 4: Allocator has correct amount of free space.
    EXPECT_EQ ( 256 + header_size , dynamic_allocator_query_free ( allocator ) );

    // TEST 5: dynamic_allocator_allocate returns a handle to a valid output buffer.
    void* blk2 = dynamic_allocator_allocate ( allocator , 256 );
    EXPECT_NEQ ( 0 , blk2 );

    // TEST 6: Allocator has correct amount of free space.
    EXPECT_EQ ( 0 , dynamic_allocator_query_free ( allocator ) );

    // TEST 7: dynamic_allocator_free succeeds.
    EXPECT ( dynamic_allocator_free ( allocator , blk2 ) );

    // TEST 8: Allocator has correct amount of free space.
    EXPECT_EQ ( 256 + header_size , dynamic_allocator_query_free ( allocator ) );

    // TEST 9: dynamic_allocator_free succeeds.
    EXPECT ( dynamic_allocator_free ( allocator , blk0 ) );

    // TEST 10: Allocator has correct amount of free space.
    EXPECT_EQ ( 512 + 2 * header_size , dynamic_allocator_query_free ( allocator ) );

    // TEST 11: dynamic_allocator_free succeeds.
    EXPECT ( dynamic_allocator_free ( allocator , blk1 ) );

    // TEST 12: Allocator has correct amount of free space.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( allocator ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    dynamic_allocator_destroy ( &allocator );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

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
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 allocator_size = 1024;
    const u64 alignment = 1;
    const u64 header_size = dynamic_allocator_header_size () + alignment;
    const u64 total_allocator_size = allocator_size + 3 * header_size;

    dynamic_allocator_t* allocator = 0;

    EXPECT ( dynamic_allocator_create ( total_allocator_size , 0 , 0 , &allocator ) );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator );

    // Verify that all the allocator memory is currently free prior to the test.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( allocator ) );

    // Fill allocator prior to the test.
    void* blk0 = dynamic_allocator_allocate ( allocator , 256 );
    EXPECT_NEQ ( 0 , blk0 );
    EXPECT_EQ ( 768 + 2 * header_size , dynamic_allocator_query_free ( allocator ) );
    void* blk1 = dynamic_allocator_allocate ( allocator , 512 );
    EXPECT_NEQ ( 0 , blk1 );
    EXPECT_EQ ( 256 + header_size , dynamic_allocator_query_free ( allocator ) );
    void* blk2 = dynamic_allocator_allocate ( allocator , 256 );
    EXPECT_NEQ ( 0 , blk2 );
    EXPECT_EQ ( 0 , dynamic_allocator_query_free ( allocator ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: dynamic_allocator_allocate logs an error and fails when inadequate free space remains within the backing freelist data structure.
    LOGWARN ( "The following warning and error are intentionally triggered by a test:" );
    void* blk_fail = dynamic_allocator_allocate ( allocator , 256 );
    EXPECT_EQ ( 0 , blk_fail );

    // TEST 2: dynamic_allocator_allocate has the same amount of free space remaining.
    EXPECT_EQ ( 0 , dynamic_allocator_query_free ( allocator ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    dynamic_allocator_destroy ( &allocator );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

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
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 allocator_size = 1024;
    const u64 alignment = 1;
    const u64 header_size = dynamic_allocator_header_size () + alignment;
    const u64 total_allocator_size = allocator_size + 3 * header_size;

    dynamic_allocator_t* allocator = 0;
    
    EXPECT ( dynamic_allocator_create ( total_allocator_size , 0 , 0 , &allocator ) );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator );

    // Verify that all the allocator memory is currently free prior to the test.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( allocator ) );

    // Fill allocator prior to the test.
    void* blk0 = dynamic_allocator_allocate ( allocator , 256 );
    EXPECT_NEQ ( 0 , blk0 );
    EXPECT_EQ ( 768 + 2 * header_size , dynamic_allocator_query_free ( allocator ) );
    void* blk1 = dynamic_allocator_allocate ( allocator , 512 );
    EXPECT_NEQ ( 0 , blk1 );
    EXPECT_EQ ( 256 + header_size , dynamic_allocator_query_free ( allocator ) );
    void* blk2 = dynamic_allocator_allocate ( allocator , 128 );
    EXPECT_NEQ ( 0 , blk2 );
    EXPECT_EQ ( 128 , dynamic_allocator_query_free ( allocator ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: dynamic_allocator_allocate logs an error and fails when inadequate free space remains within the backing freelist data structure.
    LOGWARN ( "The following warning and error are intentionally triggered by a test:" );
    void* blk_fail = dynamic_allocator_allocate ( allocator , 256 );
    EXPECT_EQ ( 0 , blk_fail );

    // TEST 2: dynamic_allocator_allocate has the same amount of free space remaining.
    EXPECT_EQ ( 128 , dynamic_allocator_query_free ( allocator ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    dynamic_allocator_destroy ( &allocator );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

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
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 allocator_size = 1024;
    const u64 alignment = 16;
    const u64 total_allocator_size = allocator_size + dynamic_allocator_header_size () + alignment;
    
    dynamic_allocator_t* allocator = 0;
    u64 blk_size;
    u16 blk_alignment;
    void* blk;

    EXPECT ( dynamic_allocator_create ( total_allocator_size , 0 , 0 , &allocator ) );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator );

    // Verify that all the allocator memory is currently free prior to the test.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( allocator ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: dynamic_allocator_allocate_aligned returns a handle to a valid output buffer.
    blk = dynamic_allocator_allocate_aligned ( allocator , 1024 , alignment );
    EXPECT_NEQ ( 0 , blk );
    
    EXPECT ( dynamic_allocator_size_alignment ( blk , &blk_size , &blk_alignment ) );

    // TEST 2: dynamic_allocator_allocate_aligned allocates the block with the correct alignment.
    EXPECT_EQ ( alignment , blk_alignment );

    // TEST 3: dynamic_allocator_allocate_aligned allocates the correct number of bytes.
    EXPECT_EQ ( 1024 , blk_size );

    // TEST 4: Dynamic allocator has correct amount of free space remaining.
    EXPECT_EQ ( 0 , dynamic_allocator_query_free ( allocator ) );

    // TEST 5: dynamic_allocator_free_aligned succeeds.
    EXPECT ( dynamic_allocator_free_aligned ( allocator , blk ) );

    // TEST 6: Dynamic allocator has correct amount of free space remaining.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( allocator ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    dynamic_allocator_destroy ( &allocator );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

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
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 header_size = dynamic_allocator_header_size ();
    const u32 alloc_count = 4;

    dynamic_allocator_t* allocator = 0;
    u64 currently_allocated = 0;
    u64 block_size;
    u16 block_alignment;
    
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

    LOGWARN ( "The following warning is intentionally triggered by a test:" );
    EXPECT ( dynamic_allocator_create ( total_allocator_size , 0 , 0 , &allocator ) );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator );

    // Verify that all the allocator memory is currently free prior to the test.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( allocator ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    {
        u64 i = 0;

        // TEST 1: dynamic_allocator_allocate_aligned returns a handle to a valid output buffer.
        allocs[ i ].block = dynamic_allocator_allocate_aligned ( allocator , allocs[ i ].size , allocs[ i ].alignment );
        EXPECT_NEQ ( 0 , allocs[ i ].block );

        EXPECT ( dynamic_allocator_size_alignment ( allocs[ i ].block , &block_size , &block_alignment ) );
        
        // TEST 2: dynamic_allocator_allocate_aligned allocates the block with the correct alignment.
        EXPECT_EQ ( allocs[ i ].alignment , block_alignment );

        // TEST 3: dynamic_allocator_allocate_aligned allocates the correct number of bytes.
        EXPECT_EQ ( allocs[ i ].size , block_size );

        // TEST 4: Dynamic allocator has correct amount of free space remaining.
        currently_allocated += allocs[ i ].size + header_size + allocs[ i ].alignment;
        EXPECT_EQ ( total_allocator_size - currently_allocated , dynamic_allocator_query_free ( allocator ) );
    }
    {
        u64 i = 1;

        // TEST 5: dynamic_allocator_allocate_aligned returns a handle to a valid output buffer.
        allocs[ i ].block = dynamic_allocator_allocate_aligned ( allocator , allocs[ i ].size , allocs[ i ].alignment );
        EXPECT_NEQ ( 0 , allocs[ i ].block );

        EXPECT ( dynamic_allocator_size_alignment ( allocs[ i ].block , &block_size , &block_alignment ) );
        
        // TEST 6: dynamic_allocator_allocate_aligned allocates the block with the correct alignment.
        EXPECT_EQ ( allocs[ i ].alignment , block_alignment );

        // TEST 7: dynamic_allocator_allocate_aligned allocates the correct number of bytes.
        EXPECT_EQ ( allocs[ i ].size , block_size );

        // TEST 8: Dynamic allocator has correct amount of free space remaining.
        currently_allocated += allocs[ i ].size + header_size + allocs[ i ].alignment;
        EXPECT_EQ ( total_allocator_size - currently_allocated , dynamic_allocator_query_free ( allocator ) );
    }
    {
        u64 i = 2;

        // TEST 9: dynamic_allocator_allocate_aligned returns a handle to a valid output buffer.
        allocs[ i ].block = dynamic_allocator_allocate_aligned ( allocator , allocs[ i ].size , allocs[ i ].alignment );
        EXPECT_NEQ ( 0 , allocs[ i ].block );


        EXPECT ( dynamic_allocator_size_alignment ( allocs[ i ].block , &block_size , &block_alignment ) );
        
        // TEST 10: dynamic_allocator_allocate_aligned allocates the block with the correct alignment.
        EXPECT_EQ ( allocs[ i ].alignment , block_alignment );

        // TEST 11: dynamic_allocator_allocate_aligned allocates the correct number of bytes.
        EXPECT_EQ ( allocs[ i ].size , block_size );

        // TEST 12: Dynamic allocator has correct amount of free space remaining.
        currently_allocated += allocs[ i ].size + header_size + allocs[ i ].alignment;
        EXPECT_EQ ( total_allocator_size - currently_allocated , dynamic_allocator_query_free ( allocator ) );
    }
    {
        u64 i = 3;

        // TEST 13: dynamic_allocator_allocate_aligned returns a handle to a valid output buffer.
        allocs[ i ].block = dynamic_allocator_allocate_aligned ( allocator , allocs[ i ].size , allocs[ i ].alignment );
        EXPECT_NEQ ( 0 , allocs[ i ].block );

        EXPECT ( dynamic_allocator_size_alignment ( allocs[ i ].block , &block_size , &block_alignment ) );
        
        // TEST 14: dynamic_allocator_allocate_aligned allocates the block with the correct alignment.
        EXPECT_EQ ( allocs[ i ].alignment , block_alignment );

        // TEST 15: dynamic_allocator_allocate_aligned allocates the correct number of bytes.
        EXPECT_EQ ( allocs[ i ].size , block_size );

        // TEST 16: Dynamic allocator has correct amount of free space remaining.
        currently_allocated += allocs[ i ].size + header_size + allocs[ i ].alignment;
        EXPECT_EQ ( total_allocator_size - currently_allocated , dynamic_allocator_query_free ( allocator ) );
    }
    {
        u32 i = 1;

        // TEST 17: dynamic_allocator_free_aligned succeeds.
        EXPECT ( dynamic_allocator_free_aligned ( allocator , allocs[ i ].block ) );

        allocs[ i ].block = 0; // Ensure block is null prior to continuing the test.

        // TEST 18: Dynamic allocator has correct amount of free space remaining.
        currently_allocated -= allocs[ i ].size + header_size + allocs[ i ].alignment;
        EXPECT_EQ ( total_allocator_size - currently_allocated , dynamic_allocator_query_free ( allocator ) );
    }
    {
        u32 i = 3;

        // TEST 19: dynamic_allocator_free_aligned succeeds.
        EXPECT ( dynamic_allocator_free_aligned ( allocator , allocs[ i ].block ) );

        allocs[ i ].block = 0; // Ensure block is null prior to continuing the test.

        // TEST 20: Dynamic allocator has correct amount of free space remaining.
        currently_allocated -= allocs[ i ].size + header_size + allocs[ i ].alignment;
        EXPECT_EQ ( total_allocator_size - currently_allocated , dynamic_allocator_query_free ( allocator ) );
    }
    {
        u32 i = 2;

        // TEST 21: dynamic_allocator_free_aligned succeeds.
        EXPECT ( dynamic_allocator_free_aligned ( allocator , allocs[ i ].block ) );

        allocs[ i ].block = 0; // Ensure block is null prior to continuing the test.

        // TEST 22: Dynamic allocator has correct amount of free space remaining.
        currently_allocated -= allocs[ i ].size + header_size + allocs[ i ].alignment;
        EXPECT_EQ ( total_allocator_size - currently_allocated , dynamic_allocator_query_free ( allocator ) );
    }
    {
        u32 i = 0;

        // TEST 23: dynamic_allocator_free_aligned succeeds.
        EXPECT ( dynamic_allocator_free_aligned ( allocator , allocs[ i ].block ) );

        allocs[ i ].block = 0; // Ensure block is null prior to continuing the test.

        // TEST 24: Dynamic allocator has correct amount of free space remaining.
        currently_allocated -= allocs[ i ].size + header_size + allocs[ i ].alignment;
        EXPECT_EQ ( total_allocator_size - currently_allocated , dynamic_allocator_query_free ( allocator ) );
    }

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    dynamic_allocator_destroy ( &allocator );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

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
    if ( !( *data ).block )
    {
        LOGERROR( "test_dynamic_allocator_util_allocate:  dynamic_allocator_allocate_aligned failed (1)." );
        return false;
    }
    u64 block_size;
    u16 block_alignment;
    if ( !dynamic_allocator_size_alignment ( ( *data ).block , &block_size , &block_alignment ) )
    {
        LOGERROR( "test_dynamic_allocator_util_allocate:  dynamic_allocator_allocate_aligned failed (2)." );
        return false;
    }
    if ( ( *data ).alignment != block_alignment )
    {
        LOGERROR( "test_dynamic_allocator_util_allocate:  dynamic_allocator_allocate_aligned failed (3)." );
        return false;
    }
    if ( ( *data ).size != block_size )
    {
        LOGERROR( "test_dynamic_allocator_util_allocate:  dynamic_allocator_allocate_aligned failed (4)." );
        return false;
    }
    *currently_allocated += ( *data ).size + header_size + ( *data ).alignment;
    if ( ( total_allocator_size - *currently_allocated ) != dynamic_allocator_query_free ( allocator ) )
    {
        LOGERROR( "test_dynamic_allocator_util_allocate:  dynamic_allocator_allocate_aligned failed (5)." );
        return false;
    }
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
        LOGERROR( "test_dynamic_allocator_util_free:  dynamic_allocator_free_aligned failed (1)." );
        return false;
    }
    ( *data ).block = 0;
    *currently_allocated -= ( *data ).size + header_size + ( *data ).alignment;
    if ( ( total_allocator_size - *currently_allocated ) != dynamic_allocator_query_free ( allocator ) )
    {
        LOGERROR( "test_dynamic_allocator_util_free:  dynamic_allocator_free_aligned failed (2)." );
        return false;
    }
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
    global_allocation_count = MEMORY_ALLOCATION_COUNT;
    
    const u64 header_size = dynamic_allocator_header_size ();
    const u32 alloc_count = 65556;

    dynamic_allocator_t* allocator = 0;
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

    EXPECT ( dynamic_allocator_create ( total_allocator_size , 0 , 0 , &allocator                     ) );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator );

    // Verify that all the allocator memory is currently free prior to the test.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( allocator ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // Allocate blocks of random size and alignment until the allocator is full.
    for ( u32 i = 0; i < alloc_count; ++i )
    {
        // TEST 1: dynamic_allocator_allocate_aligned succeeds with random size and alignment.
        if ( !test_dynamic_allocator_util_allocate ( allocator , &allocs[ i ] , &currently_allocated , header_size , total_allocator_size ) )
        {
            LOGERROR ( "test_dynamic_allocator_multiple_allocation_aligned_different_alignments_random:  test_dynamic_allocator_util_allocate failed on index: %i." , i );
            return false;
        }
    }

    // TEST 2: Dynamic allocator has correct amount of free space remaining.
    EXPECT_EQ ( 0 , dynamic_allocator_query_free ( allocator ) );

    // Free blocks in random order until the allocator is empty.
    for ( u32 i = 0; i < alloc_count; ++i )
    {
        // TEST 3: dynamic_allocator_free_aligned succeeds with random size and alignment.
        if ( !test_dynamic_allocator_util_free ( allocator , &allocs[ i ] , &currently_allocated , header_size , total_allocator_size ) )
        {
            LOGERROR ("test_dynamic_allocator_multiple_allocation_aligned_different_alignments_random:  test_dynamic_allocator_util_free failed on index: %i." , i );
            return false;
        }
    }

    // TEST 4: Dynamic allocator has correct amount of free space remaining.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( allocator ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    dynamic_allocator_destroy ( &allocator );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

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
    global_allocation_count = MEMORY_ALLOCATION_COUNT;
    
    const u32 max_op = 1000000;
    const u64 header_size = dynamic_allocator_header_size ();
    const u32 alloc_count = 65556;

    dynamic_allocator_t* allocator = 0;
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

    EXPECT ( dynamic_allocator_create ( total_allocator_size , 0 , 0 , &allocator ) );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , allocator );

    // Verify that all the allocator memory is currently free prior to the test.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( allocator ) );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    u32 op = 0;
    
    // Allocate and free at random, until the maximum number of allowed operations has been reached.
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
                    if ( !test_dynamic_allocator_util_allocate ( allocator , &allocs[ i ] , &currently_allocated , header_size , total_allocator_size ) )
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
                    if ( !test_dynamic_allocator_util_free ( allocator , &allocs[ i ] , &currently_allocated , header_size , total_allocator_size ) )
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

    // Free any remaining blocks.
    for ( u64 i = 0; i < alloc_count; ++i )
    {
        if ( allocs[ i ].block )
        {
            if ( !test_dynamic_allocator_util_free ( allocator , &allocs[ i ] , &currently_allocated , header_size , total_allocator_size ) )
            {
                LOGERROR ( "test_dynamic_allocator_multiple_allocation_and_free_aligned_different_alignments_random:  test_dynamic_allocator_util_free failed on index: %i." , i );
                return false;
            }
        }
    }

    // TEST 3: Dynamic allocator has correct amount of free space remaining.
    EXPECT_EQ ( total_allocator_size , dynamic_allocator_query_free ( allocator ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    dynamic_allocator_destroy ( &allocator );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( allocator_amount_allocated , memory_amount_allocated ( MEMORY_TAG_DYNAMIC_ALLOCATOR ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}
void
test_register_dynamic_allocator
( void )
{
    test_register ( test_dynamic_allocator_create_and_destroy , "Creating or destroying a dynamic allocator." ); 
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