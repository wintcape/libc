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

/** @brief Rough bound on maximum system memory usage. */
#define TEST_MEMORY_REQUIREMENT \
    GIBIBYTES ( 2.5 )

/** @brief Type definition for program state. */
typedef struct
{
    u64     logger_memory_requirement;
    void*   logger;
}
state_t;

/** @brief Program state. */
static state_t* state;

int
main
( void )
{
    memory_startup ( TEST_MEMORY_REQUIREMENT );
    state = memory_allocate ( sizeof ( state_t )
                            , MEMORY_TAG_APPLICATION
                            );
    
    // Initialize logger.
    logger_startup ( &( *state ).logger_memory_requirement , 0 );
    ( *state ).logger = memory_allocate ( ( *state ).logger_memory_requirement
                                        , MEMORY_TAG_APPLICATION
                                        );
    logger_startup ( &( *state ).logger_memory_requirement
                   , ( *state ).logger
                   );

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
    LOGDEBUG ( "Running test suite. . ." );
    const bool fail = test_run_all ();

    // Shutdown logger.
    logger_shutdown ( ( *state ).logger );
    memory_free ( ( *state ).logger
                , ( *state ).logger_memory_requirement
                , MEMORY_TAG_APPLICATION
                );

    memory_free ( state , sizeof ( state_t ) , MEMORY_TAG_APPLICATION );
    memory_shutdown ();

    return fail;
}
