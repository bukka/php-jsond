/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Jakub Zelenka <bukka@php.net>                                |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_JSOND_BUFFER_H
#define	PHP_JSOND_BUFFER_H

#include "php.h"

#define PHP_JSOND_NOOP ((void) 0)

#ifdef PHP_JSOND_BUF_TYPE_NATIVE

/* static buffer size */
#define PHP_JSOND_BUFFER_STATIC_SIZE 2048
/* size of extra allocation when extending buffer */
#define PHP_JSOND_BUFFER_EXTRA_ALLOC_SIZE 0

/* flags */
#define PHP_JSOND_BUFFER_FLAG_MARK_DBUF 1

typedef struct _php_jsond_buffer{
	char sbuf[PHP_JSOND_BUFFER_STATIC_SIZE];
	char *dbuf;
	char *ptr;
	char *end;
	char *mark;
	size_t dsize;
	int flags;
} php_jsond_buffer;

void php_jsond_buffer_init(php_jsond_buffer *buf);
void php_jsond_buffer_destroy(php_jsond_buffer *buf);
void php_jsond_buffer_flush(php_jsond_buffer *buf, size_t pre_alloc_size);
void php_jsond_buffer_finish(php_jsond_buffer *buf);
void php_jsond_buffer_alloc(php_jsond_buffer *buf, size_t len);
void php_jsond_buffer_reset(php_jsond_buffer *buf);

static inline void php_jsond_buffer_append_stringl(php_jsond_buffer *buf, const char *str, size_t len)
{
	char *new_ptr = buf->ptr + len;
	if (new_ptr <= buf->end) {
		memcpy(buf->ptr, str, len);
		buf->ptr = new_ptr;
	} else {
		php_jsond_buffer_flush(buf, len + PHP_JSOND_BUFFER_EXTRA_ALLOC_SIZE);
		memcpy(buf->dbuf + buf->dsize, str, len);
		buf->dsize += len;
	}
}

static inline void php_jsond_buffer_append_char(php_jsond_buffer *buf, char c) /* {{{ */
{
	if (buf->ptr > buf->end) {
		php_jsond_buffer_flush(buf, PHP_JSOND_BUFFER_EXTRA_ALLOC_SIZE);
	}
	*buf->ptr = c;
	buf->ptr++;
}
/* }}} */

#define PHP_JSOND_INT_BUFFER_SIZE 32

static inline void php_jsond_buffer_append_long(php_jsond_buffer *buf, long l) /* {{{ */
{
	char str[PHP_JSOND_INT_BUFFER_SIZE];
	char *p = &str[PHP_JSOND_INT_BUFFER_SIZE];
	zend_bool negative;

	if (l < 0) {
		negative = 1;
		l = -l;
	} else {
		negative = 0;
	}

	do {
		*--p = (char) (l % 10) + '0';
		l /= 10;
	} while (l > 0);

	if (negative) {
		*--p = '-';
	}

	php_jsond_buffer_append_stringl(buf, p, &str[PHP_JSOND_INT_BUFFER_SIZE] - p);
}
/* }}} */

static inline char *php_jsond_buffer_block_open(php_jsond_buffer *buf, size_t len) /* {{{ */
{
	if (buf->ptr + len > buf->end) {
		php_jsond_buffer_flush(buf, PHP_JSOND_BUFFER_EXTRA_ALLOC_SIZE);
	}
	return buf->ptr;
}
/* }}} */

static inline void php_jsond_buffer_block_close(php_jsond_buffer *buf, size_t len) /* {{{ */
{
	buf->ptr += len;
}
/* }}} */

static inline void php_jsond_buffer_mark_set(php_jsond_buffer *buf) /* {{{ */
{
	buf->mark = buf->ptr;
}
/* }}} */

static inline void php_jsond_buffer_mark_del(php_jsond_buffer *buf) /* {{{ */
{
	buf->mark = NULL;
	buf->flags &= ~PHP_JSOND_BUFFER_FLAG_MARK_DBUF;
}
/* }}} */

#define PHP_JSOND_BUFFER_STRVAL(_buf) (_buf).dbuf
#define PHP_JSOND_BUFFER_STRLEN(_buf) (_buf).dsize

#define PHP_JSOND_BUF_PTR(_buf) (_buf)->ptr

#define PHP_JSOND_BUF_CHOP(_buf, _n) \
	ZEND_ASSERT((_buf)->ptr - _n > &(_buf)->sbuf[0]); \
	(_buf)->ptr -= _n

#define PHP_JSOND_BUF_INIT(_buf) \
	php_jsond_buffer_init(_buf)

#define PHP_JSOND_BUF_DESTROY(_buf) \
	 php_jsond_buffer_destroy(_buf)

#define PHP_JSOND_BUF_FINISH(_buf) \
	 php_jsond_buffer_finish(_buf)

