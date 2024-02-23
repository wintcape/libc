/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file platform/filesystem.c
 * @brief Implementation of the platform/filesystem header.
 * (see platform/filesystem.h for additional details)
 */
#include "platform/filesystem.h"

#include "container/string.h"

#include "core/logger.h"
#include "core/memory.h"

#include "math/math.h"

#include "platform/platform.h"

// Standard libc dependencies.
#include <stdio.h>

bool
file_exists
(   const char* path
,   FILE_MODE   mode
)
{
    return platform_file_exists ( path , mode );
}

bool
file_open
(   const char* path
,   FILE_MODE   mode_
,   bool        binary
,   file_t*     file_
)
{
    ( *file_ ).valid = false;
    ( *file_ ).handle = 0;

    // Interpret mode argument.
    const char* mode;
    if ( ( mode_ & FILE_MODE_READ ) && ( mode_ & FILE_MODE_WRITE ) )
    {
        if ( binary )
        {
            mode = "w+b";
        }
        else
        {
            mode = "w+";
        }
    }
    else if ( ( mode_ & FILE_MODE_READ ) && !( mode_ & FILE_MODE_WRITE ) )
    {
        if ( binary )
        {
            mode = "rb";
        }
        else
        {
            mode = "r";
        }
    }
    else if ( !( mode_ & FILE_MODE_READ ) && ( mode_ & FILE_MODE_WRITE ) )
    {
        if ( binary )
        {
            mode = "wb";
        }
        else
        {
            mode = "w";
        }
    }
    else
    {
        LOGERROR ( "file_open: Invalid file mode." );
        return false;
    }

    FILE* file = fopen ( path , mode );
    if ( !file )
    {
        const i64 error = platform_error_code ();
        char message[ STACK_STRING_MAX_SIZE ];
        platform_error_message ( error , message , STACK_STRING_MAX_SIZE ); 
        LOGERROR ( "file_open: fopen failed for filepath: %s.\n\t           Reason: %s.\n\t           Code:   %i"
                 , path
                 , message
                 , error
                 );
        return false;
    }
    
    ( *file_ ).handle = file;
    ( *file_ ).valid = true;
    return true;
}

void
file_close
(   file_t* file_
)
{
    if ( !( *file_ ).handle || !( *file_ ).valid )
    {
        return;
    }
    FILE* file = ( *file_ ).handle;
    ( *file_ ).valid = false;
    if ( fclose ( file ) )
    {
        const i64 error = platform_error_code ();
        char message[ STACK_STRING_MAX_SIZE ];
        platform_error_message ( error , message , STACK_STRING_MAX_SIZE ); 
        LOGERROR ( "file_close: fclose failed for file %@.\n\t            Reason: %s.\n\t            Code:   %i"
                 , ( *file_ ).handle
                 , message
                 , error
                 );
    }
    ( *file_ ).handle = 0;
}

u64
file_size
(   file_t* file
)
{
    if ( !( *file ).handle || !( *file ).valid )
    {
        return 0;
    }
    fseek ( ( *file ).handle , 0 , SEEK_END );
    const u64 size = ftell ( ( *file ).handle );
    rewind ( ( *file ).handle );
    return size;
}

bool
file_read
(   file_t* file_
,   u64     size
,   void*   dst
,   u64*    read
)
{    
    if ( !( *file_ ).handle || !( *file_ ).valid || !dst )
    {
        return false;
    }
    FILE* file = ( *file_ ).handle;
    *read = fread ( dst , 1 , size , file );
    if ( ferror ( file ) )
    {
        const i64 error = platform_error_code ();
        char message[ STACK_STRING_MAX_SIZE ];
        platform_error_message ( error , message , STACK_STRING_MAX_SIZE ); 
        LOGERROR ( "file_read: fread failed for file %@.\n\t           Reason: %s.\n\tCode:   %i"
                 , file
                 , message
                 , error
                 );
        return false;
    }
    return true;
}

