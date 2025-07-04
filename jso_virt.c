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

 #include "php.h"
 #include "jso.h"
 #include "jso_virt.h"

/* Get the JSO type. */
jso_value_type jso_virt_value_type(jso_virt_value *val)
{
	switch (Z_TYPE_P(val)) {
		case IS_NULL:
			return JSO_TYPE_NULL;
		case IS_FALSE:
		case IS_TRUE:
			return JSO_TYPE_BOOL;
		case IS_LONG:
			return JSO_TYPE_INT;
		case IS_DOUBLE:
			return JSO_TYPE_DOUBLE;
		case IS_STRING:
			return JSO_TYPE_STRING;
		case IS_ARRAY:
			return HT_IS_PACKED(Z_ARR_P(val)) ? JSO_TYPE_ARRAY : JSO_TYPE_OBJECT;
		case IS_OBJECT:
			return JSO_TYPE_OBJECT;
		default:
			return JSO_TYPE_ERROR;
	}
}

static bool jso_virt_array_equals(jso_virt_array *varr, jso_array *arr)
{
    if (jso_virt_array_len(varr) != JSO_ARRAY_LEN(arr)) {
        return false;
    }
    jso_uint idx = 0;
    jso_value *val;
    jso_virt_value *vval;
    JSO_ARRAY_FOREACH(arr, val) {
        if ((vval = zend_hash_index_find(Z_ARR_P(varr), idx++)) == NULL) {
            return false;
        }
        if (!jso_virt_value_equals(vval, val)) {
            return false;
        }
    } JSO_ARRAY_FOREACH_END;

    return true;
}

static bool jso_virt_object_equals(jso_virt_object *vobj, jso_object *obj)
{
    if (jso_virt_object_count(vobj) != JSO_OBJECT_COUNT(obj)) {
        return false;
    }
    jso_string *key;
    jso_value *val;
    jso_virt_value *vval;
    HashTable *ht = PHP_JSOND_OBJ_PROPS(vobj);
    JSO_OBJECT_FOREACH(obj, key, val) {
        if ((vval = zend_hash_str_find(ht, (const char *)JSO_STRING_VAL(key), JSO_STRING_LEN(key))) == NULL) {
            return false;
        }
        if (!jso_virt_value_equals(vval, val)) {
            return false;
        }
    } JSO_OBJECT_FOREACH_END;

    return true;
}

/* Check if virtual value is equal to JSO value. */
bool jso_virt_value_equals(jso_virt_value *vval, jso_value *val)
{
	jso_value_type vval_type = jso_virt_value_type(vval);
	if (vval_type != JSO_TYPE_P(val)) {
		return false;
	}

	switch (val->type) {
		case JSO_TYPE_NULL:
			return true;
		case JSO_TYPE_BOOL:
            return (Z_TYPE_P(vval) == IS_TRUE && JSO_IVAL_P(val)) ||
                    (Z_TYPE_P(vval) == IS_FALSE && !JSO_IVAL_P(val));
		case JSO_TYPE_INT:
			return jso_virt_value_int(vval) == JSO_IVAL_P(val);
		case JSO_TYPE_DOUBLE:
			return jso_virt_value_double(vval) == JSO_DVAL_P(val);
		case JSO_TYPE_STRING:
			return Z_STRLEN_P(vval) == JSO_SLEN_P(val) &&
                jso_string_equals_to_cstr(JSO_STR_P(val), (const char *)jso_virt_value_string(vval));
		case JSO_TYPE_ARRAY:
			return jso_virt_array_equals(jso_virt_value_array(vval), JSO_ARRVAL_P(val));
		case JSO_TYPE_OBJECT:
			return jso_virt_object_equals(jso_virt_value_object(vval), JSO_OBJVAL_P(val));
			break;
		default:
			return false;
	}
}

static bool jso_virt_array_contains(jso_virt_array *arr, jso_virt_value *val, uint from)
{
    jso_virt_value *av;
    uint idx = 0;
    JSO_VIRT_ARRAY_FOREACH(arr, av) {
        if (idx++ >= from && zend_compare(av, val) == 0) {
            return true;
        }
    } JSO_VIRT_ARRAY_FOREACH_END;

    return false;
}

bool jso_virt_array_is_unique(jso_virt_array *arr)
{
    jso_virt_value *val;
    uint from = 0;
    JSO_VIRT_ARRAY_FOREACH(arr, val) {
        if (jso_virt_array_contains(arr, val, ++from)) {
            return false;
        }
    } JSO_VIRT_ARRAY_FOREACH_END;

    return true;
}
