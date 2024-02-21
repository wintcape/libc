/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file platform/macos.c
 * @brief Implementation of the platform header for macOS.
 * (see platform.h for additional details)
 */
#include "platform/platform.h"

////////////////////////////////////////////////////////////////////////////////
// Begin platform layer.
#if PLATFORM_MAC == 1

#include "core/logger.h"
#include "core/memory.h"

// Platform layer dependencies.
#include <mach/mach_time.h>
#include <pthread.h>
#if _POSIX_C_SOURCE >= 199309L
    #include <time.h>   // nanosleep
#else
    #include <unistd.h> // usleep
#endif

// Standard libc dependencies.
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PLATFORM_MAC_ERROR_PTHREAD_CREATE        1 /** @brief Internal error code. */
#define PLATFORM_MAC_ERROR_PTHREAD_DETACH        2 /** @brief Internal error code. */
#define PLATFORM_MAC_ERROR_PTHREAD_CANCEL        3 /** @brief Internal error code. */
#define PLATFORM_MAC_ERROR_PTHREAD_MUTEX_INIT    4 /** @brief Internal error code. */
#define PLATFORM_MAC_ERROR_PTHREAD_MUTEX_DESTROY 5 /** @brief Internal error code. */
#define PLATFORM_MAC_ERROR_PTHREAD_MUTEX_LOCK    6 /** @brief Internal error code. */
#define PLATFORM_MAC_ERROR_PTHREAD_MUTEX_UNLOCK  7 /** @brief Internal error code. */

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

i64
platform_error_code
( void )
{
    return errno;
}

char*
platform_error_message
(   const i64 error
)
{
    mutex_t mutex;
    if ( !_platform_mutex_create ( &mutex ) || !_platform_mutex_lock ( &mutex ) )
    {
        return string_create_from ( "" );
    }
    char* message;
    if ( error < sys_nerr )
    {
        message = string_create_from ( sys_errlist[ error ] ); 
    }
    else
    {
        message = string_create_from ( "Unknown code." );
    }
    _platform_mutex_unlock ( &mutex );
    return message;
}

