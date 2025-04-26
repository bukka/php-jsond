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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jso_io_memory.h"
#include "jso.h"

static size_t jso_io_memory_read(jso_io *io, size_t size)
{
	return 0;
}

static size_t jso_io_memory_write(jso_io *io, const jso_ctype *buffer, size_t size)
{
	size_t not_used = JSO_IO_SIZE(io) - (size_t) (JSO_IO_LIMIT(io) - JSO_IO_BUFFER(io));

	if (size > not_used && jso_io_buffer_alloc(io, size) == JSO_FAILURE) {
		return 0;
	}

	memcpy(JSO_IO_LIMIT(io), buffer, size * sizeof(jso_ctype));

	return size;
}

static int jso_io_memory_printf(jso_io *io, const char *format, ...)
{
	va_list args;
	int rsize;
	size_t not_used = JSO_IO_SIZE(io) - (size_t) (JSO_IO_LIMIT(io) - JSO_IO_BUFFER(io));

	va_start(args, format);
	rsize = vsnprintf((char *) JSO_IO_LIMIT(io), not_used, format, args);
	va_end(args);
	if (rsize > not_used) {
		jso_io_buffer_alloc(io, rsize + 1);
		return vsnprintf((char *) JSO_IO_LIMIT(io), rsize, format, args);
	}
	JSO_IO_LIMIT(io) += rsize;

	return rsize;
}

static jso_rc jso_io_memory_flush(jso_io *io)
{
	return JSO_SUCCESS;
}

static int jso_io_memory_error(jso_io *io)
{
	return 0;
}

static jso_rc jso_io_memory_free(jso_io *io)
{
	jso_free(JSO_IO_BUFFER(io));
	jso_free(io);

	return JSO_SUCCESS;
}

JSO_API jso_io *jso_io_memory_open_ex(size_t size, int flags)
{
	jso_io *io;

	/* alloc io and set ops */
	io = JSO_IO_ALLOC();
	JSO_IO_OP(io, read) = jso_io_memory_read;
	JSO_IO_OP(io, write) = jso_io_memory_write;
	JSO_IO_OP(io, printf) = jso_io_memory_printf;
	JSO_IO_OP(io, flush) = jso_io_memory_flush;
	JSO_IO_OP(io, error) = jso_io_memory_error;
	JSO_IO_OP(io, free) = jso_io_memory_free;

	/* pre-allocate buffer */
	jso_io_buffer_alloc(io, size);

	return io;
}

JSO_API jso_io *jso_io_momory_open()
{
	return jso_io_memory_open_ex(JSO_IO_MEMORY_BUFF_SIZE, 0);
}
