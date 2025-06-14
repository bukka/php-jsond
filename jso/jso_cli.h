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

/**
 * @file jso_cli.h
 * @brief Command line interface
 */

#ifndef JSO_CLI_H
#define JSO_CLI_H

#include "jso_types.h"
#include "jso_io.h"
#include "jso_schema.h"

/**
 * @brief CLI output type.
 */
typedef enum {
	JSO_OUTPUT_NONE,
	JSO_OUTPUT_HELP,
	JSO_OUTPUT_MINIMAL,
	JSO_OUTPUT_PRETTY,
	JSO_OUTPUT_DEBUG
} jso_cli_output_type;

/**
 * @brief CLI options.
 */
typedef struct _jso_cli_options {
	/** maximal depth of the parsed json */
	jso_uint max_depth;
	/** the output type */
	jso_cli_output_type output_type;
	/** input stream */
	jso_io *is;
	/** output stream */
	jso_io *os;
	/** error stream */
	jso_io *es;
	/** JsonSchema to use for validation */
	jso_schema *schema;
} jso_cli_options;

/**
 * Callback function for parameter with value.
 * @param value value if any
 * @param options CLI options
 */
typedef jso_rc (*jso_cli_param_value_callback)(const char *value, jso_cli_options *options);

/**
 * Callback function for parameter without value.
 * @param options CLI options
 */
typedef jso_rc (*jso_cli_param_flag_callback)(jso_cli_options *options);

/**
 * @brief Specification of the parameters.
 */
typedef struct _jso_cli_param {
	/** the long param name */
	const char *long_name;
	/** the short param name */
	char short_name;
	/* the param description */
	const char *description;
	/** whether the param requires value */
	jso_bool has_value;
	/** the callback called when the param is processed */
	union {
		jso_cli_param_value_callback callback_value;
		jso_cli_param_flag_callback callback_flag;
	} callback;
} jso_cli_param;

/**
 * @brief CLI context.
 */
typedef struct _jso_cli_ctx {
	/** array of parsed parameters */
	const jso_cli_param *params;
} jso_cli_ctx;

/**
 * Parameter with value entry setter.
 * @param _long_name long name
 * @param _short_name short name
 * @param _description param description
 * @param _callback_fce callback function of type @ref jso_cli_param_flag_callback
 */
#define JSO_CLI_PARAM_ENTRY_VALUE(_long_name, _short_name, _description, _callback_fce) \
	{ _long_name, _short_name, _description, JSO_TRUE, { (void *) _callback_fce } },

/**
 * Parameter without value entry setter.
 * @param _long_name long name
 * @param _short_name short name
 * @param _description param description
 * @param _callback_fce callback function of type @ref jso_cli_param_value_callback
 */
#define JSO_CLI_PARAM_ENTRY_FLAG(_long_name, _short_name, _description, _callback_fce) \
	{ _long_name, _short_name, _description, JSO_FALSE, { (void *) _callback_fce } },

/**
 * End of parameter entry array
 */
#define JSO_CLI_PARAM_ENTRY_END \
	{ \
		NULL, 0, NULL, JSO_FALSE, \
		{ \
			NULL \
		} \
	}

/**
 * Pre-initialize CLI options
 * @param options CLI options
 */
JSO_API void jso_cli_options_init_pre(jso_cli_options *options);

/**
 * Post-initialize CLI options
 * @param options CLI options
 */
JSO_API void jso_cli_options_init_post(jso_cli_options *options);

/**
 * Destroy CLI options
 * @param options CLI options
 */
JSO_API jso_rc jso_cli_options_destroy(jso_cli_options *options);

/**
 * Parse file input with supplied options.
 * @param file_path the file that is parsed
 * @param options CLI options
 * @param result pointer to value where the result is saved to (memory managed by caller)
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_cli_parse_file(
		const char *file_path, jso_cli_options *options, jso_value *result);

/**
 * Parse arguments from the `main` function for supplied `ctx`.
 * @param argc number of arguments
 * @param argv arguments array
 * @param ctx CLI context
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_cli_parse_args_ex(int argc, const char *argv[], jso_cli_ctx *ctx);

/**
 * Parse arguments from the `main` function.
 * @param argc number of arguments
 * @param argv arguments array
 * @return @ref JSO_SUCCESS on success, otherwise @ref JSO_FAILURE.
 */
JSO_API jso_rc jso_cli_parse_args(int argc, const char *argv[]);

/**
 * Register params.
 * @param params number of arguments for registration
 * @param ctx CLI context
 * @note If there is already a parameter registered, then it is replaced.
 */
JSO_API void jso_cli_register_params(jso_cli_ctx *ctx, const jso_cli_param *params);

/**
 * Register default params.
 * @param ctx CLI context
 */
JSO_API void jso_cli_register_default_params(jso_cli_ctx *ctx);

#endif /* JSO_CLI_H */
