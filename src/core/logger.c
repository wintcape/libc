/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file core/logger.c
 * @brief Implementation of the core/logger and core/assert headers.
 * (see core/logger.h and core/assert.h for additional details)
 */
#include "core/assert.h"
#include "core/logger.h"

#include "container/string.h"

#include "core/memory.h"

/** @brief Log filepath. */
#define LOG_FILEPATH "console.log"

/** @brief Output message prefixes. */
static const char* log_level_prefixes[] = { LOG_LEVEL_PREFIX_FATAL
                                          , LOG_LEVEL_PREFIX_ERROR
                                          , LOG_LEVEL_PREFIX_WARN
                                          , LOG_LEVEL_PREFIX_INFO
                                          , LOG_LEVEL_PREFIX_DEBUG
                                          , LOG_LEVEL_PREFIX_TRACE
                                          , LOG_LEVEL_PREFIX_SILENT
                                          };

/** @brief Output message colors. */
static const char* log_level_colors[] = { LOG_LEVEL_COLOR_FATAL
                                        , LOG_LEVEL_COLOR_ERROR
                                        , LOG_LEVEL_COLOR_WARN
                                        , LOG_LEVEL_COLOR_INFO
                                        , LOG_LEVEL_COLOR_DEBUG
                                        , LOG_LEVEL_COLOR_TRACE
                                        };

/** @brief Type definition for logger subsystem state. */
typedef struct
{
    file_t file;
}
state_t;

/** @brief Global subsystem state. */
static state_t* state = 0;

/**
 * @brief Primary implementation of print (see print).
 * 
 * @param file File to print to.
 * @param string The string to print.
 * @param string_length The number of characters in the string.
 */
void
_print
(   file_t*         file
,   const char*     string
,   const u64       string_length
);

/**
 * @brief Appends a message to the log file.
 * 
 * Use logger_file_append to explicitly specify string length, or 
 * _logger_file_append to compute the lengths of null-terminated strings before
 * passing them to logger_file_append.
 * 
 * @param message The message string to append.
 * @param message_length The message length (in characters).
 */
void
logger_file_append
(   const char* message
,   const u64   message_length
);

#define _logger_file_append(message) \
    logger_file_append ( (message) , _string_length ( message ) )

bool
logger_startup
(   u64*    memory_requirement
,   void*   state_
)
{
    if ( !memory_requirement )
    {
        LOGERROR ( "logger_startup: Missing argument: memory_requirement." );
        return false;
    }

    *memory_requirement = sizeof ( state_t );

    if ( !state_ )
    {
        return true;
    }

    state = state_;
    memory_clear ( state , sizeof ( state_t ) );
    
    // Initialize log file.
    if ( !file_open ( LOG_FILEPATH , FILE_MODE_WRITE , &( *state ).file ) )
    {
        PRINTERROR ( LOG_LEVEL_COLOR_ERROR LOG_LEVEL_PREFIX_ERROR
                     "logger_startup: Unable to open '"LOG_FILEPATH"' for writing."
                     ANSI_CC_RESET "\n"
                   );
        return false;
    }

    return true;
}

void
logger_shutdown
(   void* state_
)
{
    if ( !state )
    {
        return;
    }

    // Close log file.
    file_close ( &( *state ).file );

    state = 0;
}

void
logger_log
(   const LOG_LEVEL level
,   const char*     message
,   args_t          args
)
{
    const bool err = level < LOG_WARN;
    const bool colored = level != LOG_INFO;

    char* raw = _string_format ( message , args );

    // Write plaintext to log file.
    _string_insert ( raw , 0 , log_level_prefixes[ level ] );
    logger_file_append ( raw , string_length ( raw ) );
    if ( level == LOG_SILENT )
    {
        return;
    }
    string_remove ( raw , 0 , _string_length ( log_level_prefixes[ level ] ) );

    // Write ANSI-formatted text to console.
    char* formatted = string_format ( ANSI_CC_RESET"%s%s%s%S"ANSI_CC_RESET"\n"
                                    , log_level_colors[ level ]
                                    , log_level_prefixes[ level ]
                                    , ( colored ) ? "" : ANSI_CC_RESET
                                    , raw
                                    );
    file_t file;
    ( err ) ? file_stderr ( &file )
            : file_stdout ( &file )
            ;
    _print ( &file , formatted , string_length ( formatted ) );

    string_destroy ( raw );
    string_destroy ( formatted );
}

void
print
(   file_t*         file
,   const char*     message
,   args_t          args
)
{
    if ( !file || !( *file ).handle || !( *file ).valid )
    {
        return;
    }
    char* raw = _string_format ( message , args );
    char* formatted = string_format ( ANSI_CC_RESET"%S"ANSI_CC_RESET  , raw );
    _print ( file , formatted , string_length ( formatted ) );
    string_destroy ( raw );
    string_destroy ( formatted );
}

void
assertf
(   const char* expression
,   const char* message
,   const char* file
,   const i32   line
)
{
    if ( *message == '\0' )
    {
        LOGFATAL ( "Assertion failure in file %s (line %i): %s"
                 , file , line , expression
                 );
    }
    else
    {
        LOGFATAL ( "Assertion failure in file %s (line %i): %s\n\tMessage: %s"
                 , file , line , expression , message
                 );
    }
}

void
_print
(   file_t*         file
,   const char*     string
,   const u64       string_length
)
{
    u64 written;
    file_write ( file
               , ( string_length + 1 ) * sizeof ( char )
               , string
               , &written
               );
}

void
logger_file_append
(   const char* message
,   const u64   message_length
)
{
    if (   !state
        || !( *state ).file.handle
        || !( *state ).file.valid
        || !message
        || !message_length
       )
    {
        return;
    }
    if ( !file_write_line ( &( *state ).file
                          , ( message_length + 1 ) * sizeof ( char )
                          , message
                          ))
    {
        PRINTERROR ( LOG_LEVEL_COLOR_ERROR
                     "logger_file_append: Error writing to log file '"LOG_FILEPATH"'."
                     ANSI_CC_RESET "\n"
                   );
    }
}
