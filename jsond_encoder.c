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
  | Author: Omar Kilani <omar@php.net>                                   |
  |         Jakub Zelenka <bukka@php.net>                                |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_JSOND_BUF_TYPE_NATIVE
#define PHPC_SMART_CSTR_INCLUDE 1
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/html.h"
#include "php_jsond.h"
#include "php_jsond_encoder.h"
#include "php_jsond_buffer.h"
#include "php_jsond_utf8_decoder.h"
#include <zend_exceptions.h>

/* double limits */
#include <float.h>
#if defined(DBL_MANT_DIG) && defined(DBL_MIN_EXP)
#define PHP_JSON_DOUBLE_MAX_LENGTH (3 + DBL_MANT_DIG - DBL_MIN_EXP)
#else
#define PHP_JSON_DOUBLE_MAX_LENGTH 1080
#endif

ZEND_EXTERN_MODULE_GLOBALS(jsond)

static const char php_json_digits[] = "0123456789abcdef";

static int php_json_determine_array_type(phpc_val *val TSRMLS_DC) /* {{{ */
{
	int i;
	HashTable *myht = PHPC_ARRVAL_P(val);

	i = myht ? PHPC_HASH_NUM_ELEMENTS(myht) : 0;
	if (i > 0) {
		phpc_ulong_t index, idx = 0;
		PHPC_STR_DECLARE(key);
		PHPC_STR_LEN_UNUSED(key);

		PHPC_HASH_FOREACH_KEY(myht, index, key) {
			if (PHPC_STR_EXISTS(key) || index != idx) {
				return PHP_JSON_OUTPUT_OBJECT;
			}
			idx++;
		} PHPC_HASH_FOREACH_END();
	}

	return PHP_JSON_OUTPUT_ARRAY;
}
/* }}} */

/* Pretty printing support functions */

static inline void php_json_pretty_print_char(php_json_buffer *buf, int options, char c TSRMLS_DC) /* {{{ */
{
	if (options & PHP_JSON_PRETTY_PRINT) {
		PHP_JSON_BUF_APPEND_CHAR(buf, c);
	}
}
/* }}} */

static inline void php_json_pretty_print_indent(php_json_buffer *buf, int options TSRMLS_DC) /* {{{ */
{
	int i;

	if (options & PHP_JSON_PRETTY_PRINT) {
		for (i = 0; i < JSOND_G(encoder_depth); ++i) {
			PHP_JSON_BUF_APPEND_STRING(buf, "    ", 4);
		}
	}
}
/* }}} */

/* Double encoding */

static inline int php_json_is_valid_double(double d) /* {{{ */
{
	return !zend_isinf(d) && !zend_isnan(d);
}
/* }}} */

static inline void php_json_encode_double(php_json_buffer *buf, double d, int options TSRMLS_DC) /* {{{ */
{
	size_t len;
	PHP_JSON_BUF_DOUBLE_BLOCK_INIT(buf, num, PHP_JSON_DOUBLE_MAX_LENGTH);
	php_gcvt(d, EG(precision), '.', 'e', &num[0]);
	len = strlen(num);
	if ((options & PHP_JSON_PRESERVE_ZERO_FRACTION) && strchr(num, '.') == NULL) {
		memcpy(&num[len], ".0", sizeof(".0"));
		len += 2;
	}
	PHP_JSON_BUF_DOUBLE_BLOCK_CLOSE(buf, num, len);
}
/* }}} */

/* String encoding */

static inline char *php_json_escape_string_flush_ex(php_json_buffer *buf, char *mark, char *s, char *esc, int esc_len, int extralen) /* {{{ */
{
	char *prechar = s - extralen;
	if (prechar != mark) {
		PHP_JSON_BUF_APPEND_STRING(buf, mark, prechar - mark);
	}
	if (esc_len) {
		PHP_JSON_BUF_APPEND_STRING(buf, esc, esc_len);
	}
	return s + 1;
}
/* }}} */