i32
platform_processor_core_count
( void )
{
    LOGINFO ( "platform_processor_core_count: %i cores available."
            , [ [ NSProcessInfo processInfo ] processorCount ]
            );
    return [ [ NSProcessInfo processInfo ] processorCount ];
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
            case PLATFORM_MAC_ERROR_PTHREAD_CREATE:
            {
                platform_function_name = "pthread_create";
            }
            break;

            case PLATFORM_MAC_ERROR_PTHREAD_DETACH:
            {
                platform_function_name = "pthread_detach";
            }
            break;

            default:
            {
                platform_function_name = "An unknown GNU/MAC process";
            }
            break;
        }

        const i64 error = platform_error_code ();
        char* message = platform_error_message ( error );
        LOGERROR ( "platform_thread_create ("PLATFORM_STRING"): %s failed.\n\t                                    Reason: %S\n\t                                    Code:   %i"
                 , platform_function_name
                 , message
                 , error
                 );
        string_destroy ( message );
        
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

    if ( _platform_thread_destroy ( thread ) )
    {
        const i64 error = platform_error_code ();
        char* message = platform_error_message ( error );
        LOGERROR ( "platform_thread_destroy ("PLATFORM_STRING"): pthread_cancel failed on thread #%u.\n\t                                     Reason: %S\n\t                                     Code:   %i"
                 , ( *thread ).id
                 , message
                 , error
                 );
        string_destroy ( message );
    }
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

    if ( _platform_thread_detach ( thread ) )
    {
        const i64 error = platform_error_code ();
        char* message = platform_error_message ( error );
        LOGERROR ( "platform_thread_detach ("PLATFORM_STRING"): pthread_detach failed on thread #%u.\n\t                                    Reason: %S\n\t                                    Code:   %i"
                 , ( *thread ).id
                 , message
                 , error
                 );
        string_destroy ( message );
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
        char* message = platform_error_message ( error );
        LOGERROR ( "platform_thread_cancel ("PLATFORM_STRING"): pthread_cancel failed on thread #%u.\n\t                                    Reason: %S\n\t                                    Code:   %i"
                 , ( *thread ).id
                 , message
                 , error
                 );
        string_destroy ( message );
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

    if ( _platform_mutex_create ( mutex ) )
    {
        const i64 error = platform_error_code ();
        char* message = platform_error_message ( error );
        LOGERROR ( "platform_mutex_create ("PLATFORM_STRING"): pthread_mutex_init failed.\n\t                                   Reason: %S\n\t                                   Code:   %i"
                 , message
                 , error
                 );
        string_destroy ( message );
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
        char* message = platform_error_message ( error );
        LOGERROR ( "platform_mutex_destroy ("PLATFORM_STRING"): pthread_mutex_destroy failed on mutex %@.\n\t                                    Reason: %S\n\t                                    Code:   %i"
                 , mutex
                 , message
                 , error
                 );
        string_destroy ( message );
    }
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

    if ( _platform_mutex_lock ( mutex ) )
    {
        const i64 error = platform_error_code ();
        char* message = platform_error_message ( error );
        LOGERROR ( "platform_mutex_lock ("PLATFORM_STRING"): pthread_mutex_lock failed on mutex %@.\n\t                                 Reason: %S\n\t                                 Code:   %i"
                 , mutex
                 , message
                 , error
                 );
        string_destroy ( message );
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

    if ( _platform_mutex_unlock ( mutex ) )
    {
        const i64 error = platform_error_code ();
        char* message = platform_error_message ( error );
        LOGERROR ( "platform_mutex_unlock ("PLATFORM_STRING"): pthread_mutex_unlock failed on mutex %@.\n\t                                   Reason: %S\n\t                                   Code:   %i"
                 , mutex
                 , message
                 , error
                 );
        string_destroy ( message );
        return false;
    }

    return true;
}

i32
_platform_thread_create
(   thread_start_function_t function
,   void*                   args
,   bool                    auto_detach
,   thread_t*               thread
)
{
    DISABLE_WARNING ( -Wcast-function-type );
    const i32 result = pthread_create ( ( pthread_t* ) &( *thread ).id
                                      , 0
                                      , ( void* (*)( void* ) ) function
                                      , args
                                      );
    REENABLE_WARNING ();

    if ( result )
    {
        return PLATFORM_MAC_ERROR_PTHREAD_CREATE;
    }

    if ( auto_detach )
    {
        if ( pthread_detach ( ( *thread ).id ) )
        {
            return PLATFORM_MAC_ERROR_PTHREAD_DETACH;
        }
    }

    ( *thread ).internal = memory_allocate ( sizeof ( u64 )
                                           , MEMORY_TAG_THREAD
                                           );
    *( ( u64* )( ( *thread ).internal ) ) = ( *thread ).id;

    return 0;
}

i32
_platform_thread_destroy
(   thread_t* thread
)
{
    return _platform_thread_cancel ( thread );
}

i32
_platform_thread_detach
(   thread_t* thread
)
{
    if ( !thread || !( *thread ).internal )
    {
        return 0;
    }

    if ( pthread_detach ( *( ( pthread_t* )( ( *thread ).internal ) ) ) )
    {
        return PLATFORM_MAC_ERROR_PTHREAD_DETACH;
    }

    memory_free ( ( *thread ).internal , sizeof ( u64 ) , MEMORY_TAG_THREAD );
    ( *thread ).internal = 0;

    return 0;
}

i32
_platform_thread_cancel
(   thread_t* thread
)
{
    if ( pthread_cancel ( *( ( pthread_t* )( ( *thread ).internal ) ) ) )
    {
        return PLATFORM_MAC_ERROR_PTHREAD_CANCEL;
    }

    memory_free ( ( *thread ).internal , sizeof ( u64 ) , MEMORY_TAG_THREAD );
    ( *thread ).internal = 0;
    ( *thread ).id = 0;

    return 0;
}

i32
_platform_thread_wait
(   thread_t* thread
)
{   // TODO: Implement this.
    return 0;
}

i32
_platform_thread_wait_timeout
(   thread_t*   thread
,   const u64   timeout_ms
)
{   // TODO: Implement this.
    return 0;
}

i32
_platform_thread_active
(   thread_t* thread
)
{   // TODO: Implement this.
    return !( *thread ).internal;
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
    pthread_mutexattr_t pthread_mutex_attr;
    pthread_mutexattr_init ( &pthread_mutex_attr );
    pthread_mutexattr_settype ( &pthread_mutex_attr , PTHREAD_MUTEX_RECURSIVE );
    
    pthread_mutex_t pthread_mutex;
    if ( pthread_mutex_init ( &pthread_mutex , &pthread_mutex_attr ) )
    {
        return PLATFORM_MAC_ERROR_PTHREAD_MUTEX_INIT;
    }

    ( *mutex ).internal = memory_allocate ( sizeof ( pthread_mutex_t )
                                          , MEMORY_TAG_MUTEX
                                          );
    *( ( pthread_mutex_t* )( ( *mutex ).internal ) ) = pthread_mutex;

    return 0;
}

i32
_platform_mutex_destroy
(   thread_t* thread
)
{
    if ( pthread_mutex_destroy ( ( pthread_mutex_t* )( ( *mutex ).internal ) ) )
    {
        return PLATFORM_MAC_ERROR_PTHREAD_MUTEX_DESTROY;
    }

    memory_free ( ( *mutex ).internal
                , sizeof ( pthread_mutex_t* )
                , MEMORY_TAG_MUTEX
                );
    ( *mutex ).internal = 0;

    return 0;
}

i32
_platform_mutex_lock
(   thread_t* thread
)
{
    if ( pthread_mutex_lock ( ( pthread_mutex_t* )( ( *mutex ).internal ) ) )
    {
        return PLATFORM_MAC_ERROR_PTHREAD_MUTEX_LOCK;
    }
    return 0;
}

i32
_platform_mutex_lock
(   thread_t* thread
)
{
    if ( pthread_mutex_unlock ( ( pthread_mutex_t* )( ( *mutex ).internal ) ) )
    {
        return PLATFORM_MAC_ERROR_PTHREAD_MUTEX_UNLOCK;
    }
    return 0;
}

#endif  // End platform layer.
////////////////////////////////////////////////////////////////////////////////
