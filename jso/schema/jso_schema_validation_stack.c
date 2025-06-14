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

#include "jso_schema_validation_stack.h"

#include "jso_schema_error.h"

#include "../jso.h"

jso_rc jso_schema_validation_stack_init(
		jso_schema *schema, jso_schema_validation_stack *stack, size_t capacity)
{
	stack->positions = jso_malloc(capacity * sizeof(jso_schema_validation_position));
	if (stack->positions == NULL) {
		jso_schema_error_format(
				schema, JSO_SCHEMA_ERROR_STACK_ALLOC, "Allocating stack positions failed");
		return JSO_FAILURE;
	}
	stack->root_schema = schema;
	stack->capacity = capacity;
	stack->size = 0;
	stack->last_separator = NULL;
	stack->depth = 0;

	return JSO_SUCCESS;
}

void jso_schema_validation_stack_clear(jso_schema_validation_stack *stack)
{
	jso_free(stack->positions);
}

jso_schema_validation_position *jso_schema_validation_stack_root_position(
		const jso_schema_validation_stack *stack)
{
	JSO_ASSERT_GT(stack->capacity, 0);
	return &stack->positions[0];
}

static jso_rc jso_schema_validation_stack_resize_if_needed(jso_schema_validation_stack *stack)
{
	size_t capacity = stack->capacity;
	if (stack->size < capacity) {
		return JSO_SUCCESS;
	}

	size_t new_capacity = capacity * 2;
	jso_schema_validation_position *positions
			= jso_realloc(stack->positions, sizeof(jso_schema_validation_position) * new_capacity);
	if (stack->positions == NULL) {
		jso_schema_error_format(stack->root_schema, JSO_SCHEMA_ERROR_STACK_ALLOC,
				"Re-allocating stack positions failed");
		return JSO_FAILURE;
	}
	stack->positions = positions;
	stack->capacity = new_capacity;
	return JSO_SUCCESS;
}

static inline jso_schema_validation_position *jso_schema_validation_stack_next(
		jso_schema_validation_stack *stack)
{
	jso_schema_validation_position *position = &stack->positions[stack->size++];
	// clear position before use
	memset(position, 0, sizeof(jso_schema_validation_position));

	return position;
}

jso_schema_validation_position *jso_schema_validation_stack_push_basic(
		jso_schema_validation_stack *stack, jso_schema_value *current_value,
		jso_schema_validation_position *parent)
{
	if (jso_schema_validation_stack_resize_if_needed(stack) == JSO_FAILURE) {
		return NULL;
	}

	jso_schema_validation_position *next = jso_schema_validation_stack_next(stack);
	next->current_value = current_value;
	next->parent = parent;
	next->depth = stack->depth;
	if (stack->last_separator != NULL) {
		next->layer_start = stack->last_separator - stack->positions + 1;
	}

	return next;
}

jso_schema_validation_position *jso_schema_validation_stack_push_composed(
		jso_schema_validation_stack *stack, jso_schema_value *current_value,
		jso_schema_validation_position *parent,
		jso_schema_validation_composition_type composition_type)
{
	if (jso_schema_validation_stack_resize_if_needed(stack) == JSO_FAILURE) {
		return NULL;
	}

	jso_schema_validation_position *next = jso_schema_validation_stack_next(stack);
	next->position_type = JSO_SCHEMA_VALIDATION_POSITION_COMPOSED;
	next->composition_type = composition_type;
	next->current_value = current_value;
	next->parent = parent;
	next->depth = stack->depth;
	if (stack->last_separator != NULL) {
		next->layer_start = stack->last_separator - stack->positions + 1;
	}

	return next;
}

jso_schema_validation_position *jso_schema_validation_stack_push_separator(
		jso_schema_validation_stack *stack)
{
	if (jso_schema_validation_stack_resize_if_needed(stack) == JSO_FAILURE) {
		return NULL;
	}

	jso_schema_validation_position *next = jso_schema_validation_stack_next(stack);
	next->position_type = JSO_SCHEMA_VALIDATION_POSITION_SENTINEL;
	next->parent = stack->last_separator;
	next->layer_start = stack->size - 1;
	stack->last_separator = next;
	stack->depth++;

	return next;
}

jso_schema_validation_position *jso_schema_validation_stack_pop(jso_schema_validation_stack *stack)
{
	if (stack->size == 0) {
		return NULL;
	}

	return &stack->positions[--stack->size];
}

void jso_schema_validation_stack_reset(jso_schema_validation_stack *stack)
{
	stack->size = stack->mark;
}

void jso_schema_validation_stack_mark(jso_schema_validation_stack *stack)
{
	stack->mark = stack->size;
}

void jso_schema_validation_stack_layer_iterator_start(
		jso_schema_validation_stack *stack, jso_schema_validation_stack_layer_iterator *iterator)
{
	JSO_ASSERT_GT(stack->size, 0);
	jso_schema_validation_position *last_position = &stack->positions[stack->size - 1];
	iterator->start = iterator->index = last_position->layer_start;
}

jso_schema_validation_position *jso_schema_validation_stack_layer_iterator_next(
		jso_schema_validation_stack *stack, jso_schema_validation_stack_layer_iterator *iterator)
{
	if (iterator->index >= stack->size) {
		return NULL;
	}
	jso_schema_validation_position *pos = &stack->positions[iterator->index];
	if (JSO_SCHEMA_VALIDATION_POSITION_IS_SENTINEL(pos)) {
		return NULL;
	}
	iterator->index++;
	return pos;
}

void jso_schema_validation_stack_layer_reverse_iterator_start(
		jso_schema_validation_stack *stack, jso_schema_validation_stack_layer_iterator *iterator)
{
	JSO_ASSERT_GT(stack->size, 0);
	iterator->finished = false;
	iterator->start = iterator->index = stack->size;
}

jso_schema_validation_position *jso_schema_validation_stack_layer_reverse_iterator_next(
		jso_schema_validation_stack *stack, jso_schema_validation_stack_layer_iterator *iterator)
{
	if (iterator->finished) {
		return NULL;
	}
	jso_schema_validation_position *pos = &stack->positions[--iterator->index];
	bool is_sentinel = JSO_SCHEMA_VALIDATION_POSITION_IS_SENTINEL(pos);
	if (is_sentinel || iterator->index == 0) {
		iterator->finished = true;
		return is_sentinel ? NULL : pos;
	}
	return pos;
}

void jso_schema_validation_stack_layer_remove(jso_schema_validation_stack *stack)
{
	if (stack->last_separator != NULL) {
		stack->size = stack->last_separator - stack->positions;
		stack->depth--;
		stack->last_separator = stack->last_separator->parent;
	} else {
		stack->size = stack->depth = 0;
	}
}

void jso_schema_validation_stack_layer_reset_positions(jso_schema_validation_stack *stack)
{
	jso_schema_validation_stack_layer_iterator iterator;
	jso_schema_validation_position *pos;
	jso_schema_validation_stack_layer_iterator_start(stack, &iterator);
	while ((pos = jso_schema_validation_stack_layer_iterator_next(stack, &iterator))) {
		pos->validation_result = JSO_SCHEMA_VALIDATION_VALID;
		pos->validation_invalid_reason = JSO_SCHEMA_VALIDATION_INVALID_REASON_NONE;
		pos->is_final_validation_result = false;
		pos->count = 0;
		pos->one_of_valid = 0;
		pos->any_of_valid = 0;
		pos->type_valid = 0;
	}
}