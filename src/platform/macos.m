/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file platform/macos.c
 * @brief Implementation of the platform header for GNU/Linux-based
 * operating systems.
 * (see platform.h for additional details)
 */
#include "platform/platform.h"

////////////////////////////////////////////////////////////////////////////////
// Begin platform layer.
#if PLATFORM_MAC == 1

#include "core/logger.h"
#include "core/memory.h"

// Platform layer dependencies.
#include <errno.h>
#include <mach/mach_time.h>
#include <pthread.h>
#if _POSIX_C_SOURCE >= 199309L
    #include <time.h>   // nanosleep
#else
    #include <unistd.h> // usleep
#endif

// Standard libc dependencies.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
            LOGERROR ( "platform_thread_create: Missing argument: function (threaded process to run)." );
            return false;
        }
        if ( !thread )
        {
            LOGERROR ( "platform_thread_create: Missing argument: thread (output buffer)." );
            return false;
        }
    }

    // TODO: Fix this.
    i32 result;
    DISABLE_WARNING ( -Wcast-function-type );
    result = pthread_create ( ( pthread_t* ) &( *thread ).id
                            , 0
                            , ( void* (*)( void* ) ) function
                            , args
                            );
    REENABLE_WARNING ();
    
    if ( result )
    {
        switch ( result )
        {
            case EAGAIN:
            {
                LOGERROR ( "platform_thread_create: Thread creation failed.\n\tReason:  Insufficient resources available on the host platform." );
                return false;
            }
            case EINVAL:
            {
                LOGERROR ( "platform_thread_create: Thread creation failed.\n\tReason:  Invalid thread attribute(s) supplied as arguments." );
                return false;
            }
            default:
            {
                LOGERROR ( "platform_thread_create: Thread creation failed.\n\tReason:  %s\n\tCode:    %i"
                         , strerror ( errno )
                         , errno
                         );
                return false;
            }
        }
    }

    LOGDEBUG ( "Starting process on new thread %u." , ( *thread ).id );

    if ( auto_detach )
    {
        result = pthread_detach ( ( *thread ).id );
        if ( result )
        {
            switch ( result )
            {
                case EINVAL:
                {
                    LOGERROR ( "platform_thread_create: Failed to detach the new thread.\n\tReason:  Thread is non-joinable." );
                    return false;
                }
                case ESRCH:
                {
                    LOGERROR ( "platform_thread_create: Failed to detach the new thread.\n\tReason:  No thread with the ID %u could be found."
                             , ( *thread ).id
                             );
                    return false;
                }
                default:
                {
                    LOGERROR ( "platform_thread_create: Failed to detach the new thread.\n\tReason:  %s\n\tCode:    %i"
                             , strerror ( errno )
                             , errno
                             );
                    return false;
                }
            }
        }
    }
    else
    {
        ( *thread ).internal = memory_allocate ( sizeof ( u64 )
                                               , MEMORY_TAG_THREAD
                                               );
        *( ( u64* )( ( *thread ).internal ) ) = ( *thread ).id;
    }

    return true;
}

