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
 * @brief Undefines preprocessor bindings which may cause name conflicts with
 * the standard libc headers.
 */
#undef nan
#undef finite
#undef abs
#undef floor
#undef ceiling
#undef pow
#undef sqrt
#undef exp
#undef ln
#undef log
#undef sin
#undef cos
#undef tan
#undef asin
#undef acos
#undef atan
#undef sinh
#undef cosh
#undef tanh
#undef random

// Standard libc dependencies.
#include <math.h>
#include <stdlib.h>

// ( see random ).
static bool random_seeded = false;

bool
math_nan
(   f32 x
)
{
    return isnan ( x );
}

bool
math_finite
(   f32 x
)
{
    return !isinf ( x );
}

f32
math_abs
(   f32 x
)
{
    return fabsf ( x );
}

f32
math_floor
(   f32 x
)
{
    return floorf ( x );
}

f32
math_ceiling
(   f32 x
)
{
    return ceilf ( x );
}

f32
math_pow
(   f32 x
,   f32 y
)
{
    return powf ( x , y );
}

f32
math_sqrt
(   f32 x
)
{
    return sqrtf ( x );
}

f32
math_exp
(   f32 x
)
{
    return expf ( x );
}

f32
math_ln
(   f32 x
)
{
    return logf ( x );
}

f32
math_log
(   f32 x
)
{
    return log10f ( x );
}

f32
math_sin
(   f32 x
)
{
    return sinf ( x );
}

f32
math_cos
(   f32 x
)
{
    return cosf ( x );
}

f32
math_tan
(   f32 x
)
{
    return tanf ( x );
}

f32
math_asin
(   f32 x
)
{
    return asinf ( x );
}

f32
math_acos
(   f32 x
)
{
    return acosf ( x );
}

f32
math_atan
(   f32 x
)
{
    return atanf ( x );
}

f32
math_sinh
(   f32 x
)
{
    return sinhf ( x );
}

f32
math_cosh
(   f32 x
)
{
    return coshf ( x );
}

f32
math_tanh
(   f32 x
)
{
    return tanhf ( x );
}

i32
math_random
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
math_random2
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
math_random64
( void )
{
    const u64 a = math_random () & 0xFFFF;
    const u64 b = math_random () & 0xFFFF;
    const u64 c = math_random () & 0xFFFF;
    const u64 d = math_random () & 0xFFFF;
    return a | ( b << 16 ) | ( c << 32 ) | ( d << 48 );
}

f32
math_randomf
( void )
{
    return ( f32 ) math_random () / ( f32 ) RAND_MAX;
}

f32
math_randomf2
(   f32 min
,   f32 max
)
{
    return min + ( f32 ) math_random () / ( ( f32 ) RAND_MAX / ( max - min ) );
}
