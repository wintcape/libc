/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file core/logger.h
 * @brief Provides an interface for an application subsystem which handles
 * the logging of runtime information.
 */
#ifndef LOGGER_H
#define LOGGER_H

#include "common.h"

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
 * @return false on error, true otherwise.
 */
bool
logger_startup
(   u64*    memory_requirement
,   void*   state
);

/**
 * @brief Terminates the logger subsystem.
 * 
 * @param state .
 */
void
logger_shutdown
(   void* state
);

/**
 * @brief Logs a message according to the logging elevation protocol.
 * 
 * @param level The log elevation.
 * @param mesg Formatted message to log.
 * @param arg_count Number of elements in the variadic argument list.
 * @param args Variadic argument list.
 */
void
logger_log
(   const LOG_LEVEL level
,   const char*     mesg
,   u64             arg_count
,   u64*            args
);

/** @brief Alias for calling logger_log with __VA_ARGS__. */
#define LOG(level,mesg,...)                                     \
    do                                                          \
    {                                                           \
        DISABLE_WARNING ( -Wint-conversion )                    \
        logger_log ( (level) , (mesg) , ARGS ( __VA_ARGS__ ) ); \
        REENABLE_WARNING ()                                     \
    }                                                           \
    while ( 0 )

// LOG: Fatal.
#define LOGFATAL(mesg,...) \
    LOG ( LOG_FATAL , (mesg) , ##__VA_ARGS__ )

// LOG: Error.
#define LOGERROR(mesg,...) \
    LOG ( LOG_ERROR , (mesg) , ##__VA_ARGS__ )

// LOG: Warn.
#if LOG_WARN_ENABLED == 1
#define LOGWARN(mesg,...) \
    LOG ( LOG_WARN , (mesg) , ##__VA_ARGS__ )
#else
#define LOGWARN(mesg,...)
#endif

// LOG: Info.
#if LOG_INFO_ENABLED == 1
#define LOGINFO(mesg,...) \
    LOG ( LOG_INFO , (mesg) , ##__VA_ARGS__ )
#else
#define LOGINFO(mesg,...)
#endif

// LOG: Debug.
#if LOG_DEBUG_ENABLED == 1
#define LOGDEBUG(mesg,...) \
    LOG ( LOG_DEBUG , (mesg) , ##__VA_ARGS__ )
#else
#define LOGDEBUG(mesg,...)
#endif

// LOG: Trace.
#if LOG_TRACE_ENABLED == 1
#define LOGTRACE(mesg,...) \
    LOG ( LOG_TRACE , (mesg) , ##__VA_ARGS__ )
#else
#define LOGTRACE(mesg,...)
#endif

// LOG: Silent.
#if LOG_SILENT_ENABLED == 1
#define LOGSILENT(mesg,...) \
    LOG ( LOG_SILENT , (mesg) , ##__VA_ARGS__ )
#else
#define LOGSILENT(mesg,...)
#endif

/**
 * @brief Writes a formatted message to file.
 * 
 * Use PRINT to print to stdout, use PRINTERROR to print to stderr.
 * 
 * @param mesg Formatted message to print to file.
 * @param arg_count Number of elements in the variadic argument list.
 * @param args Variadic argument list.
 */
void
print
(   file_handle_t*  file
,   const char*     mesg
,   u64             arg_count
,   u64*            args
);

/** @brief Alias for calling print with __VA_ARGS__. */
#define PRINT(mesg,...)                                  \
    do                                                   \
    {                                                    \
        file_handle_t file;                              \
        file_stdout ( &file );                           \
        DISABLE_WARNING ( -Wint-conversion )             \
        print ( &file , (mesg) , ARGS ( __VA_ARGS__ ) ); \
        REENABLE_WARNING ()                              \
    }                                                    \
    while ( 0 )

/** @brief Alias for calling print with __VA_ARGS__. */
#define PRINTERROR(mesg,...)                             \
    do                                                   \
    {                                                    \
        file_handle_t file;                              \
        file_stderr ( &file );                           \
        DISABLE_WARNING ( -Wint-conversion )             \
        print ( &file , (mesg) , ARGS ( __VA_ARGS__ ) ); \
        REENABLE_WARNING ()                              \
    }                                                    \
    while ( 0 )

#endif  // LOGGER_H