static inline char *php_json_escape_string_flush(php_json_buffer *buf, char *mark, char *s, char *esc, int esc_len) /* {{{ */
{
	return php_json_escape_string_flush_ex(buf, mark, s, esc, esc_len, 0);
}
/* }}} */

static void php_json_escape_string(php_json_buffer *buf, char *s, phpc_str_size_t len, int options TSRMLS_DC) /* {{{ */
{
	size_t count;
	int codepoint;
	int state = 0;
	int codelen = 0;
	char *mark = s;
	PHP_JSON_BUF_MARK_DECLARE(buf);

	if (len == 0) {
		PHP_JSON_BUF_APPEND_STRING(buf, "\"\"", 2);
		return;
	}

	if (options & PHP_JSON_NUMERIC_CHECK) {
		double d;
		int type;
		long p;

		if ((type = is_numeric_string(s, len, &p, &d, 0)) != 0) {
			if (type == IS_LONG) {
				PHP_JSON_BUF_APPEND_LONG(buf, p);
				return;
			}
			if (type == IS_DOUBLE && php_json_is_valid_double(d)) {
				php_json_encode_double(buf, d, options TSRMLS_CC);
				return;
			}
		}
	}

	/* set mark */
	PHP_JSON_BUF_MARK_SET(buf);

	/* pre-allocate for string length plus 2 quotes */
	PHP_JSON_BUF_ALLOC(buf, len);
	PHP_JSON_BUF_APPEND_CHAR(buf, '"');

	for (count = 0; count < len; count++, s++) {
		if (php_json_utf8_decode(&state, &codepoint, (unsigned char) *s)) {
			codelen++;
			continue;
		}

		switch (codepoint) {
			case '"':
				if (options & PHP_JSON_HEX_QUOT) {
					mark = php_json_escape_string_flush(buf, mark, s, "\\u0022", 6);
				} else {
					mark = php_json_escape_string_flush(buf, mark, s, "\\\"", 2);
				}
				break;

			case '\\':
				mark = php_json_escape_string_flush(buf, mark, s, "\\\\", 2);
				break;

			case '/':
				if (!(options & PHP_JSON_UNESCAPED_SLASHES)) {
					mark = php_json_escape_string_flush(buf, mark, s, "\\/", 2);
				}
				break;

			case '\b':
				mark = php_json_escape_string_flush(buf, mark, s, "\\b", 2);
				break;

			case '\f':
				mark = php_json_escape_string_flush(buf, mark, s, "\\f", 2);
				break;

			case '\n':
				mark = php_json_escape_string_flush(buf, mark, s, "\\n", 2);
				break;

			case '\r':
				mark = php_json_escape_string_flush(buf, mark, s, "\\r", 2);
				break;

			case '\t':
				mark = php_json_escape_string_flush(buf, mark, s, "\\t", 2);
				break;

			case '<':
				if (options & PHP_JSON_HEX_TAG) {
					mark = php_json_escape_string_flush(buf, mark, s, "\\u003C", 6);
				}
				break;

			case '>':
				if (options & PHP_JSON_HEX_TAG) {
					mark = php_json_escape_string_flush(buf, mark, s, "\\u003E", 6);
				}
				break;

			case '&':
				if (options & PHP_JSON_HEX_AMP) {
					mark = php_json_escape_string_flush(buf, mark, s, "\\u0026", 6);
				}
				break;

			case '\'':
				if (options & PHP_JSON_HEX_APOS) {
					mark = php_json_escape_string_flush(buf, mark, s, "\\u0027", 6);
				}
				break;

			default:
				if (codepoint < ' ' || (!(options & PHP_JSON_UNESCAPED_UNICODE) && codepoint > 0x7f)) {
					mark = php_json_escape_string_flush_ex(buf, mark, s, "\\u", 2, codepoint < ' ' ? 0 : codelen);
					codelen = 0;
					if (codepoint <= 0xffff) {
						PHP_JSON_BUF_APPEND_CHAR(buf, php_json_digits[(codepoint & 0xf000) >> 12]);
						PHP_JSON_BUF_APPEND_CHAR(buf, php_json_digits[(codepoint & 0xf00)  >> 8]);
						PHP_JSON_BUF_APPEND_CHAR(buf, php_json_digits[(codepoint & 0xf0)   >> 4]);
						PHP_JSON_BUF_APPEND_CHAR(buf, php_json_digits[(codepoint & 0xf)]);
					} else {
						PHP_JSON_BUF_APPEND_CHAR(buf, php_json_digits[((0xD7C0 + (codepoint >> 10)) & 0xf000) >> 12]);
						PHP_JSON_BUF_APPEND_CHAR(buf, php_json_digits[((0xD7C0 + (codepoint >> 10)) & 0xf00)  >> 8]);
						PHP_JSON_BUF_APPEND_CHAR(buf, php_json_digits[((0xD7C0 + (codepoint >> 10)) & 0xf0)   >> 4]);
						PHP_JSON_BUF_APPEND_CHAR(buf, php_json_digits[((0xD7C0 + (codepoint >> 10)) & 0xf)]);
						PHP_JSON_BUF_APPEND_STRING(buf, "\\u", 2);
						PHP_JSON_BUF_APPEND_CHAR(buf, php_json_digits[((0xDC00 + (codepoint & 0x3FF)) & 0xf000) >> 12]);
						PHP_JSON_BUF_APPEND_CHAR(buf, php_json_digits[((0xDC00 + (codepoint & 0x3FF)) & 0xf00)  >> 8]);
						PHP_JSON_BUF_APPEND_CHAR(buf, php_json_digits[((0xDC00 + (codepoint & 0x3FF)) & 0xf0)   >> 4]);
						PHP_JSON_BUF_APPEND_CHAR(buf, php_json_digits[((0xDC00 + (codepoint & 0x3FF)) & 0xf)]);
					}
				}
				break;
		}
	}

	if (state != PHP_JSON_UTF8_ACCEPT) {
		JSOND_G(error_code) = PHP_JSON_ERROR_UTF8;
		PHP_JSON_BUF_RESET(buf);
		PHP_JSON_BUF_APPEND_STRING(buf, "null", 4);
	} else {
		if (mark < s) {
			PHP_JSON_BUF_APPEND_STRING(buf, mark, s - mark);
		}
		PHP_JSON_BUF_APPEND_CHAR(buf, '"');
	}
	PHP_JSON_BUF_MARK_DELETE(buf);
}
/* }}} */

