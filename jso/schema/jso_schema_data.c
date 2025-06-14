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

#include "jso_schema_data.h"
#include "jso_schema_error.h"

#include "../jso.h"

#define JSO_SCHEMA_DATA_TYPES_BUF_SIZE 256

static size_t jso_schema_data_types_cstr_append(char *buf, size_t pos, const char *cstr)
{
	size_t cstr_len = strlen(cstr);
	size_t end_pos = pos + cstr_len;
	if (end_pos >= JSO_SCHEMA_DATA_TYPES_BUF_SIZE) {
		// if the buffer is full, no new types are included
		return pos;
	}
	strncpy(buf + pos, cstr, cstr_len + 1);
	return end_pos;
}

void jso_schema_data_type_error(jso_schema *schema, const char *key, jso_value *val,
		const jso_value_type *expected_types, size_t expected_types_count)
{
	char expected_types_buf[JSO_SCHEMA_DATA_TYPES_BUF_SIZE];
	if (expected_types_count == 1) {
		jso_schema_data_types_cstr_append(
				expected_types_buf, 0, jso_value_type_to_string(expected_types[0]));
	} else if (expected_types_count > 1) {
		size_t expected_types_buf_pos
				= jso_schema_data_types_cstr_append(expected_types_buf, 0, "either ");
		for (size_t i = 0; i < expected_types_count; i++) {
			if (i > 0) {
				if (i == expected_types_count - 1) {
					expected_types_buf_pos = jso_schema_data_types_cstr_append(
							expected_types_buf, expected_types_buf_pos, " or ");
				} else {
					expected_types_buf_pos = jso_schema_data_types_cstr_append(
							expected_types_buf, expected_types_buf_pos, ", ");
				}
			}
			expected_types_buf_pos = jso_schema_data_types_cstr_append(expected_types_buf,
					expected_types_buf_pos, jso_value_type_to_string(expected_types[i]));
		}
	} else {
		jso_schema_data_types_cstr_append(expected_types_buf, 0, "no type");
	}

	jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_TYPE,
			"Invalid type for %s - expected %s but given %s", key, expected_types_buf,
			jso_value_type_to_string(JSO_TYPE_P(val)));
}

jso_rc jso_schema_data_check_type(jso_schema *schema, const char *key, jso_value *val,
		jso_value_type type, jso_value_type primary_type, jso_bool error_on_invalid_type)
{
	if (JSO_TYPE_P(val) != type && JSO_TYPE_P(val) != primary_type) {
		if (error_on_invalid_type) {
			jso_value_type expected_types[] = { type, primary_type };
			jso_schema_data_type_error(
					schema, key, val, expected_types, type == primary_type ? 1 : 2);
		}
		return JSO_FAILURE;
	}

	return JSO_SUCCESS;
}

jso_value *jso_schema_data_get_value_fast(
		jso_schema *schema, jso_value *data, const char *key, jso_uint32 keyword_flags)
{
	jso_value *val;

	if (jso_ht_get_by_cstr_key(JSO_OBJHVAL_P(data), key, &val) == JSO_FAILURE) {
		if (keyword_flags & JSO_SCHEMA_KEYWORD_FLAG_REQUIRED) {
			jso_schema_error_format(
					schema, JSO_SCHEMA_ERROR_KEYWORD_REQUIRED, "Keyword %s is required", key);
		}
		return NULL;
	}

	return val;
}

jso_value *jso_schema_data_get(jso_schema *schema, jso_value *data, const char *key,
		jso_value_type type, jso_uint32 keyword_flags, jso_bool error_on_invalid_type,
		jso_value *val)
{
	val = jso_schema_data_get_value(schema, data, key, keyword_flags, val);
	if (val == NULL) {
		return NULL;
	}

	return jso_schema_data_check_type(schema, key, val, type, type, error_on_invalid_type)
					== JSO_SUCCESS
			? val
			: NULL;
}
