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
 * @file jso_parser_hooks_decode.h
 * @brief Parser hooks for decoding
 */

#ifndef JSO_PARSER_HOOKS_DECODE_H
#define JSO_PARSER_HOOKS_DECODE_H

#include "../jso_parser_hooks.h"

jso_error_type jso_parser_decode_array_create(jso_parser *parser, jso_array **array);
jso_error_type jso_parser_decode_array_append(
		jso_parser *parser, jso_array *array, jso_value *value);
jso_error_type jso_parser_decode_object_create(jso_parser *parser, jso_object **object);
jso_error_type jso_parser_decode_object_update(
		jso_parser *parser, jso_object *object, jso_string *key, jso_value *value);

#endif /* JSO_PARSER_HOOKS_DECODE_H */
