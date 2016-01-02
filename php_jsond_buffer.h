/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
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

#define PHP_JSON_NOOP ((void) 0)

#ifdef PHP_JSOND_BUF_TYPE_NATIVE

/* static buffer size */
#define PHP_JSON_BUFFER_STATIC_SIZE 2048
/* size of extra allocation when extending buffer */
#define PHP_JSON_BUFFER_EXTRA_ALLOC_SIZE 0

/* flags */
#define PHP_JSON_BUFFER_FLAG_MARK_DBUF 1

typedef struct _php_json_buffer{
	char sbuf[PHP_JSON_BUFFER_STATIC_SIZE];
	char *dbuf;
	char *ptr;
	char *end;
	char *mark;
	size_t dsize;
	int flags;
} php_json_buffer;

void php_json_buffer_init(php_json_buffer *buf);
void php_json_buffer_destroy(php_json_buffer *buf);
void php_json_buffer_flush(php_json_buffer *buf, size_t pre_alloc_size);
void php_json_buffer_finish(php_json_buffer *buf);
void php_json_buffer_alloc(php_json_buffer *buf, size_t len);
void php_json_buffer_reset(php_json_buffer *buf);

static inline void php_json_buffer_append_stringl(php_json_buffer *buf, const char *str, size_t len)
{
	char *new_ptr = buf->ptr + len;
	if (new_ptr <= buf->end) {
		memcpy(buf->ptr, str, len);
		buf->ptr = new_ptr;
	} else {
		php_json_buffer_flush(buf, len + PHP_JSON_BUFFER_EXTRA_ALLOC_SIZE);
		memcpy(buf->dbuf + buf->dsize, str, len);
		buf->dsize += len;
	}
}

static inline void php_json_buffer_append_char(php_json_buffer *buf, char c) /* {{{ */
{
	if (buf->ptr > buf->end) {
		php_json_buffer_flush(buf, PHP_JSON_BUFFER_EXTRA_ALLOC_SIZE);
	}
	*buf->ptr = c;
	buf->ptr++;
}
/* }}} */

#define PHP_JSON_INT_BUFFER_SIZE 32

static inline void php_json_buffer_append_long(php_json_buffer *buf, long l) /* {{{ */
{
	char str[PHP_JSON_INT_BUFFER_SIZE];
	char *p = &str[PHP_JSON_INT_BUFFER_SIZE];
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

	php_json_buffer_append_stringl(buf, p, &str[PHP_JSON_INT_BUFFER_SIZE] - p);
}
/* }}} */

static inline char *php_json_buffer_block_open(php_json_buffer *buf, size_t len) /* {{{ */
{
	if (buf->ptr + len > buf->end) {
		php_json_buffer_flush(buf, PHP_JSON_BUFFER_EXTRA_ALLOC_SIZE);
	}
	return buf->ptr;
}
/* }}} */

static inline void php_json_buffer_block_close(php_json_buffer *buf, size_t len) /* {{{ */
{
	buf->ptr += len;
}
/* }}} */

static inline void php_json_buffer_mark_set(php_json_buffer *buf) /* {{{ */
{
	buf->mark = buf->ptr;
}
/* }}} */

static inline void php_json_buffer_mark_del(php_json_buffer *buf) /* {{{ */
{
	buf->mark = NULL;
	buf->flags &= ~PHP_JSON_BUFFER_FLAG_MARK_DBUF;
}
/* }}} */

#define PHP_JSON_BUFFER_STRVAL(_buf) (_buf).dbuf
#define PHP_JSON_BUFFER_STRLEN(_buf) (_buf).dsize

#define PHP_JSON_BUF_INIT(_buf) \
	php_json_buffer_init(_buf)

#define PHP_JSON_BUF_DESTROY(_buf) \
	 php_json_buffer_destroy(_buf)

#define PHP_JSON_BUF_FINISH(_buf) \
	 php_json_buffer_finish(_buf)

#define PHP_JSON_BUF_FLUSH(_buf, _pre_alloc_size) \
	php_json_buffer_flush(_buf, _pre_alloc_size)

#define PHP_JSON_BUF_ALLOC(_buf, _len) \
	php_json_buffer_alloc(_buf, _len)

