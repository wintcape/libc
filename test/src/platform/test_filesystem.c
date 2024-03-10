/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file memory/test_linear_allocator.c
 * @brief Implementation of the memory/test_linear_allocator header.
 * (see memory/test_linear_allocator.h for additional details)
 * 
 * TODO: If one of these fails, they all fail because of a broken pipe;
 * when the test exits early, the file is not closed. Fix this.
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
    // TEST: file_exists fails on a filepath at which a file does not exist.
    EXPECT_NOT ( file_exists ( FILE_NAME_TEST_DNE , FILE_MODE_ACCESS ) );

    // TEST: file_exists succeeds, verifying the existence of all input files that will be used for subsequent tests.
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

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: file_open logs an error and fails if no path is provided.
    EXPECT_NOT ( file_open ( 0 , FILE_MODE_READ , &file ) );

    // TEST: file_open logs an error and fails if no output buffer is provided for the file.
    EXPECT_NOT ( file_open ( FILE_NAME_TEST_IN_FILE , FILE_MODE_READ , 0 ) );

    // TEST: file_open logs an error and fails if the provided file mode is invalid.
    EXPECT_NOT ( file_open ( FILE_NAME_TEST_IN_FILE , 0 , &file ) );

    // TEST: file_open succeeds in read mode on the first input file (null-terminated text file).
    EXPECT ( file_open ( FILE_NAME_TEST_IN_FILE , FILE_MODE_READ , &file ) );

    // TEST: File contains a valid file handle following successful file_open.
    EXPECT_NEQ ( 0 , file.handle );

    // TEST: File is valid following successful file_open.
    EXPECT ( file.valid );

    // Verify input file is not empty prior to additional testing.
    EXPECT_NEQ ( 0 , file_size ( &file ) );

    file_close ( &file );

    // TEST: File handle is null following file_close.
    EXPECT_EQ ( 0 , file.handle );

    // TEST: File handle is invalidated following file_close.
    EXPECT_NOT ( file.valid );

    // TEST: file_open succeeds in read mode on the second input file (binary file).
    EXPECT ( file_open ( FILE_NAME_TEST_IN_FILE_BINARY , FILE_MODE_READ , &file ) );

    // TEST: File contains a valid file handle following successful file_open.
    EXPECT_NEQ ( 0 , file.handle );

    // TEST: File is valid following successful file_open.
    EXPECT ( file.valid );

    // Verify input file is not empty prior to additional testing.
    EXPECT_NEQ ( 0 , file_size ( &file ) );

    file_close ( &file );

    // TEST: File handle is null following file_close.
    EXPECT_EQ ( 0 , file.handle );

    // TEST: File handle is invalidated following file_close.
    EXPECT_NOT ( file.valid );
    
    // TEST: file_open succeeds in read mode on the third input file (empty file).
    EXPECT ( file_open ( FILE_NAME_TEST_IN_FILE_EMPTY , FILE_MODE_READ , &file ) );

    // TEST: File contains a valid file handle following successful file_open.
    EXPECT_NEQ ( 0 , file.handle );

    // TEST: File is valid following successful file_open.
    EXPECT ( file.valid );

    // Verify (empty) input file **is** empty prior to additional testing.
    EXPECT_EQ ( 0 , file_size ( &file ) );

    file_close ( &file );

    // TEST: File handle is null following file_close.
    EXPECT_EQ ( 0 , file.handle );

    // TEST: File handle is invalidated following file_close.
    EXPECT_NOT ( file.valid );

    // TEST: file_open succeeds in write mode on the output file.
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , &file ) );

    // TEST: File contains a valid file handle following successful file_open.
    EXPECT_NEQ ( 0 , file.handle );

    // TEST: File is valid following successful file_open.
    EXPECT ( file.valid );

    // Verify output file has been truncated by opening in write mode.
    EXPECT_EQ ( 0 , file_size ( &file ) );

    file_close ( &file );

    // TEST: File handle is null following file_close.
    EXPECT_EQ ( 0 , file.handle );

    // TEST: File handle is invalidated following file_close.
    EXPECT_NOT ( file.valid );

    // TEST: file_open succeeds in read mode on the output file.
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_READ , &file ) );

    // TEST: File contains a valid file handle following successful file_open.
    EXPECT_NEQ ( 0 , file.handle );

    // TEST: File is valid following successful file_open.
    EXPECT ( file.valid );

    file_close ( &file );

    // TEST: File handle is null following file_close.
    EXPECT_EQ ( 0 , file.handle );

    // TEST: File handle is invalidated following file_close.
    EXPECT_NOT ( file.valid );

    // TEST: file_open succeeds in read+write mode on the output file.
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_READ | FILE_MODE_WRITE , &file ) );

    // TEST: File contains a valid file handle following successful file_open.
    EXPECT_NEQ ( 0 , file.handle );

    // TEST: File is valid following successful file_open.
    EXPECT ( file.valid );

    file_close ( &file );

    // TEST: File handle is null following file_close.
    EXPECT_EQ ( 0 , file.handle );

    // TEST: File handle is invalidated following file_close.
    EXPECT_NOT ( file.valid );

    // Open one of the non-empty input files in read+write mode for testing.
    EXPECT ( file_open ( FILE_NAME_TEST_IN_FILE_BINARY , FILE_MODE_READ | FILE_MODE_WRITE , &file ) );
    
    // Verify success of file_open prior to testing.
    EXPECT_NEQ ( 0 , file.handle ); 
    EXPECT ( file.valid );

    // TEST: file_open does not truncate a non-empty file opened in read+write mode.
    EXPECT_NEQ ( 0 , file_size ( &file ) );

    file_close ( &file );

    // Verify success of file_close.
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

    EXPECT ( file_open ( FILE_NAME_TEST_IN_FILE_EMPTY , FILE_MODE_READ , &file ) );

    LOGWARN ( "The following errors are intentionally triggered by a test:" );

    // TEST: file_read fails if no file is provided.
    EXPECT_NOT ( file_read ( 0 , 100 , buffer , &read ) );

    // TEST: file_read fails if invalid file is provided.
    file_t invalid_file;
    invalid_file.valid = false;
    invalid_file.handle = 0;
    EXPECT_NOT ( file_read ( &invalid_file , 100 , buffer , &read ) );

    // TEST: file_read fails if no output buffer for file content is provided.
    EXPECT_NOT ( file_read ( &file , 100 , 0 , &read ) );

    // TEST: file_read fails if no output buffer for number of bytes read is provided.
    EXPECT_NOT ( file_read ( &file , 100 , buffer , 0 ) );

    file_close ( &file );

    // TEST: file_read fails if file is not open for read.
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , &file ) );
    EXPECT_NOT ( file_read ( &file , 100 , buffer , &read ) );
    file_close ( &file );

    // Empty file.
    EXPECT ( file_open ( FILE_NAME_TEST_IN_FILE_EMPTY , FILE_MODE_READ , &file ) );

    // TEST: file_read if file is empty.
    read = 1;
    memory_clear ( buffer , 100 );
    EXPECT ( file_read ( &file , 100 , buffer , &read ) );

    // TEST: file_read reads 0 bytes if file is empty.
    EXPECT_EQ ( 0 , read );

    file_close ( &file );

    // Text-file.
    EXPECT ( file_open ( FILE_NAME_TEST_IN_FILE , FILE_MODE_READ , &file ) );

    // TEST: file_read succeeds if buffer size is 0.
    read = 1;
    memory_clear ( buffer , 100 );
    EXPECT ( file_read ( &file , 0 , buffer , &read ) );

    // TEST: file_read reads 0 bytes if buffer size is 0.
    EXPECT_EQ ( 0 , read );

    // TEST: Given an adequately-sized buffer, file_read reads the entire file contents at once (assuming file position is at the beginning of the file).
    read = 1;
    memory_clear ( buffer , 100 );
    EXPECT ( file_read ( &file , 100 , buffer , &read ) );
    EXPECT_EQ ( _string_length ( file_content_test_in_file ) , read );
    EXPECT ( memory_equal ( buffer , file_content_test_in_file , _string_length ( file_content_test_in_file ) + 1 ) );
    
    file_close ( &file );

    // Binary-file.
    EXPECT ( file_open ( FILE_NAME_TEST_IN_FILE_BINARY , FILE_MODE_READ , &file ) );
    
    // TEST: Given an adequately-sized buffer, file_read reads the entire file contents at once (assuming file position is at the beginning of the file).
    read = 0;
    memory_clear ( buffer , 100 );
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
    LOGWARN ( "The following errors are intentionally triggered by a test:" );
    file_t invalid_file;
    invalid_file.valid = false;
    invalid_file.handle = 0;
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , &file ) );
    EXPECT_NOT ( _file_write ( 0 , file_content_test_in_file , &written ) );
    EXPECT_NOT ( _file_write ( &invalid_file , file_content_test_in_file , &written ) );
    EXPECT_NOT ( file_write ( &file , 100 , 0 , &written ) );
    EXPECT_NOT ( _file_write ( &file , file_content_test_in_file , 0 ) );
    file_close ( &file );
    EXPECT ( file_open ( FILE_NAME_TEST_IN_FILE_EMPTY , FILE_MODE_READ , &file ) );
    EXPECT_NOT ( file_write ( &file , 100 , buffer , &written ) );
    file_close ( &file );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , &file ) );
    written = 0;
    EXPECT ( _file_write ( &file , file_content_test_in_file , &written ) );
    EXPECT_EQ ( _string_length ( file_content_test_in_file ) , written );
    written = 0;
    EXPECT ( file_write ( &file , 0 , file_content_test_in_file , &written ) );
    EXPECT_EQ ( 0 , written );
    file_close ( &file );
    memory_clear ( buffer , 100 );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_READ , &file ) );
    EXPECT ( file_read ( &file , 100 , buffer , &read ) );
    EXPECT_EQ ( _string_length ( file_content_test_in_file ) , read );
    EXPECT ( memory_equal ( buffer , file_content_test_in_file , _string_length ( file_content_test_in_file ) + 1 ) );
    file_close ( &file );
    memory_clear ( buffer , 100 );
    memory_copy ( buffer , file_content_test_in_file_binary , sizeof ( file_content_test_in_file_binary ) );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , &file ) );
    written = 0;
    EXPECT ( file_write ( &file , sizeof ( file_content_test_in_file_binary ) , buffer , &written ) );
    EXPECT_EQ ( sizeof ( file_content_test_in_file_binary ) , written );
    file_close ( &file );
    memory_clear ( buffer , 100 );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_READ , &file ) );
    EXPECT ( file_read ( &file , 100 , buffer , &read ) );
    EXPECT_EQ ( sizeof ( file_content_test_in_file_binary ) , read );
    EXPECT ( memory_equal ( buffer , file_content_test_in_file_binary , read ) );
    file_close ( &file );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , &file ) );
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
    char* out_lines[ 101 ];
    for ( u64 i = 0; i < line_count; ++i )
    {
        in_lines[ i ] = string_create ();
        EXPECT_NEQ ( 0 , in_lines[ i ] );
    }
    memory_clear ( out_lines , sizeof ( char* ) * 100 );
    LOGWARN ( "The following errors are intentionally triggered by a test:" );
    file_t invalid_file;
    invalid_file.valid = false;
    invalid_file.handle = 0;
    EXPECT ( file_open ( FILE_NAME_TEST_IN_FILE , FILE_MODE_READ , &file ) );
    EXPECT_NOT ( file_read_line ( 0 , &out_lines[ 0 ] ) );
    EXPECT_NOT ( file_read_line ( &invalid_file , &out_lines[ 0 ] ) );
    EXPECT_NOT ( file_read_line ( &file , 0 ) );
    file_close ( &file );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , &file ) );
    EXPECT_NOT ( file_read_line ( &file , &out_lines[ 0 ] ) );
    for ( u64 i = 0; i < line_count; ++i )
    {
        const u64 length = random2 ( STACK_STRING_MAX_SIZE + 1
                                   , max_line_length
                                   );
        for ( u64 j = 0; j < length; ++j )
        {
            _string_push ( in_lines[ i ] , string_char ( random2 ( 33 , 126 ) ) );
        }
        _string_push ( in_lines[ i ] , string_char ( '\n' ) );
        EXPECT ( file_write ( &file , string_length ( in_lines[ i ] ) , in_lines[ i ] , &written ) );
        EXPECT_EQ ( string_length ( in_lines[ i ] ) , written );
    }
    file_close ( &file );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_READ , &file ) );
    for ( u64 i = 0; i < line_count; ++i )
    {
        EXPECT ( file_read_line ( &file , &out_lines[ i ] ) );
        EXPECT_NEQ ( 0 , out_lines[ i ] );
        EXPECT_EQ ( string_length ( in_lines[ i ] ) - 1 , string_length ( out_lines[ i ] ) );
        EXPECT ( memory_equal ( in_lines[ i ] , out_lines[ i ]  , string_length ( out_lines[ i ] ) ) );
    }
    EXPECT ( file_read_line ( &file , &out_lines[ line_count ] ) );
    EXPECT_EQ ( 0 , string_length ( out_lines[ line_count ] ) );
    EXPECT_EQ ( 0 , *( out_lines[ line_count ] ) );
    file_close ( &file );
    for ( u64 i = 0; i < line_count; ++i )
    {
        string_destroy ( in_lines[ i ] );
        string_destroy ( out_lines[ i ] );
    }
    string_destroy ( out_lines[ line_count ] );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , &file ) );
    file_close ( &file );
    return true;
}

