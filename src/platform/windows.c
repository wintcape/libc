/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file platform/windows.c
 * @brief Implementation of the platform header for Microsoft Windows
 * operating systems.
 * (see platform.h for additional details)
 * 
 * TODO: Unbuffered file I/O which handles disk alignment.
 *       Current implementation lets Windows handle this.
 */
#include "platform/platform.h"

////////////////////////////////////////////////////////////////////////////////
// Begin platform layer.
#if PLATFORM_WINDOWS == 1

#include "container/string.h"
#include "core/logger.h"
#include "core/memory.h"
#include "math/clamp.h"

// Platform layer dependencies.
#include <io.h>
#include <windows.h>
#include <windowsx.h>

// Standard libc dependencies.
#include <stdlib.h>
#include <string.h>

/** @brief Type definition for a platform-dependent file data structure. */
typedef struct
{
    HANDLE      handle;
    const char* path;
    FILE_MODE   mode;
    u64         size;
    u64         position;
}
platform_file_t;

// Global definitions for standard input, output, and error file streams.
static platform_file_t platform_stdin;  /** @brief Standard input stream handle. */
static platform_file_t platform_stdout; /** @brief Standard output stream handle. */
static platform_file_t platform_stderr; /** @brief Standard error stream handle. */

// Global system clock.
static f64              platform_clock_frequency;   /** @brief Global system clock frequency. */
static LARGE_INTEGER    platform_clock_start_time; /** @brief Global system clock start time. */

/**
 * @brief Initializes the system clock.
 */
void
platform_clock_init
( void );

void*
platform_memory_allocate
(   u64 size
)
{
    return malloc ( size );
}

void
platform_memory_free
(   void* blk
)
{
    free ( blk );
}

void*
platform_memory_clear
(   void*   blk
,   u64     size
)
{
    return memset ( blk , 0 , size );
}

void*
platform_memory_set
(   void*       blk
,   const i32   value
,   u64         size
)
{
    return memset ( blk , value , size );
}

void*
platform_memory_copy
(   void*       dst
,   const void* src
,   u64         size
)
{
    return memcpy ( dst , src , size );
}

void*
platform_memory_move
(   void*       dst
,   const void* src
,   u64         size
)
{
    return memmove ( dst , src , size );
}

bool
platform_memory_equal
(   const void* s1
,   const void* s2
,   u64         size
)
{
    return !memcmp ( s1 , s2 , size );
}

u64
platform_string_length
(   const char* string
)
{
    return strlen ( string );
}

u64
platform_string_length_clamped
(   const char* string
,   u64         limit
)
{
    return strnlen ( string , limit );
}

void
platform_array_sort
(   void*                   array
,   u64                     array_length
,   u64                     array_stride
,   comparator_function_t   comparator
)
{
    qsort ( array , array_length , array_stride , comparator );
}

bool
platform_thread_create
(   thread_start_function_t function
,   void*                   args
,   bool                    auto_detach
,   thread_t*               thread
)
{
    if ( !function || !thread )
    {
        if ( !function )
        {
            LOGERROR ( "platform_thread_create ("PLATFORM_STRING"): Missing argument: function (threaded process to run)." );
            return false;
        }
        if ( !thread )
        {
            LOGERROR ( "platform_thread_create ("PLATFORM_STRING"): Missing argument: thread (output buffer)." );
            return false;
        }
    }

    ( *thread ).internal = CreateThread ( 0
                                        , 0
                                        , ( LPTHREAD_START_ROUTINE ) function
                                        , args
                                        , 0
                                        , ( LPDWORD )( &( *thread ).id )
                                        );

    if ( !( *thread ).internal )
    {
        platform_log_error ( "platform_thread_create ("PLATFORM_STRING"): CreateThread failed." );
        return false;
    }

    if ( auto_detach )
    {
        if ( !CloseHandle ( ( *thread ).internal ) )
        {
            platform_log_error ( "platform_thread_create ("PLATFORM_STRING"): CloseHandle failed on auto-detach thread #%u."
                               , ( *thread ).id
                               );
            return false;
        }
        ( *thread ).internal = 0;
    }

    return true;
}

