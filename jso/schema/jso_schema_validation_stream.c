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

#include "jso.h"

#include "jso_schema.h"

#include "jso_schema_validation_composition.h"
#include "jso_schema_validation_array.h"
#include "jso_schema_validation_error.h"
#include "jso_schema_validation_object.h"
#include "jso_schema_validation_result.h"
#include "jso_schema_validation_stack.h"
#include "jso_schema_validation_stream.h"
#include "jso_schema_validation_value.h"

JSO_API void jso_schema_validation_stream_clear(jso_schema_validation_stream *stream)
{
	jso_schema_validation_stack_clear(&stream->stack);
}

JSO_API jso_rc jso_schema_validation_stream_init(
		jso_schema *schema, jso_schema_validation_stream *stream, size_t stack_capacity)
{
	JSO_ASSERT_GE(stack_capacity, 1);
	jso_schema_validation_stack *stack = JSO_STREAM_VALIDATION_STREAM_STACK_P(stream);

	// Initialize validation stack
	if (jso_schema_validation_stack_init(schema, stack, stack_capacity) == JSO_FAILURE) {
		return JSO_FAILURE;
	}
	// Root element needs to be always pushed.
	if (jso_schema_validation_stack_push_basic(stack, schema->root, NULL) == NULL) {
		jso_schema_validation_stream_clear(stream);
		return JSO_FAILURE;
	}

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_object_start(jso_schema_validation_stream *stream)
{
	jso_schema_validation_stack_layer_iterator iterator;
	jso_schema_validation_position *pos;
	jso_schema_validation_stack *stack = JSO_STREAM_VALIDATION_STREAM_STACK_P(stream);
	jso_schema *schema = stack->root_schema;

	// Iterate to do a composition check and initial type check.
	jso_schema_validation_stack_layer_iterator_start(stack, &iterator);
	while ((pos = jso_schema_validation_stack_layer_iterator_next(stack, &iterator))) {
		jso_schema_value *value = pos->current_value;
		if (jso_schema_value_is_type_of(value, JSO_SCHEMA_VALUE_TYPE_OBJECT)) {
			if (jso_schema_validation_composition_check(stack, pos) == JSO_FAILURE) {
				return JSO_FAILURE;
			}
		} else {
			pos->validation_result = jso_schema_validation_value_type_error(
					schema, pos, JSO_SCHEMA_VALUE_TYPE_P(value), JSO_SCHEMA_VALUE_TYPE_OBJECT);
		}
	}

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_object_key(
		jso_schema_validation_stream *stream, jso_virt_string *key)
{
	jso_schema_validation_stack_layer_iterator iterator;
	jso_schema_validation_position *pos;
	jso_schema_validation_stack *stack = JSO_STREAM_VALIDATION_STREAM_STACK_P(stream);
	jso_schema *schema = stack->root_schema;

	// Start parent iteration.
	jso_schema_validation_stack_layer_iterator_start(stack, &iterator);
	// Push the stack so schemas for the key property is pushed to the new layer.
	if (jso_schema_validation_stack_push_separator(stack) == NULL) {
		return JSO_FAILURE;
	}
	// Iterate through all positions.
	while ((pos = jso_schema_validation_stack_layer_iterator_next(stack, &iterator))) {
		// Keep count of object elements.
		++pos->count;
		if (pos->validation_result == JSO_SCHEMA_VALIDATION_VALID
				&& JSO_SCHEMA_VALUE_TYPE_P(pos->current_value) == JSO_SCHEMA_VALUE_TYPE_OBJECT) {
			// Validate object key for valid object schemas - add schema for properties and others
			pos->validation_result = jso_schema_validation_object_key(stack, pos, key);
			if (jso_schema_validation_stream_should_terminate(schema, pos)) {
				return JSO_FAILURE;
			}
		}
		// Remember the current position key.
		pos->object_key = key;
	}

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_object_update(jso_schema_validation_stream *stream,
		jso_virt_object *instance_object, jso_virt_string *instance_key,
		jso_virt_value *instance_item)
{
	// Currently there is nothing to do.
	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_object_end(jso_schema_validation_stream *stream)
{
	// Currently there is nothing to do.
	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_array_start(jso_schema_validation_stream *stream)
{
	jso_schema_validation_stack_layer_iterator iterator;
	jso_schema_validation_position *pos;
	jso_schema_validation_stack *stack = JSO_STREAM_VALIDATION_STREAM_STACK_P(stream);
	jso_schema *schema = stack->root_schema;

	// Iterate to do a composition check and initial type check.
	jso_schema_validation_stack_layer_iterator_start(stack, &iterator);
	while ((pos = jso_schema_validation_stack_layer_iterator_next(stack, &iterator))) {
		jso_schema_value *value = pos->current_value;
		if (jso_schema_value_is_type_of(value, JSO_SCHEMA_VALUE_TYPE_ARRAY)) {
			if (jso_schema_validation_composition_check(stack, pos) == JSO_FAILURE) {
				return JSO_FAILURE;
			}
		} else {
			pos->validation_result = jso_schema_validation_value_type_error(
					schema, pos, JSO_SCHEMA_VALUE_TYPE_P(value), JSO_SCHEMA_VALUE_TYPE_ARRAY);
		}
	}
	// Now the reverse iteration is done to propagate result
	jso_schema_validation_stack_layer_reverse_iterator_start(stack, &iterator);
	while ((pos = jso_schema_validation_stack_layer_reverse_iterator_next(stack, &iterator))) {
		jso_schema_validation_result_propagate(schema, pos);
	}

	// Start next iteration round in the parent.
	jso_schema_validation_stack_layer_iterator_start(stack, &iterator);
	// Push separator so the schema for the item at the current index is added to the new layer.
	if (jso_schema_validation_stack_push_separator(stack) == NULL) {
		return JSO_FAILURE;
	}
	// Iterate to add array start positions.
	while ((pos = jso_schema_validation_stack_layer_iterator_next(stack, &iterator))) {
		if (pos->validation_result == JSO_SCHEMA_VALIDATION_VALID) {
			jso_schema_value *value = pos->current_value;
			if (JSO_SCHEMA_VALUE_TYPE_P(value) == JSO_SCHEMA_VALUE_TYPE_ARRAY) {
				// It adds position for the first item in the array except other things.
				jso_schema_validation_result result = jso_schema_validation_array_start(stack, pos);
				if (result == JSO_SCHEMA_VALIDATION_ERROR) {
					return JSO_FAILURE;
				}
			}
		}
	}

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_array_append(jso_schema_validation_stream *stream,
		jso_virt_array *instance_array, jso_virt_value *instance_item)
{
	jso_schema_validation_stack_layer_iterator iterator;
	jso_schema_validation_position *pos;
	jso_schema_validation_stack *stack = JSO_STREAM_VALIDATION_STREAM_STACK_P(stream);

	// Start iteration round in the parent.
	jso_schema_validation_stack_layer_iterator_start(stack, &iterator);
	// Push separator so the schema for the item at the current index is added to the new layer.
	if (jso_schema_validation_stack_push_separator(stack) == NULL) {
		return JSO_FAILURE;
	}
	jso_schema *schema = stack->root_schema;
	while ((pos = jso_schema_validation_stack_layer_iterator_next(stack, &iterator))) {
		// Count number of items.
		++pos->count;
		// The array append is called only for valid array schema values, and it adds schema for the
		// next item.
		if (JSO_SCHEMA_VALUE_TYPE_P(pos->current_value) == JSO_SCHEMA_VALUE_TYPE_ARRAY
				&& jso_schema_validation_array_append(stack, pos) != JSO_SCHEMA_VALIDATION_VALID) {
			if (jso_schema_validation_stream_should_terminate(schema, pos)) {
				return JSO_FAILURE;
			}
			jso_schema_validation_result_propagate(schema, pos);
		}
	}

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_array_end(jso_schema_validation_stream *stream)
{
	// Remove layer for previously added schema in the last append.
	jso_schema_validation_stack_layer_remove(JSO_STREAM_VALIDATION_STREAM_STACK_P(stream));
	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_validation_stream_value(
		jso_schema_validation_stream *stream, jso_virt_value *instance)
{
	jso_schema_validation_stack_layer_iterator iterator;
	jso_schema_validation_position *pos;
	jso_schema_validation_stack *stack = JSO_STREAM_VALIDATION_STREAM_STACK_P(stream);
	jso_schema *schema = stack->root_schema;

	jso_value_type instance_type = jso_virt_value_type(instance);
	// Array has already added composition during array start so skip it.
	if (instance_type != JSO_TYPE_ARRAY) {
		// Iterate through positions to check composition for all types except array and object
		jso_schema_validation_stack_layer_iterator_start(stack, &iterator);
		while ((pos = jso_schema_validation_stack_layer_iterator_next(stack, &iterator))) {
			if (pos->validation_result == JSO_SCHEMA_VALIDATION_VALID) {
				if (instance_type == JSO_TYPE_OBJECT) {
					// For object, do the pre validation that adds dependency schemas
					if (JSO_SCHEMA_VALUE_TYPE_P(pos->current_value) == JSO_SCHEMA_VALUE_TYPE_OBJECT
							&& jso_schema_validation_object_pre_value(stack, pos)
									!= JSO_SCHEMA_VALIDATION_VALID) {
						if (jso_schema_validation_stream_should_terminate(schema, pos)) {
							return JSO_FAILURE;
						}
						jso_schema_validation_result_propagate(schema, pos);
					}
				} else if (jso_schema_validation_composition_check(stack, pos) == JSO_FAILURE) {
					return JSO_FAILURE;
				}
			}
		}
	}
	// Now the reverse iteration is done and each applicable value validated. The reverse order
	// is done so parent position is validate after children.
	jso_schema_validation_stack_layer_reverse_iterator_start(stack, &iterator);
	while ((pos = jso_schema_validation_stack_layer_reverse_iterator_next(stack, &iterator))) {
		if (!pos->is_final_validation_result
				&& pos->validation_result == JSO_SCHEMA_VALIDATION_VALID
				&& (pos->composition_type != JSO_SCHEMA_VALIDATION_COMPOSITION_ANY
						|| !pos->parent->any_of_valid)) {
			pos->validation_result = jso_schema_validation_value(schema, stack, pos, instance);
			if (jso_schema_validation_stream_should_terminate(schema, pos)) {
				return JSO_FAILURE;
			}
		}
		jso_schema_validation_result_propagate(schema, pos);
	}
	// Reset the layer to the last separator.
	jso_schema_validation_stack_layer_remove(stack);

	return JSO_SUCCESS;
}

JSO_API jso_schema_validation_result jso_schema_validation_stream_final_result(
		jso_schema_validation_stream *stream)
{
	jso_schema_validation_position *pos = jso_schema_validation_stack_root_position(
			JSO_STREAM_VALIDATION_STREAM_STACK_P(stream));

	return pos->validation_result;
}