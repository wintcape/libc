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
    const char* hello = "Hello world!";
    char* string = string_allocate_from ( hello );
    EXPECT_EQ ( _string_length ( hello ) , _string_length ( string ) );
    EXPECT ( memory_equal ( string , hello , _string_length ( string ) ) );
    string_free ( string );
    return true;
}

u8
test_string_create_and_destroy
( void )
{
    char* string = string_create ();
    EXPECT_EQ ( 0 , string_length ( string ) );
    string_destroy ( string );
    const char* hello = "Hello world!";
    string = string_create_from ( hello );
    EXPECT_EQ ( _string_length ( hello ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , hello , string_length ( string ) ) );
    char* copy = string_copy ( string );
    EXPECT_EQ ( string_length ( copy ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , copy , string_length ( string ) ) );
    string_destroy ( copy );
    string_destroy ( string );
    return true;
}

u8
test_string_push
( void )
{
    const char* to_push = "push";
    const u64 max_op = 100000;
    char* string = string_create ();
    LOGDEBUG ( "Appending %i bytes to an empty string. . ." , max_op * _string_length ( to_push ) );
    char* old_string = string_allocate ( max_op * _string_length ( to_push ) + 1 );
    for ( u64 i = 0; i < max_op; ++i )
    {
        const u64 old_length = string_length ( string );
        memory_copy ( old_string , string , old_length );
        _string_push ( string , to_push );
        EXPECT_NEQ ( 0 , string );
        EXPECT_EQ ( old_length , string_length ( string ) - _string_length ( to_push ) );
        EXPECT ( memory_equal ( to_push , &string[ old_length ] , _string_length ( to_push ) ) );
    }
    LOGDEBUG ( "  Done." );
    string_free ( old_string );
    string_destroy ( string );
    return true;
}