void
platform_thread_destroy
(   thread_t* thread
)
{
    if ( !thread || !( *thread ).internal )
    {
        return;
    }

    DWORD code;
    if ( !GetExitCodeThread ( ( *thread ).internal , &code ) )
    {
        platform_log_error ( "platform_thread_destroy ("PLATFORM_STRING"): GetExitCodeThread failed on thread #%u."
                           , ( *thread ).id
                           );
    }

    if ( !CloseHandle ( ( *thread ).internal ) )
    {
        platform_log_error ( "platform_thread_destroy ("PLATFORM_STRING"): CloseHandle failed on thread #%u."
                           , ( *thread ).id
                           );
        return;
    }

    ( *thread ).internal = 0;
    ( *thread ).id = 0;
}

void
platform_thread_detach
(   thread_t* thread
)
{
    if ( !thread || !( *thread ).internal )
    {
        return;
    }

    if ( !CloseHandle ( ( *thread ).internal ) )
    {
        platform_log_error ( "platform_thread_detach ("PLATFORM_STRING"): CloseHandle failed on thread #%u."
                           , ( *thread ).id
                           );
        return;
    }

    ( *thread ).internal = 0;
}

void
platform_thread_cancel
(   thread_t* thread
)
{
    if ( !thread || !( *thread ).internal )
    {
        return;
    }

    if ( !TerminateThread ( ( *thread ).internal , 0 ) )
    {
        platform_log_error ( "platform_thread_cancel ("PLATFORM_STRING"): TerminateThread failed on thread #%u."
                           , ( *thread ).id
                           );
        return;
    }

    ( *thread ).internal = 0;
}

bool
platform_thread_wait
(   thread_t* thread
)
{
    return platform_thread_wait_timeout ( thread , INFINITE );
}

bool
platform_thread_wait_timeout
(   thread_t*   thread
,   u64         timeout_ms
)
{
    if ( !thread || !( *thread ).internal )
    {
        return false;
    }
    return WaitForSingleObject ( ( *thread ).internal , timeout_ms ) == WAIT_OBJECT_0;
}

bool
platform_thread_active
(   thread_t* thread
)
{
    if ( !thread || !( *thread ).internal )
    {
        return false;
    }
    return WaitForSingleObject ( ( *thread ).internal , 0 ) == WAIT_TIMEOUT;
}

void
platform_thread_sleep
(   thread_t*   thread
,   u64         ms
)
{
    platform_sleep ( ms );
}

u64
platform_thread_id
( void )
{
    return GetCurrentThreadId ();
}

bool
platform_mutex_create
(   mutex_t* mutex
)
{
    if ( !mutex )
    {
        LOGERROR ( "platform_mutex_create ("PLATFORM_STRING"): Missing argument: mutex (output buffer)." );
        return false;
    }

    ( *mutex ).internal = CreateMutex ( 0 , 0 , 0 );
    if ( !( *mutex ).internal )
    {
        platform_log_error ( "platform_mutex_create ("PLATFORM_STRING"): CreateMutex failed." );
        return false;
    }

    return true;
}

void
platform_mutex_destroy
(   mutex_t* mutex
)
{
    if ( !mutex || !( *mutex ).internal )
    {
        return;
    }

    if ( !CloseHandle ( ( *mutex ).internal ) )
    {
        platform_log_error ( "platform_mutex_destroy ("PLATFORM_STRING"): CloseHandle failed on mutex %@."
                           , mutex
                           );
    }

    ( *mutex ).internal = 0;
}

bool
platform_mutex_lock
(   mutex_t* mutex
)
{
    if ( !mutex || !( *mutex ).internal )
    {
        return false;
    }

    switch ( WaitForSingleObject ( ( *mutex ).internal , INFINITE ) )
    {
        case WAIT_OBJECT_0:
        {
            return true;
        }
        case WAIT_ABANDONED:
        {
            platform_log_error ( "platform_mutex_lock ("PLATFORM_STRING"): WaitForSingleObject failed on mutex %@."
                               , mutex
                               );
            return false;
        }
    }

    return true;
}

