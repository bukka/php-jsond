/*
 * Copyright (c) 2025 Jakub Zelenka. All rights reserved.
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
 * @file jso_virt.h
 * @brief JSO virtual API that can be replaced by embedders
 *
 * This is minimal API that can be overwritten by application that embeds JSO.
 */

#ifndef JSO_VIRT_H
#define JSO_VIRT_H

#include "php.h"

#include "jso_types.h"
#include "jso_string.h"
#include "jso_array.h"
#include "jso_object.h"
#include "jso_value.h"

/* types */

/**
 * @brief value type
 */
#define jso_virt_value zval

/**
 * @brief string type
 */
#define jso_virt_string zend_string

/**
 * @brief array type
 */
#define jso_virt_array zend_array

/**
 * @brief object type
 */
#define jso_virt_object zend_object

/* value */

/**
 * Get the JSO type.
 *
 * @param val virtual value
 * @return value type of @ref jso_value_type
 */
jso_value_type jso_virt_value_type(jso_virt_value *val);

/**
 * Get integer value of the supplied virtual value.
 *
 * @param val virtual value
 * @return integer value
 */
static inline jso_int jso_virt_value_int(jso_virt_value *val)
{
	return Z_LVAL_P(val);
}

/**
 * Get double value of the supplied virtual value.
 *
 * @param val virtual value
 * @return double value
 */
static inline jso_double jso_virt_value_double(jso_virt_value *val)
{
	return Z_DVAL_P(val);
}

/**
 * Get string value of the supplied virtual value.
 *
 * @param val virtual value
 * @return string value
 */
static inline jso_virt_string *jso_virt_value_string(jso_virt_value *val)
{
	return Z_STR_P(val);
}

/**
 * Get array value of the supplied virtual value.
 *
 * @param val virtual value
 * @return array value
 */
static inline jso_virt_array *jso_virt_value_array(jso_virt_value *val)
{
	return Z_ARR_P(val);
}

/**
 * Get object value of the supplied virtual value.
 *
 * @param val virtual value
 * @return integer value
 */
static inline jso_virt_object *jso_virt_value_object(jso_virt_value *val)
{
	return Z_OBJ_P(val);
}

/**
 * Check if virtual value is equal to JSO value.
 *
 * @param vval virtual value
 * @param val JSO value
 * @return true if the values are equal, otherwise false
 */
bool jso_virt_value_equals(jso_virt_value *vval, jso_value *val);

/* string */

/**
 * Get C string value of the string.
 *
 * @param str virtual string
 * @return C string value of the string
 */
static inline const char *jso_virt_string_val(jso_virt_string *str)
{
	return ZSTR_VAL(str);
}

/**
 * Get length of the string.
 *
 * @param str virtual string
 * @return string length
 */
static inline size_t jso_virt_string_len(jso_virt_string *str)
{
	return ZSTR_LEN(str);
}

/* array */

#define JSO_VIRT_ARRAY_FOREACH ZEND_HASH_FOREACH_VAL
#define JSO_VIRT_ARRAY_FOREACH_END ZEND_HASH_FOREACH_END()

/**
 * Get length of the array.
 *
 * @param arr virtual array
 * @return array length
 */
static inline size_t jso_virt_array_len(jso_virt_array *arr)
{
	return zend_hash_num_elements(arr);
}

/**
 * Check if array contains only unique elements.
 *
 * @param arr virtual array
 * @return true if the array is unique, otherwise false
 */
bool jso_virt_array_is_unique(jso_virt_array *arr);

/* object */

#define JSO_VIRT_OBJECT_FOREACH(_obj, _key, _val) \
	ZEND_HASH_FOREACH_STR_KEY_VAL(_obj->properties, _key, _val)
#define JSO_VIRT_OBJECT_FOREACH_END ZEND_HASH_FOREACH_END()

/**
 * Check if the key is in object.
 *
 * @param obj virtual object
 * @param key key to check
 * @return true if the key is in object, otherwise false
 */
static inline bool jso_virt_object_has_str_key(jso_virt_object *obj, jso_string *key)
{
	return zend_hash_str_find(obj->properties, JSO_STRING_VAL(key), JSO_STRING_LEN(key)) != NULL;
}

/**
 * Get length of the object.
 *
 * @param obj virtual object
 * @return object length
 */
static inline size_t jso_virt_object_count(jso_virt_object *obj)
{
	return zend_hash_num_elements(obj->properties);
}

#endif /* JSO_VIRT_H */