u8
test_string_insert_and_remove
( void )
{
    const char* to_insert[] = { "He" , "llo " , "world" , "!" };
    const char* remove0     =   "He"   "llo "   "world"   "!"  ;
    const char* remove1     =   "He"   "llo "             "!"  ;
    const char* remove2     =   "He"                      "!"  ;
    const char* remove3     =       "!"  ;
    char* string1 = string_create ();
    char* string2 = string_create ();
    LOGWARN ( "The following errors are intentionally triggered by a test:" );
    _string_insert ( string1 , string_length ( string1 ) + 1 , to_insert[ 0 ] );
    EXPECT_NEQ ( 0 , string1 );
    EXPECT_EQ ( 0 , string_length ( string1 ) );
    _string_insert ( string1 , string_length ( string1 ) , to_insert[ 0 ] );
    EXPECT_NEQ ( 0 , string1 );
    EXPECT_EQ ( _string_length ( to_insert[ 0 ] ) , string_length ( string1 ) );
    EXPECT ( memory_equal ( to_insert[ 0 ] , string1 , _string_length ( to_insert[ 0 ] ) ) );
    _string_insert ( string1 , string_length ( string1 ) , to_insert[ 2 ] );
    EXPECT_NEQ ( 0 , string1 );
    EXPECT_EQ ( _string_length ( to_insert[ 0 ] ) + _string_length ( to_insert[ 2 ] ) , string_length ( string1 ) );
    EXPECT ( memory_equal ( to_insert[ 2 ] , string1 + _string_length ( to_insert[ 0 ] ) , _string_length ( to_insert[ 2 ] ) ) );
    _string_insert ( string1 , string_length ( string1 ) , to_insert[ 3 ] );
    EXPECT_NEQ ( 0 , string1 );
    EXPECT_EQ ( _string_length ( to_insert[ 0 ] ) + _string_length ( to_insert[ 2 ] ) + _string_length ( to_insert[ 3 ] ) , string_length ( string1 ) );
    EXPECT ( memory_equal ( to_insert[ 3 ] , string1 + _string_length ( to_insert[ 0 ] ) + _string_length ( to_insert[ 2 ] ) , _string_length ( to_insert[ 3 ] ) ) );
    _string_insert ( string1 , _string_length ( to_insert[ 0 ] ) , to_insert[ 1 ] );
    EXPECT_NEQ ( 0 , string1 );
    EXPECT_EQ ( _string_length ( to_insert[ 0 ] ) + _string_length ( to_insert[ 1 ] ) + _string_length ( to_insert[ 2 ] ) + _string_length ( to_insert[ 3 ] ) , string_length ( string1 ) );
    EXPECT ( memory_equal ( remove0 , string1 , string_length ( string1 ) ) );
    _string_push ( string2 , to_insert[ 0 ] );
    _string_push ( string2 , to_insert[ 1 ] );
    _string_push ( string2 , to_insert[ 2 ] );
    _string_push ( string2 , to_insert[ 3 ] );
    EXPECT_EQ ( string_length ( string1 ) , string_length ( string2 ) );
    EXPECT ( memory_equal ( string1 , string2 , string_length ( string1 ) + 1 ) );
    string_remove ( string1 , string_length ( string1 ) , 0 );
    EXPECT_NEQ ( 0 , string1 );
    EXPECT_EQ ( _string_length ( to_insert[ 0 ] ) + _string_length ( to_insert[ 1 ] ) + _string_length ( to_insert[ 2 ] ) + _string_length ( to_insert[ 3 ] ) , string_length ( string1 ) );
    EXPECT_EQ ( string_length ( string1 ) , string_length ( string2 ) );
    EXPECT ( memory_equal ( string1 , string2 , string_length ( string1 ) ) );
    string_remove ( string1 , string_length ( string1 ) - 5 , 30 );
    EXPECT_NEQ ( 0 , string1 );
    EXPECT_EQ (   _string_length ( to_insert[ 0 ] ) + _string_length ( to_insert[ 1 ] ) + _string_length ( to_insert[ 2 ] ) + _string_length ( to_insert[ 3 ] )  , string_length ( string1 ) );
    EXPECT_EQ ( string_length ( string1 ) , string_length ( string2 ) );
    EXPECT ( memory_equal ( string1 , string2 , string_length ( string1 ) ) );
    string_remove ( string1 , _string_length ( to_insert[ 0 ] ) + _string_length ( to_insert[ 1 ] ) , _string_length ( to_insert[ 2 ] ) );
    EXPECT_NEQ ( 0 , string1 );
    EXPECT_EQ ( _string_length ( to_insert[ 0 ] ) + _string_length ( to_insert[ 1 ] ) + _string_length ( to_insert[ 3 ] )  , string_length ( string1 ) );
    EXPECT ( memory_equal ( string1 , remove1 , string_length ( string1 ) ) );
    string_remove ( string1 , _string_length ( to_insert[ 0 ] ) , _string_length ( to_insert[ 1 ] ) );
    EXPECT_NEQ ( 0 , string1 );
    EXPECT_EQ ( _string_length ( to_insert[ 0 ] ) + _string_length ( to_insert[ 3 ] ) , string_length ( string1 ) );
    EXPECT ( memory_equal ( string1 , remove2 , string_length ( string1 ) ) );
    string_remove ( string1 , 0 , _string_length ( to_insert[ 0 ] ) );
    EXPECT_NEQ ( 0 , string1 );
    EXPECT_EQ ( _string_length ( to_insert[ 3 ] )  , string_length ( string1 )  );
    EXPECT ( memory_equal ( string1 , remove3 , string_length ( string1 ) ) );
    string_remove ( string1 , 0 , string_length ( string1 ) );
    EXPECT_NEQ ( 0 , string1 );
    EXPECT_EQ ( 0 , string_length ( string1 ) );
    string_remove ( string1 , 0 , 0 );
    EXPECT_NEQ ( 0 , string1 );
    EXPECT_EQ ( 0 , string_length ( string1 ) );
    string_remove ( string1 , 0 , 1 );
    EXPECT_NEQ ( 0 , string1 );
    EXPECT_EQ ( 0 , string_length ( string1 ) );
    string_destroy ( string1 );
    string_destroy ( string2 );
    return true;
}