bool
platform_mutex_unlock
(   mutex_t* mutex
)
{
    if ( !mutex || !( *mutex ).internal )
    {
        return false;
    }

    if ( !ReleaseMutex ( ( *mutex ).internal ) )
    {
        platform_log_error ( "platform_mutex_unlock ("PLATFORM_STRING"): ReleaseMutex failed on mutex %@."
                           , mutex
                           );
        return false;
    }

    return true;
}

bool
platform_file_exists
(   const char* path
,   FILE_MODE   mode_
)
{
    if ( !path )
    {
        LOGERROR ( "platform_file_exists ("PLATFORM_STRING"): Missing argument: path (filepath to test)." );
        return false;
    }

    i32 mode;
    if ( ( mode_ & FILE_MODE_READ ) && ( mode_ & FILE_MODE_WRITE ) )
    {
        mode = 6;
    }
    else if ( ( mode_ & FILE_MODE_READ ) && !( mode_ & FILE_MODE_WRITE ) )
    {
        mode = 4;
    }
    else if ( !( mode_ & FILE_MODE_READ ) && ( mode_ & FILE_MODE_WRITE ) )
    {
        mode = 2;
    }
    else
    {
        mode = 0;
    }

    return !_access ( path , mode );
}

bool
platform_file_open
(   const char* path
,   FILE_MODE   mode_
,   file_t*     file_
)
{
    if ( !file_ )
    {
        LOGERROR ( "platform_file_open ("PLATFORM_STRING"): Missing argument: file (output buffer)." );
        return false;
    }

    ( *file_ ).valid = false;
    ( *file_ ).handle = 0;

    if ( !path )
    {
        LOGERROR ( "platform_file_open ("PLATFORM_STRING"): Missing argument: path (filepath to open)." );
        return false;
    }

    i32 mode;
    bool truncate;
    if ( ( mode_ & FILE_MODE_READ ) && ( mode_ & FILE_MODE_WRITE ) )
    {
        mode = GENERIC_READ | GENERIC_WRITE;
        truncate = false;
    }
    else if ( ( mode_ & FILE_MODE_READ ) && !( mode_ & FILE_MODE_WRITE ) )
    {
        mode = GENERIC_READ;
        truncate = false;
    }
    else if ( !( mode_ & FILE_MODE_READ ) && ( mode_ & FILE_MODE_WRITE ) )
    {
        mode = GENERIC_WRITE;
        truncate = true;
    }
    else
    {
        LOGERROR ( "platform_file_open ("PLATFORM_STRING"): Value of mode argument was invalid; it should be a valid file mode." );
        return false;
    }

    HANDLE handle = CreateFile ( path
                               , mode
                               , 0
                               , 0
                               , OPEN_ALWAYS
                               , FILE_ATTRIBUTE_NORMAL
                               , 0
                               );
    
    if ( handle == INVALID_HANDLE_VALUE )
    {
        platform_log_error ( "platform_file_open ("PLATFORM_STRING"): CreateFile failed on file: %s"
                           , path
                           );
        return false;
    }

    // Set read-write position to the start of the file.
    if ( SetFilePointer ( handle , 0 , 0 , FILE_BEGIN ) == INVALID_SET_FILE_POINTER )
    {
        platform_log_error ( "platform_file_open ("PLATFORM_STRING"): SetFilePointer failed on file: %s"
                           , path
                           );
        return false;
    }

    // If in write mode, truncate the file content.
    if ( truncate && !SetEndOfFile ( handle ) )
    {
        platform_log_error ( "platform_file_open ("PLATFORM_STRING"): SetEndOfFile failed on file: %s"
                           , path
                           );
        return false;
    }

    // Precompute the file size.
    LARGE_INTEGER size;
    if ( !GetFileSizeEx ( handle , &size ) )
    {
        platform_log_error ( "platform_file_size ("PLATFORM_STRING"): GetFileSizeEx failed on file: %s"
                           , path
                           );
        return false;
    }

    platform_file_t* file = memory_allocate ( sizeof ( platform_file_t )
                                            , MEMORY_TAG_FILE
                                            );
    ( *file ).handle = handle;
    ( *file ).path = path;
    ( *file ).mode = mode_;
    ( *file ).size = size.QuadPart;
    ( *file ).position = 0;
    
    ( *file_ ).handle = file;
    ( *file_ ).valid = true;
    return true;
}

