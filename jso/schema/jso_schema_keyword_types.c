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

#include "jso_schema_keyword_types.h"
#include "jso_schema_error.h"

#include "../jso.h"

static inline size_t jso_schema_keyword_add_type(jso_schema *schema, const char *key,
		jso_value_type type, size_t pos, jso_value_type *types, size_t types_size)
{
	if (pos >= types_size) {
		if (pos == types_size) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_KEYWORD_TYPE,
					"The number of types is too big to report for key %s", key);
		}
	} else {
		types[pos] = type;
	}
	return pos + 1;
}

static size_t jso_schema_keyword_get_any_types(
		jso_schema *schema, const char *key, jso_value_type *types, size_t types_size)
{
	size_t pos = jso_schema_keyword_add_type(schema, key, JSO_TYPE_NULL, 0, types, types_size);
	pos = jso_schema_keyword_add_type(schema, key, JSO_TYPE_BOOL, pos, types, types_size);
	pos = jso_schema_keyword_add_type(schema, key, JSO_TYPE_INT, pos, types, types_size);
	pos = jso_schema_keyword_add_type(schema, key, JSO_TYPE_DOUBLE, pos, types, types_size);
	pos = jso_schema_keyword_add_type(schema, key, JSO_TYPE_STRING, pos, types, types_size);
	pos = jso_schema_keyword_add_type(schema, key, JSO_TYPE_ARRAY, pos, types, types_size);
	return jso_schema_keyword_add_type(schema, key, JSO_TYPE_OBJECT, pos, types, types_size);
}

static size_t jso_schema_keyword_get_null_types(
		jso_schema *schema, const char *key, jso_value_type *types, size_t types_size)
{
	return jso_schema_keyword_add_type(schema, key, JSO_TYPE_NULL, 0, types, types_size);
}

static size_t jso_schema_keyword_get_bool_types(
		jso_schema *schema, const char *key, jso_value_type *types, size_t types_size)
{
	return jso_schema_keyword_add_type(schema, key, JSO_TYPE_BOOL, 0, types, types_size);
}

static size_t jso_schema_keyword_get_number_types(
		jso_schema *schema, const char *key, jso_value_type *types, size_t types_size)
{
	size_t pos = jso_schema_keyword_add_type(schema, key, JSO_TYPE_INT, 0, types, types_size);
	return jso_schema_keyword_add_type(schema, key, JSO_TYPE_DOUBLE, pos, types, types_size);
}

static size_t jso_schema_keyword_get_string_types(
		jso_schema *schema, const char *key, jso_value_type *types, size_t types_size)
{
	return jso_schema_keyword_add_type(schema, key, JSO_TYPE_STRING, 0, types, types_size);
}

static size_t jso_schema_keyword_get_array_types(
		jso_schema *schema, const char *key, jso_value_type *types, size_t types_size)
{
	return jso_schema_keyword_add_type(schema, key, JSO_TYPE_ARRAY, 0, types, types_size);
}

static size_t jso_schema_keyword_get_object_types(
		jso_schema *schema, const char *key, jso_value_type *types, size_t types_size)
{
	return jso_schema_keyword_add_type(schema, key, JSO_TYPE_OBJECT, 0, types, types_size);
}

static size_t jso_schema_keyword_get_object_or_array_types(
		jso_schema *schema, const char *key, jso_value_type *types, size_t types_size)
{
	size_t pos = jso_schema_keyword_add_type(schema, key, JSO_TYPE_OBJECT, 0, types, types_size);
	return jso_schema_keyword_add_type(schema, key, JSO_TYPE_ARRAY, pos, types, types_size);
}

typedef size_t (*jso_schema_keyword_get_types_callback)(
		jso_schema *schema, const char *key, jso_value_type *types, size_t types_size);

static const jso_schema_keyword_get_types_callback schema_keyword_get_types_callbacks[] = {
	[JSO_SCHEMA_KEYWORD_TYPE_ANY] = jso_schema_keyword_get_any_types,
	[JSO_SCHEMA_KEYWORD_TYPE_NULL] = jso_schema_keyword_get_null_types,
	[JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN] = jso_schema_keyword_get_bool_types,
	[JSO_SCHEMA_KEYWORD_TYPE_INTEGER] = jso_schema_keyword_get_number_types,
	[JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER] = jso_schema_keyword_get_number_types,
	[JSO_SCHEMA_KEYWORD_TYPE_DOUBLE] = jso_schema_keyword_get_number_types,
	[JSO_SCHEMA_KEYWORD_TYPE_NUMBER] = jso_schema_keyword_get_number_types,
	[JSO_SCHEMA_KEYWORD_TYPE_STRING] = jso_schema_keyword_get_string_types,
	[JSO_SCHEMA_KEYWORD_TYPE_REGEXP] = jso_schema_keyword_get_string_types,
	[JSO_SCHEMA_KEYWORD_TYPE_ARRAY] = jso_schema_keyword_get_array_types,
	[JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_STRINGS] = jso_schema_keyword_get_array_types,
	[JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS] = jso_schema_keyword_get_array_types,
	[JSO_SCHEMA_KEYWORD_TYPE_OBJECT] = jso_schema_keyword_get_object_types,
	[JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT] = jso_schema_keyword_get_object_types,
	[JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS] = jso_schema_keyword_get_object_types,
	[JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS_OR_ARRAY_OF_STRINGS]
	= jso_schema_keyword_get_object_or_array_types,
	[JSO_SCHEMA_KEYWORD_TYPE_REGEXP_OBJECT_OF_SCHEMA_OBJECTS] = jso_schema_keyword_get_object_types,
};

size_t jso_schema_keyword_get_types(jso_schema *schema, const char *key,
		jso_schema_keyword_type keyword_type, jso_value_type *types, size_t types_size)
{
	return schema_keyword_get_types_callbacks[keyword_type](schema, key, types, types_size);
}
