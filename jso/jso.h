/*
 * Copyright (c) 2012-2023 Jakub Zelenka. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

/**
 * @file jso.h
 * @brief Main API header
 */

#ifndef JSO_H
#define JSO_H

#include <config.h>
#include "jso_types.h"
#include "jso_assert.h"
#include "jso_value.h"
#include "jso_error.h"
#include "jso_array.h"
#include "jso_object.h"
#include "jso_re.h"
#include "jso_virt.h"

/**
 * Get a max of two values.
 * @param value_1 first value
 * @param value_2 second value
 * @return `value_1` if `value_1` > `value_2`, otherwise `value_2`
 */
#define JSO_MAX(value_1, value_2) ((value_1) > (value_2) ? (value_1) : (value_2))

/**
 * Get a min of two values.
 * @param value_1 first value
 * @param value_2 second value
 * @return `value_1` if `value_1` < `value_2`, otherwise `value_2`
 */
#define JSO_MIN(value_1, value_2) ((value_1) < (value_2) ? (value_1) : (value_2))

/* jso int limits */
#if SIZEOF_LONG == 4
#define JSO_INT_MAX_LENGTH 10
#define JSO_INT_MAX_DIGITS "2147483648"
#elif SIZEOF_LONG == 8
#define JSO_INT_MAX_LENGTH 19
#define JSO_INT_MAX_DIGITS "9223372036854775808"
#else
#error "Unknown SIZEOF_LONG"
#endif

/**
 * Suppress warnings about unused variable.
 * @param unused_variable the variable that is not used
 */
#define JSO_USE(unused_variable) ((void) (unused_variable))

/**
 * Suppress warnings about unused variable and 1 extra parameter.
 * @param uv0 the variable that is not used
 * @param uv1 the parameter that is not used
 */
#define JSO_USE_1(uv0, uv1) \
	JSO_USE(uv0); \
	JSO_USE(uv1)

/**
 * Suppress warnings about unused variable and 2 extra parameters.
 * @param uv0 the variable that is not used
 * @param uv1 the first parameter that is not used
 * @param uv2 the second parameter that is not used
 */
#define JSO_USE_2(uv0, uv1, uv2) \
	JSO_USE(uv0); \
	JSO_USE(uv1); \
	JSO_USE(uv2)

/* memeroy functions - just redefinitions for future extending (checking) */

#ifndef jso_malloc
/**
 * @brief Memory allocation
 */
#define jso_malloc malloc
#endif

#ifndef jso_realloc
/**
 * @brief Memory reallocation
 */
#define jso_realloc realloc
#endif

#ifndef jso_calloc
/**
 * @brief Memory allocation with zeroing all bytes
 */
#define jso_calloc calloc
#endif

#ifndef jso_free
/**
 * @brief Memory freeing
 */
#define jso_free free
#endif

#endif /* JSO_H */
