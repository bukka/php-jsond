/*
 * Copyright (c) 2012-2023 Jakub Zelenka. All rights reserved.
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
 * @file jso_object.h
 * @brief Object header
 */

#ifndef JSO_OBJECT_H
#define JSO_OBJECT_H

#include "jso_types.h"
#include "jso_ht.h"

/**
 * @brief Object structure.
 */
struct _jso_object {
	/** embedded hash table */
	jso_ht ht;
	/* object reference count */
	jso_uint16 refcount;
	/* object flags */
	jso_uint16 flags;
};

/**
 * Get object items count.
 *
 * @param _obj object
 * @return Count of the object items.
 */
#define JSO_OBJECT_COUNT(_obj) (_obj)->ht.count

/**
 * Get object items capacity.
 *
 * @param _obj object
 * @return Capacity of all object items.
 */
#define JSO_OBJECT_CAPACITY(_obj) (_obj)->ht.capacity

/**
 * Get object underlaying hash table.
 *
 * @param _obj object
 * @return Hash table.
 */
#define JSO_OBJECT_HT(_obj) &(_obj)->ht

/**
 * Get object flags of the supplied object.
 *
 * @param _obj pointer to @ref jso_object
 * @return String flags.
 */
#define JSO_OBJECT_FLAGS(_obj) (_obj)->flags

/**
 * Get reference count of the supplied object.
 *
 * @param _obj pointer to @ref jso_object
 * @return References count value.
 */
#define JSO_OBJECT_REFCOUNT(_obj) (_obj)->refcount

/**
 * @brief Object iteration function callback.
 */
typedef void (*jso_object_callback)(jso_string *key, jso_value *val);

/**
 * @brief Object iteration function callback with argument.
 */
typedef void (*jso_object_with_arg_callback)(jso_string *key, jso_value *val, void *arg);

/**
 * Allocate object.
 *
 * @return New initialized object or NULL on error.
 */
JSO_API jso_object *jso_object_alloc();

/**
 * Inititalize object.
 *
 * @param obj object
 */
JSO_API void jso_object_init(jso_object *obj);

/**
 * Free object.
 *
 * @param obj object
 */
JSO_API void jso_object_free(jso_object *obj);

/**
 * Resize object.
 *
 * @param obj object
 * @param size new object size
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_object_resize(jso_object *obj, size_t size);

/**
 * Add value with a key to the object.
 *
 * @param obj object
 * @param key key of the value
 * @param val value to add
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_object_add(jso_object *obj, jso_string *key, jso_value *val);

/**
 * Get value with the supplied key in the object.
 *
 * @param obj object
 * @param key key of the value
 * @param val pointer to set with the returned value
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_object_get(jso_object *obj, jso_string *key, jso_value **val);

/**
 * Get value with the supplied C string key in the object.
 *
 * @param obj object
 * @param key key of the value
 * @param key_len key length
 * @param val pointer to set with the returned value
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_object_get_by_cstr_key(
		jso_object *obj, const char *key, size_t key_len, jso_value **val);

/**
 * Check whether key exist in the object.
 *
 * @param obj object
 * @param key key of the value
 * @return @ref JSO_TRUE if key exists, otherwise @ref JSO_FALSE.
 */
JSO_API jso_bool jso_object_has(jso_object *obj, jso_string *key);

/**
 * Call callback for each value in the object.
 *
 * @param obj object
 * @param cbk callback function
 */
JSO_API void jso_object_apply(jso_object *obj, jso_object_callback cbk);

/**
 * Call callback with extra argument for each value in the object.
 *
 * @param obj object
 * @param cbk callback function
 * @param arg callback argument
 */
JSO_API void jso_object_apply_with_arg(
		jso_object *obj, jso_object_with_arg_callback cbk, void *arg);

/**
 * Check whether two objects are equal.
 *
 * @param obj1 first object
 * @param obj2 second object
 * @return @ref JSO_TRUE if the objects are equal, otherwise @ref JSO_FALSE.
 */
JSO_API jso_bool jso_object_equals(jso_object *obj1, jso_object *obj2);

/**
 * Copy object.
 *
 * @param obj object to copy
 * @return The copied object.
 */
static inline jso_object *jso_object_copy(jso_object *obj)
{
	++JSO_OBJECT_REFCOUNT(obj);
	return obj;
}

/**
 * @brief Macro to start iteration of the object.
 * @param _obj object
 * @param _key entry key
 * @param _val entry value pointer
 */
#define JSO_OBJECT_FOREACH(_obj, _key, _val) \
	do { \
		jso_ht *_ht = &(_obj)->ht; \
	JSO_HT_FOREACH(_ht, _key, _val)

/**
 * @brief Macro to end iteration of the object.
 */
#define JSO_OBJECT_FOREACH_END \
	JSO_HT_FOREACH_END; \
	} \
	while (0)

#endif /* JSO_OBJECT_H */
