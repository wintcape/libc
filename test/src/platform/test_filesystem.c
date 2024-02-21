/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file memory/test_linear_allocator.c
 * @brief Implementation of the memory/test_linear_allocator header.
 * (see memory/test_linear_allocator.h for additional details)
 */
#include "platform/test_filesystem.h"

#include "test/expect.h"

#include "container/string.h"

#include "core/memory.h"

#define FILE_NAME_TEST_DNE             "test/assets/file-dne"
#define FILE_NAME_TEST_IN_FILE         "test/assets/in-file.txt"
#define FILE_NAME_TEST_IN_FILE_EMPTY   "test/assets/in-file-empty.txt"
#define FILE_NAME_TEST_IN_FILE_BINARY  "test/assets/in-file-binary"
#define FILE_NAME_TEST_OUT_FILE        "test/assets/out-file"

static const char* file_content_test_in_file = "This is a file with\nthree lines and 50\ncharacters.";
static const i8 file_content_test_in_file_binary[] = { 89 , 44 , 7 , -63 , 107 , -29 , 125 , -104 , -114 , -98 , -101 , -21 , -96 , -103 , 92 , 47 , 52 , 31 , 107 , -60 , -18 , -64 , 41 , 120 , -76 , -20 , -2 , -57 , 40 , 29 , 4 , -66 , 117 , -96 , 121 , 32 , -80 , -90 , 54 , 14 , 0 , -77 , -4 , -104 , -76 , -83 , -58 , 36 , -69 , 55 };

u8
test_file_exists
( void )
{
    EXPECT_NOT ( file_exists ( FILE_NAME_TEST_DNE , FILE_MODE_ACCESS ) );
    EXPECT ( file_exists ( FILE_NAME_TEST_IN_FILE , FILE_MODE_ACCESS ) );
    EXPECT ( file_exists ( FILE_NAME_TEST_IN_FILE_EMPTY , FILE_MODE_ACCESS ) );
    EXPECT ( file_exists ( FILE_NAME_TEST_IN_FILE_BINARY , FILE_MODE_ACCESS ) );
    return true;
}

u8
test_file_open_and_close
( void )
{
    file_t file;
    EXPECT ( file_open ( FILE_NAME_TEST_IN_FILE , FILE_MODE_READ , false , &file ) );
    EXPECT_NEQ ( 0 , file.handle );
    EXPECT ( file.valid );
    file_close ( &file );
    EXPECT_EQ ( 0 , file.handle );
    EXPECT_NOT ( file.valid );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , false , &file ) );
    EXPECT_NEQ ( 0 , file.handle );
    EXPECT ( file.valid );
    file_close ( &file );
    EXPECT_EQ ( 0 , file.handle );
    EXPECT_NOT ( file.valid );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_READ | FILE_MODE_WRITE , false , &file ) );
    EXPECT_NEQ ( 0 , file.handle );
    EXPECT ( file.valid );
    file_close ( &file );
    EXPECT_EQ ( 0 , file.handle );
    EXPECT_NOT ( file.valid );
    EXPECT ( file_open ( FILE_NAME_TEST_IN_FILE_BINARY , FILE_MODE_READ , true , &file ) );
    EXPECT_NEQ ( 0 , file.handle );
    EXPECT ( file.valid );
    file_close ( &file );
    EXPECT_EQ ( 0 , file.handle );
    EXPECT_NOT ( file.valid );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , true , &file ) );
    EXPECT_NEQ ( 0 , file.handle );
    EXPECT ( file.valid );
    file_close ( &file );
    EXPECT_EQ ( 0 , file.handle );
    EXPECT_NOT ( file.valid );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_READ | FILE_MODE_WRITE , true , &file ) );
    EXPECT_NEQ ( 0 , file.handle );
    EXPECT ( file.valid );
    file_close ( &file );
    EXPECT_EQ ( 0 , file.handle );
    EXPECT_NOT ( file.valid );
    return true;
}

