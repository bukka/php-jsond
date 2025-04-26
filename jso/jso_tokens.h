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
 * @file jso_tokens.h
 * @brief Tokens returned from scanning.
 */

#ifndef JSO_TOKENS_H
#define JSO_TOKENS_H

/**
 * @brief JSON null value.
 */
#define JSO_T_NUL 1

/**
 * @brief JSON true value.
 */
#define JSO_T_TRUE 2

/**
 * @brief JSON false value.
 */
#define JSO_T_FALSE 3

/**
 * @brief JSON long number value.
 */
#define JSO_T_LONG 4

/**
 * @brief JSON double number value.
 */
#define JSO_T_DOUBLE 5

/**
 * @brief JSON string value.
 */
#define JSO_T_STRING 6

/**
 * @brief Error in string.
 */
#define JSO_T_ESTRING 7

/**
 * @brief Memory allocation error.
 */
#define JSO_T_ENOMEM 125

/**
 * @brief IO error.
 */
#define JSO_T_EOI 126

/**
 * @brief Other error.
 */
#define JSO_T_ERROR 127

#endif /* JSO_TOKENS_H */
