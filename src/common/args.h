/**
 * @author Matthew Weissel (mweissel3@gatech.edu)
 * @file common/pragma.h
 * @brief Defines a preprocessor binding for handling functions which may accept
 * a variable number of arguments.
 */
#ifndef ARGS_H
#define ARGS_H

#include "common/types.h"

/**
 * @brief Preprocessor binding to pass a variable number of arguments to a
 * function using the __VA_ARGS__ preprocessor directive.
 * 
 * Functions which accept this argument list should have the following
 * parameters as the final two parameters of the function:
 *      - u64  : The number of elements in the variadic argument list.
 *      - u64* : The variadic argument list. The arguments each get implicitly
 *               casted to u64 before the list is passed, such that the spacing
 *               between list elements is uniform.
 * The parameters need to be defined in the same order as specified above:
 * the element count, followed by the list of u64.
 * 
 * Under GCC and Clang, this will trip the compiler warning -Wint-conversion.
 * This can be disabled before using the macro and reenabled afterwards if
 * compiling with -Werror (see common/warn.h).
 * 
 * LIMITATIONS :
 * 
 * Currently doesn't support f32, and f64 has to be passed by address, not
 * value. More fixes and format specifier functionality hopefully coming soon.
 * 
 * EXAMPLE USAGE :
 * 
 *     i32 _my_function ( u8 a , u8 b , u64 arg_count , u64* args );
 * 
 *     #define my_function(a,b,...)                               \
 *         ({                                                     \
 *             DISABLE_WARNING ( -Wint-conversion )               \
 *             _my_function ( (a) , (b) , ARGS ( __VA_ARGS__ ) ); \
 *             REENABLE_WARNING ()                                \
 *         })
 */
#define ARGS(...)                                        \
    sizeof ( ( u64[] ){ __VA_ARGS__ } ) / sizeof ( u64 ) \
  , ( u64[] ){ __VA_ARGS__ }

#endif  // ARGS_H