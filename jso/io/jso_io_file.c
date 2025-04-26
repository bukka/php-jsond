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
#include <sys/stat.h>
#include "jso_io_file.h"
#include "jso.h"

static size_t jso_io_file_read(jso_io *io, size_t size)
{
	size_t count, buffered;

	/* number of characters in the buffer that have not been processed yet */
	buffered = (size_t) (JSO_IO_LIMIT(io) - JSO_IO_CURSOR(io));

	/* already in the buffer */
	if (buffered >= size) {
		return size;
	}

	/* make sure that we have enough space in the buffer */
	if (jso_io_buffer_alloc(io, size) == JSO_FAILURE) {
		return 0;
	}

	/* read data from file */
	count = fread(JSO_IO_LIMIT(io), sizeof(jso_ctype), JSO_IO_SIZE(io) - buffered,
			JSO_IO_FILE_HANDLE_GET(io));
	if (count > 0) {
		JSO_IO_LIMIT(io) += count;
	}
	count += buffered;

	/* return count if it is smaller than size, otherwise size */
	return count < size ? count : size;
}

static size_t jso_io_file_write(jso_io *io, const jso_ctype *buffer, size_t size)
{
	return fwrite(buffer, sizeof(jso_ctype), size, JSO_IO_FILE_HANDLE_GET(io));
}

static int jso_io_file_printf(jso_io *io, const char *format, ...)
{
	va_list args;
	int written;

	va_start(args, format);
	written = vfprintf(JSO_IO_FILE_HANDLE_GET(io), format, args);
	va_end(args);

	return written;
}

static jso_rc jso_io_file_flush(jso_io *io)
{
	return fflush(JSO_IO_FILE_HANDLE_GET(io)) == 0 ? JSO_SUCCESS : JSO_FAILURE;
}

static int jso_io_file_error(jso_io *io)
{
	JSO_IO_ERRNO(io) = ferror(JSO_IO_FILE_HANDLE_GET(io));
	return JSO_IO_ERRNO(io);
}

static jso_rc jso_io_file_free(jso_io *io)
{
	jso_rc rc = jso_io_file_close(io);

	if (JSO_IO_BUFFER(io))
		jso_free(JSO_IO_BUFFER(io));
	jso_free(io);

	return rc;
}

JSO_API jso_io *jso_io_file_open_stream(FILE *fp)
{
	jso_io *io;

	if (!fp)
		return NULL;

	io = JSO_IO_ALLOC();
	JSO_IO_FILE_HANDLE_SET(io, fp);
	JSO_IO_OP(io, read) = jso_io_file_read;
	JSO_IO_OP(io, write) = jso_io_file_write;
	JSO_IO_OP(io, printf) = jso_io_file_printf;
	JSO_IO_OP(io, flush) = jso_io_file_flush;
	JSO_IO_OP(io, error) = jso_io_file_error;
	JSO_IO_OP(io, free) = jso_io_file_free;

	return io;
}

JSO_API jso_io *jso_io_file_open(const char *filename, const char *opentype)
{
	FILE *fp = fopen(filename, opentype);

	return jso_io_file_open_stream(fp);
}

JSO_API jso_rc jso_io_file_close_ex(jso_io *io, jso_bool close_std)
{
	int rc = 0;
	FILE *fp = JSO_IO_FILE_HANDLE_GET(io);

	if (fp && (close_std || (fp != stdout && fp != stdin && fp != stderr)))
		rc = fclose(fp);

	return rc == 0 ? JSO_SUCCESS : JSO_FAILURE;
}

JSO_API jso_rc jso_io_file_close(jso_io *io)
{
	return jso_io_file_close_ex(io, JSO_FALSE);
}

JSO_API off_t jso_io_file_size(const char *filename)
{
	struct stat sbuf;
	int sres = stat(filename, &sbuf);
	if (sres) {
		return -1;
	}
	return sbuf.st_size;
}
