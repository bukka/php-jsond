/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
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
	char *new_ptr = buf->ptr + 1;
	if (new_ptr > buf->end) {
		php_json_buffer_flush(buf, PHP_JSON_BUFFER_EXTRA_ALLOC_SIZE);
	}
	*buf->ptr = c;
	buf->ptr = new_ptr;
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

#endif	/* PHP_JSOND_BUFFER_H */
