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

#include "jso_schema_validation_error.h"
#include "jso_schema_validation_scalar.h"

#include "jso_schema_error.h"
#include "jso_schema_keyword.h"
#include "jso_schema_value.h"

#include "jso.h"
#include "jso_number.h"

#include <math.h>

jso_schema_validation_result jso_schema_validation_null_value(jso_schema *schema,
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos,
		jso_virt_value *instance)
{
	if (jso_virt_value_type(instance) != JSO_TYPE_NULL) {
		return jso_schema_validation_value_type_error(
				schema, pos, JSO_TYPE_NULL, jso_virt_value_type(instance));
	}

	return JSO_SCHEMA_VALIDATION_VALID;
}

jso_schema_validation_result jso_schema_validation_boolean_value(jso_schema *schema,
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos,
		jso_value *instance)
{
	if (jso_virt_value_type(instance) != JSO_TYPE_BOOL) {
		return jso_schema_validation_value_type_error(
				schema, pos, JSO_TYPE_BOOL, jso_virt_value_type(instance));
	}

	return JSO_SCHEMA_VALIDATION_VALID;
}

jso_schema_validation_result jso_schema_validation_integer_value(jso_schema *schema,
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos,
		jso_value *instance)
{
	jso_int inst_ival;
	jso_value_type inst_type = jso_virt_value_type(instance);

	if (inst_type == JSO_TYPE_INT) {
		inst_ival = jso_virt_value_int(instance);
	} else if (inst_type == JSO_TYPE_DOUBLE) {
		if (nearbyint(jso_virt_value_double(instance)) != jso_virt_value_double(instance)) {
			jso_schema_error_set(schema, JSO_SCHEMA_ERROR_VALIDATION_TYPE,
					"Double integer type cannot have decimal point");
			pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_TYPE;
			return JSO_SCHEMA_VALIDATION_INVALID;
		}
		inst_ival = (jso_int) jso_virt_value_double(instance);
	} else {
		return jso_schema_validation_value_type_error_ex(
				schema, pos, JSO_TYPE_INT, JSO_TYPE_DOUBLE, jso_virt_value_type(instance));
	}

	jso_schema_value_integer *intval = JSO_SCHEMA_VALUE_DATA_INT_P(pos->current_value);

	if (schema->version >= JSO_SCHEMA_VERSION_DRAFT_06) {
		if (JSO_SCHEMA_KW_IS_SET(intval->minimum)) {
			jso_int kw_ival = JSO_SCHEMA_KEYWORD_DATA_INT(intval->minimum);
			if (inst_ival < kw_ival) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Value %ld is lower than minimum value %ld", inst_ival, kw_ival);
				pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
				return JSO_SCHEMA_VALIDATION_INVALID;
			}
		}
		if (JSO_SCHEMA_KW_IS_SET(intval->exclusive_minimum)) {
			jso_int kw_ival = JSO_SCHEMA_KEYWORD_DATA_INT(intval->exclusive_minimum);
			if (inst_ival <= kw_ival) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Value %ld is %s exclusive minimum value %ld", inst_ival,
						inst_ival == kw_ival ? "equal to" : "lower than", kw_ival);
				pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
				return JSO_SCHEMA_VALIDATION_INVALID;
			}
		}
		if (JSO_SCHEMA_KW_IS_SET(intval->maximum)) {
			jso_int kw_ival = JSO_SCHEMA_KEYWORD_DATA_INT(intval->maximum);
			if (inst_ival > kw_ival) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Value %ld is greater than maximum value %ld", inst_ival, kw_ival);
				pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
				return JSO_SCHEMA_VALIDATION_INVALID;
			}
		}
		if (JSO_SCHEMA_KW_IS_SET(intval->exclusive_maximum)) {
			jso_int kw_ival = JSO_SCHEMA_KEYWORD_DATA_INT(intval->exclusive_maximum);
			if (inst_ival >= kw_ival) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Value %ld is %s equal to exclusive maximum value %ld", inst_ival,
						inst_ival == kw_ival ? "equal to" : "greater than", kw_ival);
				pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
				return JSO_SCHEMA_VALIDATION_INVALID;
			}
		}
	} else { // Draft 4
		if (JSO_SCHEMA_KW_IS_SET(intval->minimum)) {
			jso_int kw_ival = JSO_SCHEMA_KEYWORD_DATA_INT(intval->minimum);
			if (inst_ival < kw_ival) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Value %ld is lower than minimum value %ld", inst_ival, kw_ival);
				pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
				return JSO_SCHEMA_VALIDATION_INVALID;
			}
			if (JSO_SCHEMA_KW_IS_SET(intval->exclusive_minimum)
					&& JSO_SCHEMA_KEYWORD_DATA_BOOL(intval->exclusive_minimum)
					&& inst_ival == kw_ival) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Value %ld is equal to exclusive minimum", inst_ival);
				pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
				return JSO_SCHEMA_VALIDATION_INVALID;
			}
		}

		if (JSO_SCHEMA_KW_IS_SET(intval->maximum)) {
			jso_int kw_ival = JSO_SCHEMA_KEYWORD_DATA_INT(intval->maximum);
			if (inst_ival > kw_ival) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Value %ld is greater than maximum value %ld", inst_ival, kw_ival);
				pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
				return JSO_SCHEMA_VALIDATION_INVALID;
			}
			if (JSO_SCHEMA_KW_IS_SET(intval->exclusive_maximum)
					&& JSO_SCHEMA_KEYWORD_DATA_BOOL(intval->exclusive_maximum)
					&& inst_ival == kw_ival) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Value %ld is equal to exclusive maximum", inst_ival);
				pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
				return JSO_SCHEMA_VALIDATION_INVALID;
			}
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(intval->multiple_of)) {
		jso_int kw_ival = JSO_SCHEMA_KEYWORD_DATA_INT(intval->multiple_of);
		if (inst_ival % kw_ival != 0) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Value %d is is not multiple of %d", inst_ival, kw_ival);
			pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
			return JSO_SCHEMA_VALIDATION_INVALID;
		}
	}

	return JSO_SCHEMA_VALIDATION_VALID;
}

