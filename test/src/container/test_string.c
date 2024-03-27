/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/test_string.c
 * @brief Implementation of the container/test_string header.
 * (see container/test_string.h for additional details)
 */
#include "container/test_string.h"

#include "test/expect.h"

#include "core/memory.h"

u8
test_string_allocate_and_free
( void )
{
    u64 global_amount_allocated;
    u64 string_amount_allocated;
    u64 global_allocation_count;

    u64 global_amount_allocated_;
    u64 string_amount_allocated_;
    u64 global_allocation_count_;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    string_amount_allocated = memory_amount_allocated ( MEMORY_TAG_STRING );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const char* hello = "Hello world!";

    ////////////////////////////////////////////////////////////////////////////
    // Start test.
    
    // TEST 1: string_allocate_from.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    string_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_STRING );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    char* string = string_allocate_from ( hello );

    // TEST 1.1: string_allocate_from performed one memory allocation.
    EXPECT_EQ ( global_allocation_count_ + 1 , MEMORY_ALLOCATION_COUNT );

    // TEST 1.2: string_allocate_from allocated the correct number of bytes with the correct memory tag (length of string + terminator + u64 (used internally to store string length to free)).
    EXPECT_EQ ( global_amount_allocated_ + _string_length ( hello ) + sizeof ( char ) + sizeof ( u64 ) , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( string_amount_allocated_ + _string_length ( hello ) + sizeof ( char ) + sizeof ( u64 ) , memory_amount_allocated ( MEMORY_TAG_STRING ) );

    // TEST 1.3: String created via string_allocate_from has identical length to the string it was created from.
    EXPECT_EQ ( _string_length ( hello ) , _string_length ( string ) );

    // TEST 1.4: String created via string_allocate_from has identical characters to the string it was created from.
    EXPECT ( memory_equal ( string , hello , _string_length ( string ) ) );

    // TEST 2: string_free.

    // TEST 2.1: string_free restores the global allocator state.
    string_free ( string );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( string_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_STRING ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 2.2: string_free does not modify the global allocator state if no string is provided.
    string_destroy ( 0 );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( string_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_STRING ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( string_amount_allocated , memory_amount_allocated ( MEMORY_TAG_STRING ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_string_create_and_destroy
( void )
{
    u64 global_amount_allocated;
    u64 array_amount_allocated;
    u64 global_allocation_count;

    u64 global_amount_allocated_;
    u64 array_amount_allocated_;
    u64 global_allocation_count_;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    array_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ARRAY );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const char* hello = "Hello world!";

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: string_create.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    array_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ARRAY );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    char* string = string_create ();

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , string );

    // TEST 1.1: string_create performed one memory allocation.
    EXPECT_EQ ( global_allocation_count_ + 1 , MEMORY_ALLOCATION_COUNT );

    // TEST 1.2: string_create allocated the correct number of bytes with the correct memory tag (array is used internally to represent a resizable string).
    EXPECT_EQ ( global_amount_allocated_ + array_size ( string ) , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated_ + array_size ( string ) , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );

    // TEST 1.3: String created via string_create has 0 length.
    EXPECT_EQ ( 0 , string_length ( string ) );

    // TEST 1.4: String created via string_create has a null terminator.
    EXPECT_EQ ( 0 , *string );

    // TEST 1.5: string_destroy restores the global allocator state.
    string_destroy ( string );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 2: string_create_from.

    string = string_create_from ( hello );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , string );

    // TEST 2.1: string_create_from performed one memory allocation.
    EXPECT_EQ ( global_allocation_count_ + 1 , MEMORY_ALLOCATION_COUNT );

    // TEST 2.2: string_create_from allocated the correct number of bytes with the correct memory tag (array is used internally to represent a resizable string).
    EXPECT_EQ ( array_amount_allocated_ + array_size ( string ) , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );

    // TEST 2.3: String created via string_create_from has identical length to the string it was created from.
    EXPECT_EQ ( _string_length ( hello ) , string_length ( string ) );
    
    // TEST 2.4: String created via string_create_from has identical characters to the string it was created from.
    EXPECT ( memory_equal ( string , hello , string_length ( string ) + 1 ) );

    // TEST 3: string_copy.

    char* copy = string_copy ( string );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , copy );

    // TEST 3.1: string_copy performed one memory allocation.
    EXPECT_EQ ( global_allocation_count + 1 + 1 , MEMORY_ALLOCATION_COUNT );

    // TEST 3.2: string_copy allocated the correct number of bytes with the correct memory tag (array is used internally to represent a resizable string).
    EXPECT_EQ ( array_amount_allocated + array_size ( string ) + array_size ( copy ) , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    
    // TEST 3.3: String created via string_copy has identical length to the string it was created from.
    EXPECT_EQ ( string_length ( copy ) , string_length ( string ) );
    
    // TEST 3.4: String created via string_copy has identical characters to the string it was created from.
    EXPECT ( memory_equal ( string , copy , string_length ( string ) + 1 ) );

    // TEST 3.5: string_destroy restores the global allocator state.
    string_destroy ( copy );
    string_destroy ( string );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 4: string_create handles invalid argument.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    array_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ARRAY );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // TEST 4.1: string_create logs an error and fails if provided capacity is invalid.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    EXPECT_EQ ( 0 , _string_create ( 0 ) );

    // TEST 4.2: string_create does not allocate memory on failure.
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // TEST 5: string_destroy handles invalid argument.

    // Copy the current global allocator state prior to the test.
    global_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ALL );
    array_amount_allocated_ = memory_amount_allocated ( MEMORY_TAG_ARRAY );
    global_allocation_count_ = MEMORY_ALLOCATION_COUNT;

    // TEST 5.1: string_destroy does not modify the global allocator state if no string is provided.
    string_destroy ( 0 );
    EXPECT_EQ ( global_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated_ , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( global_allocation_count_ , MEMORY_ALLOCATION_COUNT );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_string_push
( void )
{
    u64 global_amount_allocated;
    u64 array_amount_allocated;
    u64 string_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    array_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ARRAY );
    string_amount_allocated = memory_amount_allocated ( MEMORY_TAG_STRING );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const char* to_push = "push";
    const u64 op_count = 100000;

    char* string = string_create ();
    char* old_string = string_allocate ( op_count * _string_length ( to_push ) + 1 );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , string );
    EXPECT_NEQ ( 0 , old_string );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    for ( u64 i = 0; i < op_count; ++i )
    {
        // Copy string state prior to performing the operation being tested.
        const u64 old_length = string_length ( string );
        memory_copy ( old_string , string , old_length );

        _string_push ( string , to_push );

        // Verify there was no memory error prior to the test.
        EXPECT_NEQ ( 0 , string );

        // TEST 1: string_push increases the length of the string by the length of the string being appended.
        EXPECT_EQ ( old_length , string_length ( string ) - _string_length ( to_push ) );
        
        // TEST 2: string_push appends the correct characters to the end of the string.
        EXPECT ( memory_equal ( to_push , &string[ old_length ] , _string_length ( to_push ) ) );

        // TEST 3: string_push leaves the remainder of the string unmodified.
        EXPECT ( memory_equal ( string , old_string , old_length ) );
    }
    
    // End test.
    ////////////////////////////////////////////////////////////////////////////

    string_free ( old_string );
    string_destroy ( string );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( string_amount_allocated , memory_amount_allocated ( MEMORY_TAG_STRING ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_string_insert_and_remove
( void )
{
    u64 global_amount_allocated;
    u64 array_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    array_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ARRAY );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const char* to_insert[] = { "He" , "llo " , "world" , "!" };
    const char* insert1     =          "llo "                  ;
    const char* insert2     =   "He"   "llo "                  ;
    const char* insert3     =   "He"   "llo "             "!"  ;
    const char* full        =   "He"   "llo "   "world"   "!"  ;
    const char* remove1     =   "He"   "llo "             "!"  ;
    const char* remove2     =   "He"                      "!"  ;
    const char* remove3     =                             "!"  ;
    char* string1 = string_create ();
    char* string2 = string_create ();
    char* string;

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , string1 );
    EXPECT_NEQ ( 0 , string2 );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: string_insert handles invalid arguments.

    // TEST 1.1: string_insert logs an error if the index is invalid.
    LOGWARN ( "The following error is intentionally triggered by a test:" );
    string = string1;
    _string_insert ( string1 , string_length ( string1 ) + 1 , to_insert[ 0 ] );

    // TEST 1.2: string_insert does not perform memory allocation if the index is invalid.
    EXPECT_EQ ( string , string1 );

    // TEST 1.3: string_insert does not modify string length if the index is invalid.
    EXPECT_EQ ( 0 , string_length ( string1 ) );

    // TEST 2: string_insert.

    _string_insert ( string1 , 0 , to_insert[ 1 ] );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , string1 );

    // TEST 2.1: string_insert increases the length of the string by the length of the string being inserted.
    EXPECT_EQ ( _string_length ( insert1 ) , string_length ( string1 ) );

    // TEST 2.2: string_insert inserts the correct number of characters to the string at the correct index, leaving the remainder of the string unmodified.
    EXPECT ( memory_equal ( string1 , insert1 , string_length ( string1 ) ) );
    
    _string_insert ( string1 , 0 , to_insert[ 0 ] );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , string1 );

    // TEST 2.3: string_insert increases the length of the string by the length of the string being inserted.
    EXPECT_EQ ( _string_length ( insert2 ) , string_length ( string1 ) );

    // TEST 2.4: string_insert inserts the correct number of characters to the string at the correct index, leaving the remainder of the string unmodified.
    EXPECT ( memory_equal ( string1 , insert2 , string_length ( string1 ) ) );

    _string_insert ( string1 , string_length ( string1 ) , to_insert[ 3 ] );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , string1 );

    // TEST 2.5: string_insert increases the length of the string by the length of the string being inserted.
    EXPECT_EQ ( _string_length ( insert3 ) , string_length ( string1 ) );

    // TEST 2.6: string_insert inserts the correct number of characters to the string at the correct index, leaving the remainder of the string unmodified.
    EXPECT ( memory_equal ( string1 , insert3 , string_length ( string1 ) ) );

    _string_insert ( string1 , _string_length ( to_insert[ 0 ] ) + _string_length ( to_insert[ 1 ] ) , to_insert[ 2 ] );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , string1 );

   // TEST 2.7: string_insert increases the length of the string by the length of the string being inserted.
    EXPECT_EQ ( _string_length ( full ) , string_length ( string1 ) );

    // TEST 2.8: string_insert inserts the correct number of characters to the string at the correct index, leaving the remainder of the string unmodified.
    EXPECT ( memory_equal ( string1 , full , string_length ( string1 ) ) );

    // TEST 2.9: string_insert yields the same results as string_push when inserting at the end of the array.
    _string_push ( string2 , to_insert[ 0 ] );
    _string_push ( string2 , to_insert[ 1 ] );
    _string_push ( string2 , to_insert[ 2 ] );
    _string_push ( string2 , to_insert[ 3 ] );
    EXPECT_EQ ( string_length ( string2 ) , string_length ( string1 ) );
    EXPECT ( memory_equal ( string1 , string2 , string_length ( string1 ) + 1 ) );

    // TEST 3: string_remove handles invalid arguments.

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 3.1: string_remove logs an error if the index is invalid.
    string = string1;
    string_remove ( string1 , string_length ( string1 ) , 1 );

    // TEST 3.2: string_remove does not perform memory allocation if the index is invalid* (current implementation doesn't allocate in general either, but I don't need to test for that).
    EXPECT_EQ ( string , string1 );

    // TEST 3.3: string_remove does not modify string length if the index is invalid.
    EXPECT_EQ ( string_length ( string2 ) , string_length ( string1 ) );

    // TEST 3.4: string_remove does not modify string characters if the index is invalid.
    EXPECT ( memory_equal ( string1 , string2 , string_length ( string1 ) ) );

    // TEST 3.5: string_remove logs an error if the index is invalid.
    string = string1;
    string_remove ( string1 , string_length ( string1 ) + 1 , 0 );

    // TEST 3.6: string_remove does not perform memory allocation if the index is invalid* (current implementation doesn't allocate in general either, but I don't need to test for that).
    EXPECT_EQ ( string , string1 );

    // TEST 3.7: string_remove does not modify string length if the index is invalid.
    EXPECT_EQ ( string_length ( string2 ) , string_length ( string1 ) );

    // TEST 3.8: string_remove does not modify string characters if the index is invalid.
    EXPECT ( memory_equal ( string1 , string2 , string_length ( string1 ) ) );

    // TEST 3.9: string_remove logs an error if the index is invalid.
    string = string1;
    string_remove ( string1 , string_length ( string1 ) - 5 , 6 );

    // TEST 3.10: string_remove does not perform memory allocation if the index is invalid* (current implementation doesn't allocate in general either, but I don't need to test for that).
    EXPECT_EQ ( string , string1 );

    // TEST 3.11: string_remove does not modify string length if the index is invalid.
    EXPECT_EQ ( string_length ( string2 ) , string_length ( string1 ) );

    // TEST 3.12: string_remove does not modify string characters if the index is invalid.
    EXPECT ( memory_equal ( string1 , string2 , string_length ( string1 ) ) );

    // TEST 3.13: string_remove does not fail when count is 0.
    string = string1;
    string_remove ( string1 , 0 , 0 );

    // TEST 3.14: string_remove does not perform memory allocation if the count is 0* (current implementation doesn't allocate in general either, but I don't need to test for that).
    EXPECT_EQ ( string , string1 );

    // TEST 3.15: string_remove does not modify string length if the count is 0.
    EXPECT_EQ ( string_length ( string2 ) , string_length ( string1 ) );

    // TEST 3.16: string_remove does not modify string characters if the count is 0.
    EXPECT ( memory_equal ( string1 , string2 , string_length ( string1 ) ) );

    // TEST 4: string_remove.

    string_remove ( string1 , _string_length ( to_insert[ 0 ] ) + _string_length ( to_insert[ 1 ] ) , _string_length ( to_insert[ 2 ] ) );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , string1 );

    // TEST 4.1: string_remove decreases the length of the string by the number of characters being removed.
    EXPECT_EQ ( _string_length ( remove1 )  , string_length ( string1 ) );
    
    // TEST 4.2: string_remove removes the correct number of characters from the string at the correct index, leaving the rest of the string unmodified.
    EXPECT ( memory_equal ( string1 , remove1 , string_length ( string1 ) ) );

    string_remove ( string1 , _string_length ( to_insert[ 0 ] ) , _string_length ( to_insert[ 1 ] ) );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , string1 );

    // TEST 4.3: string_remove decreases the length of the string by the number of characters being removed.
    EXPECT_EQ ( _string_length ( remove2 ) , string_length ( string1 ) );
    
    // TEST 4.4: string_remove removes the correct number of characters from the string at the correct index, leaving the rest of the string unmodified.
    EXPECT ( memory_equal ( string1 , remove2 , string_length ( string1 ) ) );
    
    string_remove ( string1 , 0 , _string_length ( to_insert[ 0 ] ) );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , string1 );

    // TEST 4.5: string_remove decreases the length of the string by the number of characters being removed.
    EXPECT_EQ ( _string_length ( remove3 )  , string_length ( string1 )  );
    
    // TEST 4.6: string_remove removes the correct number of characters from the string at the correct index, leaving the rest of the string unmodified.
    EXPECT ( memory_equal ( string1 , remove3 , string_length ( string1 ) ) );
    
    string_remove ( string1 , 0 , string_length ( string1 ) );
    
    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , string1 );

    // TEST 4.7: string_remove decreases the length of the string by the number of characters being removed.
    EXPECT_EQ ( 0 , string_length ( string1 ) );

    // TEST 4.8: string_remove removes the correct number of characters from the string at the correct index, leaving the rest of the string unmodified.
    EXPECT_EQ ( 0 , *string1 );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    string_destroy ( string1 );
    string_destroy ( string2 );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_string_insert_and_remove_random
