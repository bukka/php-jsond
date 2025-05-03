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
#include "jso_schema_validation_error.h"
#include "jso_schema_validation_object.h"
#include "jso_schema_validation_result.h"
#include "jso_schema_validation_stack.h"
#include "jso_schema_validation_string.h"

#include "jso_schema_error.h"
#include "jso_schema_keyword.h"

#include "jso_re.h"
#include "jso.h"

jso_schema_validation_result jso_schema_validation_object_key(jso_schema_validation_stack *stack,
		jso_schema_validation_position *pos, jso_virt_string *key)
{
	if (pos->is_final_validation_result) {
		return JSO_SCHEMA_VALIDATION_VALID;
	}

	jso_schema *schema = stack->root_schema;
	jso_schema_value *value = pos->current_value;
	jso_schema_value_object *objval = JSO_SCHEMA_VALUE_DATA_OBJ_P(value);

	if (JSO_SCHEMA_KW_IS_SET(objval->max_properties)) {
		jso_uint kw_uval = JSO_SCHEMA_KEYWORD_DATA_UINT(objval->max_properties);
		size_t objlen = pos->count;
		if (objlen > kw_uval) {
			jso_schema_validation_set_final_result(pos, JSO_SCHEMA_VALIDATION_INVALID);
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Object number of properties is %zu which is greater than maximum number of "
					"properties %lu",
					objlen, kw_uval);
			pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
			return JSO_SCHEMA_VALIDATION_INVALID;
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(objval->property_names)) {
		jso_schema_validation_position *top_pos = jso_schema_validation_stack_push_basic(
				stack, JSO_SCHEMA_KEYWORD_DATA_SCHEMA_OBJ(objval->property_names), pos);
		if (top_pos == NULL) {
			return JSO_SCHEMA_VALIDATION_ERROR;
		}
		// Iterate through positions to check composition
		jso_schema_validation_stack_layer_iterator iterator;
		jso_schema_validation_position *key_pos;
		jso_schema_validation_stack_layer_iterator_start(stack, &iterator);
		while ((key_pos = jso_schema_validation_stack_layer_iterator_next(stack, &iterator))) {
			if (jso_schema_validation_composition_check(stack, key_pos) == JSO_FAILURE) {
				return JSO_FAILURE;
			}
		}
		// Now the reverse iteration is done and each applicable value validated. The reverse order
		// is done so parent position is validate after children.
		jso_schema_validation_stack_layer_reverse_iterator_start(stack, &iterator);
		while ((key_pos
				= jso_schema_validation_stack_layer_reverse_iterator_next(stack, &iterator))) {
			if (!key_pos->is_final_validation_result
					&& key_pos->validation_result == JSO_SCHEMA_VALIDATION_VALID
					&& (key_pos->composition_type != JSO_SCHEMA_VALIDATION_COMPOSITION_ANY
							|| !key_pos->parent->any_of_valid)) {
				if (key_pos->current_value->type == JSO_SCHEMA_VALUE_TYPE_STRING) {
					key_pos->validation_result
							= jso_schema_validation_string_value_str(schema, key_pos, key);
					if (jso_schema_validation_stream_should_terminate(schema, key_pos)) {
						return JSO_FAILURE;
					}
				}
				jso_schema_validation_result_propagate(schema, key_pos);
			}
		}

		bool property_names_invalid = top_pos->validation_result != JSO_SCHEMA_VALIDATION_VALID;

		// Reset the layer to the last separator and push new separator to basically reset positions
		jso_schema_validation_stack_layer_remove(stack);
		if (jso_schema_validation_stack_push_separator(stack) == NULL) {
			return JSO_SCHEMA_VALIDATION_ERROR;
		}

		if (property_names_invalid) {
			jso_schema_validation_set_final_result(pos, JSO_SCHEMA_VALIDATION_INVALID);
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Object key %s does not validate against propertyNames schema",
					jso_virt_string_val(key));
			pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
			return JSO_SCHEMA_VALIDATION_INVALID;
		}
	}

	bool found = false;

	if (JSO_SCHEMA_KW_IS_SET(objval->properties)) {
		jso_value *result;
		jso_object *props = JSO_SCHEMA_KEYWORD_DATA_OBJ_SCHEMA_OBJ(objval->properties);
		if (jso_object_get_by_cstr_key(
					props, jso_virt_string_val(key), jso_virt_string_len(key), &result)
				== JSO_SUCCESS) {
			JSO_ASSERT_EQ(JSO_TYPE_P(result), JSO_TYPE_SCHEMA_VALUE);
			if (jso_schema_validation_stack_push_basic(stack, JSO_SVVAL_P(result), pos) == NULL) {
				return JSO_SCHEMA_VALIDATION_ERROR;
			}
			found = true;
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(objval->pattern_properties)) {
		jso_string *pkey;
		jso_value *pval;
		jso_object *pattern_props
				= JSO_SCHEMA_KEYWORD_DATA_OBJ_SCHEMA_OBJ(objval->pattern_properties);
		JSO_OBJECT_FOREACH(pattern_props, pkey, pval)
		{
			JSO_ASSERT_EQ(JSO_TYPE_P(pval), JSO_TYPE_SCHEMA_VALUE);
			jso_schema_value *schema_value = JSO_SVVAL_P(pval);
			jso_re_code *code = JSO_SCHEMA_VALUE_REGEXP_P(schema_value);
			JSO_ASSERT_NOT_NULL(code);
			jso_re_match_data *match_data = jso_re_match_data_create(code);
			int match_result = jso_re_match(
					jso_virt_string_val(key), jso_virt_string_len(key), code, match_data);
			jso_re_match_data_free(match_data);
			if (match_result > 0) {
				if (jso_schema_validation_stack_push_basic(stack, schema_value, pos) == NULL) {
					return JSO_SCHEMA_VALIDATION_ERROR;
				}
				found = true;
			}
		}
		JSO_OBJECT_FOREACH_END;
		JSO_USE(pkey);
	}

	if (!found && JSO_SCHEMA_KW_IS_SET(objval->additional_properties)) {
		if (JSO_SCHEMA_KEYWORD_TYPE(objval->additional_properties)
				== JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT) {
			if (jso_schema_validation_stack_push_basic(stack,
						JSO_SCHEMA_KEYWORD_DATA_SCHEMA_OBJ(objval->additional_properties), pos)
					== NULL) {
				return JSO_SCHEMA_VALIDATION_ERROR;
			}
		} else {
			JSO_ASSERT_EQ(JSO_SCHEMA_KEYWORD_TYPE(objval->additional_properties),
					JSO_SCHEMA_KEYWORD_TYPE_BOOLEAN);
			if (!JSO_SCHEMA_KEYWORD_DATA_BOOL(objval->additional_properties)) {
				jso_schema_validation_set_final_result(pos, JSO_SCHEMA_VALIDATION_INVALID);
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Object does not allow additional properties but added property with key "
						"%s which "
						"is is not found in properties or matches any pattern property",
						jso_virt_string_val(key));
				pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
				return JSO_SCHEMA_VALIDATION_INVALID;
			}
		}
	}

	return JSO_SCHEMA_VALIDATION_VALID;
}

