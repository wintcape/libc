/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file container/string/format.c
 * @brief Implementation of the container/string/format header.
 * (see container/string/format.h for additional details)
 */
#include "container/string/format.h"
#include "container/string.h"

#include "core/assert.h"
#include "core/logger.h"
#include "core/memory.h"

#include "math/math.h"

// Global constants.
#define STRING_FORMAT_MAX_FLOATING_POINT_STRING_LENGTH             2048 /** @brief Maximum floating point string buffer length. */
#define STRING_FORMAT_MAX_FLOATING_POINT_ABBREVIATED_STRING_LENGTH 64   /** @brief Maximum abbreviated floating point string buffer length. */

/** @brief Type and instance definitions for string padding tag. */
typedef enum
{
    STRING_FORMAT_PADDING_NONE
,   STRING_FORMAT_PADDING_LEFT
,   STRING_FORMAT_PADDING_RIGHT
}
STRING_FORMAT_PADDING;

/** @brief Type definition for a container to hold string padding configuration info. */
typedef struct
{
    STRING_FORMAT_PADDING   tag;
    bool                    fixed;
    char                    value;
    u64                     length;
}
string_format_padding_t;

/** @brief Type and instance definitions for string sign tag. */
typedef enum
{
    STRING_FORMAT_SIGN_NONE
,   STRING_FORMAT_SIGN_SHOW
,   STRING_FORMAT_SIGN_HIDE
}
STRING_FORMAT_SIGN;

/** @brief Type definition for a container to hold string sign configuration info. */
typedef struct
{
    STRING_FORMAT_SIGN tag;
}
string_format_sign_t;

/** @brief Type definition for a container to hold string fix-precision configuration info. */
typedef struct
{
    bool    tag;
    u8      precision;
}
string_format_fix_precision_t;

/** @brief Type definition for a container to hold format specifier info. */
typedef struct
{
    STRING_FORMAT_SPECIFIER         tag;
    u64                             length;

    bool                            modifiers[ STRING_FORMAT_MODIFIER_COUNT ];
    string_format_padding_t         padding;
    string_format_sign_t            sign;
    string_format_fix_precision_t   fix_precision;
}
string_format_specifier_t;

/** @brief Type definition for an internal state. */
typedef struct
{
    const char* format;
    u64         format_length;

    args_t      args;
    arg_t*      next_arg;
    u64         args_remaining;

    const char* copy_start;
    const char* copy_end;

    char*       string;
}
state_t;

/** @brief Defines next copy size. */
#define STRING_FORMAT_COPY_SIZE(state) \
    MAX ( 0 , ( *(state) ).copy_end - ( *(state) ).copy_start )

/** @brief Defines the read limit for the format string. */
#define STRING_FORMAT_READ_LIMIT(state) \
    ( ( *(state) ).format + ( *(state) ).format_length )

/**
 * @brief Advances to the next argument in the variadic argument list.
 * 
 * @param state Internal state arguments.
 */
void
_string_format_consume_next_argument
(   state_t* state
);

/**
 * @brief Validates a format specifier.
 * 
 * @param state Internal state arguments.
 * @param read Current read head.
 * @param format_specifier Output buffer for format specifier
 * (possibly invalidated).
 * @return Format specifier string length.
 */
void
_string_format_validate_format_specifier
(   state_t*                    state
,   const char*                 read
,   string_format_specifier_t*  format_specifier
);

// Implementation of _string_format_validate_format_specifier
// ( see _string_format_validate_format_specifier ).
void _string_format_validate_format_specifier_ignore ( state_t* state , const char** read , string_format_specifier_t* format_specifier );
void _string_format_validate_format_specifier_raw ( state_t* state , const char** read , string_format_specifier_t* format_specifier );
void _string_format_validate_format_specifier_integer ( state_t* state , const char** read , string_format_specifier_t* format_specifier );
void _string_format_validate_format_specifier_floating_point ( state_t* state , const char** read , string_format_specifier_t* format_specifier );
void _string_format_validate_format_specifier_floating_point_show_fractional ( state_t* state , const char** read , string_format_specifier_t* format_specifier );
void _string_format_validate_format_specifier_floating_point_abbreviated ( state_t* state , const char** read , string_format_specifier_t* format_specifier );
void _string_format_validate_format_specifier_floating_point_fractional_only ( state_t* state , const char** read , string_format_specifier_t* format_specifier );
void _string_format_validate_format_specifier_address ( state_t* state , const char** read , string_format_specifier_t* format_specifier );
void _string_format_validate_format_specifier_character ( state_t* state , const char** read , string_format_specifier_t* format_specifier );
void _string_format_validate_format_specifier_string ( state_t* state , const char** read , string_format_specifier_t* format_specifier );
void _string_format_validate_format_specifier_resizable_string ( state_t* state , const char** read , string_format_specifier_t* format_specifier );
void _string_format_validate_format_modifier_pad ( state_t* state , const char** read , const bool fixed , string_format_specifier_t* format_specifier );
void _string_format_validate_format_modifier_sign ( state_t* state , const char** read , STRING_FORMAT_SIGN sign, string_format_specifier_t* format_specifier );
void _string_format_validate_format_modifier_fix_precision ( state_t* state , const char** read , string_format_specifier_t* format_specifier );