bool
file_read_line
(   file_t* file_
,   char**  dst
)
{
    if ( !( *file_ ).handle || !( *file_ ).valid  )
    {
        return false;
    }
    FILE* file = ( *file_ ).handle;
    char buffer[ STACK_STRING_MAX_SIZE ];
    char* string = string_create ();
    while ( fgets ( buffer , STACK_STRING_MAX_SIZE , file ) )
    {
        string_push ( string
                    , buffer
                    , _string_length_clamped ( buffer , STACK_STRING_MAX_SIZE )
                    );
        if ( newline ( string[ string_length ( string ) - 1 ] ) )
        {
            break;
        }
    }
    *dst = string;
    if ( ferror ( file ) )
    {
        const i64 error = platform_error_code ();
        char message[ STACK_STRING_MAX_SIZE ];
        platform_error_message ( error , message , STACK_STRING_MAX_SIZE ); 
        LOGERROR ( "file_read_line: fgets failed for file %@.\n\t                Reason: %s.\n\t                Code:   %i"
                 , file
                 , message
                 , error
                 );
        return false;
    }
    return true;
}

bool
file_read_all
(   file_t* file_
,   u8**    dst
,   u64*    read
)
{
    if ( !( *file_ ).handle || !( *file_ ).valid || !dst )
    {
        return false;
    }
    const u64 size = file_size ( file_ );
    u8* string = ( u8* ) string_allocate ( sizeof ( u8 ) * size );
    FILE* file = ( *file_ ).handle;
    *read = fread ( string , 1 , size , file );
    string[ size ] = 0; // Append terminator.
    *dst = string;
    if ( ferror ( file ) )
    {
        const i64 error = platform_error_code ();
        char message[ STACK_STRING_MAX_SIZE ];
        platform_error_message ( error , message , STACK_STRING_MAX_SIZE ); 
        LOGERROR ( "file_read_all: fread failed for file %@.\n\t               Reason: %s.\n\t               Code:   %i"
                 , file
                 , message
                 , error
                 );
        return false;
    }
    return *read == size;
}

bool
file_write
(   file_t*     file_
,   u64         size
,   const void* src
,   u64*        written
)
{
    if ( !( *file_ ).handle || !( *file_ ).valid )
    {
        return false;
    }
    FILE* file = ( *file_ ).handle;
    *written = fwrite ( src , 1 , size , file );
    fflush ( file );
    if ( ferror ( file ) )
    {
        const i64 error = platform_error_code ();
        char message[ STACK_STRING_MAX_SIZE ];
        platform_error_message ( error , message , STACK_STRING_MAX_SIZE ); 
        LOGERROR ( "file_write: fwrite failed for file %@.\n\t            Reason: %s.\n\t            Code:   %i"
                 , file
                 , message
                 , error
                 );
        return false;
    }
    return *written == size;
}

bool
file_write_line
(   file_t*     file_
,   u64         size
,   const char* src
,   u64*        written
)
{
    if ( !( *file_ ).handle || !( *file_ ).valid )
    {
        return false;
    }
    FILE* file = ( *file_ ).handle;
    *written = fwrite ( src , sizeof ( char ) , size , file )
             + fwrite ( string_char ( '\n' ) , sizeof ( char ) , 1 , file )
             ;
    fflush ( file );
    if ( ferror ( file ) )
    {
        const i64 error = platform_error_code ();
        char message[ STACK_STRING_MAX_SIZE ];
        platform_error_message ( error , message , STACK_STRING_MAX_SIZE ); 
        LOGERROR ( "file_write_line: fwrite failed for file %@.\n\t                 Reason: %s.\n\t                 Code:   %i"
                 , file
                 , message
                 , error
                 );
        return false;
    }
    return *written == size + sizeof ( char );
}

void
file_stdin
(   file_t* file
)
{
    ( *file ).handle = stdin;
    ( *file ).valid = true;
}

void
file_stdout
(   file_t* file
)
{
    ( *file ).handle = stdout;
    ( *file ).valid = true;
}

void
file_stderr
(   file_t* file
)
{
    ( *file ).handle = stderr;
    ( *file ).valid = true;
}