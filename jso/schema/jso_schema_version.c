/*
 * Copyright (c) 2023 Jakub Zelenka. All rights reserved.
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
#include "jso_schema_version.h"

#include "../jso.h"

jso_rc jso_schema_version_set(
		jso_schema *schema, jso_value *data, jso_schema_version default_version)
{
	jso_string *version = jso_schema_data_get_str(schema, data, "$schema");

	if (version == NULL) {
		if (default_version == JSO_SCHEMA_VERSION_NONE) {
			return jso_schema_error_set(schema, JSO_SCHEMA_ERROR_VERSION,
					"The $schema field is omitted and no default version is set");
		}
		schema->version = default_version;
		return JSO_SUCCESS;
	}
	if (jso_string_equals_to_cstr(version, JSO_SCHEMA_VERSION_IDENTIFIER_DRAFT_04)) {
		schema->version = JSO_SCHEMA_VERSION_DRAFT_04;
		return JSO_SUCCESS;
	}
	if (jso_string_equals_to_cstr(version, JSO_SCHEMA_VERSION_IDENTIFIER_DRAFT_06)) {
		schema->version = JSO_SCHEMA_VERSION_DRAFT_06;
		return JSO_SUCCESS;
	}
	if (jso_string_equals_to_cstr(version, JSO_SCHEMA_VERSION_IDENTIFIER_DRAFT_07)) {
		return jso_schema_error_set(
				schema, JSO_SCHEMA_ERROR_VERSION, "Draft 7 is not yet supported");
	}
	if (jso_string_equals_to_cstr(version, JSO_SCHEMA_VERSION_IDENTIFIER_DRAFT_2019_09)) {
		return jso_schema_error_set(
				schema, JSO_SCHEMA_ERROR_VERSION, "Draft 2019-09 is not yet supported");
	}
	if (jso_string_equals_to_cstr(version, JSO_SCHEMA_VERSION_IDENTIFIER_DRAFT_2020_12)) {
		return jso_schema_error_set(
				schema, JSO_SCHEMA_ERROR_VERSION, "Draft 2020-12 is not yet supported");
	}
	if (jso_string_equals_to_cstr(version, JSO_SCHEMA_VERSION_IDENTIFIER_DEPRECATED_LATEST)) {
		return jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VERSION,
				"The identified %s is not supported as it is deprecated by spec",
				JSO_SCHEMA_VERSION_IDENTIFIER_DEPRECATED_LATEST);
	}

	return jso_schema_error_format(schema, JSO_SCHEMA_ERROR_VERSION,
			"Unknown $schema %s, only drafts 4 and 6 are currently supported",
			JSO_STRING_VAL(version));
}