/**
 * @brief Parses the next argument according to the current format specifier and
 * modifier(s).
 * 
 * @param state Internal state arguments.
 * @param read Current read head.
 * @param format_specifier A format specifier.
 */
void
_string_format_parse_next_argument
(   state_t*                            state
,   const string_format_specifier_t*    format_specifier
);

// Implementation of _string_format_parse_next_argument
// ( see _string_format_parse_next_argument ).
u64 _string_format_parse_next_argument_ignore ( state_t* state, const string_format_specifier_t* format_specifier );
u64 _string_format_parse_next_argument_raw ( state_t* state , const string_format_specifier_t* format_specifier );
u64 _string_format_parse_next_argument_integer ( state_t* state , const string_format_specifier_t* format_specifier );
u64 _string_format_parse_next_argument_floating_point ( state_t* state , const string_format_specifier_t* format_specifier );
u64 _string_format_parse_next_argument_floating_point_show_fractional ( state_t* state , const string_format_specifier_t* format_specifier );
u64 _string_format_parse_next_argument_floating_point_abbreviated ( state_t* state , const string_format_specifier_t* format_specifier );
u64 _string_format_parse_next_argument_floating_point_fractional_only ( state_t* state , const string_format_specifier_t* format_specifier );
u64 _string_format_parse_next_argument_address ( state_t* state , const string_format_specifier_t* format_specifier );
u64 _string_format_parse_next_argument_character ( state_t* state , const string_format_specifier_t* format_specifier );
u64 _string_format_parse_next_argument_string ( state_t* state , const string_format_specifier_t* format_specifier );
u64 _string_format_parse_next_argument_resizable_string ( state_t* state , const string_format_specifier_t* format_specifier );

/**
 * @brief Wrapper for string_push that respects the left- and right- 'padding'
 * format modifiers, if they are set.
 * 
 * @param string The handle to a resizable string to append to.
 * @param src The string to append.
 * @param src_length The number of characters contained by src.
 * @param format_specifier A format specifier.
 * @return The number of characters written to the string being constructed.
 */
u64
_string_format_push
(   char**                              string
,   const char*                         src
,   const u64                           src_length
,   const string_format_specifier_t*    format_specifier
);

char*
_string_format
(   const char* format
,   args_t      args
)
{
    if ( !format || ( args.arg_count && !args.args ) )
    {
        if ( !format )
        {
            LOGERROR ( "_string_format: Missing argument: format." );
        }
        if ( args.arg_count && !args.args )
        {
            LOGERROR ( "_string_format: Invalid argument: args. List is null, but count indicates it should contain %u element%s."
                     , args.arg_count
                     , ( args.arg_count > 1 ) ? "s" : ""
                     );
        }
        return string_create_from ( "" );
    }

    state_t state;
    state.format = format;
    state.format_length = _string_length ( format );
    state.args = args;
    state.next_arg = state.args.args;
    state.args_remaining = state.args.arg_count;
    state.string = _string_create ( state.format_length + 1 );

    const char* read = state.format;
    state.copy_start = read;
    for (;;)
    {
        if ( !state.args_remaining || read >= STRING_FORMAT_READ_LIMIT ( &state ) )
        {
            break;
        }

        if ( *read != STRING_FORMAT_SPECIFIER_TOKEN_ID )
        {
            read += 1;
            continue;
        }
        
        string_format_specifier_t format_specifier;
        _string_format_validate_format_specifier ( &state
                                                 , read + 1
                                                 , &format_specifier
                                                 );
                                                 
        if ( format_specifier.tag == STRING_FORMAT_SPECIFIER_INVALID )
        {
            if ( state.args_remaining )
            {
                LOGWARN ( "_string_format: Illegal format specifier encountered on index %i of the formatting string. Skipping argument %i.\n\t                `%s`"
                        , read - state.format
                        , state.args.arg_count - state.args_remaining + 1
                        , state.format
                        );
                _string_format_consume_next_argument ( &state );
            }
            read += 1;
            continue;
        }

        if ( format_specifier.tag == STRING_FORMAT_SPECIFIER_IGNORE )
        {
            string_push ( state.string
                        , string_char ( STRING_FORMAT_SPECIFIER_TOKEN_IGNORE )
                        , 1
                        );
            read += 2;
            continue;
        }
        
        state.copy_end = read;
        string_push ( state.string
                    , state.copy_start
                    , STRING_FORMAT_COPY_SIZE ( &state )
                    );
        state.copy_start = state.copy_end + format_specifier.length;

        _string_format_parse_next_argument ( &state , &format_specifier );
        read += 1;
    }

    state.copy_end = STRING_FORMAT_READ_LIMIT ( &state );
    string_push ( state.string
                , state.copy_start
                , STRING_FORMAT_COPY_SIZE ( &state )
                );
                
    return state.string;
}

void
_string_format_consume_next_argument
(   state_t* state
)
{
    ( *state ).next_arg += 1;
    ( *state ).args_remaining -= 1;
}

