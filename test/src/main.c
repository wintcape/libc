/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file main.c
 * @brief Entry point for the test suite program.
 * 
 * SYSTEM REQUIREMENTS: ~6.00 GiB free disk space.
 *                      ~2.81 GiB free RAM.
 */
#include "test/test.h"

#include "core/logger.h"
#include "core/memory.h"

#include "container/test_array.h"
#include "container/test_hashtable.h"
#include "container/test_freelist.h"
#include "container/test_queue.h"
#include "container/test_string.h"

#include "memory/test_dynamic_allocator.h"
#include "memory/test_linear_allocator.h"

#include "platform/test_filesystem.h"

/** @brief Rough bound on maximum system memory usage: 2.50 GiB. */
#define TEST_MEMORY_REQUIREMENT \
    GiB ( 2.5 )

/** @brief Log filepath. */
#define LOG_FILEPATH "console.log"

/** @brief Colored text. */
#define COLORED(text) \
    ANSI_CC ( ANSI_CC_FG_DARK_GREEN ) text ANSI_CC_RESET

int
main
( void )
{
    memory_startup ( TEST_MEMORY_REQUIREMENT );
    logger_startup ( LOG_FILEPATH , 0 , 0 );

    // Initialize tests.
    test_startup ();
    test_register_linear_allocator ();
    test_register_freelist ();
    test_register_dynamic_allocator ();
    test_register_array ();
    test_register_string ();
    test_register_queue ();
    test_register_hashtable ();
    test_register_filesystem ();

    // Run tests.
    LOGINFO ( "Running test suite. . ." );
    const bool fail = test_run_all ();
    if ( !fail )
    {
        PRINT ( COLORED ( "libc ("PLATFORM_STRING") ver. %i.%i.%i: All tests passed." ) "\n\n"
              , VERSION_MAJOR , VERSION_MINOR , VERSION_PATCH
              );
    }
    
    logger_shutdown ();
    memory_shutdown ();
    return fail;
}
