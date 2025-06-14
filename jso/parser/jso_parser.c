/*
 * Copyright (c) 2025 Jakub Zelenka. All rights reserved.
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

#include "../jso_parser.h"
#include "../jso_parser_hooks.h"
#include "../io/jso_io_string.h"

JSO_API void jso_parser_init_ex(jso_parser *parser, const jso_parser_hooks *hooks)
{
	memset(parser, 0, sizeof(jso_parser));
	memcpy(&parser->hooks, hooks, sizeof(jso_parser_hooks));
}

JSO_API void jso_parser_init(jso_parser *parser)
{
	jso_parser_init_ex(parser, jso_parser_hooks_decode());
}

JSO_API jso_rc jso_parse_io(jso_io *io, jso_parser_options *options, jso_value *result)
{
	jso_rc rc;
	jso_parser parser;

	/* init scanner */
	jso_parser_init(&parser);
	jso_scanner_init(&parser.scanner, io);

	/* set max depth (0 = unlimited) */
	parser.max_depth = options->max_depth;

	/* parse */
	if (jso_yyparse(&parser) == 0) {
		rc = JSO_SUCCESS;
	} else {
		rc = JSO_FAILURE;
	}

	*result = parser.result;

	return rc;
}

JSO_API jso_rc jso_parse_cstr(
		const char *cstr, size_t len, jso_parser_options *options, jso_value *result)
{
	jso_io *io = jso_io_string_open_from_cstr(cstr, len);
	jso_rc rc = jso_parse_io(io, options, result);
	JSO_IO_FREE(io);
	return rc;
}