void
platform_file_close
(   file_t* file_
)
{
    if ( !file_ || !( *file_ ).valid || !( *file_ ).handle )
    {
        return;
    }

    ( *file_ ).valid = false;

    platform_file_t* file = ( *file_ ).handle;
    if ( !CloseHandle ( ( *file ).handle ) )
    {
        platform_log_error ( "platform_file_close ("PLATFORM_STRING"): CloseHandle failed on file: %s"
                           , ( *file ).path
                           );
    }

    memory_free ( file
                , sizeof ( platform_file_t )
                , MEMORY_TAG_FILE
                );
    ( *file_ ).handle = 0;
}

u64
platform_file_size
(   file_t* file_
)
{
    if ( !file_ )
    {
        LOGERROR ( "platform_file_size ("PLATFORM_STRING"): Missing argument: file." );
        return 0;
    }

    if ( !( *file_ ).handle || !( *file_ ).valid )
    {
        return 0;
    }

    platform_file_t* file = ( *file_ ).handle;
    return ( *file ).size;
}

u64
platform_file_position_get
(   file_t* file_
)
{
    if ( !file_ )
    {
        LOGERROR ( "platform_file_position_get ("PLATFORM_STRING"): Missing argument: file." );
        return 0;
    }

    if ( !( *file_ ).handle || !( *file_ ).valid )
    {
        return 0;
    }

    platform_file_t* file = ( *file_ ).handle;
    return ( *file ).position;
}

bool
platform_file_position_set
(   file_t* file_
,   u64     position
)
{
    if ( !file_ )
    {
        LOGERROR ( "platform_file_position_set ("PLATFORM_STRING"): Missing argument: file." );
        return 0;
    }

    if ( !( *file_ ).handle || !( *file_ ).valid )
    {
        return 0;
    }

    platform_file_t* file = ( *file_ ).handle;

    // Out-of-bounds position? Y/N
    if ( position > ( *file ).size )
    {
        LOGERROR ( "platform_file_position_set ("PLATFORM_STRING"): The provided position is outside of the file boundary." );
        return 0;
    }

    // Update host platform file position.
    LARGE_INTEGER amount;
    amount.QuadPart = position;
    if ( !SetFilePointerEx ( ( *file ).handle
                           , amount
                           , 0
                           , FILE_BEGIN
                           ))
    {
        platform_log_error ( "platform_file_position_set ("PLATFORM_STRING"): SetFilePointerEx failed on file: %s"
                           , ( *file ).path
                           );
        return false;
    }

    // Update host platform file position.
    ( *file ).position = position;
    
    return true;
}