void
_string_format_validate_format_specifier
(   state_t*                    state
,   const char*                 read
,   string_format_specifier_t*  format_specifier
)
{
    if ( read >= STRING_FORMAT_READ_LIMIT ( state ) )
    {
        ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_INVALID;
        return;
    }

    const char* const read_ = read;

    memory_clear ( ( *format_specifier ).modifiers
                 , sizeof ( bool ) * STRING_FORMAT_MODIFIER_COUNT
                 );
    ( *format_specifier ).padding.tag = STRING_FORMAT_PADDING_NONE;
    ( *format_specifier ).padding.value = 0;
    ( *format_specifier ).padding.length = 0;
    ( *format_specifier ).sign.tag = STRING_FORMAT_SIGN_NONE;
    ( *format_specifier ).fix_precision.tag = false;
    ( *format_specifier ).fix_precision.precision = 6; // Default.

    switch ( *read )
    {
        case STRING_FORMAT_SPECIFIER_TOKEN_IGNORE:
        {
            _string_format_validate_format_specifier_ignore ( state , &read , format_specifier );
            ( *format_specifier ).length = read - read_ + 1;
            return;
        }
        case STRING_FORMAT_SPECIFIER_TOKEN_RAW:
        {
            _string_format_validate_format_specifier_raw ( state , &read , format_specifier );
            ( *format_specifier ).length = read - read_ + 1;
            return;
        }
        case STRING_FORMAT_SPECIFIER_TOKEN_INTEGER:
        {
            _string_format_validate_format_specifier_integer ( state , &read , format_specifier );
            ( *format_specifier ).length = read - read_ + 1;
            return;
        }
        case STRING_FORMAT_SPECIFIER_TOKEN_FLOATING_POINT:
        {
            _string_format_validate_format_specifier_floating_point ( state , &read , format_specifier );
            ( *format_specifier ).length = read - read_ + 1;
            return;
        }
        case STRING_FORMAT_SPECIFIER_TOKEN_FLOATING_POINT_SHOW_FRACTIONAL:
        {
            _string_format_validate_format_specifier_floating_point_show_fractional ( state , &read , format_specifier );
            ( *format_specifier ).length = read - read_ + 1;
            return;
        }
        case STRING_FORMAT_SPECIFIER_TOKEN_FLOATING_POINT_ABBREVIATED:
        {
            _string_format_validate_format_specifier_floating_point_abbreviated ( state , &read , format_specifier );
            ( *format_specifier ).length = read - read_ + 1;
            return;
        }
        case STRING_FORMAT_SPECIFIER_TOKEN_FLOATING_POINT_FRACTIONAL_ONLY:
        {
            _string_format_validate_format_specifier_floating_point_fractional_only ( state , &read , format_specifier );
            ( *format_specifier ).length = read - read_ + 1;
            return;
        }
        case STRING_FORMAT_SPECIFIER_TOKEN_ADDRESS:
        {
            _string_format_validate_format_specifier_address ( state , &read , format_specifier );
            ( *format_specifier ).length = read - read_ + 1;
            return;
        }
        case STRING_FORMAT_SPECIFIER_TOKEN_CHARACTER:
        {
            _string_format_validate_format_specifier_character ( state , &read , format_specifier );
            ( *format_specifier ).length = read - read_ + 1;
            return;
        }
        case STRING_FORMAT_SPECIFIER_TOKEN_STRING:
        {
            _string_format_validate_format_specifier_string ( state , &read , format_specifier );
            ( *format_specifier ).length = read - read_ + 1;
            return;
        }
        case STRING_FORMAT_SPECIFIER_TOKEN_RESIZABLE_STRING:
        {
            _string_format_validate_format_specifier_resizable_string ( state , &read , format_specifier );
            ( *format_specifier ).length = read - read_ + 1;
            return;
        }
    }

    for ( STRING_FORMAT_MODIFIER i = 0; i < STRING_FORMAT_MODIFIER_COUNT; ++i )
    {
        switch ( *read )
        {
            case STRING_FORMAT_MODIFIER_TOKEN_PAD:
            {
                _string_format_validate_format_modifier_pad ( state , &read , true , format_specifier );
                break;
            }
            case STRING_FORMAT_MODIFIER_TOKEN_PAD_MINIMUM:
            {
                _string_format_validate_format_modifier_pad ( state , &read , false , format_specifier );
                break;
            }
            case STRING_FORMAT_MODIFIER_TOKEN_SHOW_SIGN:
            {
                _string_format_validate_format_modifier_sign ( state , &read , STRING_FORMAT_SIGN_SHOW , format_specifier );
                break;
            }
            case STRING_FORMAT_MODIFIER_TOKEN_HIDE_SIGN:
            {
                _string_format_validate_format_modifier_sign ( state , &read , STRING_FORMAT_SIGN_HIDE , format_specifier );
                break;
            }
            case STRING_FORMAT_MODIFIER_TOKEN_FIX_PRECISION:
            {
                _string_format_validate_format_modifier_fix_precision ( state , &read , format_specifier );
                break;
            }

            case STRING_FORMAT_SPECIFIER_TOKEN_IGNORE:
            {
                _string_format_validate_format_specifier_ignore ( state , &read , format_specifier );
                ( *format_specifier ).length = read - read_ + 1;
                return;
            }
            case STRING_FORMAT_SPECIFIER_TOKEN_RAW:
            {
                _string_format_validate_format_specifier_raw ( state , &read , format_specifier );
                ( *format_specifier ).length = read - read_ + 1;
                return;
            }
            case STRING_FORMAT_SPECIFIER_TOKEN_INTEGER:
            {
                _string_format_validate_format_specifier_integer ( state , &read , format_specifier );
                ( *format_specifier ).length = read - read_ + 1;
                return;
            }
            case STRING_FORMAT_SPECIFIER_TOKEN_FLOATING_POINT:
            {
                _string_format_validate_format_specifier_floating_point ( state , &read , format_specifier );
                ( *format_specifier ).length = read - read_ + 1;
                return;
            }
            case STRING_FORMAT_SPECIFIER_TOKEN_FLOATING_POINT_SHOW_FRACTIONAL:
        {
            _string_format_validate_format_specifier_floating_point_show_fractional ( state , &read , format_specifier );
            ( *format_specifier ).length = read - read_ + 1;
            return;
        }
            case STRING_FORMAT_SPECIFIER_TOKEN_FLOATING_POINT_ABBREVIATED:
            {
                _string_format_validate_format_specifier_floating_point_abbreviated ( state , &read , format_specifier );
                ( *format_specifier ).length = read - read_ + 1;
                return;
            }
            case STRING_FORMAT_SPECIFIER_TOKEN_FLOATING_POINT_FRACTIONAL_ONLY:
            {
                _string_format_validate_format_specifier_floating_point_fractional_only ( state , &read , format_specifier );
                ( *format_specifier ).length = read - read_ + 1;
                return;
            }
            case STRING_FORMAT_SPECIFIER_TOKEN_ADDRESS:
            {
                _string_format_validate_format_specifier_address ( state , &read , format_specifier );
                ( *format_specifier ).length = read - read_ + 1;
                return;
            }
            case STRING_FORMAT_SPECIFIER_TOKEN_CHARACTER:
            {
                _string_format_validate_format_specifier_character ( state , &read , format_specifier );
                ( *format_specifier ).length = read - read_ + 1;
                return;
            }
            case STRING_FORMAT_SPECIFIER_TOKEN_STRING:
            {
                _string_format_validate_format_specifier_string ( state , &read , format_specifier );
                ( *format_specifier ).length = read - read_ + 1;
                return;
            }
            case STRING_FORMAT_SPECIFIER_TOKEN_RESIZABLE_STRING:
            {
                _string_format_validate_format_specifier_resizable_string ( state , &read , format_specifier );
                ( *format_specifier ).length = read - read_ + 1;
                return;
            }
        }
    }
    ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_INVALID;
}

