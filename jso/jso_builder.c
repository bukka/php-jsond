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

#include "jso_builder.h"
#include "jso.h"

static inline jso_builder_value *jso_builder_alloc_value()
{
	return jso_calloc(1, sizeof(jso_builder_value));
}

static jso_rc jso_builder_start(jso_builder *builder)
{
	if (builder->current != NULL) {
		// it can be used only on the start
		return JSO_FAILURE;
	}

	if (builder->first == NULL) {
		builder->first = jso_builder_alloc_value();
		if (builder->first == NULL) {
			return JSO_FAILURE;
		}
	}
	builder->current = builder->first;

	return JSO_SUCCESS;
}

static jso_rc jso_builder_set_array(jso_builder *builder)
{
	jso_array *arr = jso_array_alloc();
	if (arr == NULL) {
		return JSO_FAILURE;
	}
	JSO_VALUE_SET_ARRAY(builder->current->val, arr);

	return JSO_SUCCESS;
}

static jso_rc jso_builder_set_object(jso_builder *builder)
{
	jso_object *obj = jso_object_alloc();
	if (obj == NULL) {
		return JSO_FAILURE;
	}
	JSO_VALUE_SET_OBJECT(builder->current->val, obj);

	return JSO_SUCCESS;
}

static inline bool jso_builder_value_is_not_a(jso_builder *builder, jso_value_type type)
{
	jso_builder_value *bv = builder->current;

	return bv == NULL || JSO_TYPE(bv->val) != type;
}

static inline bool jso_builder_value_is_not_array(jso_builder *builder)
{
	return jso_builder_value_is_not_a(builder, JSO_TYPE_ARRAY);
}

static inline bool jso_builder_value_is_not_object(jso_builder *builder)
{
	return jso_builder_value_is_not_a(builder, JSO_TYPE_OBJECT);
}

void jso_builder_init(jso_builder *builder)
{
	memset(builder, 0, sizeof(jso_builder));
}

void jso_builder_clear(jso_builder *builder)
{
	jso_builder_value *builder_value = builder->first;
	while (builder_value != NULL) {
		jso_builder_value *current_builder_value = builder_value;
		builder_value = builder_value->next;
		jso_free(current_builder_value);
	}
	jso_builder_init(builder);
}

void jso_builder_clear_all(jso_builder *builder)
{
	jso_value_free(jso_builder_get_value(builder));
	jso_builder_clear(builder);
}

jso_rc jso_builder_array_start(jso_builder *builder)
{
	if (jso_builder_start(builder) == JSO_FAILURE
			|| jso_builder_set_array(builder) == JSO_FAILURE) {
		return JSO_FAILURE;
	}

	return JSO_SUCCESS;
}

jso_rc jso_builder_array_add_null(jso_builder *builder)
{
	if (jso_builder_value_is_not_array(builder)) {
		return JSO_FAILURE;
	}

	jso_value val;
	JSO_VALUE_SET_NULL(val);

	return jso_array_append(JSO_ARRVAL(builder->current->val), &val);
}

jso_rc jso_builder_array_add_bool(jso_builder *builder, jso_bool value)
{
	if (jso_builder_value_is_not_array(builder)) {
		return JSO_FAILURE;
	}

	jso_value val;
	JSO_VALUE_SET_BOOL(val, value);

	return jso_array_append(JSO_ARRVAL(builder->current->val), &val);
}

jso_rc jso_builder_array_add_int(jso_builder *builder, jso_int value)
{
	if (jso_builder_value_is_not_array(builder)) {
		return JSO_FAILURE;
	}

	jso_value val;
	JSO_VALUE_SET_INT(val, value);

	return jso_array_append(JSO_ARRVAL(builder->current->val), &val);
}

jso_rc jso_builder_array_add_double(jso_builder *builder, jso_double value)
{
	if (jso_builder_value_is_not_array(builder)) {
		return JSO_FAILURE;
	}

	jso_value val;
	JSO_VALUE_SET_DOUBLE(val, value);

	return jso_array_append(JSO_ARRVAL(builder->current->val), &val);
}

jso_rc jso_builder_array_add_cstr(jso_builder *builder, const char *value)
{
	if (jso_builder_value_is_not_array(builder)) {
		return JSO_FAILURE;
	}

	jso_value val;
	jso_string *str = jso_string_create_from_cstr(value);
	JSO_VALUE_SET_STRING(val, str);

	return jso_array_append(JSO_ARRVAL(builder->current->val), &val);
}

jso_rc jso_builder_array_add_string(jso_builder *builder, jso_string *str)
{
	if (jso_builder_value_is_not_array(builder)) {
		return JSO_FAILURE;
	}

	jso_value val;
	JSO_VALUE_SET_STRING(val, str);

	return jso_array_append(JSO_ARRVAL(builder->current->val), &val);
}

static jso_rc jso_builder_move_next(jso_builder *builder)
{
	jso_builder_value *next = builder->current->next;
	if (next == NULL) {
		next = jso_builder_alloc_value();
		if (next == NULL) {
			return JSO_FAILURE;
		}
		next->prev = builder->current;
		builder->current->next = next;
	}

	builder->current = next;

	return JSO_SUCCESS;
}

