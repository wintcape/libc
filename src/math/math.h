/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file math/math.h
 * @brief Datatypes and operations for performing geometric operations.
 */
#ifndef MATH_H
#define MATH_H

#include "common.h"

#include "math/conversion.h"
#include "math/div.h"
#include "math/float.h"
#include "math/random.h"
#include "math/root.h"
#include "math/trig.h"

/**
 * @brief Computes the minimum of two integer values.
 */
#define MIN(a_,b_)                  \
   ({ __typeof__ (a_) a__ = (a_);   \
      __typeof__ (b_) b__ = (b_);   \
     ( a__ < b__ ) ? a__ : b__;     \
    })

/**
 * @brief Computes the maximum of two integer values.
 */
#define MAX(a_,b_)                  \
   ({ __typeof__ (a_) a__ = (a_);   \
      __typeof__ (b_) b__ = (b_);   \
      ( a__ > b__ ) ? a__ : b__;    \
    })

/**
 * @brief Clamps an integer value between a min and max (inclusive).
 * 
 * @param value The value to be clamped.
 * @param min The minimum value of the range.
 * @param max The maximum value of the range.
 * @return The clamped value.
 */
#define CLAMP(value_,min_,max_)                                                 \
    ({ __typeof__ (value_) value__ = (value_);                                  \
       __typeof__ (min_) min__ = (min_);                                        \
       __typeof__ (max_) max__ = (max_);                                        \
       ( value__ <= min__ ) ? min__ : ( value__ >= max__ ) ? max__ : value__;   \
    })

#endif  // MATH_H