void
_string_format_validate_format_specifier_ignore
(   state_t*                    state
,   const char**                read
,   string_format_specifier_t*  format_specifier
)
{
    ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_IGNORE;
    *read += 1;
}

void
_string_format_validate_format_specifier_raw
(   state_t*                    state
,   const char**                read
,   string_format_specifier_t*  format_specifier
)
{
    ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_RAW;
    *read += 1;
}

void
_string_format_validate_format_specifier_integer
(   state_t*                    state
,   const char**                read
,   string_format_specifier_t*  format_specifier
)
{
    ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_INTEGER;
    *read += 1;
}

void
_string_format_validate_format_specifier_floating_point
(   state_t*                    state
,   const char**                read
,   string_format_specifier_t*  format_specifier
)
{
    ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_FLOATING_POINT;
    *read += 1;
}

void
_string_format_validate_format_specifier_floating_point_show_fractional
(   state_t*                    state
,   const char**                read
,   string_format_specifier_t*  format_specifier
)
{
    ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_FLOATING_POINT_SHOW_FRACTIONAL;
    *read += 1;
}

void
_string_format_validate_format_specifier_floating_point_abbreviated
(   state_t*                    state
,   const char**                read
,   string_format_specifier_t*  format_specifier
)
{
    ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_FLOATING_POINT_ABBREVIATED;
    *read += 1;
}

void
_string_format_validate_format_specifier_floating_point_fractional_only
(   state_t*                    state
,   const char**                read
,   string_format_specifier_t*  format_specifier
)
{
    ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_FLOATING_POINT_FRACTIONAL_ONLY;
    *read += 1;
}

void
_string_format_validate_format_specifier_address
(   state_t*                    state
,   const char**                read
,   string_format_specifier_t*  format_specifier
)
{
    ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_ADDRESS;
    *read += 1;
}

void
_string_format_validate_format_specifier_character
(   state_t*                    state
,   const char**                read
,   string_format_specifier_t*  format_specifier
)
{
    ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_CHARACTER;
    *read += 1;
}

void
_string_format_validate_format_specifier_string
(   state_t*                    state
,   const char**                read
,   string_format_specifier_t*  format_specifier
)
{
    ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_STRING;
    *read += 1;
}

void
_string_format_validate_format_specifier_resizable_string
(   state_t*                    state
,   const char**                read
,   string_format_specifier_t*  format_specifier
)
{
    ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_RESIZABLE_STRING;
    *read += 1;
}

