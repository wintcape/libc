/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file core/logger.h
 * @brief Provides an interface for an application subsystem which handles
 * the logging of runtime information.
 */
#ifndef LOGGER_H
#define LOGGER_H

#include "common.h"

#include "core/string.h"

#include "platform/platform.h"
#include "platform/filesystem.h"

/** @brief Type and instance definitions for log elevation. */
typedef enum
{
    LOG_FATAL       = 0
,   LOG_ERROR       = 1
,   LOG_WARN        = 2
,   LOG_INFO        = 3
,   LOG_DEBUG       = 4
,   LOG_TRACE       = 5
,   LOG_SILENT      = 6

,   LOG_LEVEL_COUNT = 7
}
LOG_LEVEL;

#define LOG_WARN_ENABLED   1 /** @brief Enable log elevation LOG_WARN? Y\N */
#define LOG_INFO_ENABLED   1 /** @brief Enable log elevation LOG_INFO? Y\N */
#define LOG_DEBUG_ENABLED  1 /** @brief Enable log elevation LOG_DEBUG? Y\N */
#define LOG_TRACE_ENABLED  1 /** @brief Enable log elevation LOG_TRACE? Y\N */
#define LOG_SILENT_ENABLED 1 /** @brief Enable log elevation LOG_SILENT? Y\N */

// Auto-disable DEBUG and TRACE level logging for release builds.
#if VERSION_RELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

// Defines logger output message prefixes.
#define LOG_LEVEL_PREFIX_FATAL  "[FATAL]\t" /** @brief Logger output message prefix (LOG_FATAL). */
#define LOG_LEVEL_PREFIX_ERROR  "[ERROR]\t" /** @brief Logger output message prefix (LOG_ERROR). */
#define LOG_LEVEL_PREFIX_WARN   "[WARN]\t"  /** @brief Logger output message prefix (LOG_WARN). */
#define LOG_LEVEL_PREFIX_INFO   "[INFO]\t"  /** @brief Logger output message prefix (LOG_INFO). */
#define LOG_LEVEL_PREFIX_DEBUG  "[DEBUG]\t" /** @brief Logger output message prefix (LOG_DEBUG). */
#define LOG_LEVEL_PREFIX_TRACE  "[TRACE]\t" /** @brief Logger output message prefix (LOG_TRACE). */
#define LOG_LEVEL_PREFIX_SILENT "\t\t"      /** @brief Logger output message prefix (LOG_SILENT). */

// Defines logger output message colors (for pretty-print in console).
#define LOG_LEVEL_COLOR_FATAL      ANSI_CC ( ANSI_CC_BG_DARK_RED )    /** @brief Logger output message color (LOG_FATAL). */
#define LOG_LEVEL_COLOR_ERROR      ANSI_CC ( ANSI_CC_FG_RED )         /** @brief Logger output message color (LOG_ERROR). */
#define LOG_LEVEL_COLOR_WARN       ANSI_CC ( ANSI_CC_FG_YELLOW )      /** @brief Logger output message color (LOG_WARN). */
#define LOG_LEVEL_COLOR_INFO       ANSI_CC ( ANSI_CC_FG_DARK_GREEN )  /** @brief Logger output message color (LOG_INFO). */
#define LOG_LEVEL_COLOR_DEBUG      ANSI_CC ( ANSI_CC_FG_GRAY )        /** @brief Logger output message color (LOG_DEBUG). */
#define LOG_LEVEL_COLOR_TRACE      ANSI_CC ( ANSI_CC_FG_DARK_YELLOW ) /** @brief Logger output message color (LOG_TRACE). */

/**
 * @brief Initializes the logger subsystem.
 * 
 * Call once to get the memory requirement; call a second time passing in a
 * valid memory buffer of the required size.
 * 
 * @param memory_requirement Output buffer to read memory requirement.
 * @param state Pass 0 to read memory requirement. Otherwise, pass a buffer.
 * @return true on success; false otherwise.
 */
bool
logger_startup
(   u64*    memory_requirement
,   void*   state
);

/**
 * @brief Terminates the logger subsystem.
 * 
 * @param state Internal subsystem state.
 */
void
logger_shutdown
(   void* state
);

