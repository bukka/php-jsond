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
 * @file jso_schema_keyword_single.h
 * @brief JsonSchema single keywords retrieving / creating functionality.
 */

#ifndef JSO_SCHEMA_KEYWORD_SINGLE_H
#define JSO_SCHEMA_KEYWORD_SINGLE_H

#include "jso_schema.h"

jso_schema_keyword *jso_schema_keyword_get_ex(jso_schema *schema, jso_value *data, const char *key,
		jso_schema_value *parent, jso_schema_keyword_type keyword_type, jso_uint32 keyword_flags,
		jso_bool error_on_invalid_type, jso_schema_keyword *schema_keyword, jso_value *val);

static inline jso_schema_keyword *jso_schema_keyword_get(jso_schema *schema, jso_value *data,
		const char *key, jso_schema_value *parent, jso_schema_keyword_type keyword_type,
		jso_uint32 keyword_flags, jso_bool error_on_invalid_type,
		jso_schema_keyword *schema_keyword)
{
	return jso_schema_keyword_get_ex(schema, data, key, parent, keyword_type, keyword_flags,
			error_on_invalid_type, schema_keyword, NULL);
}

#endif /* JSO_SCHEMA_KEYWORD_SINGLE_H */