void
_string_format_validate_format_modifier_pad
(   state_t*                    state
,   const char**                read
,   const bool                  fixed
,   string_format_specifier_t*  format_specifier
)
{
    if (   ( *format_specifier ).modifiers[ STRING_FORMAT_MODIFIER_PAD_LEFT ]
        || ( *format_specifier ).modifiers[ STRING_FORMAT_MODIFIER_PAD_RIGHT ]
       )
    {
        ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_INVALID;
        return;
    }
    *read += 1;
    if ( *read + 3 >= STRING_FORMAT_READ_LIMIT ( state ) )
    {
        ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_INVALID;
        return;
    }
    STRING_FORMAT_MODIFIER modifier;
    if ( **read == STRING_FORMAT_MODIFIER_TOKEN_LEFT )
    {
        ( *format_specifier ).padding.tag = STRING_FORMAT_PADDING_LEFT;
        modifier = STRING_FORMAT_MODIFIER_PAD_LEFT;
    }
    else if ( **read == STRING_FORMAT_MODIFIER_TOKEN_RIGHT )
    {
        ( *format_specifier ).padding.tag = STRING_FORMAT_PADDING_RIGHT;
        modifier = STRING_FORMAT_MODIFIER_PAD_RIGHT;
    }
    else
    {
        ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_INVALID;
        return;
    }
    *read += 1;
    if ( !whitespace ( **read ) && !( **read >= 32 && **read <= 126 ) )
    {
        ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_INVALID;
        return;
    }
    ( *format_specifier ).padding.value = **read;
    *read += 1;
    if ( !digit ( **read ) )
    {
        ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_INVALID;
        return;
    }
    u64 length = to_digit ( **read );
    if ( !length )
    {
        ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_INVALID;
        return;
    }
    *read += 1;
    while ( *read < STRING_FORMAT_READ_LIMIT ( state ) )
    {
        if ( !digit ( **read ) )
        {
            break;
        }
        length = length * 10 + to_digit ( **read );
        *read += 1;
    }
    ( *format_specifier ).padding.length = length;
    ( *format_specifier ).padding.fixed = fixed;
    ( *format_specifier ).modifiers[ modifier ] = true;
}

void
_string_format_validate_format_modifier_sign
(   state_t*                    state
,   const char**                read
,   STRING_FORMAT_SIGN          sign
,   string_format_specifier_t*  format_specifier
)
{
    if ( sign == STRING_FORMAT_SIGN_NONE )
    {
        ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_INVALID;
        return;
    }
    if (   ( *format_specifier ).modifiers[ STRING_FORMAT_MODIFIER_HIDE_SIGN ]
        || ( *format_specifier ).modifiers[ STRING_FORMAT_MODIFIER_SHOW_SIGN ]
       )
    {
        ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_INVALID;
        return;
    }
    if ( sign == STRING_FORMAT_SIGN_HIDE )
    {
        ( *format_specifier ).modifiers[ STRING_FORMAT_MODIFIER_HIDE_SIGN ] = true;
    }
    else
    {
        ( *format_specifier ).modifiers[ STRING_FORMAT_MODIFIER_SHOW_SIGN ] = true;
    }
    
    ( *format_specifier ).sign.tag = sign;
    *read += 1;
}

void
_string_format_validate_format_modifier_fix_precision
(   state_t*                    state
,   const char**                read
,   string_format_specifier_t*  format_specifier
)
{
    if ( ( *format_specifier ).modifiers[ STRING_FORMAT_MODIFIER_FIX_PRECISION ] )
    {
        ( *format_specifier ).tag = STRING_FORMAT_SPECIFIER_INVALID;
        return;
    }
    *read += 1;
    if ( *read >= STRING_FORMAT_READ_LIMIT ( state ) )
    {
        return;
    }
    if ( digit ( **read ) )
    {
        ( *format_specifier ).fix_precision.tag = true;
        ( *format_specifier ).fix_precision.precision = to_digit ( **read );
    }
    else
    {
        ( *format_specifier ).fix_precision.tag = false;
    }
    ( *format_specifier ).modifiers[ STRING_FORMAT_MODIFIER_FIX_PRECISION ] = true;
    *read += 1;
}

void
_string_format_parse_next_argument
(   state_t*                            state
,   const string_format_specifier_t*    format_specifier
)
{
    switch ( ( *format_specifier ).tag )
    {
        case STRING_FORMAT_SPECIFIER_RAW:                            _string_format_parse_next_argument_raw ( state , format_specifier )                            ;break;
        case STRING_FORMAT_SPECIFIER_CHARACTER:                      _string_format_parse_next_argument_character ( state , format_specifier )                      ;break;
        case STRING_FORMAT_SPECIFIER_INTEGER:                        _string_format_parse_next_argument_integer ( state , format_specifier )                        ;break;
        case STRING_FORMAT_SPECIFIER_FLOATING_POINT:                 _string_format_parse_next_argument_floating_point ( state , format_specifier )                 ;break;
        case STRING_FORMAT_SPECIFIER_FLOATING_POINT_SHOW_FRACTIONAL: _string_format_parse_next_argument_floating_point_show_fractional ( state , format_specifier ) ;break;
        case STRING_FORMAT_SPECIFIER_FLOATING_POINT_ABBREVIATED:     _string_format_parse_next_argument_floating_point_abbreviated ( state , format_specifier )     ;break;
        case STRING_FORMAT_SPECIFIER_FLOATING_POINT_FRACTIONAL_ONLY: _string_format_parse_next_argument_floating_point_fractional_only ( state , format_specifier ) ;break;
        case STRING_FORMAT_SPECIFIER_ADDRESS:                        _string_format_parse_next_argument_address ( state , format_specifier )                        ;break;
        case STRING_FORMAT_SPECIFIER_STRING:                         _string_format_parse_next_argument_string ( state , format_specifier )                         ;break;
        case STRING_FORMAT_SPECIFIER_RESIZABLE_STRING:               _string_format_parse_next_argument_resizable_string ( state , format_specifier )               ;break;
        default:                                                                                                                                                    ;break;
    }
    _string_format_consume_next_argument ( state );
}

