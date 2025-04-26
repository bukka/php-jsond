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
#include "jso_schema_uri.h"

#include "jso.h"

static void jso_schema_uri_parse_internal(jso_schema_uri *uri, jso_string *uri_value)
{
	ssize_t path_start;
	ssize_t colon_pos = jso_string_find_char_pos(uri_value, ':', 0);
	if (colon_pos > 0 && JSO_STRING_LEN(uri_value) > 4 && colon_pos < JSO_STRING_LEN(uri_value) - 3
			&& JSO_STRING_VAL(uri_value)[colon_pos + 1] == '/'
			&& JSO_STRING_VAL(uri_value)[colon_pos + 2] == '/') {
		uri->host_start = colon_pos + 3;
		path_start = jso_string_find_char_pos(uri_value, '/', 0);
	} else {
		uri->host_start = JSO_STRING_POS_NOT_FOUND;
		path_start = 0;
	}
	uri->path_start = path_start;
	uri->fragment_start = jso_string_find_char_pos(uri_value, '#', path_start > 0 ? path_start : 0);
}

// Parse URI - currently very lax (this will need to be made much stricker)
jso_rc jso_schema_uri_parse(jso_schema *schema, jso_schema_uri *uri, jso_string *uri_value)
{
	jso_schema_uri_parse_internal(uri, uri_value);
	uri->uri = jso_string_copy(uri_value);
	return JSO_SUCCESS;
}

static jso_rc jso_schema_uri_merge(jso_schema *schema, jso_schema_uri *current_uri,
		jso_schema_uri *parent_uri, size_t parent_uri_prefix_len)
{
	jso_string *concat_str = jso_string_concat_prefixes(parent_uri->uri, parent_uri_prefix_len,
			current_uri->uri, JSO_STRING_LEN(current_uri->uri));
	if (concat_str == NULL) {
		jso_schema_error_format(schema, JSO_SCHEMA_ERROR_URI_ALLOC,
				"Allocation of URI concatenation result failed");
		return JSO_FAILURE;
	}
	jso_string_free(current_uri->uri);
	jso_schema_uri_parse_internal(current_uri, concat_str);
	current_uri->uri = concat_str;

	return JSO_SUCCESS;
}

jso_rc jso_schema_uri_set(jso_schema *schema, jso_schema_uri *current_uri,
		jso_schema_uri *parent_uri, jso_string *new_uri)
{
	if (jso_schema_uri_parse(schema, current_uri, new_uri) == JSO_FAILURE) {
		return JSO_FAILURE;
	}
	// if there is no parent URI (root) or new_uri is absolute, we are done
	if (parent_uri == NULL || current_uri->host_start > 0) {
		return JSO_SUCCESS;
	}
	// if current URI is just fragment, we append it to parent uri (excluding its fragment if there
	// is any)
	if (current_uri->fragment_start == 0) {
		size_t parent_uri_prefix_len = parent_uri->fragment_start >= 0
				? parent_uri->fragment_start
				: JSO_STRING_LEN(parent_uri->uri);
		return jso_schema_uri_merge(schema, current_uri, parent_uri, parent_uri_prefix_len);
	}
	// at this stage there must be path set, otherwise something went wrong
	JSO_ASSERT_GE(current_uri->path_start, 0);
	// if the path is absolute, we use just host from parent URI and use the rest from currrent URI
	if (JSO_STRING_VAL(current_uri->uri)[current_uri->path_start] == '/') {
		size_t parent_uri_prefix_len = parent_uri->path_start >= 0
				? parent_uri->path_start
				: JSO_STRING_LEN(parent_uri->uri);
		return jso_schema_uri_merge(schema, current_uri, parent_uri, parent_uri_prefix_len);
	}
	// current URI path is relative
	// if there is no path for parent, we just merge the whole parent URI with the current URI
	if (parent_uri->path_start < 0) {
		return jso_schema_uri_merge(
				schema, current_uri, parent_uri, JSO_STRING_LEN(parent_uri->uri));
	}
	// now we have relative current URI and some parent URI so we find last slash and append current
	// URI to it
	size_t last_slash = parent_uri->path_start;
	for (ssize_t pos = last_slash; pos >= 0;
			pos = jso_string_find_char_pos(parent_uri->uri, '/', pos)) {
		last_slash = (size_t) pos;
	}
	return jso_schema_uri_merge(schema, current_uri, parent_uri, last_slash);
}

jso_rc jso_schema_uri_inherit(
		jso_schema *schema, jso_schema_uri *current_uri, jso_schema_uri *parent_uri)
{
	if (parent_uri->uri) {
		current_uri->uri = jso_string_copy(parent_uri->uri);
		current_uri->path_start = parent_uri->path_start;
		current_uri->fragment_start = parent_uri->fragment_start;
	}
	return JSO_SUCCESS;
}

void jso_schema_uri_clear(jso_schema_uri *uri)
{
	jso_string_free(uri->uri);
}

bool jso_schema_uri_base_equal(jso_schema_uri *base_uri, jso_schema_uri *current_uri)
{
	if (current_uri->fragment_start == 0) {
		return true;
	}
	if (base_uri->fragment_start == 0) {
		return false;
	}
	size_t base_prefix_len = base_uri->fragment_start > 0 ? base_uri->fragment_start
														  : JSO_STRING_LEN(base_uri->uri);
	size_t current_uri_len = current_uri->fragment_start > 0 ? current_uri->fragment_start
															 : JSO_STRING_LEN(current_uri->uri);
	if (base_prefix_len != current_uri_len) {
		return false;
	}

	return jso_string_prefix_equals(base_uri->uri, current_uri->uri, base_prefix_len);
}
