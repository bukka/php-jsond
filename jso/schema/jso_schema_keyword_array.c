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

#include "jso_schema_array.h"
#include "jso_schema_data.h"
#include "jso_schema_error.h"
#include "jso_schema_keyword.h"
#include "jso_schema_value.h"

#include "jso.h"

#include <math.h>

static inline jso_rc jso_schema_keyword_check_array(
		jso_schema *schema, const char *key, jso_array *arr, jso_uint32 keyword_flags)
{
	if ((keyword_flags & JSO_SCHEMA_KEYWORD_FLAG_NOT_EMPTY) && JSO_ARRAY_LEN(arr) == 0) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_DEPS,
				"Array for keyword %s must not be empty", key);
		return JSO_FAILURE;
	}
	if ((keyword_flags & JSO_SCHEMA_KEYWORD_FLAG_UNIQUE) && !jso_array_is_unique(arr)) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_DEPS,
				"Array values for keyword %s must be unique", key);
		return JSO_FAILURE;
	}
	return JSO_SUCCESS;
}

jso_rc jso_schema_keyword_validate_array_of_strings(
		jso_schema *schema, const char *key, jso_array *arr, jso_uint32 keyword_flags)
{
	if (jso_schema_keyword_check_array(schema, key, arr, keyword_flags) == JSO_FAILURE) {
		return JSO_FAILURE;
	}
	if (!jso_array_are_all_items_of_type(arr, JSO_TYPE_STRING)) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_TYPE,
				"Array values for keyword %s must be a string", key);
		return JSO_FAILURE;
	}
	return JSO_SUCCESS;
}

jso_schema_keyword *jso_schema_keyword_get_array(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get(
			schema, data, key, JSO_TYPE_ARRAY, keyword_flags, error_on_invalid_type, val);
	if (val == NULL) {
		return NULL;
	}
	jso_array *arr = JSO_ARRVAL_P(val);
	if (jso_schema_keyword_check_array(schema, key, arr, keyword_flags) == JSO_FAILURE) {
		return NULL;
	}
	JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) = keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword) = JSO_SCHEMA_KEYWORD_TYPE_ARRAY;
	JSO_SCHEMA_KEYWORD_DATA_ARR_P(schema_keyword) = jso_array_copy(arr);
	return schema_keyword;
}

jso_schema_keyword *jso_schema_keyword_get_array_of_strings(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get(
			schema, data, key, JSO_TYPE_ARRAY, keyword_flags, error_on_invalid_type, val);
	if (val == NULL) {
		return NULL;
	}
	jso_array *arr = JSO_ARRVAL_P(val);
	if (jso_schema_keyword_validate_array_of_strings(schema, key, arr, keyword_flags)
			== JSO_FAILURE) {
		return NULL;
	}

	JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) = keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword) = JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_STRINGS;
	JSO_SCHEMA_KEYWORD_DATA_ARR_STR_P(schema_keyword) = jso_array_copy(arr);
	return schema_keyword;
}

jso_schema_keyword *jso_schema_keyword_get_array_of_schema_objects(jso_schema *schema,
		jso_value *data, const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get(
			schema, data, key, JSO_TYPE_ARRAY, keyword_flags, error_on_invalid_type, val);
	if (val == NULL) {
		return NULL;
	}
	jso_value *item;
	jso_array *arr = JSO_ARRVAL_P(val);
	if (jso_schema_keyword_check_array(schema, key, arr, keyword_flags) == JSO_FAILURE) {
		return NULL;
	}
	jso_schema_array *schema_arr = jso_schema_array_alloc(JSO_ARRAY_LEN(arr));
	if (schema_arr == NULL) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_KEYWORD_ALLOC,
				"Allocating schema array of values for keyword %s failed", key);
		return NULL;
	}
	JSO_ARRAY_FOREACH(arr, item)
	{
		if (JSO_TYPE_P(item) != JSO_TYPE_OBJECT
				&& (schema->version < JSO_SCHEMA_VERSION_DRAFT_06
						|| JSO_TYPE_P(item) != JSO_TYPE_BOOL)) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_DATA_TYPE,
					"Object value for keyword %s must be a schema object", key);
			jso_schema_array_free(schema_arr);
			return NULL;
		}
		jso_schema_value *schema_value = jso_schema_value_parse(schema, item, parent);
		if (schema_value == NULL) {
			jso_schema_array_free(schema_arr);
			return NULL;
		}
		jso_rc append_res = jso_schema_array_append(schema_arr, schema_value);
		JSO_ASSERT_EQ(append_res, JSO_SUCCESS);
	}
	JSO_ARRAY_FOREACH_END;

	JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) = keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword) = JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS;
	JSO_SCHEMA_KEYWORD_DATA_ARR_SCHEMA_OBJ_P(schema_keyword) = schema_arr;
	return schema_keyword;
}

void jso_schema_keyword_free_array(jso_schema_keyword *schema_keyword)
{
	jso_array_free(JSO_SCHEMA_KEYWORD_DATA_ARR_P(schema_keyword));
}

void jso_schema_keyword_free_array_of_strings(jso_schema_keyword *schema_keyword)
{
	jso_array_free(JSO_SCHEMA_KEYWORD_DATA_ARR_STR_P(schema_keyword));
}

void jso_schema_keyword_free_array_of_schema_objects(jso_schema_keyword *schema_keyword)
{
	jso_schema_array_free(JSO_SCHEMA_KEYWORD_DATA_ARR_SCHEMA_OBJ_P(schema_keyword));
}