u64
_string_format_parse_next_argument_raw
(   state_t*                            state
,   const string_format_specifier_t*    format_specifier
)
{
    char string[ 65 ];
    const u64 arg = *( ( *state ).next_arg );
    const u8 radix = 10;
    const u64 string_length = string_u64 ( arg , radix , string );
    return _string_format_push ( &( *state ).string
                               , string
                               , string_length
                               , format_specifier
                               );
}

u64
_string_format_parse_next_argument_integer
(   state_t*                            state
,   const string_format_specifier_t*    format_specifier
)
{
    char string[ 65 ];
    const u64 arg = *( ( *state ).next_arg );
    const u8 radix = 10;
    const bool hide_sign = ( *format_specifier ).sign.tag == STRING_FORMAT_SIGN_HIDE
                        && radix == 10
                        && ( i64 ) arg < 0
                        ;
    const bool show_sign = ( *format_specifier ).sign.tag == STRING_FORMAT_SIGN_SHOW
                        && radix == 10
                        && ( i64 ) arg >= 0
                        ;
    u64 string_length;
    if ( hide_sign )
    {
        string_length = string_u64 ( -( ( i64 ) arg ) , radix , string );
    }
    else if ( show_sign )
    {
        string[ 0 ] = '+';
        string_length = string_i64 ( arg , radix , string + 1 ) + 1;
    }
    else
    {
        string_length = string_i64 ( arg , radix , string );
    }
    return _string_format_push ( &( *state ).string
                               , string
                               , string_length
                               , format_specifier
                               );
}

// TODO: Implement everything beneath this dependency to remove it.
#include <stdio.h>

u64
_string_format_parse_next_argument_floating_point
(   state_t*                            state
,   const string_format_specifier_t*    format_specifier
)
{
    const u64 arg = *( ( *state ).next_arg );
    if ( !arg )
    {
        return _string_format_parse_next_argument_string ( state
                                                         , format_specifier
                                                         );
    }

    char string[ STRING_FORMAT_MAX_FLOATING_POINT_STRING_LENGTH ];
    const char* format;
    if ( ( *format_specifier ).sign.tag == STRING_FORMAT_SIGN_NONE )
    {
        if ( ( *format_specifier ).fix_precision.tag )
        {
            switch ( ( *format_specifier ).fix_precision.precision )
            {
                case 0:  format = "%.0lf" ;break;
                case 1:  format = "%.1lf" ;break;
                case 2:  format = "%.2lf" ;break;
                case 3:  format = "%.3lf" ;break;
                case 4:  format = "%.4lf" ;break;
                case 5:  format = "%.5lf" ;break;
                case 6:  format = "%.6lf" ;break;
                case 7:  format = "%.7lf" ;break;
                case 8:  format = "%.8lf" ;break;
                case 9:  format = "%.9lf" ;break;
                default: format = "%.lf"  ;break;
            }
        }
        else
        {
            format = "%lf";
        }
    }
    else
    {
        if ( ( *format_specifier ).fix_precision.tag )
        {
            switch ( ( *format_specifier ).fix_precision.precision )
            {
                case 0:  format = "%+.0lf" ;break;
                case 1:  format = "%+.1lf" ;break;
                case 2:  format = "%+.2lf" ;break;
                case 3:  format = "%+.3lf" ;break;
                case 4:  format = "%+.4lf" ;break;
                case 5:  format = "%+.5lf" ;break;
                case 6:  format = "%+.6lf" ;break;
                case 7:  format = "%+.7lf" ;break;
                case 8:  format = "%+.8lf" ;break;
                case 9:  format = "%+.9lf" ;break;
                default: format = "%+.lf"  ;break;
            }
        }
        else
        {
            format = "%+lf";
        }
    }

    f64 value = *( ( f64* ) arg );
    u64 integer = ( u64 ) value;
    f64 fractional = value - integer;
    i32 snprintf_result = snprintf ( string
                                   , STRING_FORMAT_MAX_FLOATING_POINT_STRING_LENGTH
                                   , format
                                   , value
                                   );
    if ( ( *format_specifier ).sign.tag == STRING_FORMAT_SIGN_HIDE )
    {
        snprintf_result -= 1;
        memory_move ( string , string + 1 , snprintf_result );
        string[ snprintf_result ] = 0; // Append terminator.
    }
    if ( !fractional )
    {
        u64 index;
        if ( string_contains ( string , snprintf_result
                            , string_char ( '.' ) , 1
                            , false
                            , &index
                            ))
        {
            snprintf_result = index;
            string[ snprintf_result ] = 0;
        }
    }
    return _string_format_push ( &( *state ).string
                               , string
                               , snprintf_result
                               , format_specifier
                               );
}