( void )
{
    u64 global_amount_allocated;
    u64 array_amount_allocated;
    u64 string_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    array_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ARRAY );
    string_amount_allocated = memory_amount_allocated ( MEMORY_TAG_STRING );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 op_count = 100000;

    char* string = string_create ();
    char* old_string = string_allocate ( op_count + 1 );

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , string );
    EXPECT_NEQ ( 0 , old_string );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    for ( u64 i = 0; i < op_count; ++i )
    {
        // Copy the string state prior to performing the operation being tested.
        const u64 old_length = string_length ( string );
        memory_copy ( old_string , string , old_length );

        // Insert a random non-zero character at a random index within the string.
        const char to_insert[] = { random2 ( 1 , 255 ) , 0 };
        const u64 insert_index = random2 ( 0 , old_length );
        _string_insert ( string , insert_index , to_insert );

        // Verify there was no memory error prior to the test.
        EXPECT_NEQ ( 0 , string );

        // TEST 1: string_insert increases the length of the string by the length of the string being inserted.
        EXPECT_EQ ( old_length + 1 , string_length ( string ) );

        // TEST 2: string_insert inserts the correct character into the string at the correct index.
        EXPECT_EQ ( *to_insert , string[ insert_index ] );

        // TEST 3: string_insert leaves the remainder of the string unmodified.
        if ( !insert_index )
        {
            EXPECT ( memory_equal ( &string[ 1 ] , &old_string[ 0 ] , old_length ) );
        }
        else if ( insert_index == old_length )
        {
            EXPECT ( memory_equal ( &string[ 0 ] , &old_string[ 0 ] , old_length ) );
        }
        else
        {
            EXPECT ( memory_equal ( &string[ 0 ] , &old_string[ 0 ] , ( insert_index - 1 ) ) );
            EXPECT ( memory_equal ( &string[ insert_index + 1 ] , &old_string[ insert_index ] , ( old_length - insert_index ) ) );
        }
    }

    while ( string_length ( string ) )
    {
        // Copy the string state prior to performing the operation being tested.
        const u64 old_length = string_length ( string );
        memory_copy ( old_string , string , old_length );

        // Remove a random number of characters from a random index within the string.
        const u64 remove_cap = ( old_length > 1000 ) ? old_length / 100 : old_length;
        const u64 remove_count = random2 ( 1 , remove_cap );
        const u64 remove_index = random2 ( 0 , old_length - remove_cap );
        string_remove ( string , remove_index , remove_count );

        // Verify there was no memory error prior to the test.
        EXPECT_NEQ ( 0 , string );

        // TEST 4: string_insert decreases the length of the string by the number of characters being removed.
        EXPECT_EQ ( old_length - remove_count , string_length ( string ) );

        // TEST 5: string_remove removes the correct number of characters from the string at the correct index, leaving the rest of the string unmodified.
        if ( !remove_index )
        {
            EXPECT ( memory_equal ( &string[ 0 ] , &old_string[ remove_count ] , string_length ( string ) ) );
        }
        else if ( remove_index == old_length - 1 )
        {
            EXPECT ( memory_equal ( &string[ 0 ] , &old_string[ 0 ] , string_length ( string ) ) );
        }
        else
        {
            EXPECT ( memory_equal ( &string[ 0 ] , &old_string[ 0 ] , remove_index - 1 ) );
            EXPECT ( memory_equal ( &string[ remove_index ] , &old_string[ remove_index + remove_count ] , string_length ( string ) - remove_index ) );
        }
    }

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    string_free ( old_string );
    string_destroy ( string );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( string_amount_allocated , memory_amount_allocated ( MEMORY_TAG_STRING ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_string_trim
( void )
{
    u64 global_amount_allocated;
    u64 array_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    array_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ARRAY );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    char* string = string_create ();
    const char* empty = "";
    const char* only_whitespace = "       \t\n\r        ";
    const char* leading_whitespace = "\n\t\t\t  <-- Trim this off -->";
    const char* trailing_whitespace = "<-- Trim this off -->  \t\t\t\n";
    const char* leading_and_trailing_whitespace = "\n\t\t\t  <-- Trim this off -->  \t\t\t\n";
    const char* trimmed = "<-- Trim this off -->";

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , string );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: string_trim does not fail on empty string.
    _string_push ( string , empty );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    string_trim ( string );
    EXPECT ( memory_equal ( string , empty , _string_length ( empty ) ) );
    string_clear ( string );

    // TEST 2: string_trim reduces a string of only whitespace to empty.
    _string_push ( string , only_whitespace );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    string_trim ( string );
    EXPECT ( memory_equal ( string , empty , _string_length ( empty ) ) );
    string_clear ( string );

    // TEST 3: string_trim trims a string with leading whitespace.
    _string_push ( string , leading_whitespace );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    string_trim ( string );
    EXPECT ( memory_equal ( string , trimmed , _string_length ( trimmed ) ) );
    string_clear ( string );

    // TEST 4: string_trim trims a string with trailing whitespace.
    _string_push ( string , trailing_whitespace );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    string_trim ( string );
    EXPECT ( memory_equal ( string , trimmed , _string_length ( trimmed ) ) );
    string_clear ( string );

    // TEST 5: string_trim trims a string with both leading and trailing whitespace.
    _string_push ( string , leading_and_trailing_whitespace );
    string_trim ( string );
    EXPECT ( memory_equal ( string , trimmed , _string_length ( trimmed ) ) );
    string_clear ( string );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    string_destroy ( string );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_string_contains
( void )
{
    const char* search1 = "This is the string to search.";
    const char search2[] = { 'H' , 'e' , 'l' , 'l' , 'o' , 0 , 'w' , 'o' , 'r' , 'l' , 'd' };
    const u64 search1_length = _string_length ( search1 );
    const u64 search2_length = sizeof ( search2 );
    const char* find_empty = "";
    const char* find_too_long = "This cannot be a substring because it is longer than both search1 and search2.";
    const char* find11 = "T";
    const char* find12 = ".";
    const char* find13 = "search. ";
    const char* find14 = "string";
    const char* find15 = "s";
    const char* find21 = "H";
    const char* find22 = "d";
    const char find23[] = { 'r' , 'l' , 'd' , 0 };
    const char* find24 = "wor";
    const char* find25 = "l";
    const char find26[] = { 'o' , 0 , 'w' };
    u64 index;

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: string_contains succeeds on first index when length of substring to find is 0.
    EXPECT ( string_contains ( search1 , search1_length , find_empty , _string_length ( find_empty ) , false , &index ) );
    EXPECT_EQ ( 0 , index );

    // TEST 2: string_contains (reverse) succeeds on final index when length of substring to find is 0.
    EXPECT ( string_contains ( search1 , search1_length , find_empty , _string_length ( find_empty ) , true , &index ) );
    EXPECT_EQ ( search1_length - 1 , index );

    // TEST 3: string_contains succeeds on first index when length of substring to find is 0 and strings are not null-terminated.
    EXPECT ( string_contains ( search2 , search2_length , find_empty , _string_length ( find_empty ) , false , &index ) );
    EXPECT_EQ ( 0 , index );

    // TEST 4: string_contains (reverse) succeeds on final index when length of substring to find is 0 and strings are not null-terminated.
    EXPECT ( string_contains ( search2 , search2_length , find_empty , _string_length ( find_empty ) , true , &index ) );
    EXPECT_EQ ( search2_length - 1 , index );

    // TEST 5: string_contains succeeds on first index when substring to find is the string to search itself.
    EXPECT ( string_contains ( search1 , search1_length , search1 , search1_length , false , &index ) );
    EXPECT_EQ ( 0 , index );
    
    // TEST 6: string_contains (reverse) succeeds on first index when substring to find is the string to search itself.
    EXPECT ( string_contains ( search1 , search1_length , search1 , search1_length , true , &index ) );
    EXPECT_EQ ( 0 , index );

    // TEST 7: string_contains succeeds on first index when substring to find is the string to search itself and strings are not null-terminated.
    EXPECT ( string_contains ( search2 , search2_length , search2 , search2_length , false , &index ) );
    EXPECT_EQ ( 0 , index );
    
    // TEST 8: string_contains (reverse) succeeds on first index when substring to find is the string to search itself and strings are not null-terminated.
    EXPECT ( string_contains ( search2 , search2_length , search2 , search2_length , true , &index ) );
    EXPECT_EQ ( 0 , index );

    // TEST 9: string_contains fails if length of substring to find exceeds length of string to search.
    EXPECT_NOT ( string_contains ( search1 , search1_length , find_too_long , _string_length ( find_too_long ) , false , &index ) );
    EXPECT_NOT ( string_contains ( search1 , search1_length , find_too_long , _string_length ( find_too_long ) , true , &index ) );
    EXPECT_NOT ( string_contains ( search2 , search2_length , find_too_long , _string_length ( find_too_long ) , false , &index ) );
    EXPECT_NOT ( string_contains ( search2 , search2_length , find_too_long , _string_length ( find_too_long ) , true , &index ) );
    
    // TEST 10: string_contains succeeds in locating the first character in the string to search.
    EXPECT ( string_contains ( search1 , search1_length , find11 , _string_length ( find11 ) , false , &index ) );
    EXPECT_EQ ( 0 , index );

    // TEST 11: string_contains (reverse) succeeds in locating the first character in the string to search.
    EXPECT ( string_contains ( search1 , search1_length , find11 , _string_length ( find11 ) , true , &index ) );
    EXPECT_EQ ( 0 , index );

    // TEST 12: string_contains succeeds in locating the first character in the string to search and strings are not null-terminated.
    EXPECT ( string_contains ( search2 , search2_length , find21 , _string_length ( find21 ) , false , &index ) );
    EXPECT_EQ ( 0 , index );

    // TEST 13: string_contains (reverse) succeeds in locating the first character in the string to search and strings are not null-terminated.
    EXPECT ( string_contains ( search2 , search2_length , find21 , _string_length ( find21 ) , true , &index ) );
    EXPECT_EQ ( 0 , index );

    // TEST 14: string_contains succeeds in locating the final character in the string to search.
    EXPECT ( string_contains ( search1 , search1_length , find12 , _string_length ( find12 ) , false , &index ) );
    EXPECT_EQ ( search1_length - 1 , index );

    // TEST 15: string_contains (reverse) succeeds in locating the final character in the string to search.
    EXPECT ( string_contains ( search1 , search1_length , find12 , _string_length ( find12 ) , true , &index ) );
    EXPECT_EQ ( search1_length - 1 , index );

    // TEST 16: string_contains succeeds in locating the final character in the string to search and strings are not null-terminated.
    EXPECT ( string_contains ( search2 , search2_length , find22 , _string_length ( find22 ) , false , &index ) );
    EXPECT_EQ ( search2_length - 1 , index );

    // TEST 17: string_contains (reverse) succeeds in locating the final character in the string to search and strings are not null-terminated.
    EXPECT ( string_contains ( search2 , search2_length , find22 , _string_length ( find22 ) , true , &index ) );
    EXPECT_EQ ( search2_length - 1 , index );

    // TEST 18: string_contains fails if the substring to find does not occur within the string to search.
    EXPECT_NOT ( string_contains ( search1 , search1_length , find13 , _string_length ( find13 ) , false , &index ) );
    EXPECT_NOT ( string_contains ( search1 , search1_length , find13 , _string_length ( find13 ) , true , &index ) );
    EXPECT_NOT ( string_contains ( search2 , search2_length , find23 , sizeof ( find23 ) , false , &index ) );
    EXPECT_NOT ( string_contains ( search2 , search2_length , find23 , sizeof ( find23 ) , true , &index ) );
    
    // TEST 19: string_contains succeeds in locating a substring which occurs once in the string to search.
    EXPECT ( string_contains ( search1 , search1_length , find14 , _string_length ( find14 ) , false , &index ) );
    EXPECT_EQ ( 12 , index );

    // TEST 20: string_contains (reverse) succeeds in locating a substring which occurs once in the string to search.
    EXPECT ( string_contains ( search1 , search1_length , find14 , _string_length ( find14 ) , false , &index ) );
    EXPECT_EQ ( 12 , index );

    // TEST 21: string_contains succeeds in locating a substring which occurs once in the string to search and strings are not null-terminated.
    EXPECT ( string_contains ( search2 , search2_length , find24 , _string_length ( find23 ) , false , &index ) );
    EXPECT_EQ ( 6 , index );

    // TEST 22: string_contains (reverse) succeeds in locating a substring which occurs once in the string to search and strings are not null-terminated.
    EXPECT ( string_contains ( search2 , search2_length , find24 , _string_length ( find23 ) , true , &index ) );
    EXPECT_EQ ( 6 , index );

    // TEST 23: string_contains succeeds in locating the first occurrence of a substring which occurs more than once in the string to search.
    EXPECT ( string_contains ( search1 , search1_length , find15 , _string_length ( find15 ) , false , &index ) );
    EXPECT_EQ ( 3 , index );

    // TEST 24: string_contains (reverse) succeeds in locating the final occurrence of a substring which occurs more than once in the string to search.
    EXPECT ( string_contains ( search1 , search1_length , find15 , _string_length ( find15 ) , true , &index ) );
    EXPECT_EQ ( 22 , index );

    // TEST 25: string_contains succeeds in locating the first occurrence of a substring which occurs more than once in the string to search and strings are not null-terminated.
    EXPECT ( string_contains ( search2 , search2_length , find25 , _string_length ( find25 ) , false , &index ) );
    EXPECT_EQ ( 2 , index );

    // TEST 26: string_contains (reverse) succeeds in locating the final occurrence of a substring which occurs more than once in the string to search and strings are not null-terminated.
    EXPECT ( string_contains ( search2 , search2_length , find25 , _string_length ( find25 ) , true , &index ) );
    EXPECT_EQ ( 9 , index );

    // TEST 27: string_contains correctly handles zero-bytes when dealing with strings that are not null-terminated.
    EXPECT ( string_contains ( search2 , search2_length , find26 , sizeof ( find26 ) , false , &index ) );
    EXPECT_EQ ( 4 , index );

    // TEST 28: string_contains (reverse) correctly handles zero-bytes when dealing with strings that are not null-terminated.
    EXPECT ( string_contains ( search2 , search2_length , find26 , sizeof ( find26 ) , true , &index ) );
    EXPECT_EQ ( 4 , index );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    return true;
}

u8
test_string_reverse
( void )
{
    const char* string_empty = "";
    const char* string_single_character = "$";
    const char* string_in = "0123456789";
    const char* string_out = "9876543210";
    char string[ 11 ];

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: string_reverse does not fail on an empty string.
    memory_copy ( string , string_empty , _string_length ( string_empty ) + 1 );
    _string_reverse ( string );
    EXPECT_EQ ( _string_length ( string_empty ) , _string_length ( string ) );
    EXPECT ( memory_equal ( string , string_empty , _string_length ( string ) ) );

    // TEST 2: string_reverse does not fail on a single-character string.
    memory_copy ( string , string_single_character , _string_length ( string_single_character ) + 1 );
    _string_reverse ( string );
    EXPECT_EQ ( _string_length ( string_single_character ) , _string_length ( string ) );
    EXPECT ( memory_equal ( string , string_single_character , _string_length ( string ) ) );

    // TEST 3: string_reverse correctly reverses an input string with more than one character.
    memory_copy ( string , string_in , _string_length ( string_in ) + 1 );
    _string_reverse ( string );
    EXPECT_EQ ( _string_length ( string_out ) , _string_length ( string ) );
    EXPECT ( memory_equal ( string , string_out , _string_length ( string ) ) );
    // (reverse it again)
    _string_reverse ( string );
    EXPECT_EQ ( _string_length ( string_in ) , _string_length ( string ) );
    EXPECT ( memory_equal ( string , string_in , _string_length ( string ) ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    return true;
}

u8
test_string_replace
( void )
{
    u64 global_amount_allocated;
    u64 array_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    array_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ARRAY );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const char* original = "Replace\r\nall\r\nnewlines\r\nwith\r\n4\r\nspaces.\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n";
    const char* replaced = "Replace    all    newlines    with    4    spaces.                                            ";
    const char* removed = "Replaceallnewlineswith4spaces.";
    const char* removed_replaced = "                                                                                                                        ";
    const char* empty = "";
    const char* to_replace = "\r\n";
    const char* replace_with = "    ";
    char* string = string_create ();

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , string );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: string_replace does not fail on an empty string.
    EXPECT_EQ ( _string_length ( empty ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , empty , string_length ( string ) + 1 ) );
    _string_replace ( string , to_replace , replace_with );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( empty ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , empty , string_length ( string ) + 1 ) );
    string_clear ( string );

    // TEST 2: string_replace does not modify the string if the substring to remove and replacement substring are identical.
    _string_push ( string , original );
    _string_replace ( string , to_replace , to_replace );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( original ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , original , string_length ( string ) + 1 ) );

    // TEST 3: string_replace does not modify the string if the substring to remove and replacement substring have identical size and characters.
    _string_replace ( string , to_replace , "\r\n" );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( original ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , original , string_length ( string ) + 1 ) );

    // TEST 4: string_replace does not modify the string if the substring to remove does not occur within it.
    _string_replace ( string , "Not found." , replace_with );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( original ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , original , string_length ( string ) + 1 ) );

    // TEST 5: string_replace replaces all instances of the substring to remove within the string with the replacement substring.
    _string_replace ( string , to_replace , replace_with );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( replaced ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , replaced , string_length ( string ) + 1 ) );
    // (undo the replacement by performing it backwards)
    _string_replace ( string , replace_with , to_replace );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( original ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , original , string_length ( string ) + 1 ) );

    // TEST 6: string_replace removes all instances of the substring to remove within the string if the length of the replacement substring is 0.
    _string_replace ( string , to_replace , empty );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( removed ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , removed , string_length ( string ) + 1 ) );

    // TEST 7: string_replace replaces every character of the string with the replacement substring if the length of the subtring to remove is 0.
    _string_replace ( string , empty , replace_with );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( removed_replaced ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , removed_replaced , string_length ( string ) + 1 ) );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    string_destroy ( string );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_string_strip_ansi
