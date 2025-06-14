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
#include "jso_schema_keyword_union.h"
#include "jso_schema_keyword_single.h"
#include "jso_schema_keyword_types.h"

#include "../jso.h"

jso_schema_keyword *jso_schema_keyword_get_union_of_2_types(jso_schema *schema, jso_value *data,
		const char *key, jso_schema_value *parent, jso_schema_keyword_type keyword_union_type_1,
		jso_schema_keyword_type keyword_union_type_2, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword)
{
	jso_value *val = jso_schema_data_get_value_fast(schema, data, key, keyword_flags);
	if (val == NULL) {
		return NULL;
	}

	// Try first type.
	jso_schema_keyword *schema_keyword_result = jso_schema_keyword_get_ex(
			schema, data, key, parent, keyword_union_type_1, 0, false, schema_keyword, val);
	if (schema_keyword_result != NULL || jso_schema_error_is_set(schema)) {
		return schema_keyword_result;
	}

	// Try second type.
	schema_keyword_result = jso_schema_keyword_get_ex(schema, data, key, parent,
			keyword_union_type_2, keyword_flags, false, schema_keyword, val);
	if (schema_keyword_result != NULL || jso_schema_error_is_set(schema)) {
		return schema_keyword_result;
	}

	// Trigger type error if types do not match.
	jso_value_type value_types[JSO_SCHEMA_KW_VALUE_TYPES_SIZE];
	size_t types_size = jso_schema_keyword_get_types(
			schema, key, keyword_union_type_1, value_types, JSO_SCHEMA_KW_VALUE_TYPES_SIZE);
	types_size += jso_schema_keyword_get_types(schema, key, keyword_union_type_2,
			&value_types[types_size], JSO_SCHEMA_KW_VALUE_TYPES_SIZE - types_size);
	jso_schema_data_type_error(schema, key, val, value_types, types_size);

	return NULL;
}