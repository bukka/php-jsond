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

#include "jso_schema_array.h"
#include "jso_schema_data.h"
#include "jso_schema_keyword.h"
#include "jso_schema_value.h"
#include "jso_schema_reference.h"
#include "jso_schema_uri.h"

#include "jso.h"

jso_schema_value *jso_schema_value_init(jso_schema *schema, jso_value *data,
		jso_schema_value *parent, const char *type_name, size_t value_size,
		jso_schema_value_type value_type, jso_bool init_keywords)
{
	jso_schema_value *value = jso_schema_value_alloc(schema, type_name);
	if (value == NULL) {
		return NULL;
	}

	jso_schema_value_common *value_data
			= jso_schema_value_data_alloc(value_size, schema, type_name);
	if (value_data == NULL) {
		jso_schema_value_free(value);
		return NULL;
	}
	value_data->parent = parent;

	JSO_SCHEMA_VALUE_DATA_COMMON_P(value) = value_data;
	JSO_SCHEMA_VALUE_TYPE_P(value) = value_type;

	if (init_keywords) {
		// set default keyword
		JSO_SCHEMA_KW_SET_ANY_EX(schema, data, default, value, value_data, default_value);

		// set metadata keywords
		JSO_SCHEMA_KW_SET_STR(schema, data, description, value, value_data);
		JSO_SCHEMA_KW_SET_STR(schema, data, title, value, value_data);

		// id keyword
		if (schema->version == JSO_SCHEMA_VERSION_DRAFT_04) {
			JSO_SCHEMA_KW_SET_STR(schema, data, id, value, value_data);
		} else {
			JSO_SCHEMA_KW_SET_STR_EX(schema, data, $id, value, value_data, id);
		}
		if (JSO_SCHEMA_KW_IS_SET(value_data->id)) {
			if (jso_schema_uri_set(schema, &value->base_uri,
						parent == NULL ? NULL : &parent->base_uri,
						JSO_SCHEMA_KEYWORD_DATA_STR(value_data->id))
					== JSO_FAILURE) {
				return NULL;
			}
		} else if (parent != NULL
				&& jso_schema_uri_inherit(schema, &value->base_uri, &parent->base_uri)
						== JSO_FAILURE) {
			return NULL;
		}

		// reference
		JSO_SCHEMA_KW_SET_STR_EX(schema, data, $ref, value, value_data, ref);
		if (JSO_SCHEMA_KW_IS_SET(value_data->ref)) {
			jso_schema_reference *ref = jso_schema_reference_create(
					schema, JSO_SCHEMA_KEYWORD_DATA_STR(value_data->ref), value);
			if (ref == NULL) {
				return NULL;
			}
			if (jso_schema_reference_resolve(ref, &value->base_uri, NULL, &schema->doc)
					== JSO_FAILURE) {
				jso_schema_reference_free(ref);
				return NULL;
			}
			value->ref = ref;
			if (JSO_OBJECT_COUNT(JSO_OBJVAL_P(data)) == 1) {
				value->flags |= JSO_SCHEMA_VALUE_FLAG_REF_ONLY;
				return value;
			}
		}

		// set other common keywords
		JSO_SCHEMA_KW_SET_WITH_FLAGS_EX(schema, data, enum, value, value_data, enum_elements,
				TYPE_ARRAY, JSO_SCHEMA_KEYWORD_FLAG_UNIQUE);
		if (schema->version >= JSO_SCHEMA_VERSION_DRAFT_06) {
			JSO_SCHEMA_KW_SET_ANY_EX(schema, data, const, value, value_data, const_value);
		}
		JSO_SCHEMA_KW_SET_ARR_OF_SCHEMA_OBJS_NE_EX(schema, data, allOf, value, value_data, all_of);
		JSO_SCHEMA_KW_SET_ARR_OF_SCHEMA_OBJS_NE_EX(schema, data, anyOf, value, value_data, any_of);
		JSO_SCHEMA_KW_SET_ARR_OF_SCHEMA_OBJS_NE_EX(schema, data, oneOf, value, value_data, one_of);
		JSO_SCHEMA_KW_SET_SCHEMA_OBJ(schema, data, not, value, value_data);
		JSO_SCHEMA_KW_SET_OBJ_OF_SCHEMA_OBJS(schema, data, definitions, value, value_data);
	}

	return value;
}
