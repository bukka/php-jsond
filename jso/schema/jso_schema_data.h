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
 * @file jso_schema_data.h
 * @brief JsonSchema data handling.
 */

#ifndef JSO_SCHEMA_DATA_H
#define JSO_SCHEMA_DATA_H

#include "jso_schema.h"

#include <stdlib.h>

void jso_schema_data_type_error(jso_schema *schema, const char *key, jso_value *val,
		const jso_value_type *expected_types, size_t expected_types_count);

jso_rc jso_schema_data_check_type(jso_schema *schema, const char *key, jso_value *val,
		jso_value_type type, jso_value_type primary_type, jso_bool error_on_invalid_type);

jso_value *jso_schema_data_get(jso_schema *schema, jso_value *data, const char *key,
		jso_value_type type, jso_uint32 keyword_flags, jso_bool error_on_invalid_type,
		jso_value *val);

jso_value *jso_schema_data_get_value_fast(
		jso_schema *schema, jso_value *data, const char *key, jso_uint32 keyword_flags);

static inline jso_value *jso_schema_data_get_value(jso_schema *schema, jso_value *data,
		const char *key, jso_uint32 keyword_flags, jso_value *val)
{
	if (val != NULL) {
		return val;
	}

	return jso_schema_data_get_value_fast(schema, data, key, keyword_flags);
}

static inline jso_string *jso_schema_data_get_str(
		jso_schema *schema, jso_value *data, const char *key)
{
	jso_value *value = jso_schema_data_get(schema, data, key, JSO_TYPE_STRING, 0, true, NULL);
	return value == NULL ? NULL : value->data.str;
}

#endif /* JSO_SCHEMA_DATA_H */
