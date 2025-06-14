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

#include <stdlib.h>
#include "jso_io_string.h"
#include "jso.h"

static size_t jso_io_string_read(jso_io *io, size_t size)
{
	return 0;
}

static size_t jso_io_string_write(jso_io *io, const jso_ctype *buffer, size_t size)
{
	return 0;
}

static int jso_io_string_printf(jso_io *io, const char *format, ...)
{
	return 0;
}

static jso_rc jso_io_string_flush(jso_io *io)
{
	return JSO_SUCCESS;
}

static int jso_io_string_error(jso_io *io)
{
	return 0;
}

static jso_rc jso_io_string_free(jso_io *io)
{
	jso_free(io);

	return JSO_SUCCESS;
}

static jso_io *jso_io_string_open(jso_ctype *buf, size_t size)
{
	jso_io *io;

	/* alloc io and set ops */
	io = JSO_IO_ALLOC();
	JSO_IO_OP(io, read) = jso_io_string_read;
	JSO_IO_OP(io, write) = jso_io_string_write;
	JSO_IO_OP(io, printf) = jso_io_string_printf;
	JSO_IO_OP(io, flush) = jso_io_string_flush;
	JSO_IO_OP(io, error) = jso_io_string_error;
	JSO_IO_OP(io, free) = jso_io_string_free;

	/* pre-allocate buffer */
	jso_io_buffer_init(io, buf, size);

	return io;
}

JSO_API jso_io *jso_io_string_open_from_cstr(const char *cstr, size_t len)
{
	return jso_io_string_open((jso_ctype *) cstr, len);
}

/**
 * Open string IO for the supplied string
 * @param str the string
 * @return New IO.
 */
JSO_API jso_io *jso_io_string_open_from_str(jso_string *str)
{
	return jso_io_string_open(JSO_STRING_VAL(str), JSO_STRING_LEN(str));
}