u8
test_file_read
( void )
{
    char buffer[ 100 ];
    file_t file;
    u64 read;
    memory_clear ( buffer , 100 );
    EXPECT ( file_open ( FILE_NAME_TEST_IN_FILE_EMPTY , FILE_MODE_READ , false , &file ) );
    EXPECT ( file_read ( &file , 100 , buffer , &read ) );
    EXPECT_EQ ( 0 , read );
    file_close ( &file );
    memory_clear ( buffer , 100 );
    EXPECT ( file_open ( FILE_NAME_TEST_IN_FILE , FILE_MODE_READ , false , &file ) );
    EXPECT ( file_read ( &file , 100 , buffer , &read ) );
    EXPECT_EQ ( _string_length ( file_content_test_in_file ) , read );
    EXPECT ( memory_equal ( buffer , file_content_test_in_file , read ) );
    file_close ( &file );
    memory_clear ( buffer , 100 );
    EXPECT ( file_open ( FILE_NAME_TEST_IN_FILE_BINARY , FILE_MODE_READ , true , &file ) );
    EXPECT ( file_read ( &file , 100 , buffer , &read ) );
    EXPECT_EQ ( sizeof ( file_content_test_in_file_binary ) , read );
    EXPECT ( memory_equal ( buffer , file_content_test_in_file_binary , read ) );
    file_close ( &file );
    return true;
}

u8
test_file_write
( void )
{
    char buffer[ 100 ];
    file_t file;
    u64 written;
    u64 read;
    memory_clear ( buffer , 100 );
    memory_copy ( buffer , file_content_test_in_file , _string_length ( file_content_test_in_file ) );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , false , &file ) );
    EXPECT ( file_write ( &file , _string_length ( file_content_test_in_file ) , buffer , &written ) );
    EXPECT_EQ ( _string_length ( file_content_test_in_file ) , written );
    file_close ( &file );
    memory_clear ( buffer , 100 );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_READ , false , &file ) );
    EXPECT ( file_read ( &file , 100 , buffer , &read ) );
    EXPECT_EQ ( _string_length ( file_content_test_in_file ) , read );
    EXPECT ( memory_equal ( buffer , file_content_test_in_file , read ) );
    file_close ( &file );
    memory_clear ( buffer , 100 );
    memory_copy ( buffer , file_content_test_in_file_binary , sizeof ( file_content_test_in_file_binary ) );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , true , &file ) );
    EXPECT ( file_write ( &file , sizeof ( file_content_test_in_file_binary ) , buffer , &written ) );
    EXPECT_EQ ( sizeof ( file_content_test_in_file_binary ) , written );
    file_close ( &file );
    memory_clear ( buffer , 100 );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_READ , true , &file ) );
    EXPECT ( file_read ( &file , 100 , buffer , &read ) );
    EXPECT_EQ ( sizeof ( file_content_test_in_file_binary ) , read );
    EXPECT ( memory_equal ( buffer , file_content_test_in_file_binary , read ) );
    file_close ( &file );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , false , &file ) );
    file_close ( &file );
    return true;
}

u8
test_file_read_line
( void )
{
    file_t file;
    u64 written;
    const u64 max_line_length = MEGABYTES ( 1 );
    const u64 line_count = 100;
    char* in_lines[ 100 ];
    char* out_lines[ 100 ];
    for ( u64 i = 0; i < line_count; ++i )
    {
        in_lines[ i ] = string_create ();
        EXPECT_NEQ ( 0 , in_lines[ i ] );
    }
    memory_clear ( out_lines , sizeof ( char* ) * 100 );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , false , &file ) );
    for ( u64 i = 0; i < line_count; ++i )
    {
        const u64 length = random2 ( STACK_STRING_MAX_SIZE + 1
                                   , max_line_length
                                   );
        for ( u64 j = 0; j < length; ++j )
        {
            _string_push ( in_lines[ i ] , string_char ( random2 ( 32 , 126 ) ) );
        }
        _string_push ( in_lines[ i ] , string_char ( '\n' ) );
        EXPECT ( file_write ( &file , string_length ( in_lines[ i ] ) , in_lines[ i ] , &written ) );
        EXPECT_EQ ( string_length ( in_lines[ i ] ) , written );
    }
    file_close ( &file );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_READ , false , &file ) );
    for ( u64 i = 0; i < line_count; ++i )
    {
        EXPECT ( file_read_line ( &file , &out_lines[ i ] ) );
        EXPECT_NEQ ( 0 , out_lines[ i ] );
        EXPECT_EQ ( string_length ( in_lines[ i ] ) , string_length ( out_lines[ i ]  ) );
        EXPECT ( memory_equal ( in_lines[ i ] , out_lines[ i ]  , string_length ( out_lines[ i ]  ) ) );
    }
    file_close ( &file );
    for ( u64 i = 0; i < line_count; ++i )
    {
        string_destroy ( in_lines[ i ] );
        string_destroy ( out_lines[ i ] );
    }
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , false , &file ) );
    file_close ( &file );
    return true;
}

