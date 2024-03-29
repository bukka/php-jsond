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

/* The compatibility changes to support at least PHP 7.2 */

#ifndef PHP_JSOND_COMPAT_H
#define	PHP_JSOND_COMPAT_H

/* Recursion protection changes */
#if PHP_VERSION_ID < 70300
#define PHP_JSON_HAS_APPLY_COUNT(_tht)  (ZEND_HASH_GET_APPLY_COUNT(_tht) > 0)
#define PHP_JSON_GET_APPLY_COUNT        ZEND_HASH_GET_APPLY_COUNT
#define PHP_JSON_INC_APPLY_COUNT        ZEND_HASH_INC_APPLY_COUNT
#define PHP_JSON_DEC_APPLY_COUNT        ZEND_HASH_DEC_APPLY_COUNT
#define PHP_JSON_APPLY_PROTECTION       ZEND_HASH_APPLY_PROTECTION
#else
#define PHP_JSON_HAS_APPLY_COUNT        GC_IS_RECURSIVE
#define PHP_JSON_GET_APPLY_COUNT        GC_IS_RECURSIVE
#define PHP_JSON_INC_APPLY_COUNT        GC_PROTECT_RECURSION
#define PHP_JSON_DEC_APPLY_COUNT        GC_UNPROTECT_RECURSION
#define PHP_JSON_APPLY_PROTECTION(_tht) (!(GC_FLAGS(_tht) & GC_IMMUTABLE))
#endif

/* zend_string_release_ex introduction */
#if PHP_VERSION_ID < 70300
#define PHP_JSOND_RELEASE_STRING zend_string_release
#else
#define PHP_JSOND_RELEASE_STRING(_str) zend_string_release_ex(_str, 0)
#endif

/* zend_std_write_property changes */
#if PHP_VERSION_ID < 80000
#define PHP_JSOND_WRITE_PROPERTY(_object, _key, _value) \
    do { \
        zval _zkey; \
        ZVAL_NEW_STR(&_zkey, _key); \
        zend_std_write_property(_object, &_zkey, _value, NULL); \
    } while(0)

#else
#define PHP_JSOND_WRITE_PROPERTY(_object, _key, _value) \
    zend_std_write_property(Z_OBJ_P(_object), _key, _value, NULL)
#endif

/* zend_dtoa sign type */
#if PHP_VERSION_ID < 80100
typedef int php_jsond_dtoa_sign_t;
#else
typedef bool php_jsond_dtoa_sign_t;
#endif

#endif	/* PHP_JSOND_COMPAT_H */