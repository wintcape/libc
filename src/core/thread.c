/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file core/thread.c
 * @brief Implementation of the core/thread header.
 * (see core/thread.h for additional details)
 */
#include "core/thread.h"

#include "core/logger.h"

#include "platform/platform.h"

bool
thread_create
(   thread_start_function_t function
,   void*                   args
,   bool                    auto_detach
,   thread_t*               thread
)
{
    if ( !platform_thread_create ( function , args , auto_detach , thread ) )
    {
        return false;
    }
    
    LOGDEBUG ( "thread_create: Starting process on new%sthread #%u."
             , auto_detach ? " auto-detach " : " "
             , ( *thread ).id
             );
    
    return true;
}

void
thread_destroy
(   thread_t* thread
)
{
    platform_thread_destroy ( thread );
}

void
thread_detach
(   thread_t* thread
)
{
    platform_thread_detach ( thread );
}

void
thread_cancel
(   thread_t* thread
)
{
    platform_thread_cancel ( thread );
}

bool
thread_wait
(   thread_t* thread
)
{
    return platform_thread_wait ( thread );
}

bool
thread_wait_timeout
(   thread_t*   thread
,   const u64   timeout_ms
)
{
    return platform_thread_wait_timeout ( thread , timeout_ms );
}

bool
thread_active
(   thread_t* thread
)
{
    return platform_thread_active ( thread );
}

void
thread_sleep
(   thread_t*   thread
,   const u64   ms
)
{
    platform_thread_sleep ( thread , ms );
}

u64
thread_id
( void )
{
    return platform_thread_id ();
}
