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

#include "jso_schema_keyword_freer.h"
#include "jso_schema_reference.h"
#include "jso_schema_uri.h"

#include "jso.h"

typedef void (*jso_schema_value_free_callback)(jso_schema_value *val);

static void jso_schema_value_free_common(jso_schema_value *val)
{
	jso_schema_value_common *comval = JSO_SCHEMA_VALUE_DATA_COMMON_P(val);
	if (comval->re != NULL) {
		jso_re_code_free(comval->re);
	}
	jso_schema_keyword_free(&comval->id);
	jso_schema_keyword_free(&comval->ref);
	jso_schema_keyword_free(&comval->default_value);
	jso_schema_keyword_free(&comval->title);
	jso_schema_keyword_free(&comval->description);
	jso_schema_keyword_free(&comval->type_any);
	jso_schema_keyword_free(&comval->type_list);
	jso_schema_keyword_free(&comval->all_of);
	jso_schema_keyword_free(&comval->any_of);
	jso_schema_keyword_free(&comval->one_of);
	jso_schema_keyword_free(&comval->not );
	jso_schema_keyword_free(&comval->enum_elements);
	jso_schema_keyword_free(&comval->const_value);
	jso_schema_keyword_free(&comval->definitions);
}

static void jso_schema_value_free_null(jso_schema_value *val)
{
	jso_free(JSO_SCHEMA_VALUE_DATA_NULL_P(val));
	JSO_SCHEMA_VALUE_DATA_NULL_P(val) = NULL;
}

static void jso_schema_value_free_boolean(jso_schema_value *val)
{
	jso_free(JSO_SCHEMA_VALUE_DATA_BOOL_P(val));
	JSO_SCHEMA_VALUE_DATA_BOOL_P(val) = NULL;
}

static void jso_schema_value_free_integer(jso_schema_value *val)
{
	jso_schema_value_integer *intval = JSO_SCHEMA_VALUE_DATA_INT_P(val);
	jso_schema_keyword_free(&intval->exclusive_maximum);
	jso_schema_keyword_free(&intval->exclusive_minimum);
	jso_schema_keyword_free(&intval->maximum);
	jso_schema_keyword_free(&intval->minimum);
	jso_schema_keyword_free(&intval->multiple_of);
	jso_free(intval);
	JSO_SCHEMA_VALUE_DATA_INT_P(val) = NULL;
}

static void jso_schema_value_free_number(jso_schema_value *val)
{
	jso_schema_value_number *numval = JSO_SCHEMA_VALUE_DATA_NUM_P(val);
	jso_schema_keyword_free(&numval->exclusive_maximum);
	jso_schema_keyword_free(&numval->exclusive_minimum);
	jso_schema_keyword_free(&numval->maximum);
	jso_schema_keyword_free(&numval->minimum);
	jso_schema_keyword_free(&numval->multiple_of);
	jso_free(numval);
	JSO_SCHEMA_VALUE_DATA_NUM_P(val) = NULL;
}

static void jso_schema_value_free_string(jso_schema_value *val)
{
	jso_schema_value_string *strval = JSO_SCHEMA_VALUE_DATA_STR_P(val);
	jso_schema_keyword_free(&strval->min_length);
	jso_schema_keyword_free(&strval->max_length);
	jso_schema_keyword_free(&strval->pattern);
	jso_free(strval);
	JSO_SCHEMA_VALUE_DATA_STR_P(val) = NULL;
}

static void jso_schema_value_free_array(jso_schema_value *val)
{
	jso_schema_value_array *arrval = JSO_SCHEMA_VALUE_DATA_ARR_P(val);
	jso_schema_keyword_free(&arrval->additional_items);
	jso_schema_keyword_free(&arrval->items);
	jso_schema_keyword_free(&arrval->max_items);
	jso_schema_keyword_free(&arrval->min_items);
	jso_schema_keyword_free(&arrval->unique_items);
	jso_schema_keyword_free(&arrval->contains);
	jso_free(arrval);
	JSO_SCHEMA_VALUE_DATA_ARR_P(val) = NULL;
}

static void jso_schema_value_free_object(jso_schema_value *val)
{
	jso_schema_value_object *objval = JSO_SCHEMA_VALUE_DATA_OBJ_P(val);
	jso_schema_keyword_free(&objval->additional_properties);
	jso_schema_keyword_free(&objval->max_properties);
	jso_schema_keyword_free(&objval->min_properties);
	jso_schema_keyword_free(&objval->properties);
	jso_schema_keyword_free(&objval->required);
	jso_schema_keyword_free(&objval->pattern_properties);
	jso_schema_keyword_free(&objval->dependencies);
	jso_schema_keyword_free(&objval->property_names);
	jso_free(objval);
	JSO_SCHEMA_VALUE_DATA_OBJ_P(val) = NULL;
}

static const jso_schema_value_free_callback schema_value_free_callbacks[] = {
	[JSO_SCHEMA_VALUE_TYPE_NULL] = jso_schema_value_free_null,
	[JSO_SCHEMA_VALUE_TYPE_BOOLEAN] = jso_schema_value_free_boolean,
	[JSO_SCHEMA_VALUE_TYPE_INTEGER] = jso_schema_value_free_integer,
	[JSO_SCHEMA_VALUE_TYPE_NUMBER] = jso_schema_value_free_number,
	[JSO_SCHEMA_VALUE_TYPE_STRING] = jso_schema_value_free_string,
	[JSO_SCHEMA_VALUE_TYPE_ARRAY] = jso_schema_value_free_array,
	[JSO_SCHEMA_VALUE_TYPE_OBJECT] = jso_schema_value_free_object,
};

void jso_schema_value_clear(jso_schema_value *val)
{
	if (val == NULL || JSO_SCHEMA_VALUE_TYPE_P(val) == JSO_SCHEMA_VALUE_TYPE_OBJECT_BOOLEAN) {
		return;
	}
	jso_schema_reference_free(val->ref);
	jso_schema_uri_clear(&val->base_uri);
	jso_schema_value_free_common(val);
	if (JSO_SCHEMA_VALUE_TYPE_P(val) != JSO_SCHEMA_VALUE_TYPE_MIXED) {
		schema_value_free_callbacks[JSO_SCHEMA_VALUE_TYPE_P(val)](val);
		JSO_SCHEMA_VALUE_TYPE_P(val) = 0;
	} else {
		jso_free(JSO_SCHEMA_VALUE_DATA_COMMON_P(val));
	}
}

void jso_schema_value_free(jso_schema_value *val)
{
	jso_schema_value_clear(val);
	jso_free(val);
}
