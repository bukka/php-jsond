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
#include "jso_cli.h"
#include "jso_parser.h"
#include "jso_encoder.h"
#include "jso_schema.h"

#include "io/jso_io_file.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static jso_rc jso_cli_param_callback_depth(const char *value, jso_cli_options *options);
static jso_rc jso_cli_param_callback_output(const char *value, jso_cli_options *options);
static jso_rc jso_cli_param_callback_help(jso_cli_options *options);
static jso_rc jso_cli_param_callback_schema(const char *value, jso_cli_options *options);

// clang-format off
const jso_cli_param jso_cli_default_params[] = {
	JSO_CLI_PARAM_ENTRY_VALUE(
		"depth",
		'd',
		"Maximum allowed object nesting depth",
		jso_cli_param_callback_depth
	)
	JSO_CLI_PARAM_ENTRY_FLAG(
		"help",
		'h',
		"This help text",
		jso_cli_param_callback_help
	)
	JSO_CLI_PARAM_ENTRY_VALUE(
		"output-type",
		'o',
		"Resulted JSON output type - either minimal, pretty or debug",
		jso_cli_param_callback_output
	)
	JSO_CLI_PARAM_ENTRY_VALUE(
		"schema",
		's',
		"JsonSchema file used for validation",
		jso_cli_param_callback_schema
	)
	JSO_CLI_PARAM_ENTRY_END
};
// clang-format on

static void jso_cli_print_help(jso_cli_options *options, jso_cli_ctx *ctx)
{
	JSO_IO_PRINTF(options->os, "Usage: jso [options...] <file>\n");
	for (const jso_cli_param *param = ctx->params; param->long_name != NULL; ++param) {
		JSO_IO_PRINTF(options->os, " -%c, --%-14s %s\n", param->short_name, param->long_name,
				param->description);
	}
}

JSO_API void jso_cli_register_params(jso_cli_ctx *ctx, const jso_cli_param *params)
{
	ctx->params = params;
}

JSO_API void jso_cli_register_default_params(jso_cli_ctx *ctx)
{
	jso_cli_register_params(ctx, jso_cli_default_params);
}

static void jso_cli_print_parsing_error(
		const char *file_path, jso_cli_options *options, jso_value *error)
{
	const char *err_desc = jso_value_get_error_description(error);
	if (err_desc == NULL) {
		JSO_IO_PRINTF(options->es, "Error value setting failed\n");
		return;
	}

	JSO_IO_PRINTF(options->es, "Parsing %s error in %s:%zu:%zu\n", err_desc, file_path,
			JSO_ELOC_P(error).first_line, JSO_ELOC_P(error).first_column);
}

static jso_rc jso_cli_parse_file_ex(
		const char *file_path, jso_cli_options *options, jso_value *result, const char *file_type)
{
	jso_io *io;
	off_t filesize;
	size_t bytes_to_read, bytes_read_once, bytes_read_total = 0;

	/* get file size */
	filesize = jso_io_file_size(file_path);
	if (filesize < 0) {
		JSO_IO_PRINTF(options->es, "Getting file size for %s '%s' failed\n", file_type, file_path);
		return JSO_FAILURE;
	}
	bytes_to_read = (size_t) filesize;

	/* open file */
	io = jso_io_file_open(file_path, "r");
	if (!io) {
		JSO_IO_PRINTF(options->es, "Opening the %s '%s' failed\n", file_type, file_path);
		return JSO_FAILURE;
	}
	/* read the whole file into the buffer */
	do {
		bytes_read_once = JSO_IO_READ(io, bytes_to_read - bytes_read_total);
		bytes_read_total += bytes_read_once;
	} while (bytes_read_once);

	/* check the read data */
	if (bytes_read_total == 0) {
		JSO_IO_PRINTF(options->es, "No data in the %s '%s'\n", file_type, file_path);
		return JSO_FAILURE;
	}
	/* check the read data */
	if (bytes_read_total < bytes_to_read) {
		JSO_IO_PRINTF(options->es, "Only %lu of %lu read from the %s '%s'\n", bytes_read_total,
				bytes_to_read, file_type, file_path);
		return JSO_FAILURE;
	}

	if (!JSO_IO_CURSOR(io)) {
		JSO_IO_PRINTF(options->es, "Cursor of the %s is not set\n", file_type);
		return JSO_FAILURE;
	}

	jso_parser_options parser_options = { .max_depth = options->max_depth };
	jso_rc rc = jso_parse_io(io, &parser_options, result);
	if (rc == JSO_FAILURE) {
		jso_cli_print_parsing_error(file_path, options, result);
	}

	/* free IO */
	JSO_IO_FREE(io);

	return rc;
}

JSO_API jso_rc jso_cli_parse_file(
		const char *file_path, jso_cli_options *options, jso_value *result)
{
	return jso_cli_parse_file_ex(file_path, options, result, "file");
}