u8
test_file_write_line
( void )
{
    char buffer[ 100 ];
    const char* in_line = "This is the line to be written to the file.";
    char* out_line;
    file_t file;
    LOGWARN ( "The following errors are intentionally triggered by a test:" );
    file_t invalid_file;
    invalid_file.valid = false;
    invalid_file.handle = 0;
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , &file ) );
    EXPECT_NOT ( file_write_line ( 0 , 100 , buffer ) );
    EXPECT_NOT ( file_write_line ( &invalid_file , 100 , buffer ) );
    EXPECT_NOT ( file_write_line ( &file , 100 , 0 ) );
    file_close ( &file );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_READ , &file ) );
    EXPECT_NOT ( file_write_line ( &file , 100 , buffer ) );
    file_close ( &file );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , &file ) );
    memory_copy ( buffer , in_line , _string_length ( in_line ) + 1 );
    EXPECT ( _file_write_line ( &file , buffer ) );
    EXPECT ( _file_write_line ( &file , buffer ) );
    EXPECT ( file_write_line ( &file , 0 , buffer ) );
    file_close ( &file );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_READ , &file ) );
    memory_clear ( buffer , 100 );
    EXPECT ( file_read_line ( &file , &out_line ) );
    EXPECT_EQ ( _string_length ( in_line ) , string_length ( out_line ) );
    EXPECT ( memory_equal ( out_line , in_line , string_length ( out_line ) ) );
    string_destroy ( out_line );
    EXPECT ( file_read_line ( &file , &out_line ) );
    EXPECT_EQ ( _string_length ( in_line ) , string_length ( out_line ) );
    EXPECT ( memory_equal ( out_line , in_line , string_length ( out_line ) ) );
    string_destroy ( out_line );
    EXPECT ( file_read_line ( &file , &out_line ) );
    EXPECT_EQ ( 0 , string_length ( out_line ) );
    string_destroy ( out_line );
    file_close ( &file );
    return true;
}