#define PHP_JSOND_BUF_FLUSH(_buf, _pre_alloc_size) \
	php_jsond_buffer_flush(_buf, _pre_alloc_size)

#define PHP_JSOND_BUF_ALLOC(_buf, _len) \
	php_jsond_buffer_alloc(_buf, _len)

#define PHP_JSOND_BUF_APPEND_STRING(_buf, _str, _len) \
	php_jsond_buffer_append_stringl(_buf, _str, _len)

#define PHP_JSOND_BUF_APPEND_CHAR(_buf, _c) \
	php_jsond_buffer_append_char(_buf, _c)

#define PHP_JSOND_BUF_APPEND_LONG(_buf, _l) \
	php_jsond_buffer_append_long(_buf, _l)

#define PHP_JSOND_BUF_DOUBLE_BLOCK_INIT(_buf, _dst, _max_len) \
	char *_dst = php_jsond_buffer_block_open(\
		_buf, MIN(PG(serialize_precision) + MAX_LENGTH_OF_DOUBLE, _max_len) + 1)

#define PHP_JSOND_BUF_DOUBLE_BLOCK_CLOSE(_buf, _dst, _len) \
	php_jsond_buffer_block_close(_buf, _len)

#define PHP_JSOND_BUF_MARK_DECLARE(_buf) \
	PHP_JSOND_NOOP

#define PHP_JSOND_BUF_MARK_SET(_buf) \
	php_jsond_buffer_mark_set(_buf)

#define PHP_JSOND_BUF_MARK_DELETE(_buf) \
	php_jsond_buffer_mark_del(_buf)

#define PHP_JSOND_BUF_RESET(_buf) \
	php_jsond_buffer_reset(_buf)

#define PHP_JSOND_BUF_LENGTH PHP_JSOND_BUFFER_STRLEN

#define PHP_JSOND_BUF_RETURN(_buf_s, return_value) do { \
		PHP_JSOND_BUF_FINISH(&_buf_s); \
		RETVAL_STRINGL(PHP_JSOND_BUFFER_STRVAL(_buf_s), (int) PHP_JSOND_BUFFER_STRLEN(_buf_s)); \
		PHP_JSOND_BUF_DESTROY(&_buf_s); \
	} while(0)


#else

#include "zend_smart_string.h"

#define php_jsond_buffer smart_string

#define PHP_JSOND_BUF_PTR(_buf) (_buf)->c

#define PHP_JSOND_BUF_CHOP(_buf, _n) \
	(_buf)->len -= _n

#define PHP_JSOND_BUF_INIT(_buf) \
	memset(_buf, 0, sizeof(smart_string))

#define PHP_JSOND_BUF_DESTROY(_buf) \
	smart_string_free(_buf);

#define PHP_JSOND_BUF_FLUSH(_buf, _pre_alloc_size)

#define PHP_JSOND_BUF_FINISH(_buf)

#define PHP_JSOND_BUF_ALLOC(_buf, _len) \
	smart_string_alloc(_buf, _len+2, 0)

#define PHP_JSOND_BUF_APPEND_STRING(_buf, _str, _len) \
	smart_string_appendl(_buf, _str, _len)

#define PHP_JSOND_BUF_APPEND_CHAR(_buf, _c) \
	smart_string_appendc(_buf, _c)

#define PHP_JSOND_BUF_APPEND_LONG(_buf, _l) \
	smart_string_append_long(_buf, _l)

#define PHP_JSOND_BUF_DOUBLE_BLOCK_INIT(_buf, _dst, _max_len) \
	char _dst[_max_len]

#define PHP_JSOND_BUF_DOUBLE_BLOCK_CLOSE(_buf, _dst, _len) \
	smart_string_appendl(_buf, _dst, _len);

#define _PHP_JSOND_BUF_MARK_NAME(_buf) _buf##__oldlen

#define PHP_JSOND_BUF_MARK_DECLARE(_buf) \
	size_t _PHP_JSOND_BUF_MARK_NAME(_buf), newlen

#define PHP_JSOND_BUF_MARK_SET(_buf) \
	_PHP_JSOND_BUF_MARK_NAME(_buf) = _buf->len

#define PHP_JSOND_BUF_MARK_DELETE(_buf) \
	PHP_JSOND_NOOP

#define PHP_JSOND_BUF_RESET(_buf) \
	buf->len = _PHP_JSOND_BUF_MARK_NAME(_buf)

#define PHP_JSOND_BUF_LENGTH(_buf_s) _buf_s.len

#define PHP_JSOND_BUF_RETURN(_buf_s, return_value) do { \
		if (_buf_s.c) { \
			RETVAL_STRINGL(_buf_s.c, _buf_s.len); \
		} else { \
			RETVAL_EMPTY_STRING(); \
		} \
		PHP_JSOND_BUF_DESTROY(&_buf_s); \
	} while(0)

#endif /* PHP_JSOND_BUF_TYPE_NATIVE */

#endif	/* PHP_JSOND_BUFFER_H */
