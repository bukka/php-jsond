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

/**
 * @file jso_schema_value.h
 * @brief JsonSchema value handling.
 */

#ifndef JSO_SCHEMA_VALUE_H
#define JSO_SCHEMA_VALUE_H

#include "../jso_schema.h"

#define JSO_SCHEMA_VALUE_TYPE_NAME(_value_type) JSO_SCHEMA_VALUE_##_value_type

jso_schema_value *jso_schema_value_alloc(jso_schema *schema, const char *type_name);

jso_schema_value_common *jso_schema_value_data_alloc(
		size_t value_size, jso_schema *schema, const char *type_name);

const char *jso_schema_value_type_to_string(jso_schema_value_type type);

#define JSO_SCHEMA_VALUE_INIT(_schema, _data, _parent, _type_name, _value_type, _init_keywords) \
	jso_schema_value_init(_schema, _data, _parent, #_type_name, \
			sizeof(jso_schema_value_##_type_name), JSO_SCHEMA_VALUE_TYPE_NAME(_value_type), \
			_init_keywords)

jso_schema_value *jso_schema_value_init(jso_schema *schema, jso_value *data,
		jso_schema_value *parent, const char *type_name, size_t value_size,
		jso_schema_value_type value_type, jso_bool init_keywords);

jso_schema_value *jso_schema_value_parse(
		jso_schema *schema, jso_value *data, jso_schema_value *parent);

jso_rc jso_schema_value_validate(jso_schema *schema, jso_schema_value *value, jso_value *instance);

#endif /* JSO_SCHEMA_VALUE_H */
