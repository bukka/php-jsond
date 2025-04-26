/*
 * Copyright (c) 2012-2023 Jakub Zelenka. All rights reserved.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "jso.h"
#include "jso_io.h"
#include "jso_object.h"
#include "jso_encoder.h"

#define JSO_INT_BUFFER_SIZE 32

static inline jso_rc jso_encoder_output_jstr(jso_encoder *encoder, jso_ctype *str, size_t len)
{
	return JSO_IO_WRITE(encoder->output, str, len) == len ? JSO_SUCCESS : JSO_FAILURE;
}

static inline jso_rc jso_encoder_output_cstr(jso_encoder *encoder, const char *str, size_t len)
{
	return jso_encoder_output_jstr(encoder, (jso_ctype *) str, len);
}

static inline jso_rc jso_encoder_output_cchar(jso_encoder *encoder, char c)
{
	return jso_encoder_output_cstr(encoder, &c, 1);
}

static inline jso_rc jso_encoder_pretty_print_indent(jso_encoder *encoder)
{
	jso_int i;

	if (!encoder->options.pretty) {
		return JSO_SUCCESS;
	}

	for (i = 0; i < encoder->depth; i++) {
		if (jso_encoder_output_cstr(
					encoder, JSO_ENCODER_PRETTY_INDENT, sizeof(JSO_ENCODER_PRETTY_INDENT) - 1)
				== JSO_FAILURE) {
			return JSO_FAILURE;
		}
	}

	return JSO_SUCCESS;
}

static inline jso_rc jso_encoder_pretty_print_elsep(jso_encoder *encoder)
{
	jso_rc rc;

	if (!encoder->options.pretty) {
		return JSO_SUCCESS;
	}

	rc = jso_encoder_output_cstr(
			encoder, JSO_ENCODER_PRETTY_ELSEP, sizeof(JSO_ENCODER_PRETTY_ELSEP) - 1);

	if (rc == JSO_SUCCESS && JSO_ENCODER_PRETTY_ELSEP[0] == '\n') {
		return jso_encoder_pretty_print_indent(encoder);
	}
	return rc;
}

static inline jso_rc jso_encoder_pretty_print_kvsep(jso_encoder *encoder)
{
	if (!encoder->options.pretty) {
		return JSO_SUCCESS;
	}

	return jso_encoder_output_cstr(
			encoder, JSO_ENCODER_PRETTY_KVSEP, sizeof(JSO_ENCODER_PRETTY_KVSEP) - 1);
}

static jso_rc jso_encoder_encode_int(jso_encoder *encoder, jso_int val)
{
	jso_ctype str[JSO_INT_BUFFER_SIZE];
	jso_ctype *p = &str[JSO_INT_BUFFER_SIZE];
	jso_bool negative;
	size_t len;

	if (val < 0) {
		negative = JSO_TRUE;
		val = -val;
	} else {
		negative = JSO_FALSE;
	}

	do {
		*--p = (jso_ctype) (val % 10) + '0';
		val /= 10;
	} while (val > 0);

	if (negative) {
		*--p = '-';
	}

	len = (size_t) (&str[JSO_INT_BUFFER_SIZE] - p);
	return jso_encoder_output_jstr(encoder, p, len);
}

static jso_rc jso_encoder_encode_double(jso_encoder *encoder, jso_double val)
{
	char buf[128], *p;
	int len;
	/* this is noop as jso_double is always double */
	double dv = (double) val;

	if (isnan(dv) || isinf(dv)) {
		return JSO_FAILURE;
	}

	len = snprintf(buf, sizeof(buf), "%.17g", dv);

	p = strchr(buf, ',');
	if (p) {
		*p = '.';
	}

	return jso_encoder_output_cstr(encoder, buf, len);
}

