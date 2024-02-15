/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file test/test.c
 * @brief Implementation of test/test header.
 * (see test/test.h for additional details)
 */
#include "test/test.h"

#include "container/array.h"
#include "container/string.h"

#include "core/clock.h"
#include "core/logger.h"
#include "core/memory.h"

#include "math/math.h"

static test_entry_t* tests;

void
test_startup
( void )
{
    tests = array_create_new ( test_entry_t );    
}

void
test_register
(   u8      ( *PFN_test )()
,   char*   description
)
{
    test_entry_t entry;
    entry.function = PFN_test;
    entry.description = description;
    array_push ( tests , entry );
}

bool
test_run_all
( void )
{
    const u32 test_count = array_length ( tests );
    
    u32 pass = 0;
    u32 fail = 0;
    u32 skip = 0;

    u64 clock_master_elapsed_seconds;
    u64 clock_master_elapsed_minutes;
    u64 clock_master_elapsed_hours;
    f64 clock_master_elapsed_fractional;

    clock_t clock_master;
    clock_start ( &clock_master );

    for ( u32 i = 0; i < test_count; ++i )
    {
        u64 clock_test_elapsed_seconds;
        u64 clock_test_elapsed_minutes;
        u64 clock_test_elapsed_hours;
        f64 clock_test_elapsed_fractional;

        clock_t clock_test;
        clock_start ( &clock_test );
        u8 result = tests[ i ].function ();
        clock_update ( &clock_test );

        if ( result == true )
        {
            pass += 1;
        }
        else if ( result == BYPASS )
        {
            LOGWARN ( "    SKIPPED:\t%s" , tests[ i ].description );
            skip += 1;
        }
        else
        {
            LOGERROR ( "    FAILED:\t%s" , tests[ i ].description );
            fail += 1;
        }

        char* status = string_format ( fail ? "*** %u FAILED ***"
                                            : "SUCCESS"
                                     , fail
                                     );

        clock_update ( &clock_master );

        display_time ( clock_master.elapsed
                     , &clock_master_elapsed_hours
                     , &clock_master_elapsed_minutes
                     , &clock_master_elapsed_seconds
                     , &clock_master_elapsed_fractional
                     );
        display_time ( clock_test.elapsed
                     , &clock_test_elapsed_hours
                     , &clock_test_elapsed_minutes
                     , &clock_test_elapsed_seconds
                     , &clock_test_elapsed_fractional
                     );
    
        LOGINFO ( "Executed %u of %u (%u skipped) %s (%Pl02u:%pl02u:%pl02u.%.6d / %Pl02u:%pl02u:%pl02u.%.6d)."
                , i + 1
                , test_count
                , skip
                , status
                , clock_test_elapsed_hours
                , clock_test_elapsed_minutes
                , clock_test_elapsed_seconds
                , &clock_test_elapsed_fractional
                , clock_master_elapsed_hours
                , clock_master_elapsed_minutes
                , clock_master_elapsed_seconds
                , &clock_master_elapsed_fractional
                );

        string_destroy ( status );
    }

    clock_stop ( &clock_master );

    display_time ( clock_master.elapsed
                 , &clock_master_elapsed_hours
                 , &clock_master_elapsed_minutes
                 , &clock_master_elapsed_seconds
                 , &clock_master_elapsed_fractional
                 );

    LOGINFO ( "Results: %u passed, %u failed, %u skipped.\n\t"
              "Took %Pl02u:%pl02u:%pl02u.%.6d seconds."
            , pass
            , fail
            , skip
            , clock_master_elapsed_hours
            , clock_master_elapsed_minutes
            , clock_master_elapsed_seconds
            , &clock_master_elapsed_fractional
            );

    array_destroy ( tests );

    return fail;
}