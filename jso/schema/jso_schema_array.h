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

/**
 * @file jso_schema_data.h
 * @brief JsonSchema array of values.
 */

#ifndef JSO_SCHEMA_ARRAY_H
#define JSO_SCHEMA_ARRAY_H

#include "../jso_schema.h"

jso_schema_array *jso_schema_array_alloc(size_t size);
jso_rc jso_schema_array_append(jso_schema_array *arr, jso_schema_value *val);
jso_schema_value *jso_schema_array_get(jso_schema_array *arr, size_t index);
void jso_schema_array_free(jso_schema_array *arr);

/**
 * @brief Macro to start iteration of the schema array.
 * @param _arr array
 * @param _val value pointer
 */
#define JSO_SCHEMA_ARRAY_FOREACH(_arr, _val) \
	do { \
		for (size_t _i = 0; _i < _arr->len; ++_i) { \
			_val = _arr->values[_i];

/**
 * @brief Macro to end iteration of the schema array.
 */
#define JSO_SCHEMA_ARRAY_FOREACH_END \
	} \
	} \
	while (0)

#endif /* JSO_SCHEMA_ARRAY_H */
