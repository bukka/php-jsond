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

/**
 * @file jso_builder.h
 * @brief JSO value builder for easier building of values.
 */

#ifndef JSO_BUILDER_H
#define JSO_BUILDER_H

#include "jso_types.h"

typedef struct _jso_builder_value jso_builder_value;

/**
 * @brief Builder value represents value in a cerain depth.
 */
struct _jso_builder_value {
	/** value holding the array or object */
	jso_value val;
	/** previous value on the depth - 1 */
	jso_builder_value *prev;
	/** next value on the depth + 1 */
	jso_builder_value *next;
};

/**
 * @brief Main structure holding the builder values
 */
typedef struct _jso_builder {
	/** current value */
	jso_builder_value *current;
	/** first value */
	jso_builder_value *first;
} jso_builder;

/**
 * Init builder.
 *
 * @param builder builder handle
 */
void jso_builder_init(jso_builder *builder);

/**
 * Clear and free all builder values.
 *
 * @param builder builder handle
 */
void jso_builder_clear(jso_builder *builder);

/**
 * Clear and free all builder values included the built value.
 *
 * @param builder builder handle
 */
void jso_builder_clear_all(jso_builder *builder);

/**
 * Start array.
 *
 * This can be called only after initializing the builder when no
 * array or object has been started.
 *
 * @param builder builder handle
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_array_start(jso_builder *builder);

/**
 * Add null value to the current value array.
 *
 * @param builder builder handle
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_array_add_null(jso_builder *builder);

/**
 * Add boolean value to the current value array.
 *
 * @param builder builder handle
 * @param value value to add
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_array_add_bool(jso_builder *builder, jso_bool value);

/**
 * Add integer value to the current value array.
 *
 * @param builder builder handle
 * @param value value to add
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_array_add_int(jso_builder *builder, jso_int value);

/**
 * Add double precision floating number value to the current value array.
 *
 * @param builder builder handle
 * @param value value to add
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_array_add_double(jso_builder *builder, jso_double value);

/**
 * Add C string value to the current value array.
 *
 * @param builder builder handle
 * @param value value to add
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_array_add_cstr(jso_builder *builder, const char *value);

/**
 * Add string value to the current value array.
 *
 * @param builder builder handle
 * @param str string to add
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_array_add_string(jso_builder *builder, jso_string *str);

/**
 * Add new empty array to the current array and the change current value to this array.
 *
 * This basically creates nested array and push it as a new current value in the linked list.
 * When @ref jso_builder_array_end is called, the previous current value is set back as a new
 * current value.
 *
 * @param builder builder handle
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_array_add_array_start(jso_builder *builder);

/**
 * Add new empty object to the current array and the change current value to this object.
 *
 * This basically creates nested object and push it as a new current value in the linked list.
 * When @ref jso_builder_array_end is called, the previous current value is set back as a new
 * current value.
 *
 * @param builder builder handle
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_array_add_object_start(jso_builder *builder);

/**
 * End the current value array and set the new current value to the previous value.
 *
 * @param builder builder handle
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_array_end(jso_builder *builder);

/**
 * Start object.
 *
 * This can be called only after initializing the builder when no object or object has been
 * started.
 *
 * @param builder builder handle
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_object_start(jso_builder *builder);

/**
 * Add null value to the current value object.
 *
 * @param builder builder handle
 * @param key object key
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_object_add_null(jso_builder *builder, const char *key);

/**
 * Add boolean value to the current value object.
 *
 * @param builder builder handle
 * @param key object key
 * @param value value to add
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_object_add_bool(jso_builder *builder, const char *key, jso_bool value);

/**
 * Add integer value to the current value object.
 *
 * @param builder builder handle
 * @param key object key
 * @param value value to add
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_object_add_int(jso_builder *builder, const char *key, jso_int value);

/**
 * Add double precision floating number value to the current value object.
 *
 * @param builder builder handle
 * @param key object key
 * @param value value to add
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_object_add_double(jso_builder *builder, const char *key, jso_double value);

/**
 * Add C string value to the current value object.
 *
 * @param builder builder handle
 * @param key object key
 * @param value value to add
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_object_add_cstr(jso_builder *builder, const char *key, const char *value);

/**
 * Add string value to the current value object.
 *
 * @param builder builder handle
 * @param key object key
 * @param str string to add
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_object_add_string(jso_builder *builder, const char *key, jso_string *str);

/**
 * Add new empty array to the current object and the change current value to this array.
 *
 * This basically creates nested array and push it as a new current value in the linked list.
 * When @ref jso_builder_array_end is called, the previous current value is set back as a new
 * current value.
 *
 * @param builder builder handle
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_object_add_array_start(jso_builder *builder, const char *key);

/**
 * Add new empty object to the current object and the change current value to this object.
 *
 * This basically creates nested object and push it as a new current value in the linked list.
 * When @ref jso_builder_array_end is called, the previous current value is set back as a new
 * current value.
 *
 * @param builder builder handle
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_object_add_object_start(jso_builder *builder, const char *key);

/**
 * End the current value object and set the new current value to the previous value.
 *
 * @param builder builder handle
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
jso_rc jso_builder_object_end(jso_builder *builder);

/**
 * Get the current value.
 *
 * @param builder builder handle
 * @return value of type @ref jso_value* on success, otherwise NULL
 */
static inline jso_value *jso_builder_get_value(jso_builder *builder)
{
	return builder->first == NULL ? NULL : &builder->first->val;
}

#endif /* JSO_BUILDER_H */
