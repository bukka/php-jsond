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

#include "../jso.h"

#include <stdarg.h>
#include <stdio.h>

jso_rc jso_schema_error_set(jso_schema *schema, jso_schema_error_type type, const char *message)
{
	size_t message_len = strlen(message) + 1;
	JSO_ASSERT_LE(message_len, JSO_SCHEMA_ERROR_FORMAT_SIZE);

	if (JSO_SCHEMA_ERROR_MESSAGE(schema) == NULL) {
		char *new_message = jso_malloc(JSO_SCHEMA_ERROR_FORMAT_SIZE + 1);
		if (new_message == NULL) {
			return JSO_FAILURE;
		}
		JSO_SCHEMA_ERROR_MESSAGE(schema) = new_message;
	}

	memcpy(JSO_SCHEMA_ERROR_MESSAGE(schema), message, message_len);
	JSO_SCHEMA_ERROR_TYPE(schema) = type;

	return JSO_SUCCESS;
}

jso_rc jso_schema_error_format(
		jso_schema *schema, jso_schema_error_type type, const char *format, ...)
{
	va_list args;
	char buf[JSO_SCHEMA_ERROR_FORMAT_SIZE + 1];

	va_start(args, format);
	int written = vsnprintf(buf, JSO_SCHEMA_ERROR_FORMAT_SIZE, format, args);
	va_end(args);

	if (written < 0) {
		return jso_schema_error_set(schema, type, "Error with incorrect format");
	}

	if (written == JSO_SCHEMA_ERROR_FORMAT_SIZE) {
		buf[JSO_SCHEMA_ERROR_FORMAT_SIZE] = '\0';
	}

	return jso_schema_error_set(schema, type, buf);
}

void jso_schema_error_clear(jso_schema_error *error)
{
	if (error->message) {
		jso_free(error->message);
		error->message = NULL;
	}
	error->type = JSO_SCHEMA_ERROR_NONE;
}

void jso_schema_error_free(jso_schema_error *error)
{
	jso_schema_error_clear(error);
	jso_free(error);
}

JSO_API jso_schema_error *jso_schema_move_error(jso_schema *schema)
{
	jso_schema_error *schema_error = JSO_SCHEMA_ERROR(schema);
	jso_schema_error *new_error = jso_malloc(sizeof(jso_schema_error));
	if (new_error == NULL) {
		return NULL;
	}
	*new_error = *schema_error;
	schema_error->message = NULL;
	schema_error->type = JSO_SCHEMA_ERROR_NONE;

	return new_error;
}
