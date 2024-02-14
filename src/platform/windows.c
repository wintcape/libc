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

#include <windows.h>
#include <windowsx.h>

#include "container/string.h"

#include "core/logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
platform_get_absolute_time
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

bool
platform_thread_create
(   PFN_thread_start    function
,   void*               args
,   bool                auto_detach
,   thread_t*           thread
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

    ( *thread ).internal = CreateThread ( 0
                                        , 0
                                        , ( LPTHREAD_START_ROUTINE ) function
                                        , args
                                        , 0
                                        , ( DWORD* ) &( *thread ).id
                                        );
    LOGDEBUG ( "Starting process on new thread #%u." , ( *thread ).id );

    if ( ( *thread ).internal )
    {
        return false;
    }

    if ( auto_detach )
    {
        CloseHandle( ( *thread ).internal );
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
    GetExitCodeThread ( ( *thread ).internal , &code );
    CloseHandle ( ( *thread ).internal );
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
    CloseHandle ( ( *thread ).internal );
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
    TerminateThread ( ( *thread ).internal , 0 );
    ( *thread ).internal = 0;
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
    const DWORD code = WaitForSingleObject ( ( *thread ).internal , INFINITE );
    return code == WAIT_OBJECT_0;
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
    const DWORD code = WaitForSingleObject ( ( *thread ).internal
                                           , timeout_ms
                                           );
    return code == WAIT_OBJECT_0;
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
    const DWORD code = WaitForSingleObject ( ( *thread ).internal , 0 );
    return code == WAIT_TIMEOUT;
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
    return GetCurrentThreadId ();
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

    ( *mutex ).internal = CreateMutex ( 0 , 0 , 0 );
    if ( !( *mutex ).internal )
    {
        LOGERROR ( "platform_mutex_create: CreateMutex failed." );
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

    CloseHandle ( ( *mutex ).internal );
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

    const DWORD result = WaitForSingleObject ( ( *mutex ).internal
                                             , INFINITE
                                             );
    switch ( result )
    {
        case WAIT_OBJECT_0:
        {
            return true;
        }
        case WAIT_ABANDONED:
        {
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
    return ReleaseMutex ( ( *mutex ).internal );
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
