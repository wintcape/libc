/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file platform/windows.c
 * @brief Implementation of the platform header for Microsoft Windows
 * operating systems.
 * (see platform.h for additional details)
 */
#include "platform/platform.h"

////////////////////////////////////////////////////////////////////////////////
// Begin platform layer.
#if PLATFORM_WINDOWS == 1

#include "core/logger.h"
#include "core/string.h"

#include "math/math.h"

// Platform layer dependencies.
#include <io.h>
#include <windows.h>
#include <windowsx.h>

/**
 * @brief Undefines preprocessor bindings from math/math which may cause name
 * conflicts with the standard libc headers.
 */
#undef abs
#undef random

// Standard libc dependencies.
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PLATFORM_WINDOWS_ERROR_CREATE_THREAD        1 /** @brief Internal error code. */
#define PLATFORM_WINDOWS_ERROR_CLOSE_HANDLE         2 /** @brief Internal error code. */
#define PLATFORM_WINDOWS_ERROR_GET_EXIT_CODE_THREAD 3 /** @brief Internal error code. */
#define PLATFORM_WINDOWS_ERROR_TERMINATE_THREAD     4 /** @brief Internal error code. */
#define PLATFORM_WINDOWS_ERROR_CREATE_MUTEX         5 /** @brief Internal error code. */
#define PLATFORM_WINDOWS_ERROR_ABANDONED_MUTEX      6 /** @brief Internal error code. */
#define PLATFORM_WINDOWS_ERROR_RELEASE_MUTEX        7 /** @brief Internal error code. */

// Global system clock. Allows for clocks to function without having to call
// platform_start first (see core/clock.h).
static f64              platform_clock_frequency;  /** @brief Global system clock frequency. */
static LARGE_INTEGER    platform_clock_start_time; /** @brief Global system clock start time. */

/**
 * @brief Initializes the system clock.
 */
void
platform_clock_init
( void );

/**
 * @brief Primary implementation of platform_thread_create
 * (see platform_thread_create).
 * 
 * @param function The callback function to run threaded.
 * @param args Internal state arguments.
 * @param auto_detach Thread should immediately release resources when work is
 * complete? Y/N. If true, the output buffer will be unset.
 * @param thread Output buffer (only set if auto_detach is false).
 * @return 0 on success; error code false.
 */
i32
_platform_thread_create
(   thread_start_function_t function
,   void*                   args
,   bool                    auto_detach
,   thread_t*               thread
);

/**
 * @brief Primary implementation of platform_thread_destroy
 * (see platform_thread_destroy).
 * 
 * @param thread The thread to free.
 * @return 0 on success; error code otherwise.
 */
i32
_platform_thread_destroy
(   thread_t* thread
);

/**
 * @brief Primary implementation of platform_thread_detach
 * (see platform_thread_detach).
 * 
 * @param thread The thread to detach.
 * @return 0 on success; error code otherwise.
 */
i32
_platform_thread_detach
(   thread_t* thread
);

/**
 * @brief Primary implementation of platform_thread_cancel
 * (see platform_thread_cancel).
 * 
 * @param thread The thread to cancel.
 * @return 0 on success; error code otherwise.
 */
i32
_platform_thread_cancel
(   thread_t* thread
);

/**
 * @brief Primary implementation of platform_thread_wait
 * (see platform_thread_wait).
 * 
 * @param thread The thread to wait for.
 * @return 0 on success; error code otherwise.
 */
i32
_platform_thread_wait
(   thread_t* thread
);

/**
 * @brief Primary implementation of platform_thread_wait_timeout
 * (see platform_thread_wait_timeout).
 * 
 * @param thread The thread to wait for.
 * @param timeout_ms The number of milliseconds to wait prior to timeout.
 * @return 0 on success; error code otherwise.
 */
i32
_platform_thread_wait_timeout
(   thread_t*   thread
,   const u64   timeout_ms
);

/**
 * @brief Primary implementation of platform_thread_active
 * (see platform_thread_active).
 * 
 * @param thread The thread to query.
 * @return 0 on success; error code otherwise.
 */