( void )
{
    u64 global_amount_allocated;
    u64 array_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    array_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ARRAY );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const char* in1 = ANSI_CC ( ANSI_CC_BG_DARK_RED ) "Strip me." ANSI_CC_RESET;
    const char* in2 = "Strip \033[0;1;2;43;44;45;46m" ANSI_CC ( ANSI_CC_BG_DARK_RED ) "me." ANSI_CC_RESET;
    const char* in3 = ANSI_CC_RESET "\033[0;1;2;43;44;45;46;101m" ANSI_CC2 ( ANSI_CC_BG_CYAN , ANSI_CC_BOLD );
    const char* empty = "";
    const char* in_illegal1 = "This should not\033[;;;;;]m be stripped.";
    const char* in_illegal2 = "This should not\033[890345298430958349058;324234234243324234234;23423423423423;234234234234234;234234234234234322342342342342342343\033m be stripped.";
    const char* in_illegal3 = "This should not\033[47;106 be stripped.";
    const char* out = "Strip me.";
    char* string = string_create ();

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , string );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: string_strip_ansi does not fail on an empty string.
    _string_push ( string , empty );
    string_strip_ansi ( string );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( 0 , string_length ( string ) );
    EXPECT ( memory_equal ( string , empty , string_length ( string ) + 1 ) );
    string_clear ( string );

    // TEST 2: string_strip_ansi removes a (short) single valid ANSI formatting code from the front and back of a string.
    _string_push ( string , in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    string_strip_ansi ( string );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out , string_length ( string ) + 1 ) );
    string_clear ( string );

    // TEST 3: string_strip_ansi removes a (short) single valid ANSI formatting code from the front and back of a string.
    _string_push ( string , in2 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    string_strip_ansi ( string );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out , string_length ( string ) + 1 ) );
    string_clear ( string );

    // TEST 4: string_strip_ansi truncates a string entirely if it is made up solely of ANSI formatting codes.
    _string_push ( string , in3 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    string_strip_ansi ( string );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( 0 , string_length ( string ) );
    EXPECT ( memory_equal ( string , empty , 1 ) );
    string_clear ( string );

    // TEST 5: string_strip_ansi ignores substrings which **almost** look like ANSI formatting codes.
    _string_push ( string , in_illegal1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    string_strip_ansi ( string );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( in_illegal1 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , in_illegal1 , string_length ( string ) + 1 ) );
    string_clear ( string );
    _string_push ( string , in_illegal2 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    string_strip_ansi ( string );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( in_illegal2 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , in_illegal2 , string_length ( string ) + 1 ) );
    string_clear ( string );
    _string_push ( string , in_illegal3 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    string_strip_ansi ( string );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , string );
    EXPECT_EQ ( _string_length ( in_illegal3 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , in_illegal3 , string_length ( string ) + 1 ) );
    string_clear ( string );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    string_destroy ( string );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

u8
test_string_u64_and_i64
( void )
{
    const i64 in1 = -9223372036854775807;
    const u64 in2 = 18446744073709551615ULL;
    const i64 in3 = -23428476892;
    const u64 in4 = 23428476892;
    const u64 in5 = 0;
    const char* out1_i64_radix16 = "8000000000000001";
    const char* out1_i64_radix10 = "-9223372036854775807";
    const char* out1_i64_radix2 = "1000000000000000000000000000000000000000000000000000000000000001";
    const char* out2_u64_radix16 = "FFFFFFFFFFFFFFFF";
    const char* out2_u64_radix10 = "18446744073709551615";
    const char* out2_u64_radix8 = "1777777777777777777777";
    const char* out2_u64_radix2 = "1111111111111111111111111111111111111111111111111111111111111111";
    const char* out3_i64_radix16 = "FFFFFFFA8B8DD024";
    const char* out3_i64_radix10 = "-23428476892";
    const char* out3_i64_radix2 = "1111111111111111111111111111101010001011100011011101000000100100";
    const char* out4_u64_radix16 = "574722FDC";
    const char* out4_u64_radix10 = "23428476892";
    const char* out4_u64_radix8 = "256434427734";
    const char* out4_u64_radix2 = "10101110100011100100010111111011100";
    const char* out5_u64_radix16 = "0";
    const char* out5_i64_radix16 = "0";
    const char* out5_u64_radix10 = "0";
    const char* out5_i64_radix10 = "0";
    const char* out5_u64_radix8 = "0";
    const char* out5_i64_radix8 = "0";
    const char* out5_u64_radix2 = "0";
    const char* out5_i64_radix2 = "0";
    char string[ 65 ];

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: string_i64, longest signed value, radix 16.
    EXPECT_EQ ( _string_length ( out1_i64_radix16 ) , string_i64 ( in1 , 16 , string ) );
    EXPECT ( memory_equal ( string , out1_i64_radix16 , _string_length ( out1_i64_radix16 ) ) );
    
    // TEST 2: string_i64, longest signed value, radix 10.
    EXPECT_EQ ( _string_length ( out1_i64_radix10 ) , string_i64 ( in1 , 10 , string ) );
    EXPECT ( memory_equal ( string , out1_i64_radix10 , _string_length ( out1_i64_radix10 ) ) );
    
    // TEST 3: string_i64, longest signed value, radix 2.
    EXPECT_EQ ( _string_length ( out1_i64_radix2 ) , string_i64 ( in1 , 2 , string ) );
    EXPECT ( memory_equal ( string , out1_i64_radix2 , _string_length ( out1_i64_radix2 ) ) );
    
    // TEST 4: string_u64, longest unsigned value, radix 16.
    EXPECT_EQ ( _string_length ( out2_u64_radix16 ) , string_u64 ( in2 , 16 , string ) );
    EXPECT ( memory_equal ( string , out2_u64_radix16 , _string_length ( out2_u64_radix16 ) ) );
    
    // TEST 5: string_u64, longest unsigned value, radix 10.
    EXPECT_EQ ( _string_length ( out2_u64_radix10 ) , string_u64 ( in2 , 10 , string ) );
    EXPECT ( memory_equal ( string , out2_u64_radix10 , _string_length ( out2_u64_radix10 ) ) );
    
    // TEST 6: string_u64, longest unsigned value, radix 8.
    EXPECT_EQ ( _string_length ( out2_u64_radix8 ) , string_u64 ( in2 , 8 , string ) );
    EXPECT ( memory_equal ( string , out2_u64_radix8 , _string_length ( out2_u64_radix8 ) ) );
    
    // TEST 7: string_u64, longest unsigned value, radix 2.
    EXPECT_EQ ( _string_length ( out2_u64_radix2 ) , string_u64 ( in2 , 2 , string ) );
    EXPECT ( memory_equal ( string , out2_u64_radix2 , _string_length ( out2_u64_radix2 ) ) );
    
    // TEST 8: string_i64, negative value, radix 16.
    EXPECT_EQ ( _string_length ( out3_i64_radix16 ) , string_i64 ( in3 , 16 , string ) );
    EXPECT ( memory_equal ( string , out3_i64_radix16 , _string_length ( out3_i64_radix16 ) ) );
    
    // TEST 9: string_i64, negative value, radix 10.
    EXPECT_EQ ( _string_length ( out3_i64_radix10 ) , string_i64 ( in3 , 10 , string ) );
    EXPECT ( memory_equal ( string , out3_i64_radix10 , _string_length ( out3_i64_radix10 ) ) );
    
    // TEST 10: string_i64, negative value, radix 2.
    EXPECT_EQ ( _string_length ( out3_i64_radix2 ) , string_i64 ( in3 , 2 , string ) );
    EXPECT ( memory_equal ( string , out3_i64_radix2 , _string_length ( out3_i64_radix2 ) ) );
    
    // TEST 11: string_u64, radix 16.
    EXPECT_EQ ( _string_length ( out4_u64_radix16 ) , string_u64 ( in4 , 16 , string ) );
    EXPECT ( memory_equal ( string , out4_u64_radix16 , _string_length ( out4_u64_radix16 ) ) );
    
    // TEST 12: string_u64, radix 10.
    EXPECT_EQ ( _string_length ( out4_u64_radix10 ) , string_u64 ( in4 , 10 , string ) );
    EXPECT ( memory_equal ( string , out4_u64_radix10 , _string_length ( out4_u64_radix10 ) ) );
    
    // TEST 13: string_u64, radix 8.
    EXPECT_EQ ( _string_length ( out4_u64_radix8 ) , string_u64 ( in4 , 8 , string ) );
    EXPECT ( memory_equal ( string , out4_u64_radix8 , _string_length ( out4_u64_radix8 ) ) );
    
    // TEST 14: string_u64, radix 2.
    EXPECT_EQ ( _string_length ( out4_u64_radix2 ) , string_u64 ( in4 , 2 , string ) );
    EXPECT ( memory_equal ( string , out4_u64_radix2 , _string_length ( out4_u64_radix2 ) ) );
    
    // TEST 15: string_u64, value == 0, radix 16.
    EXPECT_EQ ( _string_length ( out5_u64_radix16 ) , string_u64 ( in5 , 16 , string ) );
    EXPECT ( memory_equal ( string , out5_u64_radix16 , _string_length ( out5_u64_radix16 ) ) );
    
    // TEST 16: string_u64, value == 0, radix 10.
    EXPECT_EQ ( _string_length ( out5_u64_radix10 ) , string_u64 ( in5 , 10 , string ) );
    EXPECT ( memory_equal ( string , out5_u64_radix10 , _string_length ( out5_u64_radix10 ) ) );
    
    // TEST 17: string_u64, value == 0, radix 8.
    EXPECT_EQ ( _string_length ( out5_u64_radix8 ) , string_u64 ( in5 , 8 , string ) );
    EXPECT ( memory_equal ( string , out5_u64_radix8 , _string_length ( out5_u64_radix8 ) ) );
    
    // TEST 18: string_u64, value == 0, radix 2.
    EXPECT_EQ ( _string_length ( out5_u64_radix2 ) , string_u64 ( in5 , 2 , string ) );
    EXPECT ( memory_equal ( string , out5_u64_radix2 , _string_length ( out5_u64_radix2 ) ) );
    
    // TEST 19: string_i64, value == 0, radix 16.
    EXPECT_EQ ( _string_length ( out5_i64_radix16 ) , string_i64 ( in5 , 16 , string ) );
    EXPECT ( memory_equal ( string , out5_i64_radix16 , _string_length ( out5_i64_radix16 ) ) );
    
    // TEST 20: string_i64, value == 0, radix 10.
    EXPECT_EQ ( _string_length ( out5_i64_radix10 ) , string_i64 ( in5 , 10 , string ) );
    EXPECT ( memory_equal ( string , out5_i64_radix10 , _string_length ( out5_i64_radix10 ) ) );
    
    // TEST 21: string_i64, value == 0, radix 8.
    EXPECT_EQ ( _string_length ( out5_i64_radix8 ) , string_i64 ( in5 , 8 , string ) );
    EXPECT ( memory_equal ( string , out5_i64_radix8 , _string_length ( out5_i64_radix8 ) ) );
    
    // TEST 22: string_i64, value == 0, radix 2.
    EXPECT_EQ ( _string_length ( out5_i64_radix2 ) , string_i64 ( in5 , 2 , string ) );
    EXPECT ( memory_equal ( string , out5_i64_radix2 , _string_length ( out5_i64_radix2 ) ) );
    
    // End test.
    ////////////////////////////////////////////////////////////////////////////

    return true;
}

u8
test_string_f64
( void )
{
    // TODO: Implement this.
    return BYPASS;
}

u8
test_string_format
( void )
{
    u64 global_amount_allocated;
    u64 array_amount_allocated;
    u64 global_allocation_count;

    // Copy the current global allocator state prior to the test.
    global_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ALL );
    array_amount_allocated = memory_amount_allocated ( MEMORY_TAG_ARRAY );
    global_allocation_count = MEMORY_ALLOCATION_COUNT;

    const u64 raw_in = 23428476892;
    const i64 integer_in1 = -23428476892;
    const i64 integer_in4 = 23428476892;
    const f64 float_in1 = -100098.7893573;
    const f64 float_in4 = 100098.7893573;
    const f64 float_in3 = 8723941230947.678234563498562343478952734523495893245723495782349057897563274632589346;
    const u64 address_in = 45763;
    const char* const_string_in = "Hello world!";
    char* string_in = string_create_from ( const_string_in );
    char* really_long_string_in = _string_create ( 1000 * STACK_STRING_MAX_SIZE );
    f32 f32_array_in[ 16 ] = { -8 , -7 , -6 , -5 , -4 , -3 , -2 , -1 , 0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 };
    i8 i8_array_in[ 16 ] = { -8 , -7 , -6 , -5 , -4 , -3 , -2 , -1 , 0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 };
    f32* array_in1 = array_create_from ( f32 , f32_array_in , 16 );
    i8* array_in2 = array_create_from ( i8 , i8_array_in , 16 );
    const char* string_queue_in1 = "string_queue_in1";
    const char* string_queue_in2 = "string_queue_in2";
    const char* string_queue_in3 = "string_queue_in3";
    queue_t* queue_in = queue_create ( const char* );
    EXPECT_NEQ ( 0 , queue_in );
    queue_push ( queue_in , &string_queue_in1 );
    EXPECT_NEQ ( 0 , queue_in );
    queue_push ( queue_in , &string_queue_in2 );
    EXPECT_NEQ ( 0 , queue_in );
    queue_push ( queue_in , &string_queue_in3 );
    EXPECT_NEQ ( 0 , queue_in );
    const char* format_specifier_token_string = "%";
    const char* unterminated_format_specifier_string = "%;";
    const char* illegal_fix_precision_string = "`%.10f`";
    const char* illegal_padding_string1 = "`%P .3f`";
    const char* illegal_padding_string2 = "`%Pr 0.3f`";
    const char* out1 = "23428476892";
    const char* out2 = "-23428476892";
    const char* out3 = "-100098.789357300";
    const char* out4 = "100098.789357300";
    const char* out5 = "+100098.789357300";
    const char* out6 = "+8723941230947.678711";
    const char* out6_abbreviated = "+8.723941231E+12";
    const char* out7 = "0xB2C3";
    const char* out8 = "`000000000000000000000000000000000000000000000000000000000000000000000-100098.789`";
    const char* out9 = "`-100098.789000000000000000000000000000000000000000000000000000000000000000000000`";
    const char* out10 = "`\n\n\n-100098.789`";
    const char* out11 = "0xB2C3ljldkb]l-045[pwrsg439p80tu[]";
    const char* out12 = "+23428476892";
    const char* out13 = "0x0";
    const char* out14 = "%.2.8f";
    const char* out15 = "%+-iSsfa\\n\nm``lpmr2kl\r\t";
    const char* out16 = "%Plr0i";
    const char* out17 = "%Pl 190234.6+Pr190234i";
    const char* out18 = "789357300";
    const char* out19 = "-1.000988E+05";
    const char* out20 = "qqqqqqqqqqqqqqqqqqqqqqqHello world!";
    const char* out21 = "Hello world!.......................";
    const char* out22 = "{ `-8.00`, `-7.00`, `-6.00`, `-5.00`, `-4.00`, `-3.00`, `-2.00`, `-1.00`, `0.00`, `1.00`, `2.00`, `3.00`, `4.00`, `5.00`, `6.00`, `7.00` }";
    const char* out23 = "{ `-8`, `-7`, `-6`, `-5`, `-4`, `-3`, `-2`, `-1`, ` 0`, ` 1`, ` 2`, ` 3`, ` 4`, ` 5`, ` 6`, ` 7` }";
    const char* out24 = "{ `string_queue_in1`, `string_queue_in2`, `string_queue_in3` }";
    const char* out25 = "{ `H`, `e`, `l`, `l`, `o`, ` `, `w`, `o`, `r`, `l`, `d`, `!`, `` }";
    const char* illegal_container_string1 = "%.2aaF";
    const char* illegal_container_string2 = "%.2qaF";
    const char* illegal_container_string3 = "%.2aqF";
    char* string;

    // Verify there was no memory error prior to the test.
    EXPECT_NEQ ( 0 , string_in );
    EXPECT_NEQ ( 0 , really_long_string_in );

    ////////////////////////////////////////////////////////////////////////////
    // Start test.

    // TEST 1: If given a single-character string in which the only character is the format specifier token, string_format returns a copy of the format string.
    string = string_format ( format_specifier_token_string );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( format_specifier_token_string ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , format_specifier_token_string , string_length ( string ) ) );
    string_destroy ( string );

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST 2: string_format logs an error and returns an empty string if no format string is supplied.
    string = string_format ( 0 , 0 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( 0 , string_length ( string ) );
    EXPECT_EQ ( 0 , *string );
    string_destroy ( string );

    // TEST 3: string_format logs an error and returns an empty string if the supplied variadic argument list is invalid.
    args_t args;
    args.arg_count = 1;
    args.args = 0;
    string = _string_format ( "" , args );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( 0 , string_length ( string ) );
    EXPECT_EQ ( 0 , *string );
    string_destroy ( string );

    // TEST 4: Raw (unsigned) format specifier.
    string = string_format ( "%u" , raw_in );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out1 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out1 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 5: Memory address format specifier.
    string = string_format ( "%@" , address_in );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out7 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out7 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 6: Memory address format specifier prints null pointer correctly.
    string = string_format ( "%@" , 0 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out13 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out13 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 7: Null-terminated string format specifier.
    string = string_format ( "%s" , const_string_in );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( const_string_in ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , const_string_in , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 8: Resizable string format specifier (see container/string.h).
    string = string_format ( "%s%S" , string_in , string_in );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( 2 * string_length ( string_in ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , string_in , string_length ( string_in ) ) );
    EXPECT ( memory_equal ( string + string_length ( string_in ) , string_in , string_length ( string_in ) ) );
    string_destroy ( string );
    
    // Populate a relatively long string with random characters.
    for ( u64 i = 0; i < string_length ( really_long_string_in ); ++i )
    {
        really_long_string_in[ i ] = random2 ( 0 , 255 );
    }

    // TEST 9: string_format can handle relatively long strings without crashing.
    string = string_format ( "%S%S%S%S%S%S%S%S%S%S" , really_long_string_in , really_long_string_in , really_long_string_in , really_long_string_in , really_long_string_in , really_long_string_in , really_long_string_in , really_long_string_in , really_long_string_in , really_long_string_in );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( 10 * string_length ( really_long_string_in ) , string_length ( string ) );
    for ( u64 i = 0; i < 10; ++i )
    {
        EXPECT ( memory_equal ( string + i * string_length ( really_long_string_in ) , really_long_string_in , string_length ( really_long_string_in ) ) );
    }
    string_destroy ( string );

    // TEST 10: Signed integer format specifier. Positive number.
    string = string_format ( "%i" , integer_in4 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out1 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out1 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 11: Signed integer format specifier. Negative number.
    string = string_format ( "%i" , integer_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out2 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out2 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 12: Signed integer format specifier, with hide-sign modifier. Positive number.
    string = string_format ( "%-i" , integer_in4 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out1 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out1 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 13: Signed integer format specifier, with hide-sign modifier. Negative number.
    string = string_format ( "%-i" , integer_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out1 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out1 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 14: Signed integer format specifier, with show-sign modifier. Positive number.
    string = string_format ( "%+i" , integer_in4 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out12 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out12 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 15: Signed integer format specifier, with show-sign modifier. Negative number.
    string = string_format ( "%+i" , integer_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out2 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out2 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 16: Floating point format specifier, with fix-precision modifier. Negative number.
    string = string_format ( "%.9f" , &float_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out3 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out3 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 17: Floating point format specifier, with fix-precision and show-sign modifiers. Negative number.
    string = string_format ( "%+.9f" , &float_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out3 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out3 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 18: Floating point format specifier, with fix-precision and hide-sign modifiers. Negative number.
    string = string_format ( "%-.9f" , &float_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out4 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out4 , string_length ( string ) ) );
    string_destroy ( string );
    
    // TEST 19: Floating point format specifier, with fix-precision modifier. Positive number.
    string = string_format ( "%.9f" , &float_in4 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out4 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out4 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 20: Floating point format specifier, with fix-precision and show-sign modifiers. Positive number.
    string = string_format ( "%+.9f" , &float_in4 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out5 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out5 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 21: Floating point format specifier, with fix-precision and hide-sign modifiers. Positive number.
    string = string_format ( "%-.9f" , &float_in4 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out4 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out4 , string_length ( string ) ) );
    string_destroy ( string );
    
    // TEST 22: Floating point format specifier, with show-sign modifier. Positive number.
    string = string_format ( "%+f" , &float_in3 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out6 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out6 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 23: Floating point (abbreviated-notation) format specifier, with fix-precision and show-sign modifiers. Positive number.
    string = string_format ( "%+.9e" , &float_in3 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out6_abbreviated ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out6_abbreviated , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 24: Floating point (fractional only) format specifier.
    string = string_format ( "%.9d" , &float_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out18 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out18 , string_length ( string ) ) );
    string_destroy ( string );
    
    // TEST 25: Floating point (abbreviated-notation) format specifier.
    string = string_format ( "%e" , &float_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out19 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out19 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 26: Fixed-column-width format modifier, width == 80, pad left with '0'.
    string = string_format ( "`%Pl080.3f`" , &float_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out8 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out8 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 27: Fixed-column-width format modifier, width == 80, pad right with '0'.
    string = string_format ( "`%Pr080.3f`" , &float_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out9 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out9 , string_length ( string ) ) );
    string_destroy ( string );
    
    // TEST 28: Fixed-column-width format modifier, width == 14, pad left with newline.
    string = string_format ( "`%Pl\n14.3f`" , &float_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out10 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out10 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 29: Min-width format modifier, width >= 5, pad left with tab.
    string = string_format ( "%pl\t5S" , string_in );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( string_in ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , string_in , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 30: Min-width format modifier, width >= 35, pad left with 'q'.
    string = string_format ( "%plq35S" , string_in );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out20 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out20 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 31: Min-width format modifier, width >= 35, pad right with '.'.
    string = string_format ( "%pr.35S" , string_in );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out21 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out21 , string_length ( string ) ) );
    string_destroy ( string );

    LOGWARN ( "The following warnings are intentionally triggered by a test:" );

    // TEST 32: If the format string contains an unterminated format specifier, string_format logs a warning and ignores it.
    string = string_format ( unterminated_format_specifier_string , 25 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( unterminated_format_specifier_string ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , unterminated_format_specifier_string , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 33: If the format string contains an illegal format specifier, string_format logs a warning and ignores it.
    // NOTE: Fix-precision floating point format modifier only supports a single digit for precision selection, i.e. 0-9).
    string = string_format ( illegal_fix_precision_string , &float_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( illegal_fix_precision_string ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , illegal_fix_precision_string , string_length ( string ) ) );
    string_destroy ( string );
    
    // TEST 34: If the format string contains an illegal format specifier, string_format logs a warning and ignores it.
    // NOTE: Fix-width format modifier only supports a single character for the padding character, followed by any number of digits for the width.
    string = string_format ( illegal_padding_string1 , &float_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( illegal_padding_string1 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , illegal_padding_string1 , string_length ( string ) ) );
    string_destroy ( string );
    
    // TEST 35: If the format string contains an illegal format specifier, string_format logs a warning and ignores it.
    // NOTE: Fix-width format modifier only supports a single character for the padding character, followed by any number of digits for the width.
    string = string_format ( illegal_padding_string2 , &float_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( illegal_padding_string2 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , illegal_padding_string2 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 36: Show-sign and hide-sign format modifiers do not affect inapplicable types.
    string = string_format ( "%+s" , string_in );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( string_in ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , string_in , string_length ( string ) ) );
    string_destroy ( string );
    string = string_format ( "%-s" , string_in );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( string_in ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , string_in , string_length ( string ) ) );
    string_destroy ( string );
    
    // TEST 37: Fix-precision format modifier does not affect inapplicable types.
    string = string_format ( "%-.7@ljldkb]l-045[pwrsg439p80tu[]" , address_in );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out11 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out11 , string_length ( string ) ) );
    string_destroy ( string );
    
    // TEST 38: A format modifier which overwrites a previous format modifier of the same type on the same format specifier will be considered invalid.
    string = string_format ( out14 , &float_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out14 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out14 , string_length ( string ) ) );
    string_destroy ( string );
    // ( x2 )
    string = string_format ( out15 , &integer_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out15 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out15 , string_length ( string ) ) );
    string_destroy ( string );
    // ( x3 )
    string = string_format ( out16 , &integer_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out16 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out16 , string_length ( string ) ) );
    string_destroy ( string );
    // ( x4 )
    string = string_format ( out17 , &integer_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out17 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out17 , string_length ( string ) ) );
    string_destroy ( string );
    // ( x5 )
    string = string_format ( illegal_container_string1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( illegal_container_string1 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , illegal_container_string1 , string_length ( string ) ) );
    string_destroy ( string );
    // ( x6 )
    string = string_format ( illegal_container_string2 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( illegal_container_string2 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , illegal_container_string2 , string_length ( string ) ) );
    string_destroy ( string );
    // ( x7 )
    string = string_format ( illegal_container_string3 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( illegal_container_string3 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , illegal_container_string3 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 39: string_format can handle null pointers for the following format specifiers: %s, %S, %f, %F, %e, %d.
    string = string_format ( "%s" , 0 );
    EXPECT_NEQ ( 0 , string );
    string_destroy ( string );
    string = string_format ( "%S" , 0 );
    EXPECT_NEQ ( 0 , string );
    string_destroy ( string );
    string = string_format ( "%f" , 0 );
    EXPECT_NEQ ( 0 , string );
    string_destroy ( string );
    string = string_format ( "%F" , 0 );
    EXPECT_NEQ ( 0 , string );
    string_destroy ( string );
    string = string_format ( "%e" , 0 );
    EXPECT_NEQ ( 0 , string );
    string_destroy ( string );
    string = string_format ( "%d" , 0 );
    EXPECT_NEQ ( 0 , string );
    string_destroy ( string );

    // TEST 40: Floating point format specifier, with fix-precision and array format modifiers.
    string = string_format ( "%.2aF" , array_in1 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out22 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out22 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 41: Integer format specifier, with minimum-column-width and array format modifiers.
    string = string_format ( "%apl 2i" , array_in2 );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out23 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out23 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 42: String format specifier, with queue format modifier.
    string = string_format ( "%qs" , queue_in );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out24 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out24 , string_length ( string ) ) );
    string_destroy ( string );

    // TEST 43: Character format specifier, with array format modifier.
    string = string_format ( "%ac" , string_in );
    EXPECT_NEQ ( 0 , string ); // Verify there was no memory error prior to the test.
    EXPECT_EQ ( _string_length ( out25 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out25 , string_length ( string ) ) );
    string_destroy ( string );

    // End test.
    ////////////////////////////////////////////////////////////////////////////

    string_destroy ( string_in );
    string_destroy ( really_long_string_in );
    array_destroy ( array_in1 );
    array_destroy ( array_in2 );
    queue_destroy ( queue_in );

    // Verify the test allocated and freed all of its memory properly.
    EXPECT_EQ ( global_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ALL ) );
    EXPECT_EQ ( array_amount_allocated , memory_amount_allocated ( MEMORY_TAG_ARRAY ) );
    EXPECT_EQ ( global_allocation_count , MEMORY_ALLOCATION_COUNT );

    return true;
}

void
test_register_string
( void )
{
    test_register ( test_string_allocate_and_free , "Allocating memory for a fixed-length string." );
    test_register ( test_string_create_and_destroy , "Allocating memory for a resizable string data structure." );
    test_register ( test_string_push , "Testing string 'push' operation." );
    test_register ( test_string_insert_and_remove , "Testing string 'insert' and 'remove' operations." );
    test_register ( test_string_insert_and_remove_random , "Testing string 'insert' and 'remove' operations with random indices and elements." );
    test_register ( test_string_trim , "Testing string 'trim' operation." );
    test_register ( test_string_contains , "Testing string 'contains' operation." );
    test_register ( test_string_reverse , "Testing string in-place 'reverse' operation." );
    test_register ( test_string_replace , "Testing string 'replace' operation." );
    test_register ( test_string_strip_ansi , "Stripping a string of ANSI formatting codes." );
    test_register ( test_string_u64_and_i64 , "Testing 'stringify' operation on 64-bit integers." );
    test_register ( test_string_f64 , "Testing 'stringify' operation on 64-bit floating point numbers." );
    test_register ( test_string_format , "Constructing a string using format specifiers." );
}