static jso_rc jso_encoder_encode_string(jso_encoder *encoder, jso_string *str)
{
	size_t pos;
	char c;

	jso_encoder_output_cchar(encoder, '"');
	for (pos = 0; pos < JSO_STRING_LEN(str); pos++) {
		c = JSO_STRING_VAL(str)[pos];

		switch (c) {
			case '"':
				jso_encoder_output_cstr(encoder, "\\\"", 2);
				break;

			case '\\':
				jso_encoder_output_cstr(encoder, "\\\\", 2);
				break;

			case '/':
				jso_encoder_output_cstr(encoder, "\\/", 2);
				break;

			case '\b':
				jso_encoder_output_cstr(encoder, "\\b", 2);
				break;

			case '\f':
				jso_encoder_output_cstr(encoder, "\\f", 2);
				break;

			case '\n':
				jso_encoder_output_cstr(encoder, "\\n", 2);
				break;

			case '\r':
				jso_encoder_output_cstr(encoder, "\\r", 2);
				break;

			case '\t':
				jso_encoder_output_cstr(encoder, "\\t", 2);
				break;

			default:
				jso_encoder_output_cchar(encoder, c);
		}
	}

	return jso_encoder_output_cchar(encoder, '"');
}

static jso_rc jso_encoder_encode_array(jso_encoder *encoder, jso_array *arr)
{
	jso_value *val;
	jso_bool is_first = JSO_TRUE;

	jso_encoder_output_cchar(encoder, '[');

	encoder->depth++;

	JSO_ARRAY_FOREACH(arr, val)
	{
		if (is_first) {
			is_first = JSO_FALSE;
		} else {
			jso_encoder_output_cstr(encoder, ",", 1);
		}
		jso_encoder_pretty_print_elsep(encoder);
		jso_encoder_encode(encoder, val);
	}
	JSO_ARRAY_FOREACH_END;

	encoder->depth--;

	if (!is_first) {
		jso_encoder_pretty_print_elsep(encoder);
	}

	jso_encoder_output_cchar(encoder, ']');

	return JSO_SUCCESS;
}

static jso_rc jso_encoder_encode_object(jso_encoder *encoder, jso_object *obj)
{
	jso_string *key;
	jso_value *val;
	jso_bool is_first = JSO_TRUE;

	jso_encoder_output_cchar(encoder, '{');

	encoder->depth++;

	JSO_OBJECT_FOREACH(obj, key, val)
	{
		if (is_first) {
			is_first = JSO_FALSE;
		} else {
			jso_encoder_output_cstr(encoder, ",", 1);
		}
		jso_encoder_pretty_print_elsep(encoder);
		jso_encoder_encode_string(encoder, key);
		jso_encoder_output_cstr(encoder, ":", 1);
		jso_encoder_pretty_print_kvsep(encoder);
		jso_encoder_encode(encoder, val);
	}
	JSO_OBJECT_FOREACH_END;

	encoder->depth--;

	if (!is_first) {
		jso_encoder_pretty_print_elsep(encoder);
	}

	jso_encoder_output_cchar(encoder, '}');

	return JSO_SUCCESS;
}

JSO_API void jso_encoder_init(jso_encoder *encoder, jso_io *output, jso_encoder_options *options)
{
	encoder->output = output;
	memcpy(&encoder->options, options, sizeof(jso_encoder_options));
	encoder->depth = 0;
}

JSO_API jso_rc jso_encoder_encode(jso_encoder *encoder, jso_value *val)
{
	switch (JSO_TYPE_P(val)) {
		case JSO_TYPE_NULL:
			return jso_encoder_output_cstr(encoder, "null", sizeof("null") - 1);
		case JSO_TYPE_BOOL:
			if (JSO_IVAL_P(val)) {
				return jso_encoder_output_cstr(encoder, "true", sizeof("true") - 1);
			}
			return jso_encoder_output_cstr(encoder, "false", sizeof("false") - 1);
		case JSO_TYPE_INT:
			return jso_encoder_encode_int(encoder, JSO_IVAL_P(val));
		case JSO_TYPE_DOUBLE:
			return jso_encoder_encode_double(encoder, JSO_DVAL_P(val));
		case JSO_TYPE_STRING:
			return jso_encoder_encode_string(encoder, JSO_STR_P(val));
		case JSO_TYPE_ARRAY:
			return jso_encoder_encode_array(encoder, JSO_ARRVAL_P(val));
		case JSO_TYPE_OBJECT:
			return jso_encoder_encode_object(encoder, JSO_OBJVAL_P(val));
		default:
			/* error */
			return JSO_FAILURE;
	}
}

JSO_API jso_rc jso_encode(jso_value *val, jso_io *output, jso_encoder_options *options)
{
	jso_encoder encoder;

	jso_encoder_init(&encoder, output, options);
	return jso_encoder_encode(&encoder, val);
}
