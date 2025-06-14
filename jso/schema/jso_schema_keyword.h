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
 * @file jso_schema_keyword.h
 * @brief JsonSchema keywords handling.
 */

#ifndef JSO_SCHEMA_KEYWORD_H
#define JSO_SCHEMA_KEYWORD_H

#include "../jso_schema.h"

#define JSO_SCHEMA_KW_TYPE(_kw_type) JSO_SCHEMA_KEYWORD_##_kw_type

#define JSO_SCHEMA_KW_SET_WRAP(_kw_set_call, _value, _value_data) \
	do { \
		if (_kw_set_call == JSO_FAILURE) { \
			jso_schema_value_free(_value); \
			return NULL; \
		} \
	} while (0)

#define JSO_SCHEMA_KW_SET_WITH_FLAGS_EX( \
		_schema, _data, _key, _value, _value_data, _kw_name, _kw_type, _kw_flags) \
	JSO_SCHEMA_KW_SET_WRAP( \
			jso_schema_keyword_set(_schema, _data, #_key, _value, &_value_data->_kw_name, \
					JSO_SCHEMA_KW_TYPE(_kw_type), _kw_flags), \
			_value, _value_data)

#define JSO_SCHEMA_KW_SET_WITH_FLAGS( \
		_schema, _data, _key, _value, _value_data, _kw_type, _kw_flags) \
	JSO_SCHEMA_KW_SET_WITH_FLAGS_EX( \
			_schema, _data, _key, _value, _value_data, _key, _kw_type, _kw_flags)

#define JSO_SCHEMA_KW_SET_EX(_schema, _data, _key, _value, _value_data, _kw_name, _kw_type) \
	JSO_SCHEMA_KW_SET_WITH_FLAGS_EX( \
			_schema, _data, _key, _value, _value_data, _kw_name, _kw_type, 0)

#define JSO_SCHEMA_KW_SET(_schema, _data, _key, _value, _value_data, _kw_type) \
	JSO_SCHEMA_KW_SET_EX(schema, _data, _key, _value, _value_data, _key, _kw_type)

#define JSO_SCHEMA_KW_SET_UNION_2_WITH_FLAGS( \
		_schema, _data, _key, _value, _value_data, _kw_name, _utype1, _utype2, _kw_flags) \
	JSO_SCHEMA_KW_SET_WRAP(jso_schema_keyword_set_union_of_2_types(_schema, _data, #_key, _value, \
								   &_value_data->_kw_name, JSO_SCHEMA_KW_TYPE(_utype1), \
								   JSO_SCHEMA_KW_TYPE(_utype2), _kw_flags), \
			_value, _value_data)

#define JSO_SCHEMA_KW_SET_UNION_2_EX( \
		_schema, _data, _key, _value, _value_data, _kw_name, _utype1, _utype2) \
	JSO_SCHEMA_KW_SET_UNION_2_WITH_FLAGS( \
			_schema, _data, _key, _value, _value_data, _kw_name, _utype1, _utype2, 0)

#define JSO_SCHEMA_KW_SET_UNION_2(_schema, _data, _key, _value, _value_data, _utype1, _utype2) \
	JSO_SCHEMA_KW_SET_UNION_2_EX(_schema, _data, _key, _value, _value_data, _key, _utype1, _utype2)

#define JSO_SCHEMA_KW_SET_UNION JSO_SCHEMA_KW_SET_UNION_2
#define JSO_SCHEMA_KW_SET_UNION_EX JSO_SCHEMA_KW_SET_UNION_2_EX
#define JSO_SCHEMA_KW_SET_UNION_WITH_FLAGS JSO_SCHEMA_KW_SET_UNION_2_WITH_FLAGS

#define JSO_SCHEMA_KW_SET_ANY_EX(_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_EX(_schema, _data, _key, _value, _value_data, _kw_name, TYPE_ANY)

#define JSO_SCHEMA_KW_SET_ANY(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_ANY_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_SET_BOOL_EX(_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_EX(_schema, _data, _key, _value, _value_data, _kw_name, TYPE_BOOLEAN)

#define JSO_SCHEMA_KW_SET_BOOL(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_BOOL_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_SET_INT_EX(_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_EX(_schema, _data, _key, _value, _value_data, _kw_name, TYPE_INTEGER)

#define JSO_SCHEMA_KW_SET_INT(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_INT_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_SET_NUM_EX(_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_EX(_schema, _data, _key, _value, _value_data, _kw_name, TYPE_NUMBER)

