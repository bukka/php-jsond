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

#include "../jso_schema.h"
#include "../jso.h"

jso_rc jso_schema_validate_instance(jso_schema_validation_stream *stream, jso_virt_value *instance)
{
	jso_virt_value *val;

	if (jso_virt_value_type(instance) == JSO_TYPE_ARRAY) {
		if (jso_schema_validation_stream_array_start(stream) == JSO_FAILURE) {
			return JSO_FAILURE;
		}
		jso_virt_array *array = jso_virt_value_array(instance);
		JSO_VIRT_ARRAY_FOREACH(array, val)
		{
			if (jso_schema_validate_instance(stream, val) == JSO_FAILURE) {
				return JSO_FAILURE;
			}
			if (jso_schema_validation_stream_array_append(stream, array, val) == JSO_FAILURE) {
				return JSO_FAILURE;
			}
		}
		JSO_VIRT_ARRAY_FOREACH_END;
		if (jso_schema_validation_stream_array_end(stream) == JSO_FAILURE) {
			return JSO_FAILURE;
		}
	} else if (jso_virt_value_type(instance) == JSO_TYPE_OBJECT) {
		jso_virt_string *key;
		if (jso_schema_validation_stream_object_start(stream) == JSO_FAILURE) {
			return JSO_FAILURE;
		}
		jso_virt_object *object = jso_virt_value_object(instance);
		JSO_VIRT_OBJECT_FOREACH(object, key, val)
		{
			if (jso_schema_validation_stream_object_key(stream, key) == JSO_FAILURE) {
				return JSO_FAILURE;
			}
			if (jso_schema_validate_instance(stream, val) == JSO_FAILURE) {
				return JSO_FAILURE;
			}
			if (jso_schema_validation_stream_object_update(stream, object, key, val)
					== JSO_FAILURE) {
				return JSO_FAILURE;
			}
		}
		JSO_VIRT_OBJECT_FOREACH_END;
		if (jso_schema_validation_stream_object_end(stream) == JSO_FAILURE) {
			return JSO_FAILURE;
		}
	}

	return jso_schema_validation_stream_value(stream, instance);
}

JSO_API jso_schema_validation_result jso_schema_validate(
		jso_schema *schema, jso_virt_value *instance)
{
	jso_schema_validation_result result;
	jso_schema_validation_stream stream;

	if (jso_schema_validation_stream_init(schema, &stream, 32) == JSO_FAILURE) {
		return JSO_SCHEMA_VALIDATION_ERROR;
	}

	if (jso_schema_validate_instance(&stream, instance) == JSO_FAILURE) {
		result = JSO_SCHEMA_VALIDATION_ERROR;
	} else {
		result = jso_schema_validation_stream_final_result(&stream);
	}

	jso_schema_validation_stream_clear(&stream);

	return result;
}