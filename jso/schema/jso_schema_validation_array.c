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

#include "jso_schema_validation_array.h"
#include "jso_schema_validation_composition.h"
#include "jso_schema_validation_error.h"
#include "jso_schema_validation_result.h"
#include "jso_schema_validation_stack.h"
#include "jso_schema_validation_value.h"

#include "jso_schema_array.h"
#include "jso_schema_error.h"
#include "jso_schema_keyword.h"

#include "../jso.h"

static inline jso_schema_validation_result jso_schema_validation_array_push_value(
		jso_schema_validation_stack *stack, jso_schema_value *current_value,
		jso_schema_validation_position *pos)
{
	return jso_schema_validation_stack_push_basic(stack, current_value, pos) == NULL
			? JSO_SCHEMA_VALIDATION_ERROR
			: JSO_SCHEMA_VALIDATION_VALID;
}

static jso_schema_value *jso_schema_validation_array_find_item(jso_schema_validation_stack *stack,
		jso_schema_validation_position *pos, jso_schema_value_array *arrval)
{
	if (JSO_SCHEMA_KEYWORD_TYPE(arrval->items) == JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT) {
		return JSO_SCHEMA_KEYWORD_DATA_SCHEMA_OBJ(arrval->items);
	}
	JSO_ASSERT_EQ(JSO_SCHEMA_KEYWORD_TYPE(arrval->items),
			JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS);
	jso_schema_array *items = JSO_SCHEMA_KEYWORD_DATA_ARR_SCHEMA_OBJ(arrval->items);
	size_t index = pos->count;
	return jso_schema_array_get(items, index);
}

jso_schema_validation_result jso_schema_validation_array_start(
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos)
{
	jso_schema_value *value = pos->current_value;
	jso_schema_value_array *arrval = JSO_SCHEMA_VALUE_DATA_ARR_P(value);

	if (pos->is_final_validation_result) {
		return JSO_SCHEMA_VALIDATION_VALID;
	}

	if (JSO_SCHEMA_KW_IS_SET(arrval->items)) {
		jso_schema_value *item = jso_schema_validation_array_find_item(stack, pos, arrval);
		if (item != NULL) {
			return jso_schema_validation_array_push_value(stack, item, pos);
		}
		if (JSO_SCHEMA_KW_IS_SET(arrval->additional_items)
				&& JSO_SCHEMA_KEYWORD_TYPE(arrval->additional_items)
						== JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT) {
			return jso_schema_validation_array_push_value(
					stack, JSO_SCHEMA_KEYWORD_DATA_SCHEMA_OBJ(arrval->additional_items), pos);
		}
	}

	return JSO_SCHEMA_VALIDATION_VALID;
}

jso_schema_validation_result jso_schema_validation_array_append(
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos)
{
	jso_schema *schema = stack->root_schema;
	jso_schema_value *value = pos->current_value;
	jso_schema_value_array *arrval = JSO_SCHEMA_VALUE_DATA_ARR_P(value);

	if (pos->is_final_validation_result) {
		return JSO_SCHEMA_VALIDATION_VALID;
	}

	if (JSO_SCHEMA_KW_IS_SET(arrval->max_items)) {
		jso_uint max_items = JSO_SCHEMA_KEYWORD_DATA_UINT(arrval->max_items);
		size_t arrlen = pos->count;
		if (arrlen > max_items) {
			jso_schema_validation_set_final_result(pos, JSO_SCHEMA_VALIDATION_INVALID);
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Array number of items is %zu which is greater than max number of items %lu",
					arrlen, max_items);
			pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
			return JSO_SCHEMA_VALIDATION_INVALID;
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(arrval->items)) {
		jso_schema_value *item = jso_schema_validation_array_find_item(stack, pos, arrval);
		if (item != NULL) {
			return jso_schema_validation_array_push_value(stack, item, pos);
		}
		if (JSO_SCHEMA_KW_IS_SET(arrval->additional_items)) {
			if (JSO_SCHEMA_KEYWORD_TYPE(arrval->additional_items)
					== JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT) {
				return jso_schema_validation_array_push_value(
						stack, JSO_SCHEMA_KEYWORD_DATA_SCHEMA_OBJ(arrval->additional_items), pos);
			}
			JSO_ASSERT_EQ(JSO_SCHEMA_KEYWORD_TYPE(arrval->additional_items),
					JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
			if (!JSO_SCHEMA_KEYWORD_DATA_BOOL(arrval->additional_items)) {
				jso_schema_array *items = JSO_SCHEMA_KEYWORD_DATA_ARR_SCHEMA_OBJ(arrval->items);
				JSO_ASSERT_GT(pos->count, 0);
				if (jso_schema_array_get(items, pos->count - 1) == NULL) {
					jso_schema_validation_set_final_result(pos, JSO_SCHEMA_VALIDATION_INVALID);
					jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
							"Array additional items are not allowed and number of items is lower");
					pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
					return JSO_SCHEMA_VALIDATION_INVALID;
				}
			}
		}
	}

	return JSO_SCHEMA_VALIDATION_VALID;
}

