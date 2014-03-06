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

#ifndef PHP_JSOND_PARSER_H
#define	PHP_JSOND_PARSER_H

#include "php.h"
#include "php_jsond_scanner.h"

typedef struct _php_json_parser {
	php_json_scanner scanner;
	zval result;
	long depth;
	long max_depth;
	unsigned int options;
#if ZTS
	void *zts_ctx;
#endif
} php_json_parser;

void php_json_parser_init(php_json_parser *parser TSRMLS_DC);

int php_json_yyparse(php_json_parser *parser);

#endif	/* PHP_JSOND_PARSER_H */