u64
_string_format_parse_next_argument_floating_point_show_fractional
(   state_t*                            state
,   const string_format_specifier_t*    format_specifier
)
{
    const u64 arg = *( ( *state ).next_arg );
    if ( !arg )
    {
        return _string_format_parse_next_argument_string ( state
                                                         , format_specifier
                                                         );
    }

    char string[ STRING_FORMAT_MAX_FLOATING_POINT_STRING_LENGTH ];
    const char* format;
    if ( ( *format_specifier ).sign.tag == STRING_FORMAT_SIGN_NONE )
    {
        if ( ( *format_specifier ).fix_precision.tag )
        {
            switch ( ( *format_specifier ).fix_precision.precision )
            {
                case 0:  format = "%.0lf" ;break;
                case 1:  format = "%.1lf" ;break;
                case 2:  format = "%.2lf" ;break;
                case 3:  format = "%.3lf" ;break;
                case 4:  format = "%.4lf" ;break;
                case 5:  format = "%.5lf" ;break;
                case 6:  format = "%.6lf" ;break;
                case 7:  format = "%.7lf" ;break;
                case 8:  format = "%.8lf" ;break;
                case 9:  format = "%.9lf" ;break;
                default: format = "%.lf"  ;break;
            }
        }
        else
        {
            format = "%lf";
        }
    }
    else
    {
        if ( ( *format_specifier ).fix_precision.tag )
        {
            switch ( ( *format_specifier ).fix_precision.precision )
            {
                case 0:  format = "%+.0lf" ;break;
                case 1:  format = "%+.1lf" ;break;
                case 2:  format = "%+.2lf" ;break;
                case 3:  format = "%+.3lf" ;break;
                case 4:  format = "%+.4lf" ;break;
                case 5:  format = "%+.5lf" ;break;
                case 6:  format = "%+.6lf" ;break;
                case 7:  format = "%+.7lf" ;break;
                case 8:  format = "%+.8lf" ;break;
                case 9:  format = "%+.9lf" ;break;
                default: format = "%+.lf"  ;break;
            }
        }
        else
        {
            format = "%+lf";
        }
    }
    i32 snprintf_result = snprintf ( string
                                   , STRING_FORMAT_MAX_FLOATING_POINT_STRING_LENGTH
                                   , format
                                   , *( ( f64* ) arg )
                                   );
    if ( ( *format_specifier ).sign.tag == STRING_FORMAT_SIGN_HIDE )
    {
        snprintf_result -= 1;
        memory_move ( string , string + 1 , snprintf_result );
        string[ snprintf_result ] = 0; // Append terminator.
    }
    return _string_format_push ( &( *state ).string
                               , string
                               , snprintf_result
                               , format_specifier
                               );
}

u64
_string_format_parse_next_argument_floating_point_abbreviated
(   state_t*                            state
,   const string_format_specifier_t*    format_specifier
)
{
    const u64 arg = *( ( *state ).next_arg );
    if ( !arg )
    {
        return _string_format_parse_next_argument_string ( state
                                                         , format_specifier
                                                         );
    }

    char string[ STRING_FORMAT_MAX_FLOATING_POINT_ABBREVIATED_STRING_LENGTH ];
    const char* format;
    if ( ( *format_specifier ).sign.tag == STRING_FORMAT_SIGN_NONE )
    {
        if ( ( *format_specifier ).fix_precision.tag )
        {
            switch ( ( *format_specifier ).fix_precision.precision )
            {
                case 0:  format = "%.0E" ;break;
                case 1:  format = "%.1E" ;break;
                case 2:  format = "%.2E" ;break;
                case 3:  format = "%.3E" ;break;
                case 4:  format = "%.4E" ;break;
                case 5:  format = "%.5E" ;break;
                case 6:  format = "%.6E" ;break;
                case 7:  format = "%.7E" ;break;
                case 8:  format = "%.8E" ;break;
                case 9:  format = "%.9E" ;break;
                default: format = "%.E"  ;break;
            }
        }
        else
        {
            format = "%E";
        }
    }
    else
    {
        if ( ( *format_specifier ).fix_precision.tag )
        {
            switch ( ( *format_specifier ).fix_precision.precision )
            {
                case 0:  format = "%+.0E" ;break;
                case 1:  format = "%+.1E" ;break;
                case 2:  format = "%+.2E" ;break;
                case 3:  format = "%+.3E" ;break;
                case 4:  format = "%+.4E" ;break;
                case 5:  format = "%+.5E" ;break;
                case 6:  format = "%+.6E" ;break;
                case 7:  format = "%+.7E" ;break;
                case 8:  format = "%+.8E" ;break;
                case 9:  format = "%+.9E" ;break;
                default: format = "%+.E"  ;break;
            }
        }
        else
        {
            format = "%+E";
        }
    }
    i32 snprintf_result = snprintf ( string
                                   , STRING_FORMAT_MAX_FLOATING_POINT_ABBREVIATED_STRING_LENGTH
                                   , format
                                   , *( ( f64* ) arg )
                                   );
    if ( ( *format_specifier ).sign.tag == STRING_FORMAT_SIGN_HIDE )
    {
        snprintf_result -= 1;
        memory_move ( string , string + 1 , snprintf_result );
        string[ snprintf_result ] = 0; // Append terminator.
    }
    return _string_format_push ( &( *state ).string
                               , string
                               , snprintf_result
                               , format_specifier
                               );
}

