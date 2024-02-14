/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file main.c
 * @brief Entry point for the test suite program.
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

/** @brief Rough bound on maximum system memory usage. */
#define TEST_MEMORY_REQUIREMENT \
    GIBIBYTES ( 2.5 )

int
main
( void )
{
    memory_startup ( TEST_MEMORY_REQUIREMENT );
    test_startup ();

    // Initialize tests.
    test_register_linear_allocator ();
    test_register_freelist ();
    test_register_dynamic_allocator ();
    test_register_array ();
    test_register_string ();
    test_register_queue ();
    test_register_hashtable ();
    test_register_filesystem ();

    // Run tests.
    LOGDEBUG ( "Running test suite. . ." );
    const bool fail = test_run_all ();

    memory_shutdown ();

    return fail;
}
