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

/* $Id$ */

#ifndef PHP_JSOND_H
#define PHP_JSOND_H

#define PHP_JSOND_VERSION "1.3.0"

extern zend_module_entry jsond_module_entry;
#define phpext_jsond_ptr &jsond_module_entry

#if defined(PHP_WIN32) && defined(JSOND_EXPORTS)
#define PHP_JSOND_API __declspec(dllexport)
#else
#define PHP_JSOND_API PHPAPI
#endif

#include "php.h"
#include "phpc/phpc.h"

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef PHP_JSOND_PREFIXING
#define PHP_JSOND_PREFIX json
#define PHP_JSOND_PREFIX_STRING "json"
#define PHP_JSOND_CONSTANT "JSON"
#define PHP_JSOND_SERIALIZABLE_INTERFACE JsonSerializable
#define PHP_JSOND_SERIALIZABLE_INTERFACE_STRING "JsonSerializable"
#define PHP_JSOND_SERIALIZABLE_INTERFACE_STRING_LC "jsonserializable"
#define PHP_JSOND_NAME(name) php_json_ ## name
#define PHP_JSOND_IDENT(name) json_ ## name
#define PHP_JSOND_FN(jname) ZEND_FN(json_ ## jname)
#else
#define PHP_JSOND_PREFIX jsond
#define PHP_JSOND_PREFIX_STRING "jsond"
#define PHP_JSOND_CONSTANT "JSOND"
#define PHP_JSOND_SERIALIZABLE_INTERFACE JsondSerializable
#define PHP_JSOND_SERIALIZABLE_INTERFACE_STRING "JsondSerializable"
#define PHP_JSOND_SERIALIZABLE_INTERFACE_STRING_LC "jsondserializable"
#define PHP_JSOND_NAME(jname) php_jsond_ ## jname
#define PHP_JSOND_IDENT(jname) jsond_ ## jname
#define PHP_JSOND_FN(jname) ZEND_FN(jsond_ ## jname)
#endif

#define PHP_JSOND_FUNCTION(jname) PHP_FUNCTION(PHP_JSOND_IDENT(jname))
#define PHP_JSOND_FE(jname, arginfo) PHP_FE(PHP_JSOND_IDENT(jname), arginfo)

#ifndef HASH_KEY_NON_EXISTENT
#define HASH_KEY_NON_EXISTENT HASH_KEY_NON_EXISTANT
#endif

#ifndef INIT_PZVAL_COPY
#define INIT_PZVAL_COPY(z, v)           \
	do {                                \
		(z)->value = (v)->value;        \
		Z_TYPE_P(z) = Z_TYPE_P(v);      \
		INIT_PZVAL(z)                   \
	} while (0)
#endif


/* This is important for the inline functions in debug mode as well
 * which is not done zend_always_inline. Such function are not inlined in
 * GCC 5 which results in a linking error */
#if defined(__GNUC__) && __GNUC__ >= 3
#define php_json_always_inline inline __attribute__((always_inline))
#else
#define php_json_always_inline inline
#endif

/* long limits */
#if SIZEOF_LONG == 4
#define PHP_JSON_INT_MAX_LENGTH 10
#define PHP_JSON_INT_MAX_DIGITS "2147483648"
#elif SIZEOF_LONG == 8
#define PHP_JSON_INT_MAX_LENGTH 19
#define PHP_JSON_INT_MAX_DIGITS "9223372036854775808"
#else
#error "Unknown SIZEOF_LONG"
#endif

typedef enum {
	PHP_JSON_ERROR_NONE = 0,
    PHP_JSON_ERROR_DEPTH,
    PHP_JSON_ERROR_STATE_MISMATCH,
    PHP_JSON_ERROR_CTRL_CHAR,
    PHP_JSON_ERROR_SYNTAX,
    PHP_JSON_ERROR_UTF8,
    PHP_JSON_ERROR_RECURSION,
    PHP_JSON_ERROR_INF_OR_NAN,
    PHP_JSON_ERROR_UNSUPPORTED_TYPE,
	PHP_JSON_ERROR_INVALID_PROPERTY_NAME,
	PHP_JSON_ERROR_UTF16
} php_json_error_code;

/* json_encode() options */
#define PHP_JSON_HEX_TAG	(1<<0)
#define PHP_JSON_HEX_AMP	(1<<1)
#define PHP_JSON_HEX_APOS	(1<<2)
#define PHP_JSON_HEX_QUOT	(1<<3)
#define PHP_JSON_FORCE_OBJECT	(1<<4)
#define PHP_JSON_NUMERIC_CHECK	(1<<5)
#define PHP_JSON_UNESCAPED_SLASHES	(1<<6)
#define PHP_JSON_PRETTY_PRINT	(1<<7)
#define PHP_JSON_UNESCAPED_UNICODE	(1<<8)
#define PHP_JSON_PARTIAL_OUTPUT_ON_ERROR (1<<9)
#define PHP_JSON_PRESERVE_ZERO_FRACTION (1<<10)

/* Internal flags */
#define PHP_JSON_OUTPUT_ARRAY	0
#define PHP_JSON_OUTPUT_OBJECT	1

/* json_decode() options */
#define PHP_JSON_OBJECT_AS_ARRAY	(1<<0)
#define PHP_JSON_BIGINT_AS_STRING	(1<<1)

/* default depth */
#define PHP_JSON_PARSER_DEFAULT_DEPTH 512


ZEND_BEGIN_MODULE_GLOBALS(jsond)
	int encoder_depth;
	int encode_max_depth;
	php_json_error_code error_code;
ZEND_END_MODULE_GLOBALS(jsond)

#define JSOND_G(v) PHPC_MODULE_GLOBALS_ACCESSOR(jsond, v)

#if PHPC_MODULE_HAS_THREAD_CACHE
ZEND_TSRMLS_CACHE_EXTERN();
#endif

#include "php_jsond_buffer.h"

PHP_JSOND_API void PHP_JSOND_NAME(encode)(php_json_buffer *buf, zval *val, int options TSRMLS_DC);
PHP_JSOND_API void PHP_JSOND_NAME(decode_ex)(zval *return_value, char *str, size_t str_len, int options, int depth TSRMLS_DC);
extern PHP_JSOND_API zend_class_entry *PHP_JSOND_NAME(serializable_ce);

static inline void PHP_JSOND_NAME(decode)(zval *return_value, char *str, size_t str_len, zend_bool assoc, int depth TSRMLS_DC)
{
	PHP_JSOND_NAME(decode_ex)(return_value, str, str_len, assoc ? PHP_JSON_OBJECT_AS_ARRAY : 0, depth TSRMLS_CC);
}

#endif	/* PHP_JSOND_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
