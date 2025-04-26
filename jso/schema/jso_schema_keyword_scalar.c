/*
 * Copyright (c) 2023-2025 Jakub Zelenka. All rights reserved.
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
#include "jso_schema_keyword_scalar.h"

#include "jso.h"

#include <math.h>

jso_schema_keyword *jso_schema_keyword_get_any(jso_schema *schema, jso_value *data, const char *key,
		jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get_value(schema, data, key, keyword_flags, val);
	if (val != NULL) {
		JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword)
				= keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
		JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword) = JSO_SCHEMA_KEYWORD_TYPE_ANY;
		val = jso_value_copy(val);
		JSO_ASSERT_NOT_NULL(val);
		JSO_SCHEMA_KEYWORD_DATA_ANY_P(schema_keyword) = val;
		return schema_keyword;
	}
	return NULL;
}

jso_schema_keyword *jso_schema_keyword_get_null(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	if (jso_schema_data_get(
				schema, data, key, JSO_TYPE_NULL, keyword_flags, error_on_invalid_type, val)
			!= NULL) {
		JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword)
				= keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
		JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword) = JSO_SCHEMA_KEYWORD_TYPE_NULL;
		return schema_keyword;
	}

	return NULL;
}

jso_schema_keyword *jso_schema_keyword_get_bool(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get(
			schema, data, key, JSO_TYPE_BOOL, keyword_flags, error_on_invalid_type, val);
	if (val != NULL) {
		JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword)
				= keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
		JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword) = JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN;
		JSO_SCHEMA_KEYWORD_DATA_BOOL_P(schema_keyword) = JSO_IVAL_P(val) ? true : false;
		return schema_keyword;
	}
	return NULL;
}

jso_schema_keyword *jso_schema_keyword_get_int(jso_schema *schema, jso_value *data, const char *key,
		jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get_value(schema, data, key, keyword_flags, val);
	if (val == NULL) {
		return NULL;
	}

	if (jso_schema_data_check_type(schema, key, val, JSO_TYPE_INT, JSO_TYPE_INT, false)
			== JSO_SUCCESS) {
		JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword)
				= keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
		JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword) = JSO_SCHEMA_KEYWORD_TYPE_INTEGER;
		JSO_SCHEMA_KEYWORD_DATA_INT_P(schema_keyword) = JSO_IVAL_P(val);
		return schema_keyword;
	}
	if (jso_schema_data_check_type(
				schema, key, val, JSO_TYPE_DOUBLE, JSO_TYPE_INT, error_on_invalid_type)
			== JSO_FAILURE) {
		return NULL;
	}

	double dval = JSO_DVAL_P(val);
	if (nearbyint(dval) == dval) {
		JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword)
				= keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
		JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword) = JSO_SCHEMA_KEYWORD_TYPE_INTEGER;
		JSO_SCHEMA_KEYWORD_DATA_INT_P(schema_keyword) = (jso_int) dval;
		return schema_keyword;
	}

	jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_TYPE,
			"Integer fraction is not zero for keyword %s", key);

	return NULL;
}

jso_schema_keyword *jso_schema_keyword_get_uint(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	jso_schema_keyword *schema_keyword_int = jso_schema_keyword_get_int(
			schema, data, key, error_on_invalid_type, keyword_flags, schema_keyword, val, parent);
	if (schema_keyword_int == NULL) {
		return NULL;
	}

	jso_int ival = JSO_SCHEMA_KEYWORD_DATA_INT_P(schema_keyword);

	if (keyword_flags & JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO) {
		if (ival <= 0) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_TYPE,
					"Value for for keyword %s must be greater than 0", key);
			return NULL;
		}
	} else if (ival < 0) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_TYPE,
				"Value for for keyword %s must be equal or greater than 0", key);
		return NULL;
	}
	JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword) = JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER;
	JSO_SCHEMA_KEYWORD_DATA_UINT_P(schema_keyword) = (jso_uint) ival;

	return schema_keyword;
}

jso_schema_keyword *jso_schema_keyword_get_number(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get_value(schema, data, key, keyword_flags, val);
	if (val == NULL) {
		return NULL;
	}

	if (jso_schema_data_check_type(schema, key, val, JSO_TYPE_INT, JSO_TYPE_INT, false)
			== JSO_SUCCESS) {
		JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword)
				= keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
		JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword) = JSO_SCHEMA_KEYWORD_TYPE_INTEGER;
		JSO_SCHEMA_KEYWORD_DATA_INT_P(schema_keyword) = JSO_IVAL_P(val);
		return schema_keyword;
	}
	if (jso_schema_data_check_type(
				schema, key, val, JSO_TYPE_DOUBLE, JSO_TYPE_INT, error_on_invalid_type)
			== JSO_SUCCESS) {
		JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) = keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT
				| JSO_SCHEMA_KEYWORD_FLAG_FLOATING;
		JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword) = JSO_SCHEMA_KEYWORD_TYPE_DOUBLE;
		JSO_SCHEMA_KEYWORD_DATA_DOUBLE_P(schema_keyword) = JSO_DVAL_P(val);
		return schema_keyword;
	}

	return NULL;
}

jso_schema_keyword *jso_schema_keyword_get_string(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get(
			schema, data, key, JSO_TYPE_STRING, keyword_flags, error_on_invalid_type, val);
	if (val != NULL) {
		JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword)
				= keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
		JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword) = JSO_SCHEMA_KEYWORD_TYPE_STRING;
		JSO_SCHEMA_KEYWORD_DATA_STR_P(schema_keyword) = jso_string_copy(JSO_STR_P(val));
		return schema_keyword;
	}
	return NULL;
}

void jso_schema_keyword_free_any(jso_schema_keyword *schema_keyword)
{
	jso_value_free(JSO_SCHEMA_KEYWORD_DATA_ANY_P(schema_keyword));
}

void jso_schema_keyword_free_string(jso_schema_keyword *schema_keyword)
{
	jso_string_free(JSO_SCHEMA_KEYWORD_DATA_STR_P(schema_keyword));
}