#define PHP_JSON_BUF_APPEND_STRING(_buf, _str, _len) \
	php_json_buffer_append_stringl(_buf, _str, _len)

#define PHP_JSON_BUF_APPEND_CHAR(_buf, _c) \
	php_json_buffer_append_char(_buf, _c)

#define PHP_JSON_BUF_APPEND_LONG(_buf, _l) \
	php_json_buffer_append_long(_buf, _l)

#define PHP_JSON_BUF_DOUBLE_BLOCK_INIT(_buf, _dst, _max_len) \
	char *_dst = php_json_buffer_block_open(_buf, MIN(EG(precision) + MAX_LENGTH_OF_DOUBLE, _max_len) + 1)

#define PHP_JSON_BUF_DOUBLE_BLOCK_CLOSE(_buf, _dst, _len) \
	php_json_buffer_block_close(_buf, _len)

#define PHP_JSON_BUF_MARK_DECLARE(_buf) \
	PHP_JSON_NOOP

#define PHP_JSON_BUF_MARK_SET(_buf) \
	php_json_buffer_mark_set(_buf)

#define PHP_JSON_BUF_MARK_DELETE(_buf) \
	php_json_buffer_mark_del(_buf)

#define PHP_JSON_BUF_RESET(_buf) \
	php_json_buffer_reset(_buf)

#define PHP_JSON_BUF_LENGTH PHP_JSON_BUFFER_STRLEN

#define PHP_JSON_BUF_RETURN(_buf_s, return_value) do { \
		PHP_JSON_BUF_FINISH(&_buf_s); \
		ZVAL_STRINGL(return_value, PHP_JSON_BUFFER_STRVAL(_buf_s), (int) PHP_JSON_BUFFER_STRLEN(_buf_s), 0); \
	} while(0)


#else

#include "ext/standard/php_smart_str.h"

#define php_json_buffer smart_str

#define PHP_JSON_BUF_INIT(_buf) \
	memset(_buf, 0, sizeof(smart_str))

#define PHP_JSON_BUF_DESTROY(_buf) \
	smart_str_free(_buf);

#define PHP_JSON_BUF_FLUSH(_buf, _pre_alloc_size) \
	php_json_buffer_flush(_buf, _pre_alloc_size)

#define PHP_JSON_BUF_FINISH(_buf)

#define PHP_JSON_BUF_ALLOC(_buf, _len) \
	smart_str_alloc(_buf, _len+2, 0)

#define PHP_JSON_BUF_APPEND_STRING(_buf, _str, _len) \
	smart_str_appendl(_buf, _str, _len)

#define PHP_JSON_BUF_APPEND_CHAR(_buf, _c) \
	smart_str_appendc(_buf, _c)

#define PHP_JSON_BUF_APPEND_LONG(_buf, _l) \
	smart_str_append_long(_buf, _l)

#define PHP_JSON_BUF_DOUBLE_BLOCK_INIT(_buf, _dst, _max_len) \
	char _dst[_max_len]

#define PHP_JSON_BUF_DOUBLE_BLOCK_CLOSE(_buf, _dst, _len) \
	smart_str_appendl(_buf, _dst, _len);

#define _PHP_JSON_BUF_MARK_NAME(_buf) _buf##__oldlen

#define PHP_JSON_BUF_MARK_DECLARE(_buf) \
	size_t _PHP_JSON_BUF_MARK_NAME(_buf), newlen

#define PHP_JSON_BUF_MARK_SET(_buf) \
	_PHP_JSON_BUF_MARK_NAME(_buf) = _buf->len

#define PHP_JSON_BUF_MARK_DELETE(_buf) \
	PHP_JSON_NOOP

#define PHP_JSON_BUF_RESET(_buf) \
	buf->len = _PHP_JSON_BUF_MARK_NAME(_buf)

#define PHP_JSON_BUF_LENGTH(_buf_s) _buf_s.len

#define PHP_JSON_BUF_RETURN(_buf_s, return_value) do { \
		ZVAL_STRINGL(return_value, _buf_s.c, _buf_s.len, 1); \
		PHP_JSON_BUF_DESTROY(&_buf_s); \
	} while(0)

#endif /* PHP_JSON_BUF_TYPE_NATIVE */

#endif	/* PHP_JSOND_BUFFER_H */