static jso_rc jso_cli_process_file(const char *file_path, jso_cli_options *options)
{
	jso_value result;
	jso_rc rc = jso_cli_parse_file(file_path, options, &result);

	if (options->output_type == JSO_OUTPUT_DEBUG) {
		jso_value_dump(&result, options->os);
	} else if (rc == JSO_SUCCESS) {
		jso_encoder_options enc_options;
		enc_options.max_depth = JSO_ENCODER_DEPTH_UNLIMITED;
		enc_options.pretty = options->output_type == JSO_OUTPUT_PRETTY;
		jso_encode(&result, options->os, &enc_options);
	}

	jso_value_free(&result);

	return rc;
}

static jso_rc jso_cli_param_callback_depth(const char *value, jso_cli_options *options)
{
	if (!value) {
		JSO_IO_PRINTF(options->es, "Option depth requires value\n");
		return JSO_FAILURE;
	}

	options->max_depth = atoi(value);

	return JSO_SUCCESS;
}

static jso_rc jso_cli_param_callback_help(jso_cli_options *options)
{
	options->output_type = JSO_OUTPUT_HELP;

	return JSO_SUCCESS;
}

static jso_rc jso_cli_param_callback_output(const char *value, jso_cli_options *options)
{
	if (!value) {
		JSO_IO_PRINTF(options->es, "Option output requires value\n");
		return JSO_FAILURE;
	}

	if (options->output_type != JSO_OUTPUT_NONE) {
		return JSO_FAILURE;
	}

	if (!strncmp(value, "minimal", JSO_MAX(strlen(value), sizeof("minimal") - 1))) {
		options->output_type = JSO_OUTPUT_MINIMAL;
	} else if (!strncmp(value, "pretty", JSO_MAX(strlen(value), sizeof("pretty") - 1))) {
		options->output_type = JSO_OUTPUT_PRETTY;
	} else if (!strncmp(value, "debug", JSO_MAX(strlen(value), sizeof("debug") - 1))) {
		options->output_type = JSO_OUTPUT_DEBUG;
	} else {
		JSO_IO_PRINTF(options->es, "Unknown output value %s\n", value);
		return JSO_FAILURE;
	}

	return JSO_SUCCESS;
}

static jso_rc jso_cli_param_callback_schema(const char *value, jso_cli_options *options)
{
	if (!value) {
		JSO_IO_PRINTF(options->es, "Option schema requires value\n");
		return JSO_FAILURE;
	}

	jso_value result;
	jso_rc rc = jso_cli_parse_file_ex(value, options, &result, "schema file");

	if (options->output_type == JSO_OUTPUT_DEBUG) {
		jso_value_dump(&result, options->os);
	}
	if (rc == JSO_SUCCESS) {
		jso_schema *schema = jso_schema_alloc();
		if (jso_schema_parse(schema, &result) == JSO_FAILURE) {
			JSO_IO_PRINTF(options->es, "JsonSchema parsing failed with error: %s\n",
					JSO_SCHEMA_ERROR_MESSAGE(schema));
			rc = JSO_FAILURE;
		}
		options->schema = schema;
	}

	jso_value_free(&result);

	return rc;
}

JSO_API void jso_cli_options_init_pre(jso_cli_options *options)
{
	options->max_depth = 0;
	options->output_type = JSO_OUTPUT_NONE;
	options->is = NULL;
	options->os = jso_io_file_open_stream(stdout);
	options->es = jso_io_file_open_stream(stderr);
	options->schema = NULL;
}

JSO_API void jso_cli_options_init_post(jso_cli_options *options)
{
	if (options->output_type == JSO_OUTPUT_NONE)
		options->output_type = JSO_OUTPUT_PRETTY;
	if (!options->is)
		options->is = jso_io_file_open_stream(stdin);
}

JSO_API jso_rc jso_cli_options_destroy(jso_cli_options *options)
{
	jso_rc rc = JSO_SUCCESS;

	if (options->is)
		rc = JSO_IO_FREE(options->is);
	if (options->os) {
		rc = JSO_IO_FREE(options->os) == JSO_FAILURE || rc == JSO_FAILURE ? JSO_FAILURE
																		  : JSO_SUCCESS;
	}
	if (options->es) {
		rc = JSO_IO_FREE(options->es) == JSO_FAILURE || rc == JSO_FAILURE ? JSO_FAILURE
																		  : JSO_SUCCESS;
	}
	if (options->schema) {
		jso_schema_free(options->schema);
	}

	return rc;
}

static const jso_cli_param *jso_cli_find_param(
		jso_cli_ctx *ctx, const char *long_name, size_t long_name_len, char short_name)
{
	int i;
	const jso_cli_param *param = &ctx->params[0];

	for (i = 1; param->long_name; i++) {
		if (long_name) {
			if (!strncmp(param->long_name, long_name, long_name_len)) {
				return param;
			}
		} else if (param->short_name && param->short_name == short_name) {
			return param;
		}

		param = &ctx->params[i];
	}

	return NULL;
}

static inline const jso_cli_param *jso_cli_find_param_by_long_name(
		jso_cli_ctx *ctx, const char *long_name, size_t long_name_len)
{
	return jso_cli_find_param(ctx, long_name, long_name_len, 0);
}

