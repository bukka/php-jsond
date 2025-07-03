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

#include "jso_schema_data.h"
#include "jso_schema_error.h"
#include "jso_schema_value.h"
#include "jso_schema_version.h"

#include "../jso_schema.h"

#include "../jso.h"

JSO_API jso_schema *jso_schema_alloc()
{
	return jso_calloc(1, sizeof(jso_schema));
}

JSO_API void jso_schema_init(jso_schema *schema)
{
	memset(schema, 0, sizeof(jso_schema));
}

JSO_API void jso_schema_options_init(jso_schema_options *options)
{
	memset(options, 0, sizeof(jso_schema_options));
}

JSO_API jso_rc jso_schema_parse_ex(
		jso_schema *schema, jso_value *data, const jso_schema_options *options)
{
	if (JSO_TYPE_P(data) != JSO_TYPE_OBJECT) {
		jso_schema_error_set(
				schema, JSO_SCHEMA_ERROR_ROOT_DATA_TYPE, "Root data type is not an object");
		return JSO_FAILURE;
	}

	if (jso_schema_version_set(schema, data, options->default_version) == JSO_FAILURE) {
		return JSO_FAILURE;
	}

	// Save document
	JSO_VALUE_SET_OBJECT(schema->doc, jso_object_copy(JSO_OBJVAL_P(data)));

	// Parse root value.
	jso_schema_value *root = jso_schema_value_parse(schema, data, NULL);
	if (root == NULL) {
		return JSO_FAILURE;
	}
	schema->root = root;

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_schema_parse(jso_schema *schema, jso_value *data)
{
	jso_schema_options options;
	jso_schema_options_init(&options);

	return jso_schema_parse_ex(schema, data, &options);
}

static inline void jso_schema_empty(jso_schema *schema)
{
	jso_schema_value_free(schema->root);
	jso_value_free(&schema->doc);
	jso_schema_error_clear(&schema->error);
	jso_ht_clear(&schema->uri_deref_cache);
}

JSO_API void jso_schema_clear(jso_schema *schema)
{
	jso_schema_empty(schema);
	jso_schema_init(schema);
}

JSO_API void jso_schema_free(jso_schema *schema)
{
	if (schema->is_initialized) {
		jso_schema_empty(schema);
	}
	jso_free(schema);
}
