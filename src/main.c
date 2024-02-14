/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file main.c
 * @brief Program entry point.
 */
#include "core/logger.h"

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
    return 0;
}