bool
platform_file_read
(   file_t* file_
,   u64     size
,   void*   dst
,   u64*    read
)
{
    if ( !file_ || !dst || !read )
    {
        if ( !file_ )
        {
            LOGERROR ( "platform_file_read ("PLATFORM_STRING"): Missing argument: file (file to read)." );
        }
        if ( !dst )
        {
            LOGERROR ( "platform_file_read ("PLATFORM_STRING"): Missing argument: dst (output buffer)." );
        }
        if ( !read )
        {
            LOGERROR ( "platform_file_read ("PLATFORM_STRING"): Missing argument: read (output buffer)." );
        }
        else
        {
            *read = 0;
        }
        return false;
    }

    if ( !( *file_ ).handle || !( *file_ ).valid )
    {
        *read = 0;
        return false;
    }

    platform_file_t* file = ( *file_ ).handle;

    // Illegal mode? Y/N
    if ( !( ( *file ).mode & FILE_MODE_READ ) )
    {
        LOGERROR ( "platform_file_read ("PLATFORM_STRING"): The provided file is not opened for reading: %s"
                 , ( *file ).path
                 );
        *read = 0;
        return false;
    }

    // Files >= 4 GiB in size must be read by iteratively invoking ReadFile.
    const u64 max_read_size = GiB ( 4 ) - 1;

    size = MIN ( size , ( *file ).size - ( *file ).position );
    u64 total_bytes_read = 0;
    while ( total_bytes_read < size )
    {
        DWORD bytes_read;
        if ( !ReadFile ( ( *file ).handle
                        , ( ( u8* ) dst ) + total_bytes_read
                        , MIN ( size - total_bytes_read , max_read_size )
                        , &bytes_read
                        , 0
                        ))
        {
            platform_log_error ( "platform_file_read ("PLATFORM_STRING"): ReadFile failed on file: %s"
                                , ( *file ).path
                                );
            *read = total_bytes_read;
            return false;
        }

        // Update internal file position.
        ( *file ).position += bytes_read;

        total_bytes_read += bytes_read;
    }
   
    *read = total_bytes_read;
    return total_bytes_read == size;
}

bool
platform_file_read_line
(   file_t* file_
,   char**  dst
)
{
    if ( !file_ || !dst )
    {
        if ( !file_ )
        {
            LOGERROR ( "platform_file_read_line ("PLATFORM_STRING"): Missing argument: file (file to read)." );
        }
        if ( !dst )
        {
            LOGERROR ( "platform_file_read_line ("PLATFORM_STRING"): Missing argument: dst (output buffer)." );
        }
        else
        {
            *dst = 0;
        }
        return false;
    }

    if ( !( *file_ ).handle || !( *file_ ).valid )
    {
        *dst = 0;
        return false;
    }

    platform_file_t* file = ( *file_ ).handle;

    // Illegal mode? Y/N
    if ( !( ( *file ).mode & FILE_MODE_READ ) )
    {
        LOGERROR ( "platform_file_read_line ("PLATFORM_STRING"): The provided file is not opened for reading: %s"
                 , ( *file ).path
                 );
        *dst = 0;
        return false;
    }

    char buffer[ STACK_STRING_MAX_SIZE ];
    char* string = string_create ();

    // Nothing to copy? Y/N
    u64 bytes_remaining = ( *file ).size - ( *file ).position;
    if ( !bytes_remaining )
    {
        *dst = string;
        return true;
    }
    
    bool end_of_line = false;
    do
    {
        // Read file content into a buffer for processing.
        DWORD bytes_read;
        if ( !ReadFile ( ( *file ).handle
                       , buffer
                       , MIN ( bytes_remaining , STACK_STRING_MAX_SIZE )
                       , &bytes_read
                       , 0
                       ))
        {
            platform_log_error ( "platform_file_read_line ("PLATFORM_STRING"): ReadFile failed on file: %s"
                               , ( *file ).path
                               );
            string_destroy ( string );
            *dst = 0;
            return false;
        }

        // Update internal file position.
        ( *file ).position += bytes_read;

        // End of line? Y/N
        u64 length = 0;
        while ( length < bytes_read )
        {
            if ( !buffer[ length ] || newline ( buffer[ length ] ) )
            {
                end_of_line = true;

                // Update host platform file position.
                LARGE_INTEGER amount;
                amount.QuadPart = bytes_read - length - 1;
                amount.QuadPart = -amount.QuadPart;
                if ( !SetFilePointerEx ( ( *file ).handle
                                       , amount
                                       , 0
                                       , FILE_CURRENT
                                       ))
                {
                    platform_log_error ( "platform_file_read_line ("PLATFORM_STRING"): SetFilePointerEx failed on file: %s"
                                       , ( *file ).path
                                       );
                    string_destroy ( string );
                    *dst = 0;
                    return false;
                }

                // Update internal file position.
                ( *file ).position += amount.QuadPart;

                break;
            }

            length += 1;
        }

        // Append the line segment to the output buffer.
        string_push ( string , buffer , length );

        bytes_remaining -= bytes_read;
    }
    while ( bytes_remaining && !end_of_line );
    
    *dst = string;
    return true;
}