jso_schema_validation_result jso_schema_validation_number_value(jso_schema *schema,
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos,
		jso_value *instance)
{
	jso_number inst_num;
	jso_value_type inst_type = jso_virt_value_type(instance);

	if (inst_type == JSO_TYPE_INT) {
		inst_num.ival = jso_virt_value_int(instance);
		inst_num.is_int = true;
	} else if (inst_type == JSO_TYPE_DOUBLE) {
		inst_num.dval = jso_virt_value_double(instance);
		inst_num.is_int = false;
	} else {
		return jso_schema_validation_value_type_error_ex(
				schema, pos, JSO_TYPE_INT, JSO_TYPE_DOUBLE, jso_virt_value_type(instance));
	}

	jso_schema_value_number *numval = JSO_SCHEMA_VALUE_DATA_NUM_P(pos->current_value);
	jso_number_string inst_num_str, kw_num_str;

	if (schema->version >= JSO_SCHEMA_VERSION_DRAFT_06) {
		if (JSO_SCHEMA_KW_IS_SET(numval->minimum)) {
			jso_number kw_num;
			JSO_ASSERT_EQ(
					jso_schema_keyword_convert_to_number(&numval->minimum, &kw_num), JSO_SUCCESS);
			if (jso_number_lt(&inst_num, &kw_num)) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Value %s is lower than minimum value %s",
						jso_number_cstr_from_number(&inst_num_str, &inst_num),
						jso_number_cstr_from_number(&kw_num_str, &kw_num));
				pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
				return JSO_SCHEMA_VALIDATION_INVALID;
			}
		}
		if (JSO_SCHEMA_KW_IS_SET(numval->exclusive_minimum)) {
			jso_number kw_num;
			JSO_ASSERT_EQ(jso_schema_keyword_convert_to_number(&numval->exclusive_minimum, &kw_num),
					JSO_SUCCESS);
			if (jso_number_le(&inst_num, &kw_num)) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Value %s is %s exclusive minimum value %s",
						jso_number_cstr_from_number(&inst_num_str, &inst_num),
						jso_number_eq(&inst_num, &kw_num) ? "equal to" : "lower than",
						jso_number_cstr_from_number(&kw_num_str, &kw_num));
				pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
				return JSO_SCHEMA_VALIDATION_INVALID;
			}
		}
		if (JSO_SCHEMA_KW_IS_SET(numval->maximum)) {
			jso_number kw_num;
			JSO_ASSERT_EQ(
					jso_schema_keyword_convert_to_number(&numval->maximum, &kw_num), JSO_SUCCESS);
			if (jso_number_gt(&inst_num, &kw_num)) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Value %s is greater than maximum value %s",
						jso_number_cstr_from_number(&inst_num_str, &inst_num),
						jso_number_cstr_from_number(&kw_num_str, &kw_num));
				pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
				return JSO_SCHEMA_VALIDATION_INVALID;
			}
		}
		if (JSO_SCHEMA_KW_IS_SET(numval->exclusive_maximum)) {
			jso_number kw_num;
			JSO_ASSERT_EQ(jso_schema_keyword_convert_to_number(&numval->exclusive_maximum, &kw_num),
					JSO_SUCCESS);
			if (jso_number_ge(&inst_num, &kw_num)) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Value %s is %s equal to exclusive maximum value %s",
						jso_number_cstr_from_number(&inst_num_str, &inst_num),
						jso_number_eq(&inst_num, &kw_num) ? "equal to" : "greater than",
						jso_number_cstr_from_number(&kw_num_str, &kw_num));
				pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
				return JSO_SCHEMA_VALIDATION_INVALID;
			}
		}
	} else {
		// Draft 4
		if (JSO_SCHEMA_KW_IS_SET(numval->minimum)) {
			jso_number kw_num;
			JSO_ASSERT_EQ(
					jso_schema_keyword_convert_to_number(&numval->minimum, &kw_num), JSO_SUCCESS);
			if (jso_number_lt(&inst_num, &kw_num)) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Value %s is lower than minimum value %s",
						jso_number_cstr_from_number(&inst_num_str, &inst_num),
						jso_number_cstr_from_number(&kw_num_str, &kw_num));
				pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
				return JSO_SCHEMA_VALIDATION_INVALID;
			}
			if (JSO_SCHEMA_KW_IS_SET(numval->exclusive_minimum)
					&& JSO_SCHEMA_KEYWORD_DATA_BOOL(numval->exclusive_minimum)
					&& jso_number_eq(&inst_num, &kw_num)) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Value %s is equal to exclusive minimum",
						jso_number_cstr_from_number(&inst_num_str, &inst_num));
				pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
				return JSO_SCHEMA_VALIDATION_INVALID;
			}
		}

		if (JSO_SCHEMA_KW_IS_SET(numval->maximum)) {
			jso_number kw_num;
			JSO_ASSERT_EQ(
					jso_schema_keyword_convert_to_number(&numval->maximum, &kw_num), JSO_SUCCESS);
			if (jso_number_gt(&inst_num, &kw_num)) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Value %s is greater than maximum value %s",
						jso_number_cstr_from_number(&inst_num_str, &inst_num),
						jso_number_cstr_from_number(&kw_num_str, &kw_num));
				pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
				return JSO_SCHEMA_VALIDATION_INVALID;
			}
			if (JSO_SCHEMA_KW_IS_SET(numval->exclusive_maximum)
					&& JSO_SCHEMA_KEYWORD_DATA_BOOL(numval->exclusive_maximum)
					&& jso_number_eq(&inst_num, &kw_num)) {
				jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
						"Value %s is equal to exclusive maximum",
						jso_number_cstr_from_number(&inst_num_str, &inst_num));
				pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
				return JSO_SCHEMA_VALIDATION_INVALID;
			}
		}
	}

	if (JSO_SCHEMA_KW_IS_SET(numval->multiple_of)) {
		jso_number kw_num;
		JSO_ASSERT_EQ(
				jso_schema_keyword_convert_to_number(&numval->multiple_of, &kw_num), JSO_SUCCESS);
		if (!jso_number_is_multiple_of(&inst_num, &kw_num)) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VALIDATION_KEYWORD,
					"Value %s is is not multiple of keyword value",
					jso_number_cstr_from_number(&inst_num_str, &inst_num));
			pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_KEYWORD;
			return JSO_SCHEMA_VALIDATION_INVALID;
		}
	}

	return JSO_SCHEMA_VALIDATION_VALID;
}
