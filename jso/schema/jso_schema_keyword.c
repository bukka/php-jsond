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

#include "jso_schema_keyword.h"
#include "jso_schema_keyword_freer.h"
#include "jso_schema_keyword_single.h"
#include "jso_schema_keyword_union.h"

#include "../jso.h"

static inline jso_rc jso_schema_keyword_check(
		jso_schema *schema, jso_schema_keyword *keyword, jso_schema_value *value)
{
	if (keyword == NULL) {
		return jso_schema_error_is_set(schema) ? JSO_FAILURE : JSO_SUCCESS;
	}

	if (!JSO_SCHEMA_VALUE_IS_NOT_EMPTY_P(value) && JSO_SCHEMA_KEYWORD_IS_PRESENT_P(keyword)) {
		JSO_SCHEMA_VALUE_FLAGS_P(value) |= JSO_SCHEMA_VALUE_FLAG_NOT_EMPTY;
	}

	return JSO_SUCCESS;
}

/* Set keyword. */
jso_rc jso_schema_keyword_set(jso_schema *schema, jso_value *data, const char *key,
		jso_schema_value *value, jso_schema_keyword *schema_keyword,
		jso_schema_keyword_type keyword_type, jso_uint32 keyword_flags)
{
	jso_schema_keyword *schema_keyword_ptr = jso_schema_keyword_get(
			schema, data, key, value, keyword_type, keyword_flags, true, schema_keyword);

	return jso_schema_keyword_check(schema, schema_keyword_ptr, value);
}

/* Set union keyword. */
jso_rc jso_schema_keyword_set_union_of_2_types(jso_schema *schema, jso_value *data, const char *key,
		jso_schema_value *value, jso_schema_keyword *schema_keyword,
		jso_schema_keyword_type keyword_union_type_1, jso_schema_keyword_type keyword_union_type_2,
		jso_uint32 keyword_flags)
{
	jso_schema_keyword *schema_keyword_ptr = jso_schema_keyword_get_union_of_2_types(schema, data,
			key, value, keyword_union_type_1, keyword_union_type_2, keyword_flags, schema_keyword);

	return jso_schema_keyword_check(schema, schema_keyword_ptr, value);
}

/* Convert keyword data to number */
jso_rc jso_schema_keyword_convert_to_number(jso_schema_keyword *schema_keyword, jso_number *num)
{
	switch (JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword)) {
		case JSO_SCHEMA_KEYWORD_TYPE_INTEGER:
			num->ival = JSO_SCHEMA_KEYWORD_DATA_INT_P(schema_keyword);
			num->is_int = true;
			return JSO_SUCCESS;
		case JSO_SCHEMA_KEYWORD_TYPE_UNSIGNED_INTEGER:
			num->ival = JSO_SCHEMA_KEYWORD_DATA_UINT_P(schema_keyword);
			num->is_int = true;
			return JSO_SUCCESS;
		case JSO_SCHEMA_KEYWORD_TYPE_DOUBLE:
		case JSO_SCHEMA_KEYWORD_TYPE_NUMBER:
			num->dval = JSO_SCHEMA_KEYWORD_DATA_DOUBLE_P(schema_keyword);
			num->is_int = false;
			return JSO_SUCCESS;
		default:
			return JSO_FAILURE;
	}
}

/* Clear keyword */
void jso_schema_keyword_clear(jso_schema_keyword *keyword)
{
	jso_schema_keyword_free(keyword);
	JSO_SCHEMA_KEYWORD_FLAGS_P(keyword) = 0;
}
