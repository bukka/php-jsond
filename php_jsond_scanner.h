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

#ifndef PHP_JSOND_SCANNER_H
#define	PHP_JSOND_SCANNER_H

#include "php.h"
#include "php_jsond.h"

typedef struct _php_json_scanner {
	char *buffer;                   /* the position of  first character in the buffer */
	char *cursor;                   /* cursor position */
	char *token;                    /* token position */
	char *limit;                    /* the last read character + 1 position */
	char *marker;                   /* marker position for backtracking */
	char *ctxmarker;                /* marker position for context backtracking */
	char *str_start;                /* start position of the string */
	char *pstr;                     /* string pointer for escapes conversion */
	int str_esc;                    /* number of extra characters for escaping */
	int size;                       /* size of the buffer */
	int state;                      /* condition state */             
	zval value;                     /* value */
	php_json_error_code errcode;    /* error type if there is an error */
} php_json_scanner;


void php_json_scanner_init(php_json_scanner *scanner);
int php_json_scan(php_json_scanner *s);

#endif	/* PHP_JSOND_SCANNER_H */