jso_schema_validation_result jso_schema_validation_object_pre_value(
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos)
{
	jso_schema_value_object *objval = JSO_SCHEMA_VALUE_DATA_OBJ_P(pos->current_value);

	if (JSO_SCHEMA_KW_IS_SET(objval->dependencies)) {
		jso_string *key;
		jso_value *val;
		jso_object *dependencies = JSO_SCHEMA_KEYWORD_DATA_OBJ_SCHEMA_OBJ(objval->dependencies);
		jso_schema_validation_stack_mark(stack);
		JSO_OBJECT_FOREACH(dependencies, key, val)
		{
			JSO_USE(key);
			if (JSO_TYPE_P(val) == JSO_TYPE_SCHEMA_VALUE) {
				if (jso_schema_validation_stack_push_basic(stack, JSO_SVVAL_P(val), pos) == NULL) {
					return JSO_SCHEMA_VALIDATION_ERROR;
				}
			}
		}
		JSO_OBJECT_FOREACH_END;
	}

	return JSO_SCHEMA_VALIDATION_VALID;
}

jso_schema_validation_result jso_schema_validation_object_value(jso_schema *schema,
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos,
		jso_virt_value *instance)
{
	if (jso_virt_value_type(instance) != JSO_TYPE_OBJECT) {
		return jso_schema_validation_value_type_error(
				schema, pos, JSO_TYPE_OBJECT, jso_virt_value_type(instance));
	}

	jso_schema_value_object *objval = JSO_SCHEMA_VALUE_DATA_OBJ_P(pos->current_value);

	if (JSO_SCHEMA_KW_IS_SET(objval->dependencies)) {
		jso_string *key;
		jso_value *val;
		jso_object *dependencies = JSO_SCHEMA_KEYWORD_DATA_OBJ_SCHEMA_OBJ(objval->dependencies);
		jso_virt_object *instance_obj = jso_virt_value_object(instance);
		JSO_OBJECT_FOREACH(dependencies, key, val)
		{
			if (JSO_TYPE_P(val) == JSO_TYPE_ARRAY) {
				jso_value *item;
				JSO_ARRAY_FOREACH(JSO_ARRVAL_P(val), item)
				{
					JSO_ASSERT_EQ(JSO_TYPE_P(item), JSO_TYPE_STRING);
					if (!jso_virt_object_has_str_key(instance_obj, JSO_STR_P(item))) {
						jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
								"Object key %s is required by dependency %s but it is not present",
								JSO_SVAL_P(item), JSO_STRING_VAL(key));
						pos->validation_invalid_reason
								= JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
						return JSO_SCHEMA_VALIDATION_INVALID;
					}
				}
				JSO_ARRAY_FOREACH_END;
			}
		}
		JSO_OBJECT_FOREACH_END;
	}

	if (JSO_SCHEMA_KW_IS_SET(objval->min_properties)) {
		jso_uint kw_uval = JSO_SCHEMA_KEYWORD_DATA_UINT(objval->min_properties);
		size_t objlen = jso_virt_object_count(jso_virt_value_object(instance));
		if (objlen < kw_uval) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Object number of properties is %zu which is lower than minimum number of "
					"properties %lu",
					objlen, kw_uval);
			pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
			return JSO_SCHEMA_VALIDATION_INVALID;
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(objval->required)) {
		jso_value *item;
		jso_virt_object *instance_object = jso_virt_value_object(instance);
		JSO_ARRAY_FOREACH(JSO_SCHEMA_KEYWORD_DATA_ARR_STR(objval->required), item)
		{
			if (!jso_virt_object_has_str_key(instance_object, JSO_STR_P(item))) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Object does not have requier property with key %s", JSO_SVAL_P(item));
				pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
				return JSO_SCHEMA_VALIDATION_INVALID;
			}
		}
		JSO_ARRAY_FOREACH_END;
	}

	return JSO_SCHEMA_VALIDATION_VALID;
}
