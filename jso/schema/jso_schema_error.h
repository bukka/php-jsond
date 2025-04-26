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
 * @file jso_schema_error.h
 * @brief JsonSchema error handling.
 */

#ifndef JSO_SCHEMA_ERROR_H
#define JSO_SCHEMA_ERROR_H

#include "jso_schema.h"

#define JSO_SCHEMA_ERROR_FORMAT_SIZE 512

jso_rc jso_schema_error_set(jso_schema *schema, jso_schema_error_type type, const char *message);

jso_rc jso_schema_error_format(
		jso_schema *schema, jso_schema_error_type type, const char *format, ...);

void jso_schema_error_free(jso_schema *schema);

static inline void jso_schema_error_reset(jso_schema *schema)
{
	JSO_SCHEMA_ERROR_TYPE(schema) = JSO_SCHEMA_ERROR_NONE;
}

#endif /* JSO_SCHEMA_ERROR_H */
