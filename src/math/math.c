/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file math/math.c
 * @brief Implementation of the math header.
 * (see math/math.h for additional details)
 */
#include "math/math.h"

#include "core/string.h"

#include "platform/platform.h"

/**
 * @brief Undefines preprocessor bindings which cause name conflicts with the
 * standard libc headers.
 */
#undef abs
#undef sqrt
#undef sin
#undef cos
#undef tan
#undef asin
#undef acos
#undef atan
#undef random

// Standard libc dependencies.
#include <math.h>
#include <stdlib.h>

// ( see random ).
static bool random_seeded = false;

f32
_abs
(   f32 x
)
{
    return fabsf ( x );
}

f32
_sqrt
(   f32 x
)
{
    return sqrtf ( x );
}


f32
_sin
(   f32 x
)
{
    return sinf ( x );
}

f32
_cos
(   f32 x
)
{
    return cosf ( x );
}

f32
_tan
(   f32 x
)
{
    return tanf ( x );
}

f32
_asin
(   f32 x
)
{
    return asinf ( x );
}

f32
_acos
(   f32 x
)
{
    return acosf ( x );
}

f32
_atan
(   f32 x
)
{
    return atanf ( x );
}

i32
_random
( void )
{
    if ( !random_seeded )
    {
        srand ( ( u32 ) platform_absolute_time () );
        random_seeded = true;
    }
    return rand ();
}

i32
random2
(   i32 min
,   i32 max
)
{
    if ( !random_seeded )
    {
        srand ( ( u32 ) platform_absolute_time () );
        random_seeded = true;
    }
    return ( rand () % ( max - min + 1 ) ) + min;
}

i64
random64
( void )
{
    const u64 a = _random () & 0xFFFF;
    const u64 b = _random () & 0xFFFF;
    const u64 c = _random () & 0xFFFF;
    const u64 d = _random () & 0xFFFF;
    return a | ( b << 16 ) | ( c << 32 ) | ( d << 48 );
}

f32
randomf
( void )
{
    return ( f32 ) _random () / ( f32 ) RAND_MAX;
}

f32
randomf2
(   f32 min
,   f32 max
)
{
    return min + ( f32 ) _random () / ( ( f32 ) RAND_MAX / ( max - min ) );
}