u64
_string_format_parse_next_argument_floating_point_fractional_only
(   state_t*                            state
,   const string_format_specifier_t*    format_specifier
)
{
    const u64 arg = *( ( *state ).next_arg );
    if ( !arg )
    {
        return _string_format_parse_next_argument_string ( state
                                                         , format_specifier
                                                         );
    }

    char string[ STRING_FORMAT_MAX_FLOATING_POINT_STRING_LENGTH ];
    const char* format;
    if ( ( *format_specifier ).fix_precision.tag )
    {
        switch ( ( *format_specifier ).fix_precision.precision )
        {
            case 0:  format = "%.0lf" ;break;
            case 1:  format = "%.1lf" ;break;
            case 2:  format = "%.2lf" ;break;
            case 3:  format = "%.3lf" ;break;
            case 4:  format = "%.4lf" ;break;
            case 5:  format = "%.5lf" ;break;
            case 6:  format = "%.6lf" ;break;
            case 7:  format = "%.7lf" ;break;
            case 8:  format = "%.8lf" ;break;
            case 9:  format = "%.9lf" ;break;
            default: format = "%.lf"  ;break;
        }
    }
    else
    {
        format = "%lf";
    }
    i32 snprintf_result = snprintf ( string
                                   , STRING_FORMAT_MAX_FLOATING_POINT_STRING_LENGTH
                                   , format
                                   , *( ( f64* ) arg )
                                   );
    u64 index;
    if ( string_contains ( string , snprintf_result
                         , string_char ( '.' ) , 1
                         , false
                         , &index
                         ))
    {
        index += 1;
        snprintf_result -= index;
        memory_move ( string
                    , string + index
                    , snprintf_result
                    );
        string[ snprintf_result ] = 0;
    }
    return _string_format_push ( &( *state ).string
                               , string
                               , snprintf_result
                               , format_specifier
                               );
}
// END <stdio.h> DEPENDENCY.

u64
_string_format_parse_next_argument_address
(   state_t*                            state
,   const string_format_specifier_t*    format_specifier
)
{
    const char* prefix = "0x";
    char string[ 67 ];
    const u64 arg = *( ( *state ).next_arg );
    const u8 radix = 16;
    memory_copy ( string , prefix , _string_length ( prefix ) );
    const u64 string_length = string_u64 ( arg , radix , string + _string_length ( prefix ) ) + _string_length ( prefix );
    return _string_format_push ( &( *state ).string
                               , string
                               , string_length
                               , format_specifier
                               );
}

u64
_string_format_parse_next_argument_character
(   state_t*                            state
,   const string_format_specifier_t*    format_specifier
)
{
    const u64 arg = *( ( *state ).next_arg );
    const char character = ( char ) arg;
    return _string_format_push ( &( *state ).string
                               , &character
                               , 1
                               , format_specifier
                               );
}

u64
_string_format_parse_next_argument_string
(   state_t*                            state
,   const string_format_specifier_t*    format_specifier
)
{
    const u64 arg = *( ( *state ).next_arg );
    char* string = arg ? ( ( char* ) arg ) : "";
    const u64 length = _string_length ( string );
    return _string_format_push ( &( *state ).string
                               , string
                               , length
                               , format_specifier
                               );
}

u64
_string_format_parse_next_argument_resizable_string
(   state_t*                            state
,   const string_format_specifier_t*    format_specifier
)
{
    const u64 arg = *( ( *state ).next_arg );
    if ( !arg )
    {
        return _string_format_parse_next_argument_string ( state
                                                         , format_specifier
                                                         );
    }

    char* string = ( char* ) arg;
    const u64 length = string_length ( string );
    return _string_format_push ( &( *state ).string
                               , string
                               , length
                               , format_specifier
                               );
}

u64
_string_format_push
(   char**                              string
,   const char*                         src
,   const u64                           src_length
,   const string_format_specifier_t*    format_specifier
)
{
    if ( ( *format_specifier ).padding.tag == STRING_FORMAT_PADDING_NONE )
    {
        string_push ( *string , src , src_length );
        return src_length;
    }
    if ( ( *format_specifier ).padding.length <= src_length )
    {
        if ( ( *format_specifier ).padding.fixed )
        {
            string_push ( *string
                        , src
                        , ( *format_specifier ).padding.length
                        );
            return ( *format_specifier ).padding.length;
        }
        else
        {
            string_push ( *string , src , src_length );
            return src_length;
        }
    }
    const u64 pad_length = ( *format_specifier ).padding.length - src_length;
    if ( ( *format_specifier ).padding.tag == STRING_FORMAT_PADDING_LEFT )
    {
        for ( u64 pad = pad_length; pad; --pad )
        {
            string_push ( *string , &( *format_specifier ).padding.value , 1 );
        }
    }
    string_push ( *string , src , src_length );
    if ( ( *format_specifier ).padding.tag == STRING_FORMAT_PADDING_RIGHT )
    {
        for ( u64 pad = pad_length; pad; --pad )
        {
            string_push ( *string , &( *format_specifier ).padding.value , 1 );
        }
    }
    return ( *format_specifier ).padding.length;
}