bool
platform_file_read_all
(   file_t* file_
,   u8**    dst
,   u64*    read
)
{
    if ( !file_ || !dst || !read )
    {
        if ( !file_ )
        {
            LOGERROR ( "platform_file_read_all ("PLATFORM_STRING"): Missing argument: file (file to read)." );
        }
        if ( !dst )
        {
            LOGERROR ( "platform_file_read_all ("PLATFORM_STRING"): Missing argument: dst (output buffer)." );
        }
        else
        {
            *dst = 0;
        }
        if ( !read )
        {
            LOGERROR ( "platform_file_read_all ("PLATFORM_STRING"): Missing argument: read (output buffer)." );
        }
        else
        {
            *read = 0;
        }
        return false;
    }

    if ( !( *file_ ).handle || !( *file_ ).valid )
    {
        *dst = 0;
        *read = 0;
        return false;
    }

    platform_file_t* file = ( *file_ ).handle;

    // Illegal mode? Y/N
    if ( !( ( *file ).mode & FILE_MODE_READ ) )
    {
        LOGERROR ( "platform_file_read_all ("PLATFORM_STRING"): The provided file is not opened for reading: %s"
                 , ( *file ).path
                 );
        *dst = 0;
        *read = 0;
        return false;
    }

    u8* string = ( u8* ) string_allocate ( sizeof ( u8 ) * ( ( *file ).size + 1 ) );

    // Nothing to copy? Y/N
    if ( !( *file ).size )
    {
        *dst = string;
        *read = 0;
        return true;
    }

    // Set host platform read-write position to the start of the file.
    if ( SetFilePointer ( ( *file ).handle , 0 , 0 , FILE_BEGIN ) == INVALID_SET_FILE_POINTER )
    {
        platform_log_error ( "platform_file_read_all ("PLATFORM_STRING"): SetFilePointer failed on file: %s"
                           , ( *file ).path
                           );
        string_free ( string );
        *dst = 0;
        *read = 0;
        return false;
    }

    // Update internal file position.
    ( *file ).position = 0;

    // Files >= 4 GiB in size must be read by iteratively invoking ReadFile.
    const u64 max_read_size = GiB ( 4 ) - 1;

    u64 total_bytes_read = 0;
    do
    {
        DWORD bytes_read;
        if ( !ReadFile ( ( *file ).handle
                       , string + total_bytes_read
                       , MIN ( ( *file ).size - total_bytes_read
                             , max_read_size
                             )
                       , &bytes_read
                       , 0
                       ))
        {
            platform_log_error ( "platform_file_read ("PLATFORM_STRING"): ReadFile failed on file: %s"
                               , ( *file ).path
                               );
            string_free ( string );
            *dst = 0;
            *read = total_bytes_read;
            return false;
        }

        // Update internal file position.
        ( *file ).position += bytes_read;

        total_bytes_read += bytes_read;
    }
    while ( total_bytes_read < ( *file ).size );

    // Rewind host platform file position.
    if ( SetFilePointer ( ( *file ).handle , 0 , 0 , FILE_BEGIN ) == INVALID_SET_FILE_POINTER )
    {
        platform_log_error ( "platform_file_read_all ("PLATFORM_STRING"): SetFilePointer failed on file: %s"
                           , ( *file ).path
                           );
        string_free ( string );
        *dst = 0;
        *read = total_bytes_read;
        return false;
    }

    // Update internal file position.
    ( *file ).position = 0;

    *dst = string;
    *read = total_bytes_read;
    return total_bytes_read == ( *file ).size;
}

