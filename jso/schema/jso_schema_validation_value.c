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

#include "jso_schema_error.h"
#include "jso_schema_validation_error.h"
#include "jso_schema_validation_array.h"
#include "jso_schema_validation_common.h"
#include "jso_schema_validation_object.h"
#include "jso_schema_validation_scalar.h"
#include "jso_schema_validation_string.h"
#include "jso_schema_validation_value.h"

#include "jso.h"

static const jso_schema_validation_value_callback schema_validation_value_callbacks[] = {
	[JSO_SCHEMA_VALUE_TYPE_NULL] = jso_schema_validation_null_value,
	[JSO_SCHEMA_VALUE_TYPE_BOOLEAN] = jso_schema_validation_boolean_value,
	[JSO_SCHEMA_VALUE_TYPE_INTEGER] = jso_schema_validation_integer_value,
	[JSO_SCHEMA_VALUE_TYPE_NUMBER] = jso_schema_validation_number_value,
	[JSO_SCHEMA_VALUE_TYPE_STRING] = jso_schema_validation_string_value,
	[JSO_SCHEMA_VALUE_TYPE_ARRAY] = jso_schema_validation_array_value,
	[JSO_SCHEMA_VALUE_TYPE_OBJECT] = jso_schema_validation_object_value,
};

jso_schema_validation_result jso_schema_validation_value(jso_schema *schema,
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos,
		jso_virt_value *instance)
{
	jso_schema_value *value = pos->current_value;
	jso_schema_value_type value_type = JSO_SCHEMA_VALUE_TYPE_P(value);

	if (value_type == JSO_SCHEMA_VALUE_TYPE_OBJECT_BOOLEAN) {
		if (JSO_SCHEMA_VALUE_FLAGS_P(value) & JSO_SCHEMA_VALUE_FLAG_OBJECT_TRUE) {
			return JSO_SCHEMA_VALIDATION_VALID;
		}
		jso_schema_error_set(
				schema, JSO_SCHEMA_ERROR_VALIDATION_FALSE, "Schema value is always invalid");
		pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_VALUE;
		return JSO_SCHEMA_VALIDATION_INVALID;
	}

	jso_schema_validation_result result
			= jso_schema_validation_common_value(schema, pos, value, instance);
	if (result != JSO_SCHEMA_VALIDATION_VALID) {
		return result;
	}

	if (value_type == JSO_SCHEMA_VALUE_TYPE_MIXED) {
		return JSO_SCHEMA_VALIDATION_VALID;
	}

	return schema_validation_value_callbacks[value_type](schema, stack, pos, instance);
}