i32
_platform_thread_active
(   thread_t* thread
);

/**
 * @brief Primary implementation of platform_thread_active
 * (see platform_thread_active).
 * 
 * @param thread The thread to sleep on.
 * @param ms The time to sleep, in milliseconds.
 * @return 0 on success; error code otherwise.
 */
i32
_platform_thread_sleep
(   thread_t*   thread
,   const u64   ms
);

/**
 * @brief Primary implementation of platform_mutex_create
 * (see platform_mutex_create).
 * 
 * @param mutex Output buffer.
 * @return 0 on success; error code otherwise.
 */
i32
_platform_mutex_create
(   mutex_t* mutex
);

/**
 * @brief Primary implementation of platform_mutex_destroy
 * (see platform_mutex_destroy).
 * 
 * @param mutex The mutex to free.
 * @return 0 on success; error code otherwise.
 */
i32
_platform_mutex_destroy
(   mutex_t* mutex
);

/**
 * @brief Primary implementation of platform_mutex_lock
 * (see platform_mutex_lock).
 * 
 * @param mutex The mutex to lock.
 * @return 0 on success; error code otherwise.
 */
i32
_platform_mutex_lock
(   mutex_t* mutex
);

/**
 * @brief Primary implementation of platform_mutex_unlock
 * (see platform_mutex_unlock).
 * 
 * @param mutex The mutex to unlock.
 * @return 0 on success; error code otherwise.
 */
