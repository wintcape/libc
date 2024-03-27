/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file platform/macos.m
 * @brief Implementation of the platform header for macOS/OSX-based
 * operating systems.
 * (see platform.h for additional details)
 */
#include "platform/platform.h"

////////////////////////////////////////////////////////////////////////////////
// Begin platform layer.
#if PLATFORM_MAC == 1

#include "container/string.h"
#include "core/logger.h"
#include "core/memory.h"
#include "math/clamp.h"

// Platform layer dependencies.
#define _FILE_OFFSET_BITS 64
#include <errno.h>
#include <fcntl.h>
#include <mach/mach_time.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#if _POSIX_C_SOURCE >= 199309L
    #include <time.h> // nanosleep
#endif

// Standard libc dependencies.
#include <stdlib.h>
#include <string.h>

/** @brief Type definition for a platform-dependent file data structure. */
typedef struct
{
    i32         descriptor;
    const char* path;
    FILE_MODE   mode;
    u64         size;
    u64         position;
    bool        initialized;
}
platform_file_t;

// Global definitions for standard input, output, and error file streams.
static platform_file_t platform_stdin;  /** @brief Standard input stream handle. */
static platform_file_t platform_stdout; /** @brief Standard output stream handle. */
static platform_file_t platform_stderr; /** @brief Standard error stream handle. */

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

    DISABLE_WARNING ( -Wcast-function-type );
    const i32 pthread_create_result = pthread_create ( ( pthread_t* ) &( *thread ).id
                                                     , 0
                                                     , ( void* (*)( void* ) ) function
                                                     , args
                                                     );
    REENABLE_WARNING ();
    if ( pthread_create_result )
    {
        platform_log_error ( "platform_thread_create ("PLATFORM_STRING"): pthread_create failed." );
        return false;
    }

    if ( auto_detach )
    {
        if ( pthread_detach ( ( *thread ).id ) )
        {
            platform_log_error ( "platform_thread_create ("PLATFORM_STRING"): pthread_detach failed on auto-detach thread #%u."
                               , ( *thread ).id
                               );
            return false;
        }
    }

    ( *thread ).internal = memory_allocate ( sizeof ( u64 )
                                           , MEMORY_TAG_THREAD
                                           );
    *( ( u64* )( ( *thread ).internal ) ) = ( *thread ).id;

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

    if ( pthread_cancel ( *( ( pthread_t* )( ( *thread ).internal ) ) ) )
    {
        platform_log_error ( "platform_thread_destroy ("PLATFORM_STRING"): pthread_cancel failed on thread #%u."
                           , ( *thread ).id
                           );
        return;
    }

    memory_free ( ( *thread ).internal , sizeof ( u64 ) , MEMORY_TAG_THREAD );
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

    if ( pthread_detach ( *( ( pthread_t* )( ( *thread ).internal ) ) ) )
    {
        platform_log_error ( "platform_thread_detach ("PLATFORM_STRING"): pthread_detach failed on thread #%u."
                           , ( *thread ).id
                           );
        return;
    }

    memory_free ( ( *thread ).internal , sizeof ( u64 ) , MEMORY_TAG_THREAD );
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

    if ( pthread_cancel ( *( ( pthread_t* )( ( *thread ).internal ) ) ) )
    {
        platform_log_error ( "platform_thread_cancel ("PLATFORM_STRING"): pthread_cancel failed on thread #%u."
                           , ( *thread ).id
                           );
        return;
    }

    memory_free ( ( *thread ).internal , sizeof ( u64 ) , MEMORY_TAG_THREAD );
    ( *thread ).internal = 0;
    ( *thread ).id = 0;
}

bool
platform_thread_wait
(   thread_t* thread
)
{
    // TODO: Implement this.
    if ( !thread || !( *thread ).internal )
    {
        return false;
    }
    return true;
}

bool
platform_thread_wait_timeout
(   thread_t*   thread
,   u64         timeout_ms
)
{
    // TODO: Implement this.
    if ( !thread || !( *thread ).internal )
    {
        return false;
    }
    return true;
}

