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

#include "jso_parser_hooks_decode.h"

#include "../jso.h"
#include "../jso_parser.h"

jso_error_type jso_parser_decode_schema_array_create(jso_parser *parser, jso_array **array)
{
	jso_array *arr = jso_array_alloc();
	if (arr == NULL) {
		return JSO_ERROR_ALLOC;
	}
	*array = arr;
	return JSO_ERROR_NONE;
}

jso_error_type jso_parser_decode_schema_array_append(
		jso_parser *parser, jso_array *array, jso_value *value)
{
	if (jso_schema_validation_stream_array_append(parser->schema_stream,
				jso_array_to_virt_array(array), jso_value_to_virt_value(value))
			== JSO_FAILURE) {
		return JSO_ERROR_SCHEMA;
	}

	return jso_array_append(array, value) == JSO_FAILURE ? JSO_ERROR_ALLOC : JSO_ERROR_NONE;
}

jso_error_type jso_parser_decode_schema_array_start(jso_parser *parser)
{
	return jso_schema_validation_stream_array_start(parser->schema_stream) == JSO_FAILURE
			? JSO_ERROR_SCHEMA
			: JSO_ERROR_NONE;
}

jso_error_type jso_parser_decode_schema_array_end(jso_parser *parser)
{
	return jso_schema_validation_stream_array_end(parser->schema_stream) == JSO_FAILURE
			? JSO_ERROR_SCHEMA
			: JSO_ERROR_NONE;
	;
}

jso_error_type jso_parser_decode_schema_object_create(jso_parser *parser, jso_object **object)
{
	jso_object *obj = jso_object_alloc();
	if (obj == NULL) {
		return JSO_ERROR_ALLOC;
	}
	*object = obj;
	return JSO_ERROR_NONE;
}

jso_error_type jso_parser_decode_schema_object_update(
		jso_parser *parser, jso_object *object, jso_string *key, jso_value *value)
{
	if (jso_schema_validation_stream_object_update(parser->schema_stream,
				jso_object_to_virt_object(object), jso_string_to_virt_string(key),
				jso_value_to_virt_value(value))
			== JSO_FAILURE) {
		return JSO_ERROR_SCHEMA;
	}

	return jso_object_add(object, key, value) == JSO_FAILURE ? JSO_ERROR_ALLOC : JSO_ERROR_NONE;
}

jso_error_type jso_parser_decode_schema_object_start(jso_parser *parser)
{
	return jso_schema_validation_stream_object_start(parser->schema_stream) == JSO_FAILURE
			? JSO_ERROR_SCHEMA
			: JSO_ERROR_NONE;
	;
}

jso_error_type jso_parser_decode_schema_object_end(jso_parser *parser)
{
	return jso_schema_validation_stream_object_end(parser->schema_stream) == JSO_FAILURE
			? JSO_ERROR_SCHEMA
			: JSO_ERROR_NONE;
	;
}

jso_error_type jso_parser_decode_schema_object_key(jso_parser *parser, jso_string *key)
{
	return jso_schema_validation_stream_object_key(
				   parser->schema_stream, jso_string_to_virt_string(key))
					== JSO_FAILURE
			? JSO_ERROR_SCHEMA
			: JSO_ERROR_NONE;
	;
}

jso_error_type jso_parser_decode_schema_value(jso_parser *parser, jso_value *value)
{
	return jso_schema_validation_stream_value(parser->schema_stream, jso_value_to_virt_value(value))
					== JSO_FAILURE
			? JSO_ERROR_SCHEMA
			: JSO_ERROR_NONE;
	;
}

static const jso_parser_hooks parser_hooks = {
	jso_parser_decode_schema_array_create,
	jso_parser_decode_schema_array_append,
	jso_parser_decode_schema_array_start,
	jso_parser_decode_schema_array_end,
	jso_parser_decode_schema_object_create,
	jso_parser_decode_schema_object_update,
	jso_parser_decode_schema_object_start,
	jso_parser_decode_schema_object_end,
	jso_parser_decode_schema_object_key,
	jso_parser_decode_schema_value,
};

JSO_API const jso_parser_hooks *jso_parser_hooks_decode_schema()
{
	return &parser_hooks;
}