u8
test_file_read_all
( void )
{
    const u64 filesize = MEBIBYTES ( 100 );
    file_t file;
    u64 read;
    u64 written;
    char* string_in = string_allocate ( filesize + 1 );
    u8* string_out;
    LOGWARN ( "The following errors are intentionally triggered by a test:" );
    file_t invalid_file;
    invalid_file.valid = false;
    invalid_file.handle = 0;
    EXPECT ( file_open ( FILE_NAME_TEST_IN_FILE_EMPTY , FILE_MODE_READ , &file ) );
    EXPECT_NOT ( file_read_all ( 0 , &string_out , &read ) );
    EXPECT_NOT ( file_read_all ( &invalid_file , &string_out , &read ) );
    EXPECT_NOT ( file_read_all ( &file , 0 , &read ) );
    EXPECT_NOT ( file_read_all ( &file , &string_out , 0 ) );
    EXPECT ( file_read_all ( &file , &string_out , &read ) );
    EXPECT_EQ ( 0 , read );
    EXPECT_EQ ( 0 , _string_length ( ( char* ) string_out ) );
    EXPECT_EQ ( 0 , *( string_out ) );
    string_free ( string_out );
    file_close ( &file );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , &file ) );
    EXPECT_NOT ( file_read_all ( &file , &string_out , &read ) );
    for ( u64 i = 0; i < filesize / sizeof ( u64 ); ++i )
    {
        ( ( u64* ) string_in )[ i ] = random64 ();
    }
    EXPECT ( file_write ( &file , filesize , string_in , &written ) );
    EXPECT_EQ ( filesize , written );
    file_close ( &file );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_READ , &file ) );
    EXPECT ( file_read_all ( &file , &string_out , &read ) );
    EXPECT_NEQ ( 0 , string_out );
    EXPECT_EQ ( filesize , read );
    EXPECT ( memory_equal ( string_in , string_out , read ) );
    file_close ( &file );
    string_free ( string_in );
    string_free ( string_out );
    return true;
}

