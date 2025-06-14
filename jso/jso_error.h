/*
 * Copyright (c) 2012-2025 Jakub Zelenka. All rights reserved.
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
 * @file jso_error.h
 * @brief Error API
 */

#ifndef JSO_ERROR_H
#define JSO_ERROR_H

#include "jso_types.h"

/**
 * Create a new error from supplied type and location.
 * @param type error type
 * @param loc error location
 * @return A new error instance.
 */
JSO_API jso_error *jso_error_new_ex(jso_error_type type, jso_error_location *loc);

/**
 * Craete a new error from supplied type and positions.
 * @param type error type
 * @param first_column first column position where the error was found
 * @param first_line first line position where the error was found
 * @param last_column last column position where the error was found
 * @param last_line last line position where the error was found
 * @return A new error instance.
 */
JSO_API jso_error *jso_error_new(jso_error_type type, size_t first_column, size_t first_line,
		size_t last_column, size_t last_line);

/**
 * Get the description for the supplied error type.
 * @param type error type
 * @return error type description
 */
JSO_API const char *jso_error_type_description(jso_error_type type);

/**
 * Free error.
 * @param err error instance
 */
JSO_API void jso_error_free(jso_error *err);

#endif /* JSO_ERROR_H */
