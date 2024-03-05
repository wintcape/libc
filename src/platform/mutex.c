/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file platform/mutex.c
 * @brief Implementation of the platform/mutex header.
 * (see platform/mutex.h for additional details)
 */
#include "platform/mutex.h"
#include "platform/platform.h"

bool
mutex_create
(   mutex_t* mutex
)
{
    return platform_mutex_create ( mutex );
}

void
mutex_destroy
(   mutex_t* mutex
)
{
    platform_mutex_destroy ( mutex );
}

bool
mutex_lock
(   mutex_t* mutex
)
{
    return platform_mutex_lock ( mutex );
}

bool
mutex_unlock
(   mutex_t* mutex
)
{
    return platform_mutex_unlock ( mutex );
}
