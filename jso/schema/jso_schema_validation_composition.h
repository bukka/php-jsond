/*
 * Copyright (c) 2023-2024 Jakub Zelenka. All rights reserved.
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
 * @file jso_schema_validation_composition.h
 * @brief JsonSchema validation of compositions.
 */

#ifndef JSO_SCHEMA_VALIDATION_COMPOSITION_H
#define JSO_SCHEMA_VALIDATION_COMPOSITION_H

#include "jso_schema_validation_stream.h"
#include "jso_schema.h"

jso_schema_validation_result jso_schema_validation_composition_push(
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos);

static inline jso_rc jso_schema_validation_composition_check(
		jso_schema_validation_stack *stack, jso_schema_validation_position *pos)
{
	pos->validation_result = jso_schema_validation_composition_push(stack, pos);
	if (jso_schema_validation_stream_should_terminate(stack->root_schema, pos)) {
		return JSO_FAILURE;
	}
	return JSO_SUCCESS;
}

#endif /* JSO_SCHEMA_VALIDATION_COMPOSITION_H */