/* Array encoding */

static void php_json_encode_array(php_json_buffer *buf, phpc_val *val, int options TSRMLS_DC) /* {{{ */
{
	int i, r, need_comma = 0;
	HashTable *myht;

	if (PHPC_TYPE_P(val) == IS_ARRAY) {
		myht = PHPC_ARRVAL_P(val);
		r = (options & PHP_JSON_FORCE_OBJECT) ? PHP_JSON_OUTPUT_OBJECT : php_json_determine_array_type(val TSRMLS_CC);
	} else {
		myht = PHPC_OBJPROP_P(val);
		r = PHP_JSON_OUTPUT_OBJECT;
	}

	if (myht && PHPC_HASH_GET_APPLY_COUNT(myht) > 1) {
		JSOND_G(error_code) = PHP_JSON_ERROR_RECURSION;
		PHP_JSON_BUF_APPEND_STRING(buf, "null", 4);
		return;
	}

	if (r == PHP_JSON_OUTPUT_ARRAY) {
		PHP_JSON_BUF_APPEND_CHAR(buf, '[');
	} else {
		PHP_JSON_BUF_APPEND_CHAR(buf, '{');
	}

	++JSOND_G(encoder_depth);

	i = myht ? PHPC_HASH_NUM_ELEMENTS(myht) : 0;

	if (i > 0) {
		PHPC_STR_DECLARE(key);
		phpc_val *data;
		phpc_ulong_t index;
		HashTable *tmp_ht;

		PHPC_HASH_FOREACH_KEY_VAL_IND(myht, index, key, data) {
			PHPC_PVAL_DEREF(data);
			tmp_ht = HASH_OF(PHPC_PVAL_CAST_TO_PZVAL(data));
			if (tmp_ht && PHPC_HASH_APPLY_PROTECTION(tmp_ht)) {
				PHPC_HASH_INC_APPLY_COUNT(tmp_ht);
			}

			if (r == PHP_JSON_OUTPUT_ARRAY) {
				if (need_comma) {
					PHP_JSON_BUF_APPEND_CHAR(buf, ',');
				} else {
					need_comma = 1;
				}

				php_json_pretty_print_char(buf, options, '\n' TSRMLS_CC);
				php_json_pretty_print_indent(buf, options TSRMLS_CC);
				php_json_encode_zval(buf, PHPC_PVAL_CAST_TO_PZVAL(data), options TSRMLS_CC);
			} else if (r == PHP_JSON_OUTPUT_OBJECT) {
				/* append key */
				if (PHPC_STR_EXISTS(key)) {
					if (PHPC_STR_VAL(key)[0] == '\0' && PHPC_TYPE_P(val) == IS_OBJECT) {
						/* Skip protected and private members. */
						if (tmp_ht && PHPC_HASH_APPLY_PROTECTION(tmp_ht)) {
							PHPC_HASH_DEC_APPLY_COUNT(tmp_ht);
						}
						continue;
					}

					if (need_comma) {
						PHP_JSON_BUF_APPEND_CHAR(buf, ',');
					} else {
						need_comma = 1;
					}

					php_json_pretty_print_char(buf, options, '\n' TSRMLS_CC);
					php_json_pretty_print_indent(buf, options TSRMLS_CC);

					php_json_escape_string(buf, PHPC_STR_VAL(key), PHPC_STR_LEN(key),
							options & ~PHP_JSON_NUMERIC_CHECK TSRMLS_CC);
				} else {
					if (need_comma) {
						PHP_JSON_BUF_APPEND_CHAR(buf, ',');
					} else {
						need_comma = 1;
					}

					php_json_pretty_print_char(buf, options, '\n' TSRMLS_CC);
					php_json_pretty_print_indent(buf, options TSRMLS_CC);

					PHP_JSON_BUF_APPEND_CHAR(buf, '"');
					PHP_JSON_BUF_APPEND_LONG(buf, (long) index);
					PHP_JSON_BUF_APPEND_CHAR(buf, '"');
				}
				PHP_JSON_BUF_APPEND_CHAR(buf, ':');
				/* append value */
				php_json_pretty_print_char(buf, options, ' ' TSRMLS_CC);
				php_json_encode_zval(buf, PHPC_PVAL_CAST_TO_PZVAL(data), options TSRMLS_CC);
			}

			if (tmp_ht && PHPC_HASH_APPLY_PROTECTION(tmp_ht)) {
				PHPC_HASH_DEC_APPLY_COUNT(tmp_ht);
			}

		} PHPC_HASH_FOREACH_END();

	}

	if (JSOND_G(encoder_depth) > JSOND_G(encode_max_depth)) {
		JSOND_G(error_code) = PHP_JSON_ERROR_DEPTH;
	}
	--JSOND_G(encoder_depth);

	/* Only keep closing bracket on same line for empty arrays/objects */
	if (need_comma) {
		php_json_pretty_print_char(buf, options, '\n' TSRMLS_CC);
		php_json_pretty_print_indent(buf, options TSRMLS_CC);
	}

	if (r == PHP_JSON_OUTPUT_ARRAY) {
		PHP_JSON_BUF_APPEND_CHAR(buf, ']');
	} else {
		PHP_JSON_BUF_APPEND_CHAR(buf, '}');
	}
}
/* }}} */

