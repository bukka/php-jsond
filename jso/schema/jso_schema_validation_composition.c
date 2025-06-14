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

#include "jso_schema_validation_composition.h"
#include "jso_schema_validation_stack.h"

#include "jso_schema_array.h"
#include "jso_schema_reference.h"

#include "../jso.h"

static jso_rc jso_schema_validation_composition_push_keyword_schema_objects_ex(
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos,
		jso_schema_keyword *keyword, jso_schema_validation_composition_type composition_type)
{
	if (!JSO_SCHEMA_KEYWORD_IS_PRESENT_P(keyword)) {
		return JSO_SUCCESS;
	}
	jso_schema_value *value;
	jso_schema_array *array = JSO_SCHEMA_KEYWORD_DATA_ARR_SCHEMA_OBJ_P(keyword);
	JSO_SCHEMA_ARRAY_FOREACH(array, value)
	{
		if (jso_schema_validation_stack_push_composed(stack, value, pos, composition_type)
				== NULL) {
			return JSO_FAILURE;
		}
	}
	JSO_SCHEMA_ARRAY_FOREACH_END;

	return JSO_SUCCESS;
}

static inline jso_rc jso_schema_validation_composition_push_keyword_schema_objects(
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos,
		jso_schema_keyword *keyword, jso_schema_validation_composition_type composition_type)
{
	return jso_schema_validation_composition_push_keyword_schema_objects_ex(
			stack, pos, keyword, composition_type);
}

static inline jso_rc jso_schema_validation_composition_push_keyword_schema_object(
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos,
		jso_schema_keyword *keyword, jso_schema_validation_composition_type composition_type)
{
	if (!JSO_SCHEMA_KEYWORD_IS_PRESENT_P(keyword)) {
		return JSO_SUCCESS;
	}

	jso_schema_value *value = JSO_SCHEMA_KEYWORD_DATA_SCHEMA_OBJ_P(keyword);
	if (jso_schema_validation_stack_push_composed(stack, value, pos, composition_type) == NULL) {
		return JSO_FAILURE;
	}

	return JSO_SUCCESS;
}

jso_schema_validation_result jso_schema_validation_composition_push(
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos)
{
	jso_schema_value *current_value = pos->current_value;
	jso_schema_value_common *data = JSO_SCHEMA_VALUE_DATA_COMMON_P(current_value);
	if (data == NULL) {
		JSO_ASSERT_EQ(JSO_SCHEMA_VALUE_TYPE_OBJECT_BOOLEAN, JSO_SCHEMA_VALUE_TYPE_P(current_value));
		return JSO_SCHEMA_VALIDATION_VALID;
	}

	jso_schema_reference *ref = JSO_SCHEMA_VALUE_REF_P(current_value);
	if (ref != NULL) {
		jso_schema_value *result = JSO_SCHEMA_REFERENCE_RESULT(ref);
		// result could be a NULL for $dynamicRef
		if (result == NULL) {
			jso_schema *schema = stack->root_schema;
			if (jso_schema_reference_resolve(
						ref, &schema->root->base_uri, schema->root, &schema->doc)
					== JSO_FAILURE) {
				return JSO_SCHEMA_VALIDATION_ERROR;
			}
			result = JSO_SCHEMA_REFERENCE_RESULT(ref);
			JSO_ASSERT_NOT_NULL(result);
		}
		if (jso_schema_validation_stack_push_composed(
					stack, result, pos, JSO_SCHEMA_VALIDATION_COMPOSITION_REF)
				== NULL) {
			return JSO_SCHEMA_VALIDATION_ERROR;
		}
		if (JSO_SCHEMA_VALUE_FLAGS_P(current_value) | JSO_SCHEMA_VALUE_FLAG_REF_ONLY) {
			return JSO_SCHEMA_VALIDATION_VALID;
		}
	}

	if (jso_schema_validation_composition_push_keyword_schema_objects(
				stack, pos, &data->type_any, JSO_SCHEMA_VALIDATION_COMPOSITION_TYPE_ANY)
			== JSO_FAILURE) {
		return JSO_SCHEMA_VALIDATION_ERROR;
	}
	if (jso_schema_validation_composition_push_keyword_schema_objects(
				stack, pos, &data->type_list, JSO_SCHEMA_VALIDATION_COMPOSITION_TYPE_LIST)
			== JSO_FAILURE) {
		return JSO_SCHEMA_VALIDATION_ERROR;
	}
	if (jso_schema_validation_composition_push_keyword_schema_objects(
				stack, pos, &data->all_of, JSO_SCHEMA_VALIDATION_COMPOSITION_ALL)
			== JSO_FAILURE) {
		return JSO_SCHEMA_VALIDATION_ERROR;
	}
	if (jso_schema_validation_composition_push_keyword_schema_objects(
				stack, pos, &data->any_of, JSO_SCHEMA_VALIDATION_COMPOSITION_ANY)
			== JSO_FAILURE) {
		return JSO_SCHEMA_VALIDATION_ERROR;
	}
	if (jso_schema_validation_composition_push_keyword_schema_objects(
				stack, pos, &data->one_of, JSO_SCHEMA_VALIDATION_COMPOSITION_ONE)
			== JSO_FAILURE) {
		return JSO_SCHEMA_VALIDATION_ERROR;
	}
	if (jso_schema_validation_composition_push_keyword_schema_object(
				stack, pos, &data->not, JSO_SCHEMA_VALIDATION_COMPOSITION_NOT)
			== JSO_FAILURE) {
		return JSO_SCHEMA_VALIDATION_ERROR;
	}

	return JSO_SCHEMA_VALIDATION_VALID;
}
