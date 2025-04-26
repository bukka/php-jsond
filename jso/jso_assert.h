/*
 * Copyright (c) 2023 Jakub Zelenka. All rights reserved.
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
 * @file jso_assert.h
 * @brief Assertions
 */

#ifndef JSO_ASSERT_H
#define JSO_ASSERT_H

#include <assert.h>

/**
 * Assert that expression is true.
 *
 * @note Wrapper around clib assert
 */
#define JSO_ASSERT assert

/**
 * Assert that value 1 is equal to value 2.
 *
 * @param _v1 value 1
 * @param _v2 value 2
 */
#define JSO_ASSERT_EQ(_v1, _v2) JSO_ASSERT((_v1) == (_v2))

/**
 * Assert that value 1 is lower than value 2.
 *
 * @param _v1 value 1
 * @param _v2 value 2
 */
#define JSO_ASSERT_LT(_v1, _v2) JSO_ASSERT((_v1) < (_v2))

/**
 * Assert that value 1 is lower than or equal to value 2.
 *
 * @param _v1 value 1
 * @param _v2 value 2
 */
#define JSO_ASSERT_LE(_v1, _v2) JSO_ASSERT((_v1) <= (_v2))

/**
 * Assert that value 1 is greater than value 2.
 *
 * @param _v1 value 1
 * @param _v2 value 2
 */
#define JSO_ASSERT_GT(_v1, _v2) JSO_ASSERT((_v1) > (_v2))

/**
 * Assert that value 1 is greater than or equal to value 2.
 *
 * @param _v1 value 1
 * @param _v2 value 2
 */
#define JSO_ASSERT_GE(_v1, _v2) JSO_ASSERT((_v1) >= (_v2))

/**
 * Assert that value is not null.
 *
 * @param _val value
 */
#define JSO_ASSERT_NOT_NULL(_val) JSO_ASSERT((_val) != NULL)

#endif /* JSO_ASSERT_H */