u8
test_file_write_line
( void )
{
    const char newline = '\n';
    char buffer[ 100 ];
    file_t file;
    u64 written;
    u64 read;
    memory_clear ( buffer , 100 );
    memory_copy ( buffer , file_content_test_in_file , _string_length ( file_content_test_in_file ) );
    buffer[ _string_length ( file_content_test_in_file ) ] = newline;
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , false , &file ) );
    EXPECT ( _file_write_line ( &file , buffer , &written ) );
    EXPECT_EQ ( _string_length ( file_content_test_in_file ) + sizeof ( char ) + 1 , written );
    file_close ( &file );
    memory_clear ( buffer , 100 );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_READ , false , &file ) );
    EXPECT ( file_read ( &file , 100 , buffer , &read ) );
    EXPECT_EQ ( _string_length ( file_content_test_in_file ) + sizeof ( char ) + 1 , read );
    EXPECT ( memory_equal ( buffer , file_content_test_in_file , read - 2 ) );
    EXPECT ( memory_equal ( &buffer[ read - 2 ] , &newline , sizeof ( char ) ) );
    file_close ( &file );
    memory_clear ( buffer , 100 );
    memory_copy ( buffer , file_content_test_in_file_binary , sizeof ( file_content_test_in_file_binary ) );
    buffer[ sizeof ( file_content_test_in_file_binary ) ] = newline;
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , true , &file ) );
    EXPECT ( file_write_line ( &file , sizeof ( file_content_test_in_file_binary ) + sizeof ( char ) , buffer , &written ) );
    EXPECT_EQ ( sizeof ( file_content_test_in_file_binary ) + sizeof ( char ) + 1 , written );
    file_close ( &file );
    memory_clear ( buffer , 100 );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_READ , true , &file ) );
    EXPECT ( file_read ( &file , 100 , buffer , &read ) );
    EXPECT_EQ ( sizeof ( file_content_test_in_file_binary ) + sizeof ( char ) + 1 , read );
    EXPECT ( memory_equal ( buffer , file_content_test_in_file_binary , read - 2 ) );
    EXPECT ( memory_equal ( &buffer[ read - 2 ] , &newline , sizeof ( char ) ) );
    file_close ( &file );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , false , &file ) );
    file_close ( &file );
    return true;
}

u8
test_file_read_all
( void )
{
    const u64 file_size_ = GIGABYTES ( 1 );
    f64 amount;
    const char* unit = string_bytesize ( file_size_ , &amount );
    u8* content = ( u8* ) string_allocate ( file_size_ );
    EXPECT_NEQ ( 0 , content );
    LOGDEBUG ( "Generating %.2f %s of random data. . ." , &amount , unit );
    for ( u64 i = 0; i < file_size_ / sizeof ( u64 ); i += sizeof ( u64 ) )
    {
        ( ( u64* ) content )[ i ] = random64 ();
    }
    LOGDEBUG ( "  Done." );
    u8* file_content = 0;
    file_t file;
    u64 written;
    u64 read;
    LOGDEBUG ( "Writing it to a file on the host platform. . ." );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , true , &file ) );
    EXPECT ( file_write ( &file , file_size_ , content , &written ) );
    EXPECT_EQ ( file_size_ , file_size ( &file ) );
    file_close ( &file );
    LOGDEBUG ( "  Done." );
    LOGDEBUG ( "Reading it back into program memory. . ." );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_READ , true , &file ) );
    EXPECT ( file_read_all ( &file , &file_content , &read ) );
    EXPECT_NEQ ( 0 , file_content );
    EXPECT_EQ ( file_size ( &file ) , read );
    EXPECT ( memory_equal ( file_content , content , file_size_ ) );
    file_close ( &file );
    LOGDEBUG ( "  Done." );
    string_free ( content );
    string_free ( file_content );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , false , &file ) );
    file_close ( &file );
    return true;
}

void
test_register_filesystem
( void )
{
    test_register ( test_file_exists , "Querying the host platform for the existence of a file." );
    test_register ( test_file_open_and_close , "Opening or closing a file on the host platform." );
    test_register ( test_file_read , "Reading a file on the host platform into a local buffer." );
    test_register ( test_file_write , "Writing from a local buffer to a file on the host platform." );
    test_register ( test_file_read_line , "Reading a line of text from a file on the host platform." );
    test_register ( test_file_write_line , "Writing a line of text to a file on the host platform." );
    test_register ( test_file_read_all , "Reading the entire contents of a file on the host platform into program memory." );
}