u8
test_string_insert_and_remove_random
( void )
{
    const u64 max_op = 100000;
    char* string = string_create ();
    LOGDEBUG ( "Inserting %i random-length strings into an array at random indices. . ." , max_op );
    char* old_string = string_allocate ( max_op + 1 );
    for ( u64 i = 0; i < max_op; ++i )
    {
        const u64 old_length = string_length ( string );
        memory_copy ( old_string , string , old_length );
        const char to_insert[] = { random2 ( 1 , 255 ) , 0 };
        const u64 insert_index = random2 ( 0 , old_length );
        _string_insert ( string , insert_index , to_insert );
        EXPECT_NEQ ( 0 , string );
        EXPECT_EQ ( old_length , string_length ( string ) - 1 );
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
        EXPECT_EQ ( *to_insert , string[ insert_index ] );
    }
    LOGDEBUG ( "  Done." );
    LOGDEBUG ( "Removing a random number of characters in random order. . ." );
    while ( string_length ( string ) )
    {
        const u64 old_length = string_length ( string );
        memory_copy ( old_string , string , old_length );
        const u64 remove_cap = ( old_length > 1000 ) ? old_length / 100 : old_length;
        const u64 remove_count = random2 ( 1 , remove_cap );
        const u64 remove_index = random2 ( 0 , old_length - remove_cap );
        string_remove ( string , remove_index , remove_count );
        EXPECT_NEQ ( 0 , string );
        EXPECT_EQ ( old_length , string_length ( string ) + remove_count );
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
    LOGDEBUG ( "  Done." );
    string_free ( old_string );
    string_destroy ( string );
    return true;
}

u8
test_string_trim
( void )
{
    char* string = string_create ();
    const char* empty = "";
    const char* only_whitespace = "       \t\n\r        ";
    const char* leading_whitespace = "\n\t\t\t  <-- Trim this off -->";
    const char* trailing_whitespace = "<-- Trim this off -->  \t\t\t\n";
    const char* leading_and_trailing_whitespace = "\n\t\t\t  <-- Trim this off -->  \t\t\t\n";
    const char* trimmed = "<-- Trim this off -->";
    _string_push ( string , empty );
    string_trim ( string );
    EXPECT ( memory_equal ( string , empty , _string_length ( empty ) ) );
    string_remove ( string , 0 , string_length ( string ) );
    _string_push ( string , only_whitespace );
    string_trim ( string );
    EXPECT ( memory_equal ( string , empty , _string_length ( empty ) ) );
    string_remove ( string , 0 , string_length ( string ) );
    _string_push ( string , leading_whitespace );
    string_trim ( string );
    EXPECT ( memory_equal ( string , trimmed , _string_length ( trimmed ) ) );
    string_remove ( string , 0 , string_length ( string ) );
    _string_push ( string , trailing_whitespace );
    string_trim ( string );
    EXPECT ( memory_equal ( string , trimmed , _string_length ( trimmed ) ) );
    string_remove ( string , 0 , string_length ( string ) );
    _string_push ( string , leading_and_trailing_whitespace );
    string_trim ( string );
    EXPECT ( memory_equal ( string , trimmed , _string_length ( trimmed ) ) );
    string_destroy ( string );
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
    const char* find23 = "world";
    const char* find24 = "l";
    const char find25[] = { 'o' , 0 , 'w' };
    u64 index;
    EXPECT ( string_contains ( search1 , search1_length , find_empty , _string_length ( find_empty ) , false , &index ) );
    EXPECT_EQ ( 0 , index );
    EXPECT ( string_contains ( search1 , search1_length , find_empty , _string_length ( find_empty ) , true , &index ) );
    EXPECT_EQ ( search1_length , index );
    EXPECT ( string_contains ( search2 , search2_length , find_empty , _string_length ( find_empty ) , false , &index ) );
    EXPECT_EQ ( 0 , index );
    EXPECT ( string_contains ( search2 , search2_length , find_empty , _string_length ( find_empty ) , true , &index ) );
    EXPECT_EQ ( search2_length , index );
    EXPECT ( string_contains ( search1 , search1_length , search1 , search1_length , false , &index ) );
    EXPECT_EQ ( 0 , index );
    EXPECT ( string_contains ( search1 , search1_length , search1 , search1_length , true , &index ) );
    EXPECT_EQ ( 0 , index );
    EXPECT ( string_contains ( search2 , search2_length , search2 , search2_length , false , &index ) );
    EXPECT_EQ ( 0 , index );
    EXPECT ( string_contains ( search2 , search2_length , search2 , search2_length , true , &index ) );
    EXPECT_EQ ( 0 , index );
    EXPECT_NOT ( string_contains ( search1 , search1_length , find_too_long , _string_length ( find_too_long ) , false , &index ) );
    EXPECT_NOT ( string_contains ( search1 , search1_length , find_too_long , _string_length ( find_too_long ) , true , &index ) );
    EXPECT_NOT ( string_contains ( search2 , search2_length , find_too_long , _string_length ( find_too_long ) , false , &index ) );
    EXPECT_NOT ( string_contains ( search2 , search2_length , find_too_long , _string_length ( find_too_long ) , true , &index ) );
    EXPECT ( string_contains ( search1 , search1_length , find11 , _string_length ( find11 ) , false , &index ) );
    EXPECT_EQ ( 0 , index );
    EXPECT ( string_contains ( search1 , search1_length , find11 , _string_length ( find11 ) , true , &index ) );
    EXPECT_EQ ( 0 , index );
    EXPECT ( string_contains ( search1 , search1_length , find12 , _string_length ( find12 ) , false , &index ) );
    EXPECT_EQ ( search1_length - 1 , index );
    EXPECT ( string_contains ( search1 , search1_length , find12 , _string_length ( find12 ) , true , &index ) );
    EXPECT_EQ ( search1_length - 1 , index );
    EXPECT_NOT ( string_contains ( search1 , search1_length , find13 , _string_length ( find13 ) , false , &index ) );
    EXPECT_NOT ( string_contains ( search1 , search1_length , find13 , _string_length ( find13 ) , true , &index ) );
    EXPECT ( string_contains ( search1 , search1_length , find14 , _string_length ( find14 ) , false , &index ) );
    EXPECT_EQ ( 12 , index );
    EXPECT ( string_contains ( search1 , search1_length , find14 , _string_length ( find14 ) , false , &index ) );
    EXPECT_EQ ( 12 , index );
    EXPECT ( string_contains ( search1 , search1_length , find15 , _string_length ( find15 ) , false , &index ) );
    EXPECT_EQ ( 3 , index );
    EXPECT ( string_contains ( search1 , search1_length , find15 , _string_length ( find15 ) , true , &index ) );
    EXPECT_EQ ( 22 , index );
    EXPECT ( string_contains ( search2 , search2_length , find21 , _string_length ( find21 ) , false , &index ) );
    EXPECT_EQ ( 0 , index );
    EXPECT ( string_contains ( search2 , search2_length , find21 , _string_length ( find21 ) , true , &index ) );
    EXPECT_EQ ( 0 , index );
    EXPECT ( string_contains ( search2 , search2_length , find22 , _string_length ( find22 ) , false , &index ) );
    EXPECT_EQ ( search2_length - 1 , index );
    EXPECT ( string_contains ( search2 , search2_length , find22 , _string_length ( find22 ) , true , &index ) );
    EXPECT_EQ ( search2_length - 1 , index );
    EXPECT ( string_contains ( search2 , search2_length , find23 , _string_length ( find23 ) , false , &index ) );
    EXPECT_EQ ( 6 , index );
    EXPECT ( string_contains ( search2 , search2_length , find23 , _string_length ( find23 ) , true , &index ) );
    EXPECT_EQ ( 6 , index );
    EXPECT ( string_contains ( search2 , search2_length , find24 , _string_length ( find24 ) , false , &index ) );
    EXPECT_EQ ( 2 , index );
    EXPECT ( string_contains ( search2 , search2_length , find24 , _string_length ( find24 ) , true , &index ) );
    EXPECT_EQ ( 9 , index );
    EXPECT ( string_contains ( search2 , search2_length , find25 , sizeof ( find25 ) , false , &index ) );
    EXPECT_EQ ( 4 , index );
    EXPECT ( string_contains ( search2 , search2_length , find25 , sizeof ( find25 ) , true , &index ) );
    EXPECT_EQ ( 4 , index );
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
    memory_copy ( string , string_empty , _string_length ( string_empty ) + 1 );
    _string_reverse ( string );
    EXPECT_EQ ( _string_length ( string_empty ) , _string_length ( string ) );
    EXPECT ( memory_equal ( string , string_empty , _string_length ( string ) ) );
    memory_copy ( string , string_single_character , _string_length ( string_single_character ) + 1 );
    _string_reverse ( string );
    EXPECT_EQ ( _string_length ( string_single_character ) , _string_length ( string ) );
    EXPECT ( memory_equal ( string , string_single_character , _string_length ( string ) ) );
    memory_copy ( string , string_in , _string_length ( string_in ) + 1 );
    _string_reverse ( string );
    EXPECT_EQ ( _string_length ( string_out ) , _string_length ( string ) );
    EXPECT ( memory_equal ( string , string_out , _string_length ( string ) ) );
    _string_reverse ( string );
    EXPECT_EQ ( _string_length ( string_in ) , _string_length ( string ) );
    EXPECT ( memory_equal ( string , string_in , _string_length ( string ) ) );
    return true;
}

u8
test_string_u64_and_i64
( void )
{
    const i64 in1 = -9223372036854775807;
    const u64 in2 = 18446744073709551615ULL;
    const i64 in3 = -23428476892;
    const i64 in4 = 23428476892;
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
    EXPECT_EQ ( _string_length ( out1_i64_radix16 ) , string_i64 ( in1 , 16 , string ) );
    EXPECT ( memory_equal ( string , out1_i64_radix16 , _string_length ( out1_i64_radix16 ) ) );
    EXPECT_EQ ( _string_length ( out1_i64_radix10 ) , string_i64 ( in1 , 10 , string ) );
    EXPECT ( memory_equal ( string , out1_i64_radix10 , _string_length ( out1_i64_radix10 ) ) );
    EXPECT_EQ ( _string_length ( out1_i64_radix2 ) , string_i64 ( in1 , 2 , string ) );
    EXPECT ( memory_equal ( string , out1_i64_radix2 , _string_length ( out1_i64_radix2 ) ) );
    EXPECT_EQ ( _string_length ( out2_u64_radix16 ) , string_u64 ( in2 , 16 , string ) );
    EXPECT ( memory_equal ( string , out2_u64_radix16 , _string_length ( out2_u64_radix16 ) ) );
    EXPECT_EQ ( _string_length ( out2_u64_radix10 ) , string_u64 ( in2 , 10 , string ) );
    EXPECT ( memory_equal ( string , out2_u64_radix10 , _string_length ( out2_u64_radix10 ) ) );
    EXPECT_EQ ( _string_length ( out2_u64_radix8 ) , string_i64 ( in2 , 8 , string ) );
    EXPECT ( memory_equal ( string , out2_u64_radix8 , _string_length ( out2_u64_radix8 ) ) );
    EXPECT_EQ ( _string_length ( out2_u64_radix2 ) , string_i64 ( in2 , 2 , string ) );
    EXPECT ( memory_equal ( string , out2_u64_radix2 , _string_length ( out2_u64_radix2 ) ) );
    EXPECT_EQ ( _string_length ( out3_i64_radix16 ) , string_i64 ( in3 , 16 , string ) );
    EXPECT ( memory_equal ( string , out3_i64_radix16 , _string_length ( out3_i64_radix16 ) ) );
    EXPECT_EQ ( _string_length ( out3_i64_radix10 ) , string_i64 ( in3 , 10 , string ) );
    EXPECT ( memory_equal ( string , out3_i64_radix10 , _string_length ( out3_i64_radix10 ) ) );
    EXPECT_EQ ( _string_length ( out3_i64_radix2 ) , string_i64 ( in3 , 2 , string ) );
    EXPECT ( memory_equal ( string , out3_i64_radix2 , _string_length ( out3_i64_radix2 ) ) );
    EXPECT_EQ ( _string_length ( out4_u64_radix16 ) , string_u64 ( in4 , 16 , string ) );
    EXPECT ( memory_equal ( string , out4_u64_radix16 , _string_length ( out4_u64_radix16 ) ) );
    EXPECT_EQ ( _string_length ( out4_u64_radix10 ) , string_u64 ( in4 , 10 , string ) );
    EXPECT ( memory_equal ( string , out4_u64_radix10 , _string_length ( out4_u64_radix10 ) ) );
    EXPECT_EQ ( _string_length ( out4_u64_radix8 ) , string_u64 ( in4 , 8 , string ) );
    EXPECT ( memory_equal ( string , out4_u64_radix8 , _string_length ( out4_u64_radix8 ) ) );
    EXPECT_EQ ( _string_length ( out4_u64_radix2 ) , string_u64 ( in4 , 2 , string ) );
    EXPECT ( memory_equal ( string , out4_u64_radix2 , _string_length ( out4_u64_radix2 ) ) );
    EXPECT_EQ ( _string_length ( out5_u64_radix16 ) , string_u64 ( in5 , 16 , string ) );
    EXPECT ( memory_equal ( string , out5_u64_radix16 , _string_length ( out5_u64_radix16 ) ) );
    EXPECT_EQ ( _string_length ( out5_u64_radix10 ) , string_u64 ( in5 , 10 , string ) );
    EXPECT ( memory_equal ( string , out5_u64_radix10 , _string_length ( out5_u64_radix10 ) ) );
    EXPECT_EQ ( _string_length ( out5_u64_radix8 ) , string_u64 ( in5 , 8 , string ) );
    EXPECT ( memory_equal ( string , out5_u64_radix8 , _string_length ( out5_u64_radix8 ) ) );
    EXPECT_EQ ( _string_length ( out5_u64_radix2 ) , string_u64 ( in5 , 2 , string ) );
    EXPECT ( memory_equal ( string , out5_u64_radix2 , _string_length ( out5_u64_radix2 ) ) );
    EXPECT_EQ ( _string_length ( out5_i64_radix16 ) , string_i64 ( in5 , 16 , string ) );
    EXPECT ( memory_equal ( string , out5_i64_radix16 , _string_length ( out5_i64_radix16 ) ) );
    EXPECT_EQ ( _string_length ( out5_i64_radix10 ) , string_i64 ( in5 , 10 , string ) );
    EXPECT ( memory_equal ( string , out5_i64_radix10 , _string_length ( out5_i64_radix10 ) ) );
    EXPECT_EQ ( _string_length ( out5_i64_radix8 ) , string_i64 ( in5 , 8 , string ) );
    EXPECT ( memory_equal ( string , out5_i64_radix8 , _string_length ( out5_i64_radix8 ) ) );
    EXPECT_EQ ( _string_length ( out5_i64_radix2 ) , string_i64 ( in5 , 2 , string ) );
    EXPECT ( memory_equal ( string , out5_i64_radix2 , _string_length ( out5_i64_radix2 ) ) );
    return true;
}

u8
test_string_f64
( void )
{
    return BYPASS;
}

u8
test_string_format
( void )
{
    const u64 raw_in = 23428476892;
    const i64 integer_in1 = -23428476892;
    const i64 integer_in4 = 23428476892;
    const f64 float_in1 = -100098.7893573;
    const f64 float_in4 = 100098.7893573;
    const f64 float_in3 = 8723941230947.678234563498562343478952734523495893245723495782349057897563274632589346;
    const u64 address_in = 45763;
    const char* const_string_in = "Hello world!";
    char* string_in = string_create_from ( const_string_in );
    char* string;
    const char* in1 = "%";
    string = string_format ( in1 );
    EXPECT_EQ ( _string_length ( in1 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , in1 , string_length ( string ) ) );
    string_destroy ( string );
    LOGWARN ( "The following warnings are intentionally triggered by a test:" );
    const char* in4 = "%;";
    string = string_format ( in4 , 25 );
    EXPECT_EQ ( _string_length ( in4 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , in4 , string_length ( string ) ) );
    string_destroy ( string );
    const char* illegal_fix_precision_string_in = "`%.10f`";
    string = string_format ( illegal_fix_precision_string_in , &float_in1 );
    EXPECT_EQ ( _string_length ( illegal_fix_precision_string_in ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , illegal_fix_precision_string_in , string_length ( string ) ) );
    string_destroy ( string );
    const char* out1 = "23428476892";
    string = string_format ( "%u" , raw_in );
    EXPECT_EQ ( _string_length ( out1 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out1 , string_length ( string ) ) );
    string_destroy ( string );
    string = string_format ( "%-i" , integer_in4 );
    EXPECT_EQ ( _string_length ( out1 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out1 , string_length ( string ) ) );
    string_destroy ( string );
    string = string_format ( "%-i" , integer_in1 );
    EXPECT_EQ ( _string_length ( out1 ) , string_length ( string ) );
    string_destroy ( string );
    EXPECT ( memory_equal ( string , out1 , string_length ( string ) ) );
    const char* out2 = "-23428476892";
    string = string_format ( "%i" , integer_in1 );
    EXPECT_EQ ( _string_length ( out2 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out2 , string_length ( string ) ) );
    string_destroy ( string );
    string = string_format ( "%+i" , integer_in1 );
    EXPECT_EQ ( _string_length ( out2 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out2 , string_length ( string ) ) );
    string_destroy ( string );
    const char* out3 = "-100098.789357300";
    string = string_format ( "%.9f" , &float_in1 );
    EXPECT_EQ ( _string_length ( out3 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out3 , string_length ( string ) ) );
    string_destroy ( string );
    string = string_format ( "%+.9f" , &float_in1 );
    EXPECT_EQ ( _string_length ( out3 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out3 , string_length ( string ) ) );
    string_destroy ( string );
    const char* out4 = "100098.789357300";
    string = string_format ( "%.9f" , &float_in4 );
    EXPECT_EQ ( _string_length ( out4 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out4 , string_length ( string ) ) );
    string_destroy ( string );
    string = string_format ( "%-.9f" , &float_in1 );
    EXPECT_EQ ( _string_length ( out4 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out4 , string_length ( string ) ) );
    string_destroy ( string );
    string = string_format ( "%-.9f" , &float_in4 );
    EXPECT_EQ ( _string_length ( out4 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out4 , string_length ( string ) ) );
    string_destroy ( string );
    const char* out5 = "+100098.789357300";
    string = string_format ( "%+.9f" , &float_in4 );
    EXPECT_EQ ( _string_length ( out5 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out5 , string_length ( string ) ) );
    string_destroy ( string );
    const char* out6 = "+8723941230947.678711";
    const char* out6_abbreviated = "+8.723941231E+12";
    string = string_format ( "%+f" , &float_in3 );
    EXPECT_EQ ( _string_length ( out6 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out6 , string_length ( string ) ) );
    string_destroy ( string );
    string = string_format ( "%+.9e" , &float_in3 );
    EXPECT_EQ ( _string_length ( out6_abbreviated ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out6_abbreviated , string_length ( string ) ) );
    string_destroy ( string );
    const char* out7 = "0xB2C3";
    string = string_format ( "%@" , address_in );
    EXPECT_EQ ( _string_length ( out7 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out7 , string_length ( string ) ) );
    string_destroy ( string );
    string = string_format ( "%s" , const_string_in );
    EXPECT_EQ ( _string_length ( const_string_in ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , const_string_in , string_length ( string ) ) );
    string_destroy ( string );
    string = string_format ( "%s%S" , string_in , string_in );
    EXPECT_EQ ( 2 * string_length ( string_in ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , string_in , string_length ( string_in ) ) );
    EXPECT ( memory_equal ( string + string_length ( string_in ) , string_in , string_length ( string_in ) ) );
    string_destroy ( string );
    char* really_long_string_in = _string_create ( 1000 * STACK_STRING_MAX_SIZE );
    for ( u64 i = 0; i < string_length ( really_long_string_in ); ++i )
    {
        really_long_string_in[ i ] = random2 ( 0 , 255 );
    }
    string = string_format ( "%S%S%S%S%S%S%S%S%S%S" , really_long_string_in , really_long_string_in , really_long_string_in , really_long_string_in , really_long_string_in , really_long_string_in , really_long_string_in , really_long_string_in , really_long_string_in , really_long_string_in );
    EXPECT_EQ ( 10 * string_length ( really_long_string_in ) , string_length ( string ) );
    for ( u64 i = 0; i < 10; ++i )
    {
        EXPECT ( memory_equal ( string + i * string_length ( really_long_string_in ) , really_long_string_in , string_length ( really_long_string_in ) ) );
    }
    string_destroy ( string );
    string_destroy ( really_long_string_in );
    const char* illegal_padding_string_in1 = "`%p .3f`";
    string = string_format ( illegal_padding_string_in1 , &float_in1 );
    EXPECT_EQ ( _string_length ( illegal_padding_string_in1 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , illegal_padding_string_in1 , string_length ( string ) ) );
    string_destroy ( string );
    const char* illegal_padding_string_in4 = "`%pr 0.3f`";
    string = string_format ( illegal_padding_string_in4 , &float_in1 );
    EXPECT_EQ ( _string_length ( illegal_padding_string_in4 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , illegal_padding_string_in4 , string_length ( string ) ) );
    string_destroy ( string );
    const char* out8 = "`000000000000000000000000000000000000000000000000000000000000000000000-100098.789`";
    string = string_format ( "`%pl080.3f`" , &float_in1 );
    EXPECT_EQ ( _string_length ( out8 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out8 , string_length ( string ) ) );
    string_destroy ( string );
    const char* out9 = "`-100098.789000000000000000000000000000000000000000000000000000000000000000000000`";
    string = string_format ( "`%pr080.3f`" , &float_in1 );
    EXPECT_EQ ( _string_length ( out9 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out9 , string_length ( string ) ) );
    string_destroy ( string );
    const char* out10 = "`\n\n\n-100098.789`";
    string = string_format ( "`%pl\n14.3f`" , &float_in1 );
    EXPECT_EQ ( _string_length ( out10 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out10 , string_length ( string ) ) );
    string_destroy ( string );
    string = string_format ( "%+s" , string_in );
    EXPECT_EQ ( _string_length ( string_in ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , string_in , string_length ( string ) ) );
    string_destroy ( string );
    string = string_format ( "%-s" , string_in );
    EXPECT_EQ ( _string_length ( string_in ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , string_in , string_length ( string ) ) );
    string_destroy ( string );
    const char* out11 = "0xB2C3ljldkb]l-045[pwrsg439p80tu[]";
    string = string_format ( "%-.7@ljldkb]l-045[pwrsg439p80tu[]" , address_in );
    EXPECT_EQ ( _string_length ( out11 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out11 , string_length ( string ) ) );
    string_destroy ( string );
    const char* out12 = "+23428476892";
    string = string_format ( "%+i" , integer_in4 );
    EXPECT_EQ ( _string_length ( out12 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out12 , string_length ( string ) ) );
    string_destroy ( string );
    const char* out13 = "0x0";
    string = string_format ( "%@" , 0 );
    EXPECT_EQ ( _string_length ( out13 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out13 , string_length ( string ) ) );
    string_destroy ( string );
    const char* out14 = "%.2.8f";
    string = string_format ( out14 , &float_in1 );
    EXPECT_EQ ( _string_length ( out14 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out14 , string_length ( string ) ) );
    string_destroy ( string );
    const char* out15 = "%+-iSsfa\\n\nm``lpmr2kl\r\t";
    string = string_format ( out15 , &integer_in1 );
    EXPECT_EQ ( _string_length ( out15 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out15 , string_length ( string ) ) );
    string_destroy ( string );
    const char* out16 = "%plr0i";
    string = string_format ( out16 , &integer_in1 );
    EXPECT_EQ ( _string_length ( out16 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out16 , string_length ( string ) ) );
    string_destroy ( string );
    const char* out17 = "%pl 190234.6+pr190234i";
    string = string_format ( out17 , &integer_in1 );
    EXPECT_EQ ( _string_length ( out17 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out17 , string_length ( string ) ) );
    string_destroy ( string );
    const char* out18 = "789357300";
    string = string_format ( "%.9d" , &float_in1 );
    EXPECT_EQ ( _string_length ( out18 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out18 , string_length ( string ) ) );
    string_destroy ( string );
    const char* out19 = "-1.000988E+05";
    string = string_format ( "%e" , &float_in1 );
    EXPECT_EQ ( _string_length ( out19 ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , out19 , string_length ( string ) ) );
    string_destroy ( string );
    string = string_format ( "%Pl\t5S" , string_in );
    EXPECT_EQ ( _string_length ( string_in ) , string_length ( string ) );
    EXPECT ( memory_equal ( string , string_in , string_length ( string ) ) );
    string_destroy ( string );
    string_destroy ( string_in );
    return true;
}

void
test_register_string
( void )
{
    test_register ( test_string_allocate_and_free , "Allocating memory for a fixed-length string." );
    test_register ( test_string_create_and_destroy , "Allocating memory for a mutable string data structure." );
    test_register ( test_string_push , "Testing string 'push' operation." );
    test_register ( test_string_insert_and_remove , "Testing string 'insert' and 'remove' operations." );
    test_register ( test_string_insert_and_remove_random , "Testing string 'insert' and 'remove' operations with random indices and elements." );
    test_register ( test_string_trim , "Testing string 'trim' operation." );
    test_register ( test_string_contains , "Testing string 'contains' operation." );
    test_register ( test_string_reverse , "Testing string in-place 'reverse' operation." );
    test_register ( test_string_u64_and_i64 , "Testing 'stringify' operation on 64-bit integers." );
    test_register ( test_string_f64 , "Testing 'stringify' operation on 64-bit floating point numbers." );
    test_register ( test_string_format , "Constructing a string using format specifiers." );
}