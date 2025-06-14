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
#include "jso_schema_keyword_regexp.h"

#include "../jso.h"

jso_re_code *jso_schema_keyword_get_regexp_code(
		jso_schema *schema, const char *keyword_key, jso_string *object_key, jso_string *pattern)
{
	jso_re_code *code = jso_re_code_alloc();
	if (code == NULL) {
		if (object_key == NULL) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_KEYWORD_ALLOC,
					"Allocating regular expression code for keyword %s failed", keyword_key);
		} else {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_KEYWORD_ALLOC,
					"Allocating regular expression code for key %s in keyword %s failed",
					JSO_STRING_VAL(object_key), keyword_key);
		}
		return NULL;
	}

	if (jso_re_compile(pattern, code) == JSO_FAILURE) {
		jso_ctype buf[256];
		if (object_key == NULL) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_KEYWORD_PREP,
					"Compiling regular expression for keyword %s failed at position %zu with "
					"error: %s",
					keyword_key, jso_re_get_error_offset(code),
					jso_re_get_error_message(code, buf, sizeof(buf)));
		} else {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_KEYWORD_PREP,
					"Compiling regular expression for key %s in keyword %s failed at position %zu "
					"with error: %s",
					JSO_STRING_VAL(object_key), keyword_key, jso_re_get_error_offset(code),
					jso_re_get_error_message(code, buf, sizeof(buf)));
		}
		jso_re_code_free(code);
		return NULL;
	}

	return code;
}

jso_schema_keyword *jso_schema_keyword_get_regexp(jso_schema *schema, jso_value *data,
		const char *key, jso_bool error_on_invalid_type, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword, jso_value *val, jso_schema_value *parent)
{
	val = jso_schema_data_get(
			schema, data, key, JSO_TYPE_STRING, keyword_flags, error_on_invalid_type, val);
	if (val == NULL) {
		return NULL;
	}

	jso_re_code *code = jso_schema_keyword_get_regexp_code(schema, key, NULL, JSO_STR_P(val));
	if (code == NULL) {
		return NULL;
	}

	JSO_SCHEMA_KEYWORD_FLAGS_P(schema_keyword) = keyword_flags | JSO_SCHEMA_KEYWORD_FLAG_PRESENT;
	JSO_SCHEMA_KEYWORD_TYPE_P(schema_keyword) = JSO_SCHEMA_KEYWORD_TYPE_REGEXP;
	JSO_SCHEMA_KEYWORD_DATA_RE_P(schema_keyword) = code;

	return schema_keyword;
}

void jso_schema_keyword_free_regexp(jso_schema_keyword *schema_keyword)
{
	jso_re_code_free(JSO_SCHEMA_KEYWORD_DATA_RE_P(schema_keyword));
}