i32
_platform_mutex_unlock
(   mutex_t* mutex
);

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
,   const u64   limit
)
{
    return strnlen ( string , limit );
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

i64
platform_error_code
( void )
{
    return errno;
}

u64
platform_error_message
(   const i64   error
,   char*       dst
,   const u64   dst_length
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
    return MIN ( _string_length ( dst ) , dst_length );
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

    const i32 result = _platform_thread_create ( function
                                               , args
                                               , auto_detach
                                               , thread
                                               );

    if ( result )
    {
        const char* platform_function_name;
        switch ( result )
        {
            case PLATFORM_WINDOWS_ERROR_CREATE_THREAD:
            {
                platform_function_name = "CreateThread";
            }
            break;

            case PLATFORM_WINDOWS_ERROR_CLOSE_HANDLE:
            {
                platform_function_name = "CloseHandle";
            }
            break;

            default:
            {
                platform_function_name = "An unknown Windows process";
            }
            break;
        }

        const i64 error = platform_error_code ();
        char message[ STACK_STRING_MAX_SIZE ];
        platform_error_message ( error , message , STACK_STRING_MAX_SIZE );
        LOGERROR ( "platform_thread_create ("PLATFORM_STRING"): %s failed.\n\t                                         Reason: %S\n\t                                         Code:   %i"
                 , platform_function_name
                 , message
                 , error
                 );
        return false;
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

    const i32 result = _platform_thread_destroy ( thread );

    if ( result )
    {
        const char* platform_function_name;
        switch ( result )
        {
            case PLATFORM_WINDOWS_ERROR_GET_EXIT_CODE_THREAD:
            {
                platform_function_name = "GetExitCodeThread";
            }
            break;

            case PLATFORM_WINDOWS_ERROR_CLOSE_HANDLE:
            {
                platform_function_name = "CloseHandle";
            }
            break;

            default:
            {
                platform_function_name = "An unknown Windows process";
            }
            break;
        }

        const i64 error = platform_error_code ();
        char message[ STACK_STRING_MAX_SIZE ];
        platform_error_message ( error , message , STACK_STRING_MAX_SIZE );
        LOGERROR ( "platform_thread_destroy ("PLATFORM_STRING"): %s failed on thread #%u.\n\t                                         Reason: %S\n\t                                         Code:   %i"
                 , platform_function_name
                 , ( *thread ).id
                 , message
                 , error
                 );
    }
}

voiG
platform_thread_detach
(   thread_t* thread
)
{
    if ( !thread || !( *thread ).internal )
    {
        return;
    }

    if ( _platform_thread_detach ( thread ) )
    {
        const i64 error = platform_error_code ();
        char message[ STACK_STRING_MAX_SIZE ];
        platform_error_message ( error , message , STACK_STRING_MAX_SIZE );
        LOGERROR ( "platform_thread_detach ("PLATFORM_STRING"): CloseHandle failed on thread #%u.\n\t                                         Reason: %S\n\t                                         Code:   %i"
                 , ( *thread ).id
                 , message
                 , error
                 );
    }
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

    if ( _platform_thread_cancel ( thread ) )
    {
        const i64 error = platform_error_code ();
        char message[ STACK_STRING_MAX_SIZE ];
        platform_error_message ( error , message , STACK_STRING_MAX_SIZE );
        LOGERROR ( "platform_thread_cancel ("PLATFORM_STRING"): TerminateThread failed on thread #%u.\n\t                                         Reason: %S\n\t                                         Code:   %i"
                 , ( *thread ).id
                 , message
                 , error
                 );
    }
}

bool
platform_thread_wait
(   thread_t* thread
)
{
    if ( !thread || !( *thread ).internal )
    {
        return false;
    }
    return !_platform_thread_wait ( thread );
}

bool
platform_thread_wait_timeout
(   thread_t*   thread
,   const u64   timeout_ms
)
{
    if ( !thread || !( *thread ).internal )
    {
        return false;
    }
    return !_platform_thread_wait_timeout ( thread , timeout_ms );
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
    return !_platform_thread_active ( thread );
}

void
platform_thread_sleep
(   thread_t*   thread
,   const u64   ms
)
{
    /* if ( */ _platform_thread_sleep ( thread , ms ) /* ) {} */;
}//            ^^^^^^^^^^^^^^^^^^^^^^ Does not fail.

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

    if ( _platform_mutex_create ( mutex ) )
    {
        const i64 error = platform_error_code ();
        char message[ STACK_STRING_MAX_SIZE ];
        platform_error_message ( error , message , STACK_STRING_MAX_SIZE );
        LOGERROR ( "platform_mutex_create ("PLATFORM_STRING"): CreateMutex failed.\n\t                                        Reason: %S\n\t                                        Code:   %i"
                 , message
                 , error
                 );
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

    if ( _platform_mutex_destroy ( mutex ) )
    {
        const i64 error = platform_error_code ();
        char message[ STACK_STRING_MAX_SIZE ];
        platform_error_message ( error , message , STACK_STRING_MAX_SIZE );
        LOGERROR ( "platform_mutex_destroy ("PLATFORM_STRING"): CloseHandle failed on mutex %@.\n\t                                         Reason: %S\n\t                                         Code:   %i"
                 , mutex
                 , message
                 , error
                 );
    }
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

    if ( _platform_mutex_lock ( mutex ) )
    {
        const i64 error = platform_error_code ();
        char message[ STACK_STRING_MAX_SIZE ];
        platform_error_message ( error , message , STACK_STRING_MAX_SIZE );
        LOGERROR ( "platform_mutex_lock ("PLATFORM_STRING"): WaitForSingleObject failed on mutex %@.\n\t                                       Reason: %S\n\t                                       Code:   %i"
                 , mutex
                 , message
                 , error
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
        return false;
    }

    if ( _platform_mutex_unlock ( mutex ) )
    {
        const i64 error = platform_error_code ();
        char message[ STACK_STRING_MAX_SIZE ];
        platform_error_message ( error , message , STACK_STRING_MAX_SIZE );
        LOGERROR ( "platform_mutex_unlock ("PLATFORM_STRING"): ReleaseMutex failed on mutex %@.\n\t                                         Reason: %S\n\t                                         Code:   %i"
                 , mutex
                 , message
                 , error
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
    i32 mode;
    if ( mode_ == FILE_MODE_ACCESS )
    {
        mode = 0;
    }
    else if ( ( mode_ & FILE_MODE_READ ) && ( mode_ & FILE_MODE_WRITE ) )
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
        LOGERROR ( "file_exists: Invalid file mode." );
        return false;
    }
    return !_access ( path , mode );
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

i32
_platform_thread_create
(   thread_start_function_t function
,   void*                   args
,   bool                    auto_detach
,   thread_t*               thread
)
{
    ( *thread ).internal = CreateThread ( 0
                                        , 0
                                        , ( LPTHREAD_START_ROUTINE ) function
                                        , args
                                        , 0
                                        , ( LPDWORD )( &( *thread ).id )
                                        );
    if ( !( *thread ).internal )
    {
        return PLATFORM_WINDOWS_ERROR_CREATE_THREAD;
    }
    if ( auto_detach )
    {
        if ( !CloseHandle ( ( *thread ).internal ) )
        {
            return PLATFORM_WINDOWS_ERROR_CLOSE_HANDLE;
        }
    }
    return 0;
}

i32
_platform_thread_destroy
(   thread_t* thread
)
{
    i32 result = 0;
    DWORD code;
    if ( !GetExitCodeThread ( ( *thread ).internal , &code ) )
    {
        result = PLATFORM_WINDOWS_ERROR_GET_EXIT_CODE_THREAD;
    }
    if ( !CloseHandle ( ( *thread ).internal ) )
    {
        result = PLATFORM_WINDOWS_ERROR_CLOSE_HANDLE;
    }
    ( *thread ).internal = 0;
    ( *thread ).id = 0;
    return !result;
}

i32
_platform_thread_detach
(   thread_t* thread
)
{
    if ( !CloseHandle ( ( *thread ).internal ) )
    {
        return PLATFORM_WINDOWS_ERROR_CLOSE_HANDLE;
    }
    ( *thread ).internal = 0;
    return 0;
}

i32
_platform_thread_cancel
(   thread_t* thread
)
{
    if ( !TerminateThread ( ( *thread ).internal , 0 ) )
    {
        return PLATFORM_WINDOWS_ERROR_TERMINATE_THREAD;
    }
    ( *thread ).internal = 0;
    return 0;
}

i32
_platform_thread_wait
(   thread_t* thread
)
{
    return !( WaitForSingleObject ( ( *thread ).internal , INFINITE ) == WAIT_OBJECT_0 );
}

i32
_platform_thread_wait_timeout
(   thread_t*   thread
,   const u64   timeout_ms
)
{
    return !( WaitForSingleObject ( ( *thread ).internal , timeout_ms ) == WAIT_OBJECT_0 );
}

i32
_platform_thread_active
(   thread_t* thread
)
{
    return !( WaitForSingleObject ( ( *thread ).internal , 0 ) == WAIT_TIMEOUT );
}

i32
_platform_thread_sleep
(   thread_t*   thread
,   const u64   ms
)
{
    platform_sleep ( ms );
    return 0;
}

i32
_platform_mutex_create
(   mutex_t* mutex
)
{
    ( *mutex ).internal = CreateMutex ( 0 , 0 , 0 );
    if ( !( *mutex ).internal )
    {
        return PLATFORM_WINDOWS_ERROR_CREATE_MUTEX;
    }
    return 0;
}

i32
_platform_mutex_destroy
(   mutex_t* mutex
)
{
    if ( !CloseHandle ( ( *mutex ).internal ) )
    {
        return PLATFORM_WINDOWS_ERROR_CLOSE_HANDLE;
    }
    ( *mutex ).internal = 0;
    return 0;
}

i32
_platform_mutex_lock
(   mutex_t* mutex
)
{
    switch ( WaitForSingleObject ( ( *mutex ).internal , INFINITE ) )
    {
        case WAIT_OBJECT_0:
        {
            return 0;
        }
        case WAIT_ABANDONED:
        {
            return PLATFORM_WINDOWS_ERROR_ABANDONED_MUTEX;
        }
    }
    return 0;
}

i32
_platform_mutex_unlock
(   mutex_t* mutex
)
{
    if ( !ReleaseMutex ( ( *mutex ).internal ) )
    {
        return PLATFORM_WINDOWS_ERROR_RELEASE_MUTEX;
    }
    return 0;
}

#endif  // End platform layer.
////////////////////////////////////////////////////////////////////////////////
