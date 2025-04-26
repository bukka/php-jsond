/*
 * Copyright (c) 2012-2023 Jakub Zelenka. All rights reserved.
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

#ifndef JSO_PARSER_H
#define JSO_PARSER_H

#include "jso_types.h"
#include "jso_scanner.h"

typedef struct _jso_parser jso_parser;

typedef jso_error_type (*jso_parser_hook_array_create_t)(jso_parser *parser, jso_array **array);
typedef jso_error_type (*jso_parser_hook_array_append_t)(
		jso_parser *parser, jso_array *array, jso_value *value);
typedef jso_error_type (*jso_parser_hook_array_start_t)(jso_parser *parser);
typedef jso_error_type (*jso_parser_hook_array_end_t)(jso_parser *parser);
typedef jso_error_type (*jso_parser_hook_object_create_t)(jso_parser *parser, jso_object **object);
typedef jso_error_type (*jso_parser_hook_object_update_t)(
		jso_parser *parser, jso_object *object, jso_string *key, jso_value *value);
typedef jso_error_type (*jso_parser_hook_object_start_t)(jso_parser *parser);
typedef jso_error_type (*jso_parser_hook_object_end_t)(jso_parser *parser);

typedef jso_error_type (*jso_parser_hook_object_key_t)(jso_parser *parser, jso_string *key);

typedef jso_error_type (*jso_parser_hook_value_t)(jso_parser *parser, jso_value *value);

/**
 * @brief Parser hooks
 */
typedef struct _jso_parser_hooks {
	jso_parser_hook_array_create_t array_create;
	jso_parser_hook_array_append_t array_append;
	jso_parser_hook_array_start_t array_start;
	jso_parser_hook_array_end_t array_end;
	jso_parser_hook_object_create_t object_create;
	jso_parser_hook_object_update_t object_update;
	jso_parser_hook_object_start_t object_start;
	jso_parser_hook_object_end_t object_end;
	jso_parser_hook_object_key_t object_key;
	jso_parser_hook_value_t value;
} jso_parser_hooks;

/**
 * @brief Parser location type.
 */
typedef struct _jso_parser_location {
	int first_line;
	int first_column;
	int last_line;
	int last_column;
} jso_parser_location;

/**
 * @brief Parser state.
 */
struct _jso_parser {
	jso_scanner scanner;
	jso_parser_hooks hooks;
	jso_parser_location *location;
	jso_value result;
	jso_uint depth;
	jso_uint max_depth;
};

/**
 * Increase parsing depth.
 *
 * @param parser parser instance
 * @return jso_error_type
 */
static inline jso_error_type jso_parser_depth_increase(jso_parser *parser)
{
	if (parser->max_depth && parser->depth >= parser->max_depth) {
		return JSO_ERROR_DEPTH;
	}
	++parser->depth;
	return JSO_ERROR_NONE;
}

/**
 * Decrease parsing depth.
 *
 * @param parser parser instance
 * @return jso_error_type
 */
static inline jso_error_type jso_parser_depth_decrease(jso_parser *parser)
{
	--parser->depth;
	return JSO_ERROR_NONE;
}

/**
 * Initialize parser with hooks.
 *
 * @param parser parser instance
 */
JSO_API void jso_parser_init_ex(jso_parser *parser, const jso_parser_hooks *hooks);

/**
 * Initialize parser.
 *
 * @param parser parser instance
 */
JSO_API void jso_parser_init(jso_parser *parser);

/**
 * Start parsing.
 *
 * @param parser parser instance
 * @return int Parsing result - 0 on success otherwise non zero value is returned.
 */
JSO_API int jso_yyparse(jso_parser *parser);

#endif /* JSO_SCANNER_H */
