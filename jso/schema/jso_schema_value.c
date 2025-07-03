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

#include "jso_schema_error.h"
#include "jso_schema_keyword_freer.h"
#include "jso_schema_value.h"

#include "../jso.h"

jso_schema_value *jso_schema_value_alloc(jso_schema *schema, const char *type_name)
{
	jso_schema_value *value = jso_calloc(1, sizeof(jso_schema_value));
	if (value == NULL) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_ALLOC,
				"Allocating value for type %s failed", type_name);
		return NULL;
	}

	return value;
}

jso_schema_value_common *jso_schema_value_data_alloc(
		size_t value_size, jso_schema *schema, const char *type_name)
{
	jso_schema_value_common *value_data = jso_calloc(1, value_size);
	if (value_data == NULL) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALUE_ALLOC,
				"Allocating value data for type %s failed", type_name);
		return NULL;
	}

	return value_data;
}

static const char *type_names[] = { "mixed", "null", "boolean", "integer", "number", "string",
	"string", "array", "object", "boolean object" };

const char *jso_schema_value_type_to_string(jso_schema_value_type type)
{
	JSO_ASSERT_LT(type, sizeof(type_names) / sizeof(const char *));
	return type_names[(int) type];
}
