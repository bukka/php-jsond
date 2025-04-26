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

#include "jso_schema_keyword_freer.h"
#include "jso_schema_keyword_array.h"
#include "jso_schema_keyword_object.h"
#include "jso_schema_keyword_regexp.h"
#include "jso_schema_keyword_scalar.h"
#include "jso_schema_keyword.h"

#include "jso.h"

typedef void (*jso_schema_keyword_free_callback)(jso_schema_keyword *schema_keyword);

static const jso_schema_keyword_free_callback schema_keyword_free_callbacks[] = {
	[JSO_SCHEMA_KEYWORD_TYPE_ANY] = jso_schema_keyword_free_any,
	[JSO_SCHEMA_KEYWORD_TYPE_STRING] = jso_schema_keyword_free_string,
	[JSO_SCHEMA_KEYWORD_TYPE_REGEXP] = jso_schema_keyword_free_regexp,
	[JSO_SCHEMA_KEYWORD_TYPE_ARRAY] = jso_schema_keyword_free_array,
	[JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_STRINGS] = jso_schema_keyword_free_array_of_strings,
	[JSO_SCHEMA_KEYWORD_TYPE_ARRAY_OF_SCHEMA_OBJECTS]
	= jso_schema_keyword_free_array_of_schema_objects,
	[JSO_SCHEMA_KEYWORD_TYPE_OBJECT] = jso_schema_keyword_free_object,
	[JSO_SCHEMA_KEYWORD_TYPE_SCHEMA_OBJECT] = jso_schema_keyword_free_schema_object,
	[JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS]
	= jso_schema_keyword_free_object_of_schema_objects,
	[JSO_SCHEMA_KEYWORD_TYPE_OBJECT_OF_SCHEMA_OBJECTS_OR_ARRAY_OF_STRINGS]
	= jso_schema_keyword_free_object_of_schema_objects,
	[JSO_SCHEMA_KEYWORD_TYPE_REGEXP_OBJECT_OF_SCHEMA_OBJECTS]
	= jso_schema_keyword_free_object_of_schema_objects,
};

void jso_schema_keyword_free(jso_schema_keyword *keyword)
{
	if (!JSO_SCHEMA_KW_IS_SET_P(keyword)) {
		return;
	}

	jso_schema_keyword_free_callback callback
			= schema_keyword_free_callbacks[JSO_SCHEMA_KEYWORD_TYPE_P(keyword)];
	if (callback) {
		callback(keyword);
	}
}
