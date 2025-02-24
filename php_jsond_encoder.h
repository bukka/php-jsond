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

#ifndef PHP_JSOND_ENCODER_H
#define	PHP_JSOND_ENCODER_H

#include "php.h"
#include "php_jsond_buffer.h"

typedef struct _php_jsond_encoder php_jsond_encoder;

struct _php_jsond_encoder {
	int depth;
	int max_depth;
	php_jsond_error_code error_code;
};

static inline void php_jsond_encode_init(php_jsond_encoder *encoder)
{
	memset(encoder, 0, sizeof(php_jsond_encoder));
}

int php_jsond_encode_zval(php_jsond_buffer *buf, zval *val, int options, php_jsond_encoder *encoder);

#endif	/* PHP_JSOND_ENCODER_H */

