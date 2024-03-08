/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file math/conversion.h
 * @brief Measurement conversion rates.
 */
#ifndef MATH_CONVERSION_H
#define MATH_CONVERSION_H

#include "common.h"

#include "math/trig.h"

// Time conversion.
#define S_TO_NS ( 1000.0f * 1000.0f * 1000.0f ) /** @brief Conversion factor (seconds to nanoseconds). */
#define S_TO_US ( 1000.0f * 1000.0f )           /** @brief Conversion factor (seconds to microseconds). */
#define S_TO_MS 1000.0f                         /** @brief Conversion factor (seconds to milliseconds). */
#define MS_TO_S 0.001f                          /** @brief Conversion factor (milliseconds to seconds). */

// Angle conversion.
#define DEG_TO_RAD ( PI / 180.0f ) /** @brief Conversion factor (degrees to radians). */
#define RAD_TO_DEG ( 180.0f / PI ) /** @brief Conversion factor (radians to degrees). */

/**
 * @brief Degree to radian conversion function.
 * 
 * @param deg An angle in degrees.
 * @return deg converted to radians.
 */
INLINE
f32
radians
(   f32 deg
)
{
    return DEG_TO_RAD * deg;
}

/**
 * @brief Radian to degree conversion function.
 * 
 * @param rad An angle in radians.
 * @return rad converted to degrees.
 */
INLINE
f32
degrees
(   f32 rad
)
{
    return RAD_TO_DEG * rad;
}

/**
 * @brief Converts elapsed time into appropriate units (for display purposes).
 * 
 * @param elapsed Elapsed time (in seconds).
 * @param hours Output buffer for hours. Must be non-zero.
 * @param minutes Output buffer for minutes. Must be non-zero.
 * @param seconds Output buffer for seconds. Must be non-zero.
 * @param fractional Output buffer for leftover fractional. Must be non-zero.
 */
INLINE
void
display_time
(   const f64   elapsed
,   u64*        hours
,   u64*        minutes
,   u64*        seconds
,   f64*        fractional
)
{
    *hours = ( ( u64 ) elapsed ) / ( 60 * 60 );
    *minutes = ( ( ( u64 ) elapsed ) / 60 ) % 60;
    *seconds = ( ( u64 ) elapsed ) % 60;
    *fractional = elapsed - ( ( f64 ) *seconds );
}

#endif  // MATH_CONVERSION_H