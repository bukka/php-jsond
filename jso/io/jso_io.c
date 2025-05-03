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

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "jso_io.h"
#include "jso.h"

/**
 * @brief IO Buffer pointers differences struct
 */
typedef struct _jso_io_buffer_diffs {
	/** token ptr difference from buffer ptr */
	ptrdiff_t token;
	/** cursor ptr difference from buffer ptr */
	ptrdiff_t cursor;
	/** limit ptr difference from buffer ptr */
	ptrdiff_t limit;
	/** marker ptr difference from buffer ptr */
	ptrdiff_t marker;
	/** ctxmarker ptr difference from buffer ptr */
	ptrdiff_t ctxmarker;
	/** str_start ptr difference from buffer ptr */
	ptrdiff_t str_start;
} jso_io_buffer_diffs;

static void jso_io_pointers_set(jso_io *io, jso_ctype *pos)
{
	JSO_IO_CURSOR(io) = pos;
	JSO_IO_TOKEN(io) = pos;
	JSO_IO_MARKER(io) = pos;
	JSO_IO_CTXMARKER(io) = pos;
	JSO_IO_STR_SET_START(io);
}

static void jso_io_buffer_diffs_save(jso_io *io, jso_io_buffer_diffs *diffs)
{
	diffs->token = JSO_IO_TOKEN(io) - JSO_IO_BUFFER(io);
	diffs->cursor = JSO_IO_CURSOR(io) - JSO_IO_BUFFER(io);
	diffs->limit = JSO_IO_LIMIT(io) - JSO_IO_BUFFER(io);
	diffs->marker = JSO_IO_MARKER(io) - JSO_IO_BUFFER(io);
	diffs->ctxmarker = JSO_IO_CTXMARKER(io) - JSO_IO_BUFFER(io);
	diffs->str_start = JSO_IO_STR_GET_START(io) - JSO_IO_BUFFER(io);
}

static void jso_io_buffer_diffs_load(jso_io *io, jso_io_buffer_diffs *diffs)
{
	JSO_IO_TOKEN(io) = JSO_IO_BUFFER(io) + diffs->token;
	JSO_IO_CURSOR(io) = JSO_IO_BUFFER(io) + diffs->cursor;
	JSO_IO_LIMIT(io) = JSO_IO_BUFFER(io) + diffs->limit;
	JSO_IO_MARKER(io) = JSO_IO_BUFFER(io) + diffs->marker;
	JSO_IO_CTXMARKER(io) = JSO_IO_BUFFER(io) + diffs->ctxmarker;
	JSO_IO_STR_GET_START(io) = JSO_IO_BUFFER(io) + diffs->str_start;
}

static jso_rc jso_io_buffer_alloc_new(jso_io *io, size_t size)
{
	jso_ctype *buf;

	/* use calloc to suppress valgrind warnings */
	buf = (jso_ctype *) jso_calloc(size + 1, sizeof(jso_ctype));

	if (!buf) {
		return JSO_FAILURE;
	}

	JSO_IO_SIZE(io) = size;
	JSO_IO_BUFFER(io) = buf;
	JSO_IO_LIMIT(io) = buf;
	jso_io_pointers_set(io, buf);

	return JSO_SUCCESS;
}

static jso_rc jso_io_buffer_alloc_extend(jso_io *io, size_t size)
{
	jso_io_buffer_diffs diffs;

	jso_io_buffer_diffs_save(io, &diffs);
	JSO_IO_SIZE(io) = JSO_MAX(JSO_IO_SIZE(io) * 2, size + 1);
	JSO_IO_BUFFER(io)
			= (jso_ctype *) jso_realloc(JSO_IO_BUFFER(io), JSO_IO_SIZE(io) * sizeof(jso_ctype));
	if (!JSO_IO_BUFFER(io)) {
		return JSO_FAILURE;
	}
	jso_io_buffer_diffs_load(io, &diffs);

	return JSO_SUCCESS;
}

static jso_rc jso_io_buffer_alloc_rotate(jso_io *io, size_t size)
{
	ptrdiff_t buffered = JSO_IO_LIMIT(io) - JSO_IO_CURSOR(io);
	ptrdiff_t processed = JSO_IO_CURSOR(io) - JSO_IO_BUFFER(io);

	if (JSO_IO_SIZE(io) - buffered < size
			&& jso_io_buffer_alloc_extend(io, size + buffered) == JSO_FAILURE) {
		JSO_IO_ERROR_CODE(io) = -1;
		return JSO_FAILURE;
	}
	memmove(JSO_IO_BUFFER(io), JSO_IO_CURSOR(io), buffered * sizeof(jso_ctype));

	JSO_IO_TOKEN(io) -= processed;
	JSO_IO_CURSOR(io) -= processed;
	JSO_IO_LIMIT(io) -= processed;
	JSO_IO_MARKER(io) -= processed;
	JSO_IO_CTXMARKER(io) -= processed;
	JSO_IO_STR_GET_START(io) -= processed;

	return JSO_SUCCESS;
}

JSO_API jso_rc jso_io_buffer_alloc_ex(jso_io *io, size_t size, int flags)
{
	size_t not_used;
	int strategy;

	if (!JSO_IO_BUFFER(io)) {
		return jso_io_buffer_alloc_new(io, size);
	}

	not_used = JSO_IO_SIZE(io) - (size_t) (JSO_IO_LIMIT(io) - JSO_IO_BUFFER(io));
	if (not_used <= size) {
		/* There is enough space in the buffer
		 * so we don't need to do anything. */
		return JSO_SUCCESS;
	}

	strategy = flags & JSO_IO_BUFFER_ALLOC_STRATEGY_MASK;
	if (strategy == JSO_IO_BUFFER_ALLOC_STRATEGY_AUTO) {
		/* Use ROTATE strategy if more than half buffer
		 * has been read, otherwise use EXTEND strategy. */
		strategy = (JSO_IO_CURSOR(io) - JSO_IO_BUFFER(io) > JSO_IO_SIZE(io) / 2)
				? JSO_IO_BUFFER_ALLOC_STRATEGY_ROTATE
				: JSO_IO_BUFFER_ALLOC_STRATEGY_EXTEND;
	}

	switch (strategy) {
		case JSO_IO_BUFFER_ALLOC_STRATEGY_ROTATE:
			return jso_io_buffer_alloc_rotate(io, size);
		case JSO_IO_BUFFER_ALLOC_STRATEGY_EXTEND:
			return jso_io_buffer_alloc_extend(io, size);
		default:
			return JSO_FAILURE;
	}
}

JSO_API jso_rc jso_io_buffer_alloc(jso_io *io, size_t size)
{
	return jso_io_buffer_alloc_ex(io, size, JSO_IO_BUFFER_ALLOC_STRATEGY_AUTO);
}

JSO_API jso_rc jso_io_pipe(jso_io *src_io, jso_io *dst_io)
{
	size_t read = (size_t) (JSO_IO_LIMIT(src_io) - JSO_IO_CURSOR(src_io));

	do {
		jso_io_buffer_alloc(dst_io, read);
		memcpy(JSO_IO_LIMIT(dst_io), JSO_IO_CURSOR(src_io), read * sizeof(jso_ctype));
		JSO_IO_SIZE(src_io) = 0;
		jso_io_pointers_set(src_io, JSO_IO_LIMIT(src_io));
	} while ((read = JSO_IO_READ(src_io, JSO_IO_PIPE_BUFFER)) > 0);

	return JSO_IO_ERROR(src_io) || JSO_IO_ERROR(dst_io) ? JSO_FAILURE : JSO_SUCCESS;
}
