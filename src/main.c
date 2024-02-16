/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file main.c
 * @brief Program entry point.
 */
#include "core/logger.h"
#include "platform/platform.h"

#define COLORED \
    ANSI_CC ( ANSI_CC_FG_DARK_GREEN )

int
main
(   int     argc
,   char*   argv[]
)
{
    PRINT ( COLORED "VERSION:  %i.%i.%i\n"
          , VERSION_MAJOR , VERSION_MINOR , VERSION_PATCH
          );
    LOGINFO ( "Querying "PLATFORM_STRING" for the available system processor core count. . ." );
    platform_processor_core_count ();
    PRINT ( "\n" );
    return 0;
}