/**
 * @brief Logs a message according to the logging elevation protocol.
 * 
 * @param level The log elevation.
 * @param message Formatted message to log (see container/string/format.h).
 * @param args Variadic argument list (see common/args.h).
 */
void
logger_log
(   const LOG_LEVEL level
,   const char*     message
,   args_t          args
);

/** @brief Alias for calling logger_log with __VA_ARGS__. */
#define LOG(level,message,...)                                     \
    do                                                             \
    {                                                              \
        DISABLE_WARNING ( -Wint-conversion )                       \
        logger_log ( (level) , (message) , ARGS ( __VA_ARGS__ ) ); \
        REENABLE_WARNING ()                                        \
    }                                                              \
    while ( 0 )

// LOG: Fatal.
#define LOGFATAL(message,...) \
    LOG ( LOG_FATAL , (message) , ##__VA_ARGS__ )

// LOG: Error.
#define LOGERROR(message,...) \
    LOG ( LOG_ERROR , (message) , ##__VA_ARGS__ )

// LOG: Warn.
#if LOG_WARN_ENABLED == 1
#define LOGWARN(message,...) \
    LOG ( LOG_WARN , (message) , ##__VA_ARGS__ )
#else
#define LOGWARN(message,...)
#endif

// LOG: Info.
#if LOG_INFO_ENABLED == 1
#define LOGINFO(message,...) \
    LOG ( LOG_INFO , (message) , ##__VA_ARGS__ )
#else
#define LOGINFO(message,...)
#endif

// LOG: Debug.
#if LOG_DEBUG_ENABLED == 1
#define LOGDEBUG(message,...) \
    LOG ( LOG_DEBUG , (message) , ##__VA_ARGS__ )
#else
#define LOGDEBUG(message,...)
#endif

// LOG: Trace.
#if LOG_TRACE_ENABLED == 1
#define LOGTRACE(message,...) \
    LOG ( LOG_TRACE , (message) , ##__VA_ARGS__ )
#else
#define LOGTRACE(message,...)
#endif

// LOG: Silent.
#if LOG_SILENT_ENABLED == 1
#define LOGSILENT(message,...) \
    LOG ( LOG_SILENT , (message) , ##__VA_ARGS__ )
#else
#define LOGSILENT(message,...)
#endif

/**
 * @brief Logs a platform-specific formatted error message.
 */
#define platform_log_error(message,...)                      \
    do                                                       \
    {                                                        \
        const i64 error__ = platform_error_code ();          \
        char message__[ STACK_STRING_MAX_SIZE ];             \
        platform_error_message ( error__                     \
                               , message__                   \
                               , STACK_STRING_MAX_SIZE       \
                               );                            \
        LOGERROR ( message "\n\tReason:  %s.\n\tCode:    %i" \
                 , ##__VA_ARGS__                             \
                 , message__                                 \
                 , error__                                   \
                 );                                          \
    }                                                        \
    while ( 0 );

/**
 * @brief Writes a formatted message to file.
 * 
 * Use PRINT to print to stdout, use PRINTERROR to print to stderr.
 * 
 * @param file The file to print to.
 * @param message Formatted message to print to file 
 * (see container/string/format.h).
 * @param args Variadic argument list (see common/args.h).
 */
void
print
(   file_t*         file
,   const char*     message
,   args_t          args
);

/** @brief Alias for calling print with __VA_ARGS__. */
#define PRINT(message,...)                                  \
    do                                                      \
    {                                                       \
        file_t file;                                        \
        file_stdout ( &file );                              \
        DISABLE_WARNING ( -Wint-conversion )                \
        print ( &file , (message) , ARGS ( __VA_ARGS__ ) ); \
        REENABLE_WARNING ()                                 \
    }                                                       \
    while ( 0 )

/** @brief Alias for calling print with __VA_ARGS__. */
#define PRINTERROR(message,...)                             \
    do                                                      \
    {                                                       \
        file_t file;                                        \
        file_stderr ( &file );                              \
        DISABLE_WARNING ( -Wint-conversion )                \
        print ( &file , (message) , ARGS ( __VA_ARGS__ ) ); \
        REENABLE_WARNING ()                                 \
    }                                                       \
    while ( 0 );

#endif  // LOGGER_H