u8
test_file_read_and_write_large_file
( void )
{
    const u64 buffer_size = GIBIBYTES ( 1 );
    char* in_buffer = string_allocate ( buffer_size );
    char* out_buffer = string_allocate ( buffer_size );
    file_t file;
    u64 read;
    u64 written;
    do
    {
        f64 display_amount;
        const char* display_unit = string_bytesize ( buffer_size , &display_amount );
        LOGDEBUG ( "Generating %.2f %s of random binary data to be used for stress-testing file I/O operations. . ." , &display_amount , display_unit );
    }
    while ( 0 );
    for ( u64 i = 0; i < buffer_size / sizeof ( u64 ); ++i )
    {
        ( ( u64* ) in_buffer )[ i ] = random64 ();
    }
    LOGDEBUG ( "  Done." );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , &file ) );
    do
    {
        f64 display_amount;
        const char* display_unit = string_bytesize ( buffer_size * 6, &display_amount );
        LOGDEBUG ( "Writing a %.2f %s file to disk on the host platform. . ." , &display_amount , display_unit );
    }
    while ( 0 );
    for ( u8 i = 0; i < 6; ++i )
    {
        written = 0;
        EXPECT ( file_write ( &file , buffer_size , in_buffer , &written ) );
        EXPECT_EQ ( buffer_size , written );
    }
    LOGDEBUG ( "  Done." );
    EXPECT ( file_size ( &file ) >= GIBIBYTES ( 6 ) );
    file_close ( &file );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_READ , &file ) );
    LOGDEBUG ( "Reading it back into program memory. . ." );
    for ( u8 i = 0; i < 6; ++i )
    {
        read = 0;
        memory_clear ( out_buffer , buffer_size );
        EXPECT ( file_read ( &file , buffer_size , out_buffer , &read ) );
        EXPECT_EQ ( buffer_size , read );
        EXPECT ( memory_equal ( out_buffer , in_buffer , buffer_size ) );
    }
    LOGDEBUG ( "  Done." );
    file_close ( &file );
    string_free ( in_buffer );
    string_free ( out_buffer );
    EXPECT ( file_open ( FILE_NAME_TEST_OUT_FILE , FILE_MODE_WRITE , &file ) );
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
    test_register ( test_file_read_and_write_large_file , "Testing file 'read' and 'write' operations on a file larger than 4 GiB." );
}