bool
platform_file_write
(   file_t*     file_
,   u64         size
,   const void* src
,   u64*        written
)
{
    if ( !file_ || !src || !written )
    {
        if ( !file_ )
        {
            LOGERROR ( "platform_file_write ("PLATFORM_STRING"): Missing argument: file (file to write to)." );
        }
        if ( !src )
        {
            LOGERROR ( "platform_file_write ("PLATFORM_STRING"): Missing argument: src (content to write)." );
        }
        if ( !written )
        {
            LOGERROR ( "platform_file_write ("PLATFORM_STRING"): Missing argument: written (output buffer)." );
        }
        else
        {
            *written = 0;
        }
        return false;
    }

    if ( !( *file_ ).handle || !( *file_ ).valid )
    {
        *written = 0;
        return false;
    }

    platform_file_t* file = ( *file_ ).handle;

    // Illegal mode? Y/N
    if ( !( ( *file ).mode & FILE_MODE_WRITE ) )
    {
        LOGERROR ( "platform_file_write ("PLATFORM_STRING"): The provided file is not opened for writing: %s"
                 , ( *file ).path
                 );
        *written = 0;
        return false;
    }

    // Buffers >= 4 GiB in size must be written by iteratively invoking
    // WriteFile.
    const u64 max_write_size = GiB ( 4 ) - 1;

    u64 total_bytes_written = 0;
    while ( total_bytes_written < size )
    {
        DWORD bytes_written;
        if ( !WriteFile ( ( *file ).handle
                        , ( ( u8* ) src ) + total_bytes_written
                        , MIN ( size - total_bytes_written , max_write_size )
                        , &bytes_written
                        , 0
                        ))
        {
            platform_log_error ( "platform_file_write ("PLATFORM_STRING"): WriteFile failed on file: %s"
                                , ( *file ).path
                                );
            *written = total_bytes_written;
            return false;
        }

        // Update internal file position and size.
        ( *file ).position += bytes_written;
        ( *file ).size += bytes_written;

        total_bytes_written += bytes_written;
    }

    *written = total_bytes_written;
    return total_bytes_written == size;
}

bool
platform_file_write_line
(   file_t*     file_
,   u64         size
,   const char* src
)
{
    if ( !file_ || !src )
    {
        if ( !file_ )
        {
            LOGERROR ( "platform_file_write_line ("PLATFORM_STRING"): Missing argument: file (file to write to)." );
        }
        if ( !src )
        {
            LOGERROR ( "platform_file_write_line ("PLATFORM_STRING"): Missing argument: src (content to write)." );
        }
        return false;
    }

    if ( !( *file_ ).handle || !( *file_ ).valid )
    {
        return false;
    }

    platform_file_t* file = ( *file_ ).handle;

    // Illegal mode? Y/N
    if ( !( ( *file ).mode & FILE_MODE_WRITE ) )
    {
        LOGERROR ( "platform_file_write_line ("PLATFORM_STRING"): The provided file is not opened for writing: %s"
                 , ( *file ).path
                 );
        return false;
    }

    // Buffers >= 4 GiB in size must be written by iteratively invoking
    // WriteFile.
    const u64 max_write_size = GiB ( 4 ) - 1;

    u64 total_bytes_written = 0;
    while ( total_bytes_written < size )
    {
        DWORD bytes_written;
        if ( !WriteFile ( ( *file ).handle
                        , ( ( u8* ) src ) + total_bytes_written
                        , MIN ( size - total_bytes_written , max_write_size )
                        , &bytes_written
                        , 0
                        ))
        {
            platform_log_error ( "platform_file_write ("PLATFORM_STRING"): WriteFile failed on file: %s"
                                , ( *file ).path
                                );
            return false;
        }

        // Update internal file position and size.
        ( *file ).position += bytes_written;
        ( *file ).size += bytes_written;

        total_bytes_written += bytes_written;
    }

    // Append a newline to the file.
    const char newline = '\n';
    DWORD bytes_written;
    if ( !WriteFile ( ( *file ).handle
                    , &newline
                    , sizeof ( newline )
                    , &bytes_written
                    , 0
                    ))
    {
        platform_log_error ( "platform_file_write_line ("PLATFORM_STRING"): WriteFile failed on file: %s"
                            , ( *file ).path
                            );
        return false;
    }

    // Update internal file position and size.
    ( *file ).position += bytes_written;
    ( *file ).size += bytes_written;

    total_bytes_written += bytes_written;

    return total_bytes_written == size + sizeof ( newline );
}

