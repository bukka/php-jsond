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
#define PHP_JSON_BUFFER_STATIC_SIZE 1024
/* size of extra allocation when extending buffer */
#define PHP_JSON_BUFFER_EXTRA_ALLOC_SIZE 10

typedef struct _php_json_buffer{
	char sbuf[PHP_JSON_BUFFER_STATIC_SIZE];
	char *dbuf;
	char *ptr;
	size_t dsize;
	int left;
} php_json_buffer; 

void php_json_buffer_init(php_json_buffer *buf);
void php_json_buffer_destroy(php_json_buffer *buf);
void php_json_buffer_flush(php_json_buffer *buf, size_t pre_alloc_size);
void php_json_buffer_append_char(php_json_buffer *buf, char c);
void php_json_buffer_append_stringl(php_json_buffer *buf, const char *str, size_t len);
void php_json_buffer_append_long(php_json_buffer *buf, long l);
void php_json_buffer_alloc(php_json_buffer *buf, size_t len);
char *php_json_buffer_get_block(php_json_buffer *buf, size_t len);

#define PHP_JSON_BUFFER_STRVAL(_buf) (_buf).dbuf
#define PHP_JSON_BUFFER_STRLEN(_buf) (_buf).dsize

#endif	/* PHP_JSOND_BUFFER_H */
