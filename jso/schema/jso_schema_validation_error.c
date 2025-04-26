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

#include "jso_schema_error.h"

#include "jso.h"

jso_schema_validation_result jso_schema_validation_value_type_error_ex(jso_schema *schema,
		jso_schema_validation_position *pos, jso_value_type expected,
		jso_value_type expected_alternative, jso_value_type actual)
{
	jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_TYPE,
			"Invalid validation type, expected %s or %s but received %s",
			jso_value_type_to_string(expected), jso_value_type_to_string(expected_alternative),
			jso_value_type_to_string(actual));
	pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_TYPE;
	return JSO_SCHEMA_VALIDATION_INVALID;
}

jso_schema_validation_result jso_schema_validation_value_type_error(jso_schema *schema,
		jso_schema_validation_position *pos, jso_value_type expected, jso_value_type actual)
{
	jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_TYPE,
			"Invalid validation type, expected %s but received %s",
			jso_value_type_to_string(expected), jso_value_type_to_string(actual));
	pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_TYPE;
	return JSO_SCHEMA_VALIDATION_INVALID;
}
