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

#include "jso_pointer_error.h"

#include "../jso.h"

#include <stdarg.h>
#include <stdio.h>

jso_rc jso_pointer_error_set(jso_pointer *pointer, jso_pointer_error_type type, const char *message)
{
	size_t message_len = strlen(message);
	char *new_message = jso_malloc(message_len + 1);
	if (new_message == NULL) {
		return JSO_FAILURE;
	}
	memcpy(new_message, message, message_len + 1);

	JSO_POINTER_ERROR_MESSAGE(pointer) = new_message;
	JSO_POINTER_ERROR_TYPE(pointer) = type;

	return JSO_SUCCESS;
}

jso_rc jso_pointer_error_format(
		jso_pointer *pointer, jso_pointer_error_type type, const char *format, ...)
{
	va_list args;
	char buf[JSO_POINTER_ERROR_FORMAT_SIZE + 1];

	va_start(args, format);
	int written = vsnprintf(buf, JSO_POINTER_ERROR_FORMAT_SIZE, format, args);
	va_end(args);

	if (written < 0) {
		return jso_pointer_error_set(pointer, type, "Error with incorrect format");
	}

	if (written == JSO_POINTER_ERROR_FORMAT_SIZE) {
		buf[JSO_POINTER_ERROR_FORMAT_SIZE] = '\0';
	}

	return jso_pointer_error_set(pointer, type, buf);
}

void jso_pointer_error_free(jso_pointer *pointer)
{
	if (pointer->error.message) {
		jso_free(pointer->error.message);
		pointer->error.message = NULL;
	}
	pointer->error.type = JSO_POINTER_ERROR_NONE;
}