void
platform_thread_destroy
(   thread_t* thread
)
{
    thread_cancel ( thread );
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
    const i32 result = pthread_detach ( *( ( pthread_t* )( ( *thread ).internal ) ) );
    if ( result )
    {
        switch ( result )
        {
            case EINVAL:
            {
                LOGERROR ( "platform_thread_detach: Failed to detach thread %u.\n\tReason:  Thread is non-joinable."
                         , ( *thread ).id
                         );
                break;
            }
            case ESRCH:
            {
                LOGERROR ( "platform_thread_detach: Failed to detach thread %u.\n\tReason:  No thread with the ID could be found."
                         , ( *thread ).id
                         );
                break;
            }
            default:
            {
                LOGERROR ( "platform_thread_detach: Failed to detach thread %u.\n\tReason:  %s\n\tCode:    %i"
                         , ( *thread ).id
                         , strerror ( errno )
                         , errno
                         );
                break;
            }
        }
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
    const i32 result = pthread_cancel ( *( ( pthread_t* )( ( *thread ).internal ) ) );
    if ( result )
    {
        switch ( result )
        {
            case ESRCH:
            {
                LOGERROR ( "platform_thread_detach: Failed to detach thread %u.\n\tReason:  No thread with the ID could be found."
                         , ( *thread ).id
                         );
                break;
            }
            default:
            {
                LOGERROR ( "platform_thread_detach: Failed to detach thread %u.\n\tReason:  %s\n\tCode:    %i"
                         , ( *thread ).id
                         , strerror ( errno )
                         , errno
                         );
                break;
            }
        }
    }
    memory_free ( ( *thread ).internal , sizeof ( u64 ) , MEMORY_TAG_THREAD );
    ( *thread ).internal = 0;
    ( *thread ).id = 0;
}

bool
platform_thread_wait
(   thread_t* thread
)
{   // TODO: Implement this.
    if ( !thread || !( *thread ).internal )
    {
        return false;
    }
    return true;
}

bool
platform_thread_wait_timeout
(   thread_t*   thread
,   const u64   timeout_ms
)
{   // TODO: Implement this.
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
{   // TODO: Implement this.
    return ( *thread ).internal;
}

void
platform_thread_sleep
(   thread_t*   thread
,   const u64   ms
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
        LOGERROR ( "platform_mutex_create: Missing argument: mutex (output buffer)." );
        return false;
    }

    pthread_mutexattr_t pthread_mutex_attr;
    pthread_mutexattr_init ( &pthread_mutex_attr );
    pthread_mutexattr_settype ( &pthread_mutex_attr , PTHREAD_MUTEX_RECURSIVE );
    
    pthread_mutex_t pthread_mutex;
    const i32 result = pthread_mutex_init ( &pthread_mutex
                                          , &pthread_mutex_attr
                                          );
    if ( result )
    {
        LOGERROR ( "platform_mutex_create: pthread_mutex_init failed with result %i."
                 , result
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
    const i32 result = pthread_mutex_destroy ( ( pthread_mutex_t* )( ( *mutex ).internal ) );
    switch ( result )
    {
        case 0:
        {}
        break;

        case EBUSY:
        {
            LOGERROR ( "platform_mutex_destroy: Failed to destroy mutex %@.\n\tReason:  Mutex is locked or referenced."
                     , ( *mutex ).internal
                     );
        }
        break;

        case EINVAL:
        {
            LOGERROR ( "platform_mutex_destroy: Failed to destroy mutex %@.\n\tReason:  Invalid mutex."
                     , ( *mutex ).internal
                     );
        }
        break;

        default:
        {
            LOGERROR ( "platform_mutex_destroy: Failed to destroy mutex %@.\n\tReason:  %s\n\tCode:    %i"
                     , ( *mutex ).internal
                     , strerror ( errno )
                     , errno
                     );
        }
        break;
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
        LOGERROR ( "platform_mutex_lock: No mutex was provided." );
        return false;
    }

    const i32 result = pthread_mutex_lock ( ( pthread_mutex_t* )( ( *mutex ).internal ) );
    switch ( result )
    {
        case 0:
        {}
        break;

        case EOWNERDEAD:
        {
            LOGERROR ( "platform_mutex_lock: pthread_mutex_lock failed on mutex %@.\n\tReason:  The owner thread was terminated while the mutex was still active."
                     , ( *mutex ).internal
                     );
            return false;
        }
        case EAGAIN:
        {
            LOGERROR ( "platform_mutex_lock: pthread_mutex_lock failed on mutex %@.\n\tReason:  The maximum number of recursive mutex locks has been reached."
                     , ( *mutex ).internal
                     );
            return false;
        }
        case EBUSY:
        {
            LOGERROR ( "platform_mutex_lock: pthread_mutex_lock failed on mutex %@.\n\tReason:  A mutex lock already exists."
                     , ( *mutex ).internal
                     );
            return false;
        }
        case EDEADLK:
        {
            LOGERROR ( "platform_mutex_lock: pthread_mutex_lock failed on mutex %@.\n\tReason:  A mutex deadlock was detected."
                     , ( *mutex ).internal
                     );
            return false;
        }
        default:
        {
            LOGERROR ( "platform_mutex_lock: pthread_mutex_lock failed on mutex %@.\n\tReason:  %s\n\tCode:    %i"
                     , ( *mutex ).internal
                     , strerror ( errno )
                     , errno
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
        LOGERROR ( "platform_mutex_unlock: No mutex was provided." );
        return false;
    }

    const i32 result = pthread_mutex_unlock ( ( pthread_mutex_t* )( ( *mutex ).internal ) );
    switch ( result )
    {
        case 0:
        {}
        break;

        case EOWNERDEAD:
        {
            LOGERROR ( "platform_mutex_unlock: pthread_mutex_unlock failed on mutex %@.\n\tReason:  The owner thread was terminated while the mutex was still active."
                     , ( *mutex ).internal
                     );
            return false;
        }
        case EPERM:
        {
            LOGERROR ( "platform_mutex_unlock: pthread_mutex_unlock failed on mutex %@.\n\tReason:  The mutex is not owned by the current thread."
                     , ( *mutex ).internal
                     );
            return false;
        }
        default:
        {
            LOGERROR ( "platform_mutex_unlock: pthread_mutex_unlock failed on mutex %@.\n\tReason:  %s\n\tCode:    %i"
                     , ( *mutex ).internal
                     , strerror ( errno )
                     , errno
                     );
            return false;
        }
    }

    return true;
}

#endif  // End platform layer.
////////////////////////////////////////////////////////////////////////////////