#define JSO_SCHEMA_KW_SET_NUM(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_NUM_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_SET_UINT_EX(_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_EX(_schema, _data, _key, _value, _value_data, _kw_name, TYPE_UNSIGNED_INTEGER)

#define JSO_SCHEMA_KW_SET_UINT(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_UINT_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_SET_UINT_NZ_EX(_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_WITH_FLAGS_EX(_schema, _data, _key, _value, _value_data, _kw_name, \
			TYPE_UNSIGNED_INTEGER, JSO_SCHEMA_KEYWORD_FLAG_NOT_ZERO)

#define JSO_SCHEMA_KW_SET_UINT_NZ(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_UINT_NZ_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_SET_STR_EX(_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_EX(_schema, _data, _key, _value, _value_data, _kw_name, TYPE_STRING)

#define JSO_SCHEMA_KW_SET_STR(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_STR_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_SET_RE_EX(_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_EX(_schema, _data, _key, _value, _value_data, _kw_name, TYPE_REGEXP)

#define JSO_SCHEMA_KW_SET_RE(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_RE_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_SET_SCHEMA_OBJ_EX(_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_EX(_schema, _data, _key, _value, _value_data, _kw_name, TYPE_SCHEMA_OBJECT)

#define JSO_SCHEMA_KW_SET_SCHEMA_OBJ(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_SCHEMA_OBJ_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_SET_OBJ_OF_SCHEMA_OBJS_EX( \
		_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_EX( \
			_schema, _data, _key, _value, _value_data, _kw_name, TYPE_OBJECT_OF_SCHEMA_OBJECTS)

#define JSO_SCHEMA_KW_SET_OBJ_OF_SCHEMA_OBJS(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_OBJ_OF_SCHEMA_OBJS_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_SET_ARR_OF_SCHEMA_OBJS_EX( \
		_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_EX( \
			_schema, _data, _key, _value, _value_data, _kw_name, TYPE_ARRAY_OF_SCHEMA_OBJECTS)

#define JSO_SCHEMA_KW_SET_ARR_OF_SCHEMA_OBJS(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_ARR_OF_SCHEMA_OBJS_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_SET_ARR_OF_SCHEMA_OBJS_NE_EX( \
		_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_WITH_FLAGS_EX(_schema, _data, _key, _value, _value_data, _kw_name, \
			TYPE_ARRAY_OF_SCHEMA_OBJECTS, JSO_SCHEMA_KEYWORD_FLAG_NOT_EMPTY)

#define JSO_SCHEMA_KW_SET_ARR_OF_SCHEMA_OBJS_NE(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_ARR_OF_SCHEMA_OBJS_NE_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_SET_ARR_EX(_schema, _data, _key, _value, _value_data, _kw_name) \
	JSO_SCHEMA_KW_SET_EX(_schema, _data, _key, _value, _value_data, _kw_name, TYPE_ARRAY)

#define JSO_SCHEMA_KW_SET_ARR(_schema, _data, _key, _value, _value_data) \
	JSO_SCHEMA_KW_SET_ARR_EX(_schema, _data, _key, _value, _value_data, _key)

#define JSO_SCHEMA_KW_IS_SET(_keyword) (_keyword.flags & JSO_SCHEMA_KEYWORD_FLAG_PRESENT)

#define JSO_SCHEMA_KW_IS_SET_P(_keyword) (_keyword->flags & JSO_SCHEMA_KEYWORD_FLAG_PRESENT)

jso_rc jso_schema_keyword_set(jso_schema *schema, jso_value *data, const char *key,
		jso_schema_value *value, jso_schema_keyword *schema_keyword,
		jso_schema_keyword_type keyword_type, jso_uint32 keyword_flags);

jso_rc jso_schema_keyword_set_union_of_2_types(jso_schema *schema, jso_value *data, const char *key,
		jso_schema_value *value, jso_schema_keyword *schema_keyword,
		jso_schema_keyword_type keyword_union_type_1, jso_schema_keyword_type keyword_union_type_2,
		jso_uint32 keyword_flags);

jso_rc jso_schema_keyword_validate_array_of_strings(
		jso_schema *schema, const char *key, jso_array *arr, jso_uint32 keyword_flags);

jso_rc jso_schema_keyword_convert_to_number(jso_schema_keyword *schema_keyword, jso_number *num);

void jso_schema_keyword_clear(jso_schema_keyword *keyword);

#endif /* JSO_SCHEMA_KEYWORD_H */
