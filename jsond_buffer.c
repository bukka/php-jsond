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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef PHP_JSOND_BUF_TYPE_NATIVE

#include <stddef.h>

#include "php_jsond_buffer.h"
#include "php_jsond.h"

void php_json_buffer_init(php_json_buffer *buf) /* {{{ */
{
	buf->dbuf = NULL;
	buf->ptr = &buf->sbuf[0];
	buf->end = &buf->sbuf[PHP_JSON_BUFFER_STATIC_SIZE - 1];
	buf->dsize = 0;
	buf->mark = NULL;
	buf->flags = 0;
}
/* }}} */

void php_json_buffer_destroy(php_json_buffer *buf) /* {{{ */
{
	if (buf->dbuf) {
		efree(buf->dbuf);
	}
}
/* }}} */

void php_json_buffer_flush(php_json_buffer *buf, size_t pre_alloc_size) /* {{{ */
{
	ptrdiff_t static_size = buf->ptr - &buf->sbuf[0];
	size_t size = static_size + pre_alloc_size;
	if (buf->dbuf) {
		buf->dbuf = erealloc(buf->dbuf, buf->dsize + size);
	} else {
		buf->dbuf = emalloc(size);
	}
	memcpy(buf->dbuf + buf->dsize, &buf->sbuf[0], static_size);
	/* mark dynamic buffer if mark set and is not already dynamic */
	if (buf->mark && !(buf->flags & PHP_JSON_BUFFER_FLAG_MARK_DBUF)) {
		buf->mark = &buf->dbuf[buf->dsize + (buf->mark - &buf->sbuf[0])];
		buf->flags |= PHP_JSON_BUFFER_FLAG_MARK_DBUF;
	}
	buf->dsize += static_size;
	buf->ptr = &buf->sbuf[0];
}
/* }}} */

void php_json_buffer_finish(php_json_buffer *buf) /* {{{ */
{
	php_json_buffer_flush(buf, 1);
	buf->dbuf[buf->dsize] = 0;
}
/* }}} */

void php_json_buffer_alloc(php_json_buffer *buf, size_t len) /* {{{ */
{

}
/* }}} */

void php_json_buffer_reset(php_json_buffer *buf) /* {{{ */
{
	if (!buf->mark) {
		if (buf->dbuf) {
			efree(buf->dbuf);
		}
		php_json_buffer_init(buf);
	} else {
		if (buf->flags & PHP_JSON_BUFFER_FLAG_MARK_DBUF) {
			buf->dsize = buf->mark - buf->dbuf;
			buf->dbuf = erealloc(buf->dbuf, buf->dsize);
			buf->ptr = &buf->sbuf[0];
		} else {
			buf->ptr = buf->mark;
		}
	}
}
/* }}} */

#endif /* PHP_JSON_BUF_TYPE_NATIVE */
