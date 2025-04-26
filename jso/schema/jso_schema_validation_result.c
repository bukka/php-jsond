/*jso_schema_validation_value_callback
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

#include "jso_schema_validation_result.h"

#include "jso.h"
#include "jso_schema_error.h"

void jso_schema_validation_result_propagate(jso_schema *schema, jso_schema_validation_position *pos)
{
	jso_schema_validation_position *parent_pos = pos->parent;
	if (parent_pos == NULL) {
		return;
	}
	if (parent_pos->is_final_validation_result) {
		return;
	}
	if (pos->position_type == JSO_SCHEMA_VALIDATION_POSITION_BASIC) {
		if (pos->validation_result != JSO_SCHEMA_VALIDATION_VALID) {
			jso_schema_validation_set_final_result(parent_pos, pos->validation_result);
		}
	} else {
		JSO_ASSERT_EQ(pos->position_type, JSO_SCHEMA_VALIDATION_POSITION_COMPOSED);
		switch (pos->composition_type) {
			case JSO_SCHEMA_VALIDATION_COMPOSITION_REF:
				jso_schema_validation_set_final_result(parent_pos, pos->validation_result);
				break;
			case JSO_SCHEMA_VALIDATION_COMPOSITION_TYPE_ANY:
				// Typed composition ignores failures for invalid type because it is not applicable
				if (pos->validation_result != JSO_SCHEMA_VALIDATION_VALID) {
					if (pos->validation_invalid_reason
							== JSO_SCHEMA_VALIDATION_INVALID_REASON_TYPE) {
						jso_schema_error_reset(schema);
					} else {
						jso_schema_validation_set_final_result(parent_pos, pos->validation_result);
					}
				}
				break;
			case JSO_SCHEMA_VALIDATION_COMPOSITION_TYPE_LIST:
				// Typed composition ignores failures for invalid type because it is not applicable
				if (pos->validation_result != JSO_SCHEMA_VALIDATION_VALID) {
					if (pos->validation_invalid_reason
							== JSO_SCHEMA_VALIDATION_INVALID_REASON_TYPE) {
						jso_schema_error_reset(schema);
					} else {
						jso_schema_validation_set_final_result(parent_pos, pos->validation_result);
					}
				} else {
					parent_pos->type_valid = true;
				}
				break;
			case JSO_SCHEMA_VALIDATION_COMPOSITION_ALL:
				if (pos->validation_result != JSO_SCHEMA_VALIDATION_VALID) {
					jso_schema_validation_set_final_result(parent_pos, pos->validation_result);
				}
				break;
			case JSO_SCHEMA_VALIDATION_COMPOSITION_ANY:
				if (pos->validation_result == JSO_SCHEMA_VALIDATION_VALID) {
					parent_pos->any_of_valid = true;
				}
				break;
			case JSO_SCHEMA_VALIDATION_COMPOSITION_ONE:
				if (pos->validation_result == JSO_SCHEMA_VALIDATION_VALID) {
					if (parent_pos->one_of_valid) {
						jso_schema_error_set(schema, JSO_SCHEMA_ERROR_VALIDATION_COMPOSITION,
								"More than one oneOf subschema was valid");
						pos->validation_invalid_reason
								= JSO_SCHEMA_VALIDATION_INVALID_REASON_COMPOSITION;
						jso_schema_validation_set_final_result(
								parent_pos, JSO_SCHEMA_VALIDATION_INVALID);
					} else {
						parent_pos->one_of_valid = true;
					}
				}
				break;
			default:
				JSO_ASSERT_EQ(pos->composition_type, JSO_SCHEMA_VALIDATION_COMPOSITION_NOT);
				if (pos->validation_result == JSO_SCHEMA_VALIDATION_VALID) {
					jso_schema_error_set(schema, JSO_SCHEMA_ERROR_VALIDATION_COMPOSITION,
							"Negated valid validation");
					pos->validation_invalid_reason
							= JSO_SCHEMA_VALIDATION_INVALID_REASON_COMPOSITION;
					jso_schema_validation_set_final_result(
							parent_pos, JSO_SCHEMA_VALIDATION_INVALID);
				} else {
					jso_schema_error_reset(schema);
				}
				break;
		}
	}
}