bool
platform_thread_active
(   thread_t* thread
)
{
    // TODO: Implement this.
    if ( !thread || !( *thread ).internal )
    {
        return false;
    }
    return thread && ( *thread ).internal;
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
    return pthread_self ();
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

    pthread_mutexattr_t pthread_mutex_attr;
    pthread_mutexattr_init ( &pthread_mutex_attr );
    pthread_mutexattr_settype ( &pthread_mutex_attr , PTHREAD_MUTEX_RECURSIVE );
    
    pthread_mutex_t pthread_mutex;
    if ( pthread_mutex_init ( &pthread_mutex , &pthread_mutex_attr ) )
    {
        platform_log_error ( "platform_mutex_create ("PLATFORM_STRING"): pthread_mutex_init failed on mutex %@."
                           , mutex
                           );
        return false;
    }

    ( *mutex ).internal = memory_allocate ( sizeof ( pthread_mutex_t )
                                          , MEMORY_TAG_MUTEX
                                          );
    *( ( pthread_mutex_t* )( ( *mutex ).internal ) ) = pthread_mutex;

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

    if ( pthread_mutex_destroy ( ( pthread_mutex_t* )( ( *mutex ).internal ) ) )
    {
        platform_log_error ( "platform_mutex_destroy ("PLATFORM_STRING"): pthread_mutex_destroy failed on mutex %@."
                           , mutex
                           );
        return;
    }

    memory_free ( ( *mutex ).internal
                , sizeof ( pthread_mutex_t* )
                , MEMORY_TAG_MUTEX
                );
    ( *mutex ).internal = 0;
}

bool
platform_mutex_lock
(   mutex_t* mutex
)
{
    if ( !mutex || !( *mutex ).internal )
    {
        if ( !mutex )
        {
            LOGERROR ( "platform_mutex_lock ("PLATFORM_STRING"): Missing argument: mutex." );
        }
        else
        {
            LOGERROR ( "platform_mutex_lock ("PLATFORM_STRING"): The provided mutex (%@) is not initialized."
                     , mutex
                     );
        }
        return false;
    }

    if ( pthread_mutex_lock ( ( pthread_mutex_t* )( ( *mutex ).internal ) ) )
    {
        platform_log_error ( "platform_mutex_lock ("PLATFORM_STRING"): pthread_mutex_lock failed on mutex %@."
                           , mutex
                           );
        return false;
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
        if ( !mutex )
        {
            LOGERROR ( "platform_mutex_lock ("PLATFORM_STRING"): Missing argument: mutex." );
        }
        else
        {
            LOGERROR ( "platform_mutex_lock ("PLATFORM_STRING"): The provided mutex (%@) is not initialized."
                     , mutex
                     );
        }
        return false;
    }

    if ( pthread_mutex_unlock ( ( pthread_mutex_t* )( ( *mutex ).internal ) ) )
    {
        platform_log_error ( "platform_mutex_unlock ("PLATFORM_STRING"): pthread_mutex_unlock failed on mutex %@."
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
        mode = R_OK | W_OK;
    }
    else if ( ( mode_ & FILE_MODE_READ ) && !( mode_ & FILE_MODE_WRITE ) )
    {
        mode = R_OK;
    }
    else if ( !( mode_ & FILE_MODE_READ ) && ( mode_ & FILE_MODE_WRITE ) )
    {
        mode = W_OK;
    }
    else
    {
        mode = F_OK;
    }

    return !access ( path , mode );
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
        mode = O_RDWR;
        truncate = false;
    }
    else if ( ( mode_ & FILE_MODE_READ ) && !( mode_ & FILE_MODE_WRITE ) )
    {
        mode = O_RDONLY;
        truncate = false;
    }
    else if ( !( mode_ & FILE_MODE_READ ) && ( mode_ & FILE_MODE_WRITE ) )
    {
        mode = O_WRONLY;
        truncate = true;
    }
    else
    {
        LOGERROR ( "platform_file_open ("PLATFORM_STRING"): Value of mode argument was invalid; it should be a valid file mode." );
        return false;
    }

    i32 descriptor = open ( path , mode | O_CREAT , S_IRWXU );

    if ( descriptor == -1 )
    {
        platform_log_error ( "platform_file_open ("PLATFORM_STRING"): open failed on file: %s."
                           , path
                           );
        return false;
    }

    // Set read-write position to the start of the file.
    if ( lseek ( descriptor , 0 , SEEK_SET ) == -1 )
    {
        platform_log_error ( "platform_file_open ("PLATFORM_STRING"): lseek failed on file: %s."
                           , path
                           );
        return false;
    }

    // If in write mode, truncate the file content.
    if ( truncate && ftruncate ( descriptor , 0 ) )
    {
        platform_log_error ( "platform_file_open ("PLATFORM_STRING"): ftruncate failed on file: %s."
                           , path
                           );
        return false;
    }

    // Precompute the file size.
    struct stat file_info;
    fstat ( descriptor , &file_info );

    platform_file_t* file = memory_allocate ( sizeof ( platform_file_t )
                                            , MEMORY_TAG_FILE
                                            );
    ( *file ).descriptor = descriptor;
    ( *file ).path = path;
    ( *file ).mode = mode_;
    ( *file ).size = file_info.st_size;
    ( *file ).position = 0;
    ( *file ).initialized = true;
    
    ( *file_ ).handle = file;
    ( *file_ ).valid = true;
    return true;
}