/* Serializable interface */

static void php_json_encode_serializable_object(php_json_buffer *buf, zval *val, int options TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce = Z_OBJCE_P(val);
	zval fname;
	phpc_val retval;
	HashTable* myht;
	int origin_error_code;

	if (Z_TYPE_P(val) == IS_ARRAY) {
		myht = Z_ARRVAL_P(val);
	} else {
		myht = Z_OBJPROP_P(val);
	}

	if (myht && PHPC_HASH_GET_APPLY_COUNT(myht) > 1) {
		JSOND_G(error_code) = PHP_JSON_ERROR_RECURSION;
		PHP_JSON_BUF_APPEND_STRING(buf, "null", 4);
		return;
	}

	PHPC_PZVAL_CSTR(&fname, "jsonSerialize");

	origin_error_code = JSOND_G(error_code);
	if (FAILURE == call_user_function_ex(
				EG(function_table), PHPC_PZVAL_CAST_TO_PVAL(val), &fname, &retval,
				0, NULL, 1, NULL TSRMLS_CC)
			|| PHPC_VAL_ISUNDEF(retval)) {
		zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "Failed calling %s::jsonSerialize()", ce->name);
		PHP_JSON_BUF_APPEND_STRING(buf, "null", sizeof("null") - 1);
		return;
    }
	JSOND_G(error_code) = origin_error_code;

	if (EG(exception)) {
		/* Error already raised */
		zval_ptr_dtor(&retval);
		zval_dtor(&fname);
		PHP_JSON_BUF_APPEND_STRING(buf, "null", sizeof("null") - 1);
		return;
	}

	if ((PHPC_TYPE(retval) == IS_OBJECT) &&
		(PHPC_OBJ_HANDLE(retval) == Z_OBJ_HANDLE_P(val))) {
		/* Handle the case where jsonSerialize does: return $this; by going straight to encode array */
		php_json_encode_array(buf, &retval, options TSRMLS_CC);
	} else {
		/* All other types, encode as normal */
		php_json_encode_zval(buf, PHPC_VAL_CAST_TO_PZVAL(retval), options TSRMLS_CC);
	}

	zval_ptr_dtor(&retval);
	zval_dtor(&fname);
}
/* }}} */