jso_schema_validation_result jso_schema_validation_array_value(jso_schema *schema,
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos,
		jso_virt_value *instance)
{
	if (jso_virt_value_type(instance) != JSO_TYPE_ARRAY) {
		return jso_schema_validation_value_type_error(
				schema, pos, JSO_TYPE_ARRAY, jso_virt_value_type(instance));
	}

	jso_schema_value_array *arrval = JSO_SCHEMA_VALUE_DATA_ARR_P(pos->current_value);

	if (JSO_SCHEMA_KW_IS_SET(arrval->min_items)) {
		jso_uint kw_uval = JSO_SCHEMA_KEYWORD_DATA_UINT(arrval->min_items);
		size_t arrlen = jso_virt_array_len(jso_virt_value_array(instance));
		if (arrlen < kw_uval) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Array number of items is %zu which is lower than minimum number of items %lu",
					arrlen, kw_uval);
			pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
			return JSO_SCHEMA_VALIDATION_INVALID;
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(arrval->unique_items)
			&& JSO_SCHEMA_KEYWORD_DATA_BOOL(arrval->unique_items)
			&& !jso_virt_array_is_unique(jso_virt_value_array(instance))) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD, "Array is not unique");
		pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
		return JSO_SCHEMA_VALIDATION_INVALID;
	}

	if (JSO_SCHEMA_KW_IS_SET(arrval->contains)) {
		if (jso_schema_validation_stack_push_separator(stack) == NULL) {
			return JSO_SCHEMA_VALIDATION_ERROR;
		}

		jso_schema_validation_position *top_pos = jso_schema_validation_stack_push_basic(
				stack, JSO_SCHEMA_KEYWORD_DATA_SCHEMA_OBJ(arrval->contains), pos);
		if (top_pos == NULL) {
			return JSO_SCHEMA_VALIDATION_ERROR;
		}
		// Iterate through positions to check composition
		jso_schema_validation_stack_layer_iterator iterator;
		jso_schema_validation_position *contains_pos;
		jso_schema_validation_stack_layer_iterator_start(stack, &iterator);
		while ((contains_pos = jso_schema_validation_stack_layer_iterator_next(stack, &iterator))) {
			if (jso_schema_validation_composition_check(stack, contains_pos) == JSO_FAILURE) {
				return JSO_FAILURE;
			}
		}

		jso_virt_value *instance_item;
		bool first_item = true;
		bool contains_item = false;
		JSO_VIRT_ARRAY_FOREACH(jso_virt_value_array(instance), instance_item)
		{
			if (first_item) {
				first_item = false;
			} else {
				// For the items after the first item, we need to reset all position
				jso_schema_validation_stack_layer_reset_positions(stack);
			}
			// Now the reverse iteration is done and each applicable value validated. The reverse
			// order is done so parent position is validate after children.
			jso_schema_validation_stack_layer_reverse_iterator_start(stack, &iterator);
			while ((contains_pos
					= jso_schema_validation_stack_layer_reverse_iterator_next(stack, &iterator))) {
				if (!contains_pos->is_final_validation_result
						&& contains_pos->validation_result == JSO_SCHEMA_VALIDATION_VALID
						&& (contains_pos->composition_type != JSO_SCHEMA_VALIDATION_COMPOSITION_ANY
								|| !contains_pos->parent->any_of_valid)) {
					contains_pos->validation_result = jso_schema_validation_value(
							schema, stack, contains_pos, instance_item);
					if (jso_schema_validation_stream_should_terminate(schema, contains_pos)) {
						return JSO_FAILURE;
					}
					jso_schema_validation_result_propagate(schema, contains_pos);
				}
			}

			contains_item = top_pos->validation_result == JSO_SCHEMA_VALIDATION_VALID;
			if (contains_item) {
				jso_schema_error_reset(schema);
				break;
			}
		}
		JSO_ARRAY_FOREACH_END;

		// Reset the layer to the last separator
		jso_schema_validation_stack_layer_remove(stack);

		if (!contains_item) {
			jso_schema_validation_set_final_result(pos, JSO_SCHEMA_VALIDATION_INVALID);
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Array does not contain item that validate against contains schema");
			pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
			return JSO_SCHEMA_VALIDATION_INVALID;
		}
	}

	return JSO_SCHEMA_VALIDATION_VALID;
}