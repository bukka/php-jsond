/*
 * Copyright (c) 2024-2025 Jakub Zelenka. All rights reserved.
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

#include "jso_schema_validation_error.h"
#include "jso_schema_validation_string.h"

#include "jso_schema_error.h"
#include "jso_schema_keyword.h"
#include "jso_schema_value.h"

#include "jso.h"
#include "jso_re.h"

jso_schema_validation_result jso_schema_validation_string_value_str(
		jso_schema *schema, jso_schema_validation_position *pos, jso_virt_string *instance_str)
{
	jso_schema_value_string *strval = JSO_SCHEMA_VALUE_DATA_STR_P(pos->current_value);

	if (JSO_SCHEMA_KW_IS_SET(strval->min_length)) {
		jso_uint kw_uval = JSO_SCHEMA_KEYWORD_DATA_UINT(strval->min_length);
		if (jso_virt_string_len(instance_str) < kw_uval) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"String length %zu is lower than minimum length %lu",
					jso_virt_string_len(instance_str), kw_uval);
			return JSO_SCHEMA_VALIDATION_INVALID;
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(strval->max_length)) {
		jso_uint kw_uval = JSO_SCHEMA_KEYWORD_DATA_UINT(strval->max_length);
		if (jso_virt_string_len(instance_str) > kw_uval) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"String length %zu is greater than maximum length %lu",
					jso_virt_string_len(instance_str), kw_uval);
			return JSO_SCHEMA_VALIDATION_INVALID;
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(strval->pattern)) {
		JSO_ASSERT_EQ(JSO_SCHEMA_KEYWORD_TYPE(strval->pattern), JSO_SCHEMA_KEYWORD_TYPE_REGEXP);
		jso_re_code *code = JSO_SCHEMA_KEYWORD_DATA_RE(strval->pattern);
		jso_re_match_data *match_data = jso_re_match_data_create(code);
		int match_result = jso_re_match(jso_virt_string_val(instance_str),
				jso_virt_string_len(instance_str), code, match_data);
		jso_re_match_data_free(match_data);
		if (match_result <= 0) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"String pattern %s does not match value %s", JSO_RE_CODE_PATTERN(code),
					jso_virt_string_val(instance_str));
			return JSO_SCHEMA_VALIDATION_INVALID;
		}
	}

	return JSO_SCHEMA_VALIDATION_VALID;
}

jso_schema_validation_result jso_schema_validation_string_value(jso_schema *schema,
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos,
		jso_value *instance)
{
	if (jso_virt_value_type(instance) != JSO_TYPE_STRING) {
		return jso_schema_validation_value_type_error(
				schema, pos, JSO_TYPE_STRING, jso_virt_value_type(instance));
	}

	return jso_schema_validation_string_value_str(schema, pos, jso_virt_value_string(instance));
}
