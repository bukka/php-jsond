/*
 * Copyright (c) 2012-2025 Jakub Zelenka. All rights reserved.
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

#include "jso.h"

#include <stdlib.h>

/* alloc and init new array */
JSO_API jso_array *jso_array_alloc()
{
	return jso_calloc(1, sizeof(jso_array));
}

/* init array */
JSO_API void jso_array_init(jso_array *arr)
{
	memset(arr, 0, sizeof(jso_array));
}

/* clear all array items without clearing values */
JSO_API void jso_array_clear(jso_array *arr)
{
	if (!arr)
		return;
	if (JSO_ARRAY_REFCOUNT(arr) > 0) {
		--JSO_ARRAY_REFCOUNT(arr);
		return;
	}
	jso_array_element *tmp, *el = arr->head;
	while (el) {
		tmp = el->next;
		jso_free(el);
		el = tmp;
	}
}

/* free array and its elements */
JSO_API void jso_array_free(jso_array *arr)
{
	if (!arr)
		return;
	if (JSO_ARRAY_REFCOUNT(arr) > 0) {
		--JSO_ARRAY_REFCOUNT(arr);
		return;
	}
	jso_array_element *tmp, *el = arr->head;
	while (el) {
		jso_value_free(&el->val);
		tmp = el->next;
		jso_free(el);
		el = tmp;
	}
	jso_free(arr);
}

/* append element to the array */
JSO_API jso_rc jso_array_append(jso_array *arr, jso_value *val)
{
	jso_array_element *el = jso_malloc(sizeof(jso_array_element));
	if (!el)
		return JSO_FAILURE;
	el->val = *val;
	el->next = NULL;
	if (!arr->head) {
		arr->head = arr->tail = el;
	} else {
		arr->tail->next = el;
		arr->tail = el;
	}
	++arr->len;
	return JSO_SUCCESS;
}

/* push element to the array */
JSO_API jso_rc jso_array_push(jso_array *arr, jso_value *val)
{
	jso_array_element *el = jso_malloc(sizeof(jso_array_element));
	if (!el)
		return JSO_FAILURE;
	el->val = *val;
	el->next = arr->head;
	arr->head = el;
	if (!arr->tail)
		arr->tail = el;
	++arr->len;
	return JSO_SUCCESS;
}

/* pop the head element from the array */
JSO_API jso_rc jso_array_pop(jso_array *arr)
{
	jso_array_element *el = arr->head;
	if (!el)
		return JSO_FAILURE;
	--arr->len;
	if (el == arr->tail)
		arr->head = arr->tail = NULL;
	else
		arr->head = arr->head->next;
	jso_free(el);
	return JSO_SUCCESS;
}

/* get array element at the supplied index */
JSO_API jso_rc jso_array_index(jso_array *arr, size_t index, jso_value **value)
{
	if (arr->len == 0 || index >= arr->len) {
		return JSO_FAILURE;
	}
	jso_array_element *el = arr->head;
	JSO_ASSERT_NOT_NULL(el);
	for (size_t i = 0; i < index; i++) {
		el = el->next;
		JSO_ASSERT_NOT_NULL(el);
	}
	*value = &el->val;
	return JSO_SUCCESS;
}

/* call cbk function for each element in array */
JSO_API void jso_array_apply(jso_array *arr, jso_array_callback cbk)
{
	size_t pos = 0;
	jso_array_element *el = arr->head;
	while (el) {
		cbk(pos++, &el->val);
		el = el->next;
	}
}

/* call cbk function with arg for each element in array */
JSO_API void jso_array_apply_with_arg(jso_array *arr, jso_array_with_arg_callback cbk, void *arg)
{
	size_t pos = 0;
	jso_array_element *el = arr->head;
	while (el) {
		cbk(pos++, &el->val, arg);
		el = el->next;
	}
}

/* compare arrays */
JSO_API jso_bool jso_array_equals(jso_array *arr1, jso_array *arr2)
{
	if (arr1->len != arr2->len) {
		return JSO_FALSE;
	}

	jso_array_element *el1 = arr1->head, *el2 = arr2->head;
	while (el1 && el2) {
		if (!jso_value_equals(&el1->val, &el2->val)) {
			return JSO_FALSE;
		}
		el1 = el1->next;
		el2 = el2->next;
	}

	return el1 == el2;
}

/* check item types */
JSO_API jso_bool jso_array_are_all_items_of_type(jso_array *arr, jso_value_type type)
{
	jso_array_element *el = arr->head;
	while (el) {
		if (JSO_TYPE(el->val) != type) {
			return JSO_FALSE;
		}
		el = el->next;
	}

	return JSO_TRUE;
}

/* check uniqueness */
JSO_API jso_bool jso_array_is_unique(jso_array *arr)
{
	jso_array_element *el = arr->head, *el2;
	while (el) {
		el2 = el->next;
		while (el2) {
			if (jso_value_equals(&el->val, &el2->val)) {
				return JSO_FALSE;
			}
			el2 = el2->next;
		}
		el = el->next;
	}

	return JSO_TRUE;
}