static inline const jso_cli_param *jso_cli_find_param_by_short_name(
		jso_cli_ctx *ctx, char short_name)
{
	return jso_cli_find_param(ctx, NULL, 0, short_name);
}

static const jso_cli_param *jso_cli_parse_long_option(const char *arg, size_t arg_len,
		const char **pvalue, jso_cli_options *options, jso_cli_ctx *ctx)
{
	const jso_cli_param *param;
	const char *long_name, *value;
	char *long_name_dup;
	size_t long_name_len;

	if (arg_len == 2) {
		JSO_IO_PRINTF(options->es, "The long option must have name\n");
		return NULL;
	}

	long_name = &arg[2];
	value = strchr(long_name, '=');
	if (value) {
		long_name_len = (size_t) (value - long_name);
		*pvalue = value + 1;
	} else {
		long_name_len = strlen(long_name);
		*pvalue = NULL;
	}

	param = jso_cli_find_param_by_long_name(ctx, long_name, long_name_len);
	if (param) {
		return param;
	}

	long_name_dup = jso_malloc(long_name_len + 1);
	strncpy(long_name_dup, long_name, long_name_len);
	long_name_dup[long_name_len] = '\0';
	JSO_IO_PRINTF(options->es, "Unknown option: %s\n", long_name_dup);
	jso_free(long_name_dup);

	return NULL;
}

static const jso_cli_param *jso_cli_parse_short_option(const char *arg, size_t arg_len,
		const char **pvalue, jso_cli_options *options, jso_cli_ctx *ctx)
{
	const jso_cli_param *param;
	char short_name;

	if (arg_len > 2) {
		JSO_IO_PRINTF(options->es, "The grouping of short parameters is not allowed yet\n");
		return NULL;
	}

	*pvalue = NULL;
	short_name = arg[1];

	param = jso_cli_find_param_by_short_name(ctx, short_name);
	if (param) {
		return param;
	}

	JSO_IO_PRINTF(options->es, "Unknown option: %c\n", short_name);

	return NULL;
}

static jso_rc jso_cli_parse_option(
		int *ip, int argc, const char *argv[], jso_cli_options *options, jso_cli_ctx *ctx)
{
	const jso_cli_param *param;
	int i = *ip;
	const char *arg = &argv[i][0], *value;
	size_t arg_len = strlen(arg);

	if (arg_len == 1) {
		JSO_IO_PRINTF(options->es, "Input stdin is not supported yet\n");
		return JSO_FAILURE;
	}

	param = (arg[1] == '-') ? jso_cli_parse_long_option(arg, arg_len, &value, options, ctx)
							: jso_cli_parse_short_option(arg, arg_len, &value, options, ctx);

	if (!param) {
		return JSO_FAILURE;
	}

	if (param->has_value) {
		if (!value) {
			if (i + 1 == argc || argv[i + 1][0] == '-') {
				JSO_IO_PRINTF(options->es, "No value for option: %s\n", param->long_name);
				return JSO_FAILURE;
			}
			value = &argv[i + 1][0];
			(*ip)++;
		}

		if (param->callback.callback_value(value, options) == JSO_FAILURE) {
			return JSO_FAILURE;
		}
	} else {
		if (value) {
			JSO_IO_PRINTF(options->es, "No value expected for option: %s\n", param->long_name);
			return JSO_FAILURE;
		}

		if (param->callback.callback_flag(options) == JSO_FAILURE) {
			return JSO_FAILURE;
		}
	}

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_cli_parse_args_ex(int argc, const char *argv[], jso_cli_ctx *ctx)
{
	int i;
	jso_rc rc;
	const char *file_path = NULL;
	jso_cli_options options;

	/* pre-initialize options */
	jso_cli_options_init_pre(&options);

	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			/* it is an options, so parse it */
			if (jso_cli_parse_option(&i, argc, argv, &options, ctx) == JSO_FAILURE) {
				return JSO_FAILURE;
			}
		} else if (file_path) {
			JSO_IO_PRINTF(options.es, "File path is already is set as %s\n", file_path);
			return JSO_FAILURE;
		} else {
			file_path = &argv[i][0];
		}
	}

	if (options.output_type == JSO_OUTPUT_HELP) {
		jso_cli_print_help(&options, ctx);
		return JSO_SUCCESS;
	}
	if (!file_path) {
		JSO_IO_PRINTF(options.es, "No file specified - use --help option for more information\n");
		return JSO_FAILURE;
	}

	/* post-initialize options */
	jso_cli_options_init_post(&options);

	/* parse file */
	rc = jso_cli_process_file(file_path, &options);

	/* destroy options */
	jso_cli_options_destroy(&options);

	return rc;
}

JSO_API jso_rc jso_cli_parse_args(int argc, const char *argv[])
{
	jso_cli_ctx ctx;

	jso_cli_register_default_params(&ctx);

	return jso_cli_parse_args_ex(argc, argv, &ctx);
}
