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
 * @file jso_schema_validation_stack.h
 * @brief JsonSchema validation stack functionality.
 */

#ifndef JSO_SCHEMA_VALIDATION_STACK_H
#define JSO_SCHEMA_VALIDATION_STACK_H

#include "jso_schema.h"

/**
 * @brief JsonSchema validation stack layer iterator.
 */
typedef struct _jso_schema_validation_stack_layer_iterator {
	size_t start;
	size_t index;
	jso_bool finished;
} jso_schema_validation_stack_layer_iterator;

jso_rc jso_schema_validation_stack_init(
		jso_schema *schema, jso_schema_validation_stack *stack, size_t capacity);

void jso_schema_validation_stack_clear(jso_schema_validation_stack *stack);

jso_schema_validation_position *jso_schema_validation_stack_root_position(
		const jso_schema_validation_stack *stack);

jso_schema_validation_position *jso_schema_validation_stack_push_basic(
		jso_schema_validation_stack *stack, jso_schema_value *current_value,
		jso_schema_validation_position *parent);

jso_schema_validation_position *jso_schema_validation_stack_push_composed(
		jso_schema_validation_stack *stack, jso_schema_value *current_value,
		jso_schema_validation_position *parent,
		jso_schema_validation_composition_type composition_type);

jso_schema_validation_position *jso_schema_validation_stack_push_separator(
		jso_schema_validation_stack *stack);

jso_schema_validation_position *jso_schema_validation_stack_pop(jso_schema_validation_stack *stack);

void jso_schema_validation_stack_mark(jso_schema_validation_stack *stack);

void jso_schema_validation_stack_reset(jso_schema_validation_stack *stack);

void jso_schema_validation_stack_layer_iterator_start(
		jso_schema_validation_stack *stack, jso_schema_validation_stack_layer_iterator *iterator);

jso_schema_validation_position *jso_schema_validation_stack_layer_iterator_next(
		jso_schema_validation_stack *stack, jso_schema_validation_stack_layer_iterator *iterator);

void jso_schema_validation_stack_layer_reverse_iterator_start(
		jso_schema_validation_stack *stack, jso_schema_validation_stack_layer_iterator *iterator);

jso_schema_validation_position *jso_schema_validation_stack_layer_reverse_iterator_next(
		jso_schema_validation_stack *stack, jso_schema_validation_stack_layer_iterator *iterator);

void jso_schema_validation_stack_layer_remove(jso_schema_validation_stack *stack);

void jso_schema_validation_stack_layer_reset_positions(jso_schema_validation_stack *stack);

#endif /* JSO_SCHEMA_VALIDATION_STACK_H */
