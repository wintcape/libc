/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file math/trig.h
 * @brief Defines trigonometric functions.
 */
#ifndef MATH_TRIGONOMETRY_H
#define MATH_TRIGONOMETRY_H

#include "common.h"

// Global constants.
#define PI              3.14159265358979323846f /** @brief π */
#define TWO_PI          ( 2.0f * PI )           /** @brief 2 π */
#define FOUR_PI         ( 4.0f * PI )           /** @brief 4 π */
#define HALF_PI         ( 0.5f * PI )           /** @brief π / 2 */
#define QUARTER_PI      ( 0.25f * PI )          /** @brief π / 4 */
#define DIV_PI          ( 1.0f / PI )           /** @brief 1 / π */
#define DIV_TWO_PI      ( 1.0f / TWO_PI )       /** @brief 1 / 2 π */

/**
 * @brief Defines an alias for each trigonometric function. This avoids function
 * signatures which cause name conflicts with those provided by the standard
 * libc headers.
 */
#define sin(x)  __sin ( x )  /** @brief Defines an alias for the sine function. This resolves a name conflict with the sine function provided by the standard libc headers. */
#define cos(x)  __cos ( x )  /** @brief Defines an alias for the cosine function. This resolves a name conflict with the cosine function provided by the standard libc headers. */
#define tan(x)  __tan ( x )  /** @brief Defines an alias for the tangent function. This resolves a name conflict with the tangent function provided by the standard libc headers. */
#define asin(x) __asin ( x ) /** @brief Defines an alias for the sine inverse function. This resolves a name conflict with the sine inverse function provided by the standard libc headers. */
#define acos(x) __acos ( x ) /** @brief Defines an alias for the cosine inverse function. This resolves a name conflict with the cosine inverse function provided by the standard libc headers. */
#define atan(x) __atan ( x ) /** @brief Defines an alias for the tangent inverse function. This resolves a name conflict with the tangent inverse function provided by the standard libc headers. */
#define sinh(x) __sinh ( x ) /** @brief Defines an alias for the hyperbolic sine function. This resolves a name conflict with the hyperbolic sine function provided by the standard libc headers. */
#define cosh(x) __cosh ( x ) /** @brief Defines an alias for the hyperbolic cosine function. This resolves a name conflict with the hyperbolic cosine function provided by the standard libc headers. */
#define tanh(x) __tanh ( x ) /** @brief Defines an alias for the hyperbolic tangent function. This resolves a name conflict with the hyperbolic tangent function provided by the standard libc headers. */

/**
 * @brief Sine function.
 * 
 * @param x A floating-point number.
 * @return sin(x)
 */
f32
_sin
(   f32 x
);

/**
 * @brief Cosine function.
 * 
 * @param x A floating-point number.
 * @return cos(x)
 */
f32
_cos
(   f32 x
);

/**
 * @brief Tangent function.
 * 
 * @param x A floating-point number.
 * @return tan(x)
 */
f32
_tan
(   f32 x
);

/**
 * @brief Sine inverse function.
 * 
 * @param x A floating-point number.
 * @return arcsin(x)
 */
f32
_asin
(   f32 x
);

/**
 * @brief Cosine inverse function.
 * 
 * @param x A floating-point number.
 * @return arccos(x)
 */
f32
_acos
(   f32 x
);

/**
 * @brief Tangent inverse function.
 * 
 * @param x A floating-point number.
 * @return arctan(x)
 */
f32
_atan
(   f32 x
);

/**
 * @brief Hyperbolic sine function.
 * 
 * @param x A floating-point number.
 * @return sinh(x)
 */
f32
_sinh
(   f32 x
);

/**
 * @brief Hyperbolic cosine function.
 * 
 * @param x A floating-point number.
 * @return cosh(x)
 */
f32
_cosh
(   f32 x
);

/**
 * @brief Hyperbolic tangent function.
 * 
 * @param x A floating-point number.
 * @return tanh(x)
 */
f32
_tanh
(   f32 x
);

#endif  // MATH_TRIGONOMETRY_H