/* ZVAL encoding */

void php_json_encode_zval(php_json_buffer *buf, zval *val, int options TSRMLS_DC) /* {{{ */
{
#if PHP_VERSION_ID >= 70000
again:
#endif
	switch (Z_TYPE_P(val)) {
		case IS_NULL:
			PHP_JSON_BUF_APPEND_STRING(buf, "null", 4);
			break;

		case IS_LONG:
			PHP_JSON_BUF_APPEND_LONG(buf, Z_LVAL_P(val));
			break;

		case IS_DOUBLE:
			if (php_json_is_valid_double(Z_DVAL_P(val))) {
				php_json_encode_double(buf, Z_DVAL_P(val), options TSRMLS_CC);
			} else {
				JSOND_G(error_code) = PHP_JSON_ERROR_INF_OR_NAN;
				PHP_JSON_BUF_APPEND_CHAR(buf, '0');
			}
			break;

		case IS_STRING:
			php_json_escape_string(buf, Z_STRVAL_P(val), Z_STRLEN_P(val), options TSRMLS_CC);
			break;

		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(val), PHP_JSOND_NAME(serializable_ce) TSRMLS_CC)) {
				php_json_encode_serializable_object(buf, val, options TSRMLS_CC);
				break;
			}
			/* fallthrough -- Non-serializable object */
		case IS_ARRAY:
			php_json_encode_array(buf, PHPC_PZVAL_CAST_TO_PVAL(val), options TSRMLS_CC);
			break;

#if PHP_VERSION_ID < 70000
		case IS_BOOL:
			if (Z_BVAL_P(val)) {
				PHP_JSON_BUF_APPEND_STRING(buf, "true", 4);
			} else {
				PHP_JSON_BUF_APPEND_STRING(buf, "false", 5);
			}
			break;
#else
		case IS_TRUE:
			PHP_JSON_BUF_APPEND_STRING(buf, "true", 4);
			break;

		case IS_FALSE:
			PHP_JSON_BUF_APPEND_STRING(buf, "false", 5);
			break;

		case IS_REFERENCE:
			val = Z_REFVAL_P(val);
			goto again;
#endif

		default:
			JSOND_G(error_code) = PHP_JSON_ERROR_UNSUPPORTED_TYPE;
			PHP_JSON_BUF_APPEND_STRING(buf, "null", 4);
			break;
	}

	return;
}
/* }}} */
