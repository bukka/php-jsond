/*
 * Copyright (c) 2024 Jakub Zelenka. All rights reserved.
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

#include "jso_schema_error.h"
#include "jso_schema_reference.h"
#include "jso_schema_value.h"
#include "jso_schema_uri.h"

#include "jso_pointer.h"
#include "jso.h"

jso_schema_reference *jso_schema_reference_create(
		jso_schema *schema, jso_string *ref_uri, jso_schema_value *value)
{
	jso_schema_reference *ref = jso_calloc(1, sizeof(jso_schema_reference));
	if (ref == NULL) {
		jso_schema_error_format(
				schema, JSO_SCHEMA_ERROR_REFERENCE_ALLOC, "Allocation of schema reference failed");
		return NULL;
	}
	if (jso_schema_uri_set(schema, &ref->uri, &value->base_uri, ref_uri) == JSO_FAILURE) {
		jso_free(ref);
		return NULL;
	}
	ref->schema = schema;
	ref->parent = value;
	return ref;
}

void jso_schema_reference_free(jso_schema_reference *ref)
{
	if (ref == NULL) {
		return;
	}
	if (JSO_SCHEMA_REFERENCE_REFCOUNT(ref) > 0) {
		--JSO_SCHEMA_REFERENCE_REFCOUNT(ref);
	} else {
		jso_schema_uri_clear(&ref->uri);
		jso_free(ref);
	}
}

jso_rc jso_schema_reference_resolve(jso_schema_reference *ref, jso_schema_uri *base_uri,
		jso_schema_value *root_value, jso_value *doc)
{
	if (ref->result != NULL) {
		return JSO_SUCCESS;
	}

	jso_value *result;
	jso_schema *schema = ref->schema;
	jso_string *uri_str = JSO_SCHEMA_URI_STR(ref->uri);
	if (jso_ht_get(&schema->uri_deref_cache, uri_str, &result) == JSO_SUCCESS) {
		JSO_ASSERT_EQ(JSO_TYPE_SCHEMA_VALUE, JSO_TYPE_P(result));
		ref->result = JSO_SVVAL_P(result);
		return JSO_SUCCESS;
	}

	if (!jso_schema_uri_base_equal(base_uri, &ref->uri)) {
		// TODO: add support for external reference fetching (optional)
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_REFERENCE_EXTERNAL,
				"External references are not currently supported");
		return JSO_FAILURE;
	}

	ssize_t frag_start = JSO_SCHEMA_URI_FRAGMENT_START(ref->uri);
	// if fragment is not set or fragment is the last character of URI, use schema root value
	if (frag_start < 0 || frag_start + 1 == JSO_STRING_LEN(uri_str)) {
		if (root_value == NULL) {
			jso_schema_error_format(schema, JSO_SCHEMA_ERROR_REFERENCE_RECURSIVE,
					"Recursive references are not allowed for $ref");
			return JSO_FAILURE;
		}
		ref->result = root_value;
	}

	jso_string *jp_str = jso_string_substring(
			uri_str, frag_start + 1, JSO_STRING_LEN(uri_str) - frag_start - 1);
	if (jp_str == NULL) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_REFERENCE_ALLOC,
				"Reference string pointer allocation failed");
		return JSO_FAILURE;
	}
	jso_pointer *jp = jso_pointer_create(jp_str);
	jso_string_free(jp_str);
	if (jp == NULL) {
		jso_schema_error_format(
				schema, JSO_SCHEMA_ERROR_REFERENCE_ALLOC, "Reference pointer allocation failed");
		return JSO_FAILURE;
	}
	if (JSO_POINTER_ERROR_TYPE(jp) != JSO_POINTER_ERROR_NONE) {
		// TODO: proper pointer to reference mapping
		jso_schema_error_format(
				schema, JSO_SCHEMA_ERROR_REFERENCE_POINTER, JSO_POINTER_ERROR_MESSAGE(jp));
	}

	jso_rc jp_result = jso_pointer_resolve(jp, doc, &result);
	jso_pointer_free(jp);
	if (jp_result == JSO_FAILURE) {
		jso_schema_error_format(
				schema, JSO_SCHEMA_ERROR_REFERENCE_RESOLVE, "Reference pointer resolving failed");
		return JSO_FAILURE;
	}

	jso_schema_value *ref_schema_value = jso_schema_value_parse(schema, result, ref->parent);
	if (ref_schema_value == NULL) {
		return JSO_FAILURE;
	}

	jso_string *key = jso_string_copy(uri_str);
	jso_value ref_value;
	JSO_VALUE_SET_SCHEMA_VALUE(ref_value, ref_schema_value);
	if (jso_ht_set(&schema->uri_deref_cache, key, &ref_value, true) == JSO_FAILURE) {
		jso_string_free(key);
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_REFERENCE_ALLOC,
				"Reference cache entry allocation failed");
		return JSO_FAILURE;
	}

	ref->result = ref_schema_value;

	return JSO_SUCCESS;
}