void
platform_file_stdin
(   file_t* file
)
{
    if ( !platform_stdin.handle )
    {
        platform_stdin.handle = GetStdHandle ( STD_INPUT_HANDLE );
        platform_stdin.mode = FILE_MODE_READ;
        platform_stdin.path = "stdin";
        platform_stdin.size = 0;
        platform_stdin.position = 0;
    }
    ( *file ).handle = &platform_stdin;
    ( *file ).valid = true;
}

void
platform_file_stdout
(   file_t* file
)
{
    if ( !platform_stdout.handle )
    {
        platform_stdout.handle = GetStdHandle ( STD_OUTPUT_HANDLE );
        platform_stdout.mode = FILE_MODE_WRITE;
        platform_stdout.path = "stdout";
        platform_stdout.size = 0;
        platform_stdout.position = 0;
    }
    ( *file ).handle = &platform_stdout;
    ( *file ).valid = true;
}

void
platform_file_stderr
(   file_t* file
)
{
    if ( !platform_stderr.handle )
    {
        platform_stderr.handle = GetStdHandle ( STD_ERROR_HANDLE );
        platform_stderr.mode = FILE_MODE_WRITE;
        platform_stderr.path = "stderr";
        platform_stderr.size = 0;
        platform_stderr.position = 0;
    }
    ( *file ).handle = &platform_stderr;
    ( *file ).valid = true;
}

i64
platform_error_code
( void )
{
    return GetLastError ();
}

u64
platform_error_message
(   i64     error
,   char*   dst
,   u64     dst_length
)
{
    if ( !dst || !dst_length )
    {
        if ( !dst )
        {
            LOGERROR ( "platform_error_message ("PLATFORM_STRING"): Missing argument: dst (output buffer)." );
        }
        if ( !dst_length )
        {
            LOGERROR ( "platform_error_message ("PLATFORM_STRING"): Value of dst_length argument must be non-zero." );
        }
        return 0;
    }
    if ( strerror_s ( dst , dst_length , error ) )
    {
        LOGERROR ( "platform_error_message ("PLATFORM_STRING"): Failed to retrieve an error report from the host platform." );
        return 0;
    }
    return _string_length_clamped ( dst , dst_length );
}

f64
platform_absolute_time
( void )
{    
    if ( !platform_clock_frequency )
    {
        platform_clock_init ();
    }
    
    LARGE_INTEGER time;
    QueryPerformanceCounter ( &time );
    return ( ( f64 ) time.QuadPart ) * platform_clock_frequency;
}

void
platform_sleep
(   u64 ms
)
{
    Sleep ( ms );
}

i32
platform_processor_core_count
( void )
{
    SYSTEM_INFO system_info;
    GetSystemInfo ( &system_info );
    LOGINFO ( "platform_processor_core_count ("PLATFORM_STRING"): %i cores available."
            , system_info.dwNumberOfProcessors
            );
    return system_info.dwNumberOfProcessors;
}

void
platform_clock_init
( void )
{
    LARGE_INTEGER f;
    QueryPerformanceFrequency( &f );
    platform_clock_frequency = 1.0 / ( ( f64 ) f.QuadPart );
    QueryPerformanceCounter( &platform_clock_start_time );
}

#endif  // End platform layer.
////////////////////////////////////////////////////////////////////////////////
