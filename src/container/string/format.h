/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/string/format.h
 * @brief Provides a custom string formatting interface.
 */
#ifndef STRING_FORMAT_H
#define STRING_FORMAT_H

#include "common.h"

/** @brief Type and instance definitions for format specifier tags. */
typedef enum
{
    STRING_FORMAT_SPECIFIER_IGNORE
,   STRING_FORMAT_SPECIFIER_RAW
,   STRING_FORMAT_SPECIFIER_INTEGER
,   STRING_FORMAT_SPECIFIER_FLOATING_POINT
,   STRING_FORMAT_SPECIFIER_FLOATING_POINT_ABBREVIATED
,   STRING_FORMAT_SPECIFIER_FLOATING_POINT_MANTISSA_ONLY
,   STRING_FORMAT_SPECIFIER_ADDRESS
,   STRING_FORMAT_SPECIFIER_CHARACTER
,   STRING_FORMAT_SPECIFIER_STRING
,   STRING_FORMAT_SPECIFIER_RESIZABLE_STRING

,   STRING_FORMAT_SPECIFIER_COUNT
}
STRING_FORMAT_SPECIFIER;

/** @brief Type and instance definitions for format modifier tags. */
typedef enum
{
    STRING_FORMAT_MODIFIER_PAD_LEFT
,   STRING_FORMAT_MODIFIER_PAD_RIGHT
,   STRING_FORMAT_MODIFIER_SHOW_SIGN
,   STRING_FORMAT_MODIFIER_HIDE_SIGN
,   STRING_FORMAT_MODIFIER_FIX_PRECISION

,   STRING_FORMAT_MODIFIER_COUNT
}
STRING_FORMAT_MODIFIER;

#define STRING_FORMAT_SPECIFIER_INVALID STRING_FORMAT_SPECIFIER_COUNT /** @brief An alias for detecting an invalid format specifier tag. */
#define STRING_FORMAT_MODIFIER_INVALID  STRING_FORMAT_MODIFIER_COUNT  /** @brief An alias for detecting an invalid format modifier tag. */

#define STRING_FORMAT_SPECIFIER_TOKEN_ID     '%'                              /** @brief The format specifier token. */
#define STRING_FORMAT_SPECIFIER_TOKEN_IGNORE STRING_FORMAT_SPECIFIER_TOKEN_ID /** @brief Format specifier: ignore. */

#define STRING_FORMAT_SPECIFIER_TOKEN_RAW                          'u' /** @brief Format specifier: raw. */
#define STRING_FORMAT_SPECIFIER_TOKEN_INTEGER                      'i' /** @brief Format specifier: integer. */
#define STRING_FORMAT_SPECIFIER_TOKEN_FLOATING_POINT               'f' /** @brief Format specifier: floating point. */
#define STRING_FORMAT_SPECIFIER_TOKEN_FLOATING_POINT_ABBREVIATED   'e' /** @brief Format specifier: floating point (abbreviated notation). */
#define STRING_FORMAT_SPECIFIER_TOKEN_FLOATING_POINT_MANTISSA_ONLY 'd' /** @brief Format specifier: floating point (abbreviated notation). */
#define STRING_FORMAT_SPECIFIER_TOKEN_ADDRESS                      '@' /** @brief Format specifier: address. */
#define STRING_FORMAT_SPECIFIER_TOKEN_CHARACTER                    'c' /** @brief Format specifier: character. */
#define STRING_FORMAT_SPECIFIER_TOKEN_STRING                       's' /** @brief Format specifier: string. */
#define STRING_FORMAT_SPECIFIER_TOKEN_RESIZABLE_STRING             'S' /** @brief Format specifier: resizable string. */

#define STRING_FORMAT_MODIFIER_TOKEN_PAD                           'p' /** @brief Format modifier: pad. */
#define STRING_FORMAT_MODIFIER_TOKEN_PAD_MINIMUM                   'P' /** @brief Format modifier: pad (minimum width). */
#define STRING_FORMAT_MODIFIER_TOKEN_LEFT                          'l' /** @brief Format modifier: left. */
#define STRING_FORMAT_MODIFIER_TOKEN_RIGHT                         'r' /** @brief Format modifier: right. */
#define STRING_FORMAT_MODIFIER_TOKEN_SHOW_SIGN                     '+' /** @brief Format modifier: show sign. */
#define STRING_FORMAT_MODIFIER_TOKEN_HIDE_SIGN                     '-' /** @brief Format modifier: hide sign. */
#define STRING_FORMAT_MODIFIER_TOKEN_FIX_PRECISION                 '.' /** @brief Format modifier: fix precision. */

/**
 * @brief String format function.
 * 
 * Accepts a variadic argument list (see common/args.h).
 *
 * Uses dynamic memory allocation. Call string_destroy to free.
 * 
 * FORMAT SPECIFIERS :
 * 
 * %% : Ignore (i.e. '%' character).
 * %u : Unsigned.
 * %i : Signed integer.
 * %f : Floating point number. The corresponding argument must be the address
 *      of an f64. (For additional information about this limitation, see
 *      common/args.h).
 * %e : Floating point number (short-notation). The corresponding argument
 *      must be the address of an f64. (For additional information about this
 *      limitation, see common/args.h).
 * %d : Floating point number (mantissa-only). The corresponding argument
 *      must be the address of an f64. (For additional information about this
 *      limitation, see common/args.h).
 * %@ : Address.
 * %c : Single character.
 * %s : Null-terminated string of characters.
 *      Length is computed at runtime via O(n) _string_length.
 * %S : Resizable string of characters.
 *      This includes any string created with the __string_create class of
 *      functions; their length is fetched at runtime via O(1) string_length.
 *      
 * FORMAT MODIFIERS :
 * 
 * These may each be used once preceding a format specifier. They only apply
 * to arguments of a sensible type for their purpose.
 * 
 * - pl<character><number> : Fix column width to <number>. If needed, pad with
 *                           <character> to the left.
 *                           Works with any format specifier.
 * - pr<character><number> : Fix column width to <number>. If needed, pad with
 *                           <character> to the right.
 *                           Works with any format specifier.
 * - Pl<character><number> : Set minimum column width to <number>. If needed,
 *                           pad with <character> to the left.
 *                           Works with any format specifier.
 * - Pr<character><number> : Set minimum column width to <number>. If needed,
 *                           pad with <character> to the right.
 * - + : Always include sign. Default behavior is to include the sign only for
 *       a negative number.
 *       Works only with signed numeric format specifiers: %f, %e, %i.
 * - - : Never include sign. Default behavior is to include the sign only for
 *       a negative number.
 *       Works only with signed numeric format specifiers: %f, %e, %i.
 * - .<number> : Fix mantissa precision to <number> * 10.
 *               Works only with floating point format specifiers: %f, %e, %d.
 *
 * @param format Formatting string.
 * @param arg_count Number of elements in the variadic argument list.
 * @param args Variadic argument list.
 * @return The formatted string.
 */
char*
_string_format
(   const char* format
,   u64         arg_count
,   u64*        args
);

/** @brief Alias for calling _string_format with __VA_ARGS__. */
#define string_format(format,...)                           \
    ({                                                      \
        DISABLE_WARNING ( -Wint-conversion )                \
        _string_format ( (format) , ARGS ( __VA_ARGS__ ) ); \
        REENABLE_WARNING ()                                 \
    })

#endif // STRING_FORMAT_H