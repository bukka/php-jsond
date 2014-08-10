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

#define PHP_JSON_BUFFER_SIZE 1024

#include <stdlib.h>

typedef struct _php_json_buffer{
	char *start;
	char *ptr;
	size_t left;
} php_json_buffer; 

#define PHP_JSON_BUFFER_APPEND_CHAR(buf, c)
#define PHP_JSON_BUFFER_APPEND_LONG(buf, l)
#define PHP_JSON_BUFFER_APPEND_STRL(buf, s, len)

#endif	/* PHP_JSOND_BUFFER_H */

