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

#ifndef PHP_JSOND_H
#define PHP_JSOND_H

#define PHP_JSOND_VERSION "2.0.0-dev"

extern zend_module_entry jsond_module_entry;
#define phpext_jsond_ptr &jsond_module_entry

#if defined(PHP_WIN32) && defined(JSOND_EXPORTS)
#define PHP_JSOND_API __declspec(dllexport)
#else
#define PHP_JSOND_API PHPAPI
#endif

#include "php.h"

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define PHP_JSOND_PREFIX jsond
#define PHP_JSOND_PREFIX_STRING "jsond"
#define PHP_JSOND_CONSTANT "JSOND"
#define PHP_JSOND_SERIALIZABLE_INTERFACE JsondSerializable
#define PHP_JSOND_SERIALIZABLE_INTERFACE_STRING "JsondSerializable"
#define PHP_JSOND_SERIALIZABLE_INTERFACE_STRING_LC "jsondserializable"
#define PHP_JSOND_NAME(jname) php_jsond_ ## jname
#define PHP_JSOND_IDENT(jname) jsond_ ## jname
#define PHP_JSOND_FN(jname) ZEND_FN(jsond_ ## jname)

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
#define php_jsond_always_inline inline __attribute__((always_inline))
#else
#define php_jsond_always_inline inline
#endif

/* long limits */
#if SIZEOF_LONG == 4
#define PHP_JSOND_INT_MAX_LENGTH 10
#define PHP_JSOND_INT_MAX_DIGITS "2147483648"
#elif SIZEOF_LONG == 8
#define PHP_JSOND_INT_MAX_LENGTH 19
#define PHP_JSOND_INT_MAX_DIGITS "9223372036854775808"
#else
#error "Unknown SIZEOF_LONG"
#endif

typedef enum {
	PHP_JSOND_ERROR_NONE = 0,
	PHP_JSOND_ERROR_DEPTH,
	PHP_JSOND_ERROR_STATE_MISMATCH,
	PHP_JSOND_ERROR_CTRL_CHAR,
	PHP_JSOND_ERROR_SYNTAX,
	PHP_JSOND_ERROR_UTF8,
	PHP_JSOND_ERROR_RECURSION,
	PHP_JSOND_ERROR_INF_OR_NAN,
	PHP_JSOND_ERROR_UNSUPPORTED_TYPE,
	PHP_JSOND_ERROR_INVALID_PROPERTY_NAME,
	PHP_JSOND_ERROR_UTF16,
	PHP_JSOND_ERROR_NON_BACKED_ENUM
} php_jsond_error_code;

/* json_encode() options */
#define PHP_JSOND_HEX_TAG	(1<<0)
#define PHP_JSOND_HEX_AMP	(1<<1)
#define PHP_JSOND_HEX_APOS	(1<<2)
#define PHP_JSOND_HEX_QUOT	(1<<3)
#define PHP_JSOND_FORCE_OBJECT	(1<<4)
#define PHP_JSOND_NUMERIC_CHECK	(1<<5)
#define PHP_JSOND_UNESCAPED_SLASHES	(1<<6)
#define PHP_JSOND_PRETTY_PRINT	(1<<7)
#define PHP_JSOND_UNESCAPED_UNICODE	(1<<8)
#define PHP_JSOND_PARTIAL_OUTPUT_ON_ERROR (1<<9)
#define PHP_JSOND_PRESERVE_ZERO_FRACTION (1<<10)
#define PHP_JSOND_UNESCAPED_LINE_TERMINATORS (1<<11)

/* Internal flags */
#define PHP_JSOND_OUTPUT_ARRAY	0
#define PHP_JSOND_OUTPUT_OBJECT	1

/* json_decode() options */
#define PHP_JSOND_OBJECT_AS_ARRAY	(1<<0)
#define PHP_JSOND_BIGINT_AS_STRING	(1<<1)

/* json_decode() and json_encode() common options */
#define PHP_JSOND_INVALID_UTF8_IGNORE     (1<<20)
#define PHP_JSOND_INVALID_UTF8_SUBSTITUTE (1<<21)
#define PHP_JSOND_THROW_ON_ERROR          (1<<22)

/* default depth */
#define PHP_JSOND_PARSER_DEFAULT_DEPTH 512


ZEND_BEGIN_MODULE_GLOBALS(jsond)
	int encoder_depth;
	int encode_max_depth;
	php_jsond_error_code error_code;
ZEND_END_MODULE_GLOBALS(jsond)
PHP_JSOND_API ZEND_EXTERN_MODULE_GLOBALS(jsond)

#define JSOND_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(jsond, v)

#if defined(ZTS) && defined(COMPILE_DL_JSOND)
ZEND_TSRMLS_CACHE_EXTERN();
#endif

#include "php_jsond_buffer.h"

PHP_JSOND_API zend_result php_jsond_encode(php_jsond_buffer *buf, zval *val, int options);
PHP_JSOND_API zend_result php_jsond_decode_ex(zval *return_value, const char *str, size_t str_len, int options, int depth);
extern PHP_JSOND_API zend_class_entry *php_jsond_serializable_ce;

static inline zend_result php_jsond_decode(zval *return_value, char *str, size_t str_len, bool assoc, int depth)
{
	return php_jsond_decode_ex(return_value, str, str_len, assoc ? PHP_JSOND_OBJECT_AS_ARRAY : 0, depth);
}

#endif	/* PHP_JSOND_H */
