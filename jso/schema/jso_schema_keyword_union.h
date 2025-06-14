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
 * @file jso_schema_keyword_union.h
 * @brief JsonSchema keywords union functionality.
 */

#ifndef JSO_SCHEMA_KEYWORD_UNION_H
#define JSO_SCHEMA_KEYWORD_UNION_H

#include "../jso_schema.h"

#define JSO_SCHEMA_KW_VALUE_TYPES_SIZE 8

jso_schema_keyword *jso_schema_keyword_get_union_of_2_types(jso_schema *schema, jso_value *data,
		const char *key, jso_schema_value *parent, jso_schema_keyword_type keyword_union_type_1,
		jso_schema_keyword_type keyword_union_type_2, jso_uint32 keyword_flags,
		jso_schema_keyword *schema_keyword);

#endif /* JSO_SCHEMA_KEYWORD_UNION_H */