void
platform_file_close
(   file_t* file_
)
{
    if ( !file_ || !( *file_ ).handle || !( *file_ ).valid )
    {
        return;
    }

    ( *file_ ).valid = false;

    platform_file_t* file = ( *file_ ).handle;
    if ( close ( ( *file ).descriptor ) == -1 )
    {
        platform_log_error ( "platform_file_close ("PLATFORM_STRING"): close failed on file: %s."
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
    if ( lseek ( ( *file ).descriptor , position , SEEK_SET ) == -1 )
    {
        platform_log_error ( "platform_file_position_set ("PLATFORM_STRING"): lseek failed on file: %s"
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
,   u64*    read_
)
{
    if ( !file_ || !dst || !read_ )
    {
        if ( !file_ )
        {
            LOGERROR ( "platform_file_read ("PLATFORM_STRING"): Missing argument: file (file to read)." );
        }
        if ( !dst )
        {
            LOGERROR ( "platform_file_read ("PLATFORM_STRING"): Missing argument: dst (output buffer)." );
        }
        if ( !read_ )
        {
            LOGERROR ( "platform_file_read ("PLATFORM_STRING"): Missing argument: read (output buffer)." );
        }
        else
        {
            *read_ = 0;
        }
        return false;
    }

    if ( !( *file_ ).handle || !( *file_ ).valid )
    {
        *read_ = 0;
        return false;
    }

    platform_file_t* file = ( *file_ ).handle;

    // Illegal mode? Y/N
    if ( !( ( *file ).mode & FILE_MODE_READ ) )
    {
        LOGERROR ( "platform_file_read ("PLATFORM_STRING"): The provided file is not opened for reading: %s."
                 , ( *file ).path
                 );
        *read_ = 0;
        return false;
    }

    size = MIN ( size , ( *file ).size - ( *file ).position );
    u64 total_bytes_read = 0;
    while ( total_bytes_read < size )
    {
        const ssize_t bytes_read = read ( ( *file ).descriptor
                                        , ( ( u8* ) dst ) + total_bytes_read
                                        , size - total_bytes_read
                                        );
        if ( bytes_read == -1 )
        {
            platform_log_error ( "platform_file_read ("PLATFORM_STRING"): read failed on file: %s."
                               , ( *file ).path
                               );
            *read_ = total_bytes_read;
            return false;
        }

        // Update internal file position.
        ( *file ).position += bytes_read;

        total_bytes_read += bytes_read;
    }

    *read_ = total_bytes_read;
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
        LOGERROR ( "platform_file_read_line ("PLATFORM_STRING"): The provided file is not opened for reading: %s."
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
        const ssize_t bytes_read = read ( ( *file ).descriptor
                                        , buffer
                                        , MIN ( bytes_remaining
                                              , STACK_STRING_MAX_SIZE
                                              ));
        if ( bytes_read == -1 )
        {
            platform_log_error ( "platform_file_read_line ("PLATFORM_STRING"): read failed on file: %s."
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
        while ( length < ( ( u64 ) bytes_read ) )
        {
            if ( !buffer[ length ] || newline ( buffer[ length ] ) )
            {
                end_of_line = true;

                // Update host platform file position.
                u64 amount = bytes_read - length - 1;
                amount = -amount;
                if ( lseek ( ( *file ).descriptor , amount , SEEK_CUR ) == -1 )
                {
                    platform_log_error ( "platform_file_read_line ("PLATFORM_STRING"): lseek failed on file: %s."
                                       , ( *file ).path
                                       );
                    string_destroy ( string );
                    *dst = 0;
                    return false;
                }

                // Update internal file position.
                ( *file ).position += amount;

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
,   u64*    read_
)
{
    if ( !file_ || !dst || !read_ )
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
        if ( !read_ )
        {
            LOGERROR ( "platform_file_read_all ("PLATFORM_STRING"): Missing argument: read (output buffer)." );
        }
        else
        {
            *read_ = 0;
        }
        return false;
    }

    if ( !( *file_ ).handle || !( *file_ ).valid )
    {
        *dst = 0;
        *read_ = 0;
        return false;
    }

    platform_file_t* file = ( *file_ ).handle;

    // Illegal mode? Y/N
    if ( !( ( *file ).mode & FILE_MODE_READ ) )
    {
        LOGERROR ( "platform_file_read_all ("PLATFORM_STRING"): The provided file is not opened for reading: %s."
                 , ( *file ).path
                 );
        *dst = 0;
        *read_ = 0;
        return false;
    }

    u8* string = ( u8* ) string_allocate ( sizeof ( u8 ) * ( ( *file ).size + 1 ) );

    // Nothing to copy? Y/N
    if ( !( *file ).size )
    {
        *dst = string;
        *read_ = 0;
        return true;
    }

    // Set host-platform read-write position to the start of the file.
    if ( lseek ( ( *file ).descriptor , 0 , SEEK_SET ) == -1 )
    {
        platform_log_error ( "platform_file_read_all ("PLATFORM_STRING"): lseek failed on file: %s"
                           , ( *file ).path
                           );
        string_free ( string );
        *dst = 0;
        *read_ = 0;
        return false;
    }

    // Update internal file position.
    ( *file ).position = 0;

    u64 total_bytes_read = 0;
    do
    {
        const ssize_t bytes_read = read ( ( *file ).descriptor
                                        , string + total_bytes_read
                                        , ( *file ).size - total_bytes_read
                                        );
        if ( bytes_read == -1 )
        {
            platform_log_error ( "platform_file_read_all ("PLATFORM_STRING"): read failed on file: %s"
                               , ( *file ).path
                               );
            string_free ( string );
            *dst = 0;
            *read_ = total_bytes_read;
            return false;
        }

        // Update internal file position.
        ( *file ).position += bytes_read;

        total_bytes_read += bytes_read;
    }
    while ( total_bytes_read < ( *file ).size );

    // Rewind host platform file position.
    if ( lseek ( ( *file ).descriptor , 0 , SEEK_SET ) == -1 )
    {
        platform_log_error ( "platform_file_read_all ("PLATFORM_STRING"): lseek failed on file: %s"
                           , ( *file ).path
                           );
        string_free ( string );
        *dst = 0;
        *read_ = total_bytes_read;
        return false;
    }

    // Update internal file position.
    ( *file ).position = 0;

    *dst = string;
    *read_ = total_bytes_read;
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

    u64 total_bytes_written = 0;
    while ( total_bytes_written < size )
    {
        const ssize_t bytes_written = write ( ( *file ).descriptor
                                            , ( ( u8* ) src ) + total_bytes_written
                                            , size - total_bytes_written
                                            );
        if ( bytes_written == -1 )
        {
            platform_log_error ( "platform_file_write ("PLATFORM_STRING"): write failed on file: %s"
                               , ( *file ).path
                               );
            *written = bytes_written;
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

    u64 total_bytes_written = 0;
    while ( total_bytes_written < size )
    {
        const ssize_t bytes_written = write ( ( *file ).descriptor
                                            , ( ( u8* ) src ) + total_bytes_written
                                            , size - total_bytes_written
                                            );
        if ( bytes_written == -1 )
        {
            platform_log_error ( "platform_file_write ("PLATFORM_STRING"): write failed on file: %s"
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
    const ssize_t bytes_written = write ( ( *file ).descriptor
                                        , &newline
                                        , sizeof ( newline )
                                        );
    if ( bytes_written == -1 )
    {
        platform_log_error ( "platform_file_write ("PLATFORM_STRING"): write failed on file: %s"
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
    if ( !platform_stdin.initialized )
    {
        platform_stdin.descriptor = 0;
        platform_stdin.mode = FILE_MODE_READ;
        platform_stdin.path = "stdin";
        platform_stdin.size = 0;
        platform_stdin.position = 0;
        platform_stdin.initialized = true;
    }
    ( *file ).handle = &platform_stdin;
    ( *file ).valid = true;
}

void
platform_file_stdout
(   file_t* file
)
{
    if ( !platform_stdout.initialized )
    {
        platform_stdout.descriptor = 1;
        platform_stdout.mode = FILE_MODE_WRITE;
        platform_stdout.path = "stdout";
        platform_stdout.size = 0;
        platform_stdout.position = 0;
        platform_stdout.initialized = true;
    }
    ( *file ).handle = &platform_stdout;
    ( *file ).valid = true;
}

void
platform_file_stderr
(   file_t* file
)
{
    if ( !platform_stderr.initialized )
    {
        platform_stderr.descriptor = 2;
        platform_stderr.mode = FILE_MODE_WRITE;
        platform_stderr.path = "stderr";
        platform_stderr.size = 0;
        platform_stderr.position = 0;
        platform_stderr.initialized = true;
    }
    ( *file ).handle = &platform_stderr;
    ( *file ).valid = true;
}

i64
platform_error_code
( void )
{
    return errno;
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
    if ( strerror_r ( error , dst , dst_length ) )
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
    mach_timebase_info_data_t mach_timebase_info;
    mach_timebase_info ( &mach_timebase_info );
    const u64 time = mach_absolute_time ();
    const u64 ns = ( f64 )( time * ( ( u64 ) mach_timebase_info.numer ) / ( ( f64 ) clock_timebase.denom ) );
    return ns / 1.0E9;
}

void
platform_sleep
(   u64 ms
)
{
    #if _POSIX_C_SOURCE >= 199309L
        struct timespec time;
        time.tv_sec = ms / 1000;
        time.tv_nsec = ( ms % 1000 ) * 1000 * 1000;
        nanosleep ( &time , 0 );
    #else
        if ( ms >= 1000 )
        {
            sleep ( ms / 1000 );
        }
        usleep ( ( ms % 1000 ) * 1000 );
    #endif
}

i32
platform_processor_core_count
( void )
{
    LOGINFO ( "platform_processor_core_count ("PLATFORM_STRING"): %i cores available."
            , [ [ NSProcessInfo processInfo ] processorCount ]
            );
    return [ [ NSProcessInfo processInfo ] processorCount ];
}

#endif  // End platform layer.
////////////////////////////////////////////////////////////////////////////////
