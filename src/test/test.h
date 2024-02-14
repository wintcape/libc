/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file test/test.h
 * @brief Test management subsystem.
 */
#ifndef TEST_H
#define TEST_H

#include "common.h"

/** @brief Test bypass return code. */
#define BYPASS 2

/** @brief Type definition for a test callback function. */
typedef u8 ( *PFN_test )();

/** @brief Type definition for a container to hold test info. */
typedef struct
{
    PFN_test    function;
    char*       description;
}
test_entry_t;

/**
 * @brief Initializes the test manager.
 */
void
test_startup
( void );

/**
 * @brief Registers a test with the test manager.
 * 
 * @param PFN_test A callback function.
 * @param description Test description string.
 */
void
test_register
(   PFN_test
,   char*       description
);

/**
 * @brief Runs all registered tests.
 * @return true if all tests passed; false otherwise.
 */
bool
test_run_all
( void );

#endif  // TEST_H