jso_rc jso_builder_array_add_array_start(jso_builder *builder)
{
	if (jso_builder_value_is_not_array(builder) || jso_builder_move_next(builder) == JSO_FAILURE
			|| jso_builder_set_array(builder) == JSO_FAILURE) {
		return JSO_FAILURE;
	}

	return jso_array_append(JSO_ARRVAL(builder->current->prev->val), &builder->current->val);
}

jso_rc jso_builder_array_add_object_start(jso_builder *builder)
{
	if (jso_builder_value_is_not_array(builder) || jso_builder_move_next(builder) == JSO_FAILURE
			|| jso_builder_set_object(builder) == JSO_FAILURE) {
		return JSO_FAILURE;
	}

	return jso_array_append(JSO_ARRVAL(builder->current->prev->val), &builder->current->val);
}

jso_rc jso_builder_array_end(jso_builder *builder)
{
	if (jso_builder_value_is_not_array(builder) || builder->current == NULL) {
		return JSO_FAILURE;
	}

	builder->current = builder->current->prev;

	return JSO_SUCCESS;
}

jso_rc jso_builder_object_start(jso_builder *builder)
{
	if (jso_builder_start(builder) == JSO_FAILURE
			|| jso_builder_set_object(builder) == JSO_FAILURE) {
		return JSO_FAILURE;
	}

	return JSO_SUCCESS;
}

jso_rc jso_builder_object_add_null(jso_builder *builder, const char *key)
{
	if (jso_builder_value_is_not_object(builder)) {
		return JSO_FAILURE;
	}

	jso_string *skey = jso_string_create_from_cstr(key);
	jso_value val;
	JSO_VALUE_SET_NULL(val);

	return jso_object_add(JSO_OBJVAL(builder->current->val), skey, &val);
}

jso_rc jso_builder_object_add_bool(jso_builder *builder, const char *key, jso_bool value)
{
	if (jso_builder_value_is_not_object(builder)) {
		return JSO_FAILURE;
	}

	jso_string *skey = jso_string_create_from_cstr(key);
	jso_value val;
	JSO_VALUE_SET_BOOL(val, value);

	return jso_object_add(JSO_OBJVAL(builder->current->val), skey, &val);
}

jso_rc jso_builder_object_add_int(jso_builder *builder, const char *key, jso_int value)
{

	if (jso_builder_value_is_not_object(builder)) {
		return JSO_FAILURE;
	}

	jso_string *skey = jso_string_create_from_cstr(key);
	jso_value val;
	JSO_VALUE_SET_INT(val, value);

	return jso_object_add(JSO_OBJVAL(builder->current->val), skey, &val);
}

jso_rc jso_builder_object_add_double(jso_builder *builder, const char *key, jso_double value)
{
	if (jso_builder_value_is_not_object(builder)) {
		return JSO_FAILURE;
	}

	jso_string *skey = jso_string_create_from_cstr(key);
	jso_value val;
	JSO_VALUE_SET_DOUBLE(val, value);

	return jso_object_add(JSO_OBJVAL(builder->current->val), skey, &val);
}

jso_rc jso_builder_object_add_cstr(jso_builder *builder, const char *key, const char *value)
{
	if (jso_builder_value_is_not_object(builder)) {
		return JSO_FAILURE;
	}

	jso_string *skey = jso_string_create_from_cstr(key);
	jso_value val;
	JSO_VALUE_SET_STRING(val, jso_string_create_from_cstr(value));

	return jso_object_add(JSO_OBJVAL(builder->current->val), skey, &val);
}

jso_rc jso_builder_object_add_string(jso_builder *builder, const char *key, jso_string *str)
{
	if (jso_builder_value_is_not_object(builder)) {
		return JSO_FAILURE;
	}

	jso_string *skey = jso_string_create_from_cstr(key);
	jso_value val;
	JSO_VALUE_SET_STRING(val, str);

	return jso_object_add(JSO_OBJVAL(builder->current->val), skey, &val);
}

jso_rc jso_builder_object_add_array_start(jso_builder *builder, const char *key)
{
	if (jso_builder_value_is_not_object(builder) || jso_builder_move_next(builder) == JSO_FAILURE
			|| jso_builder_set_array(builder) == JSO_FAILURE) {
		return JSO_FAILURE;
	}

	jso_string *skey = jso_string_create_from_cstr(key);

	return jso_object_add(JSO_OBJVAL(builder->current->prev->val), skey, &builder->current->val);
}

jso_rc jso_builder_object_add_object_start(jso_builder *builder, const char *key)
{
	if (jso_builder_value_is_not_object(builder) || jso_builder_move_next(builder) == JSO_FAILURE
			|| jso_builder_set_object(builder) == JSO_FAILURE) {
		return JSO_FAILURE;
	}

	jso_string *skey = jso_string_create_from_cstr(key);

	return jso_object_add(JSO_OBJVAL(builder->current->prev->val), skey, &builder->current->val);
}

jso_rc jso_builder_object_end(jso_builder *builder)
{
	if (jso_builder_value_is_not_object(builder) || builder->current == NULL) {
		return JSO_FAILURE;
	}

	builder->current = builder->current->prev;

	return JSO_SUCCESS;
}
