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
  | Author: Omar Kilani <omar@php.net>                                   |
  |         Jakub Zelenka <bukka@php.net>                                |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/html.h"
#include "ext/standard/php_smart_str.h"
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

ZEND_DECLARE_MODULE_GLOBALS(jsond)

static const char digits[] = "0123456789abcdef";

static int php_json_determine_array_type(zval **val TSRMLS_DC) /* {{{ */
{
	int i;
	HashTable *myht = HASH_OF(*val);

	i = myht ? zend_hash_num_elements(myht) : 0;
	if (i > 0) {
		char *key;
		ulong index, idx;
		uint key_len;
		HashPosition pos;

		zend_hash_internal_pointer_reset_ex(myht, &pos);
		idx = 0;
		for (;; zend_hash_move_forward_ex(myht, &pos)) {
			i = zend_hash_get_current_key_ex(myht, &key, &key_len, &index, 0, &pos);
			if (i == HASH_KEY_NON_EXISTENT) {
				break;
			}

			if (i == HASH_KEY_IS_STRING) {
				return 1;
			} else {
				if (index != idx) {
					return 1;
				}
			}
			idx++;
		}
	}

	return PHP_JSON_OUTPUT_ARRAY;
}
/* }}} */

/* Pretty printing support functions */

static inline void php_json_pretty_print_char(smart_str *buf, int options, char c TSRMLS_DC) /* {{{ */
{
	if (options & PHP_JSON_PRETTY_PRINT) {
		smart_str_appendc(buf, c);
	}
}
/* }}} */

static inline void php_json_pretty_print_indent(smart_str *buf, int options TSRMLS_DC) /* {{{ */
{
	int i;

	if (options & PHP_JSON_PRETTY_PRINT) {
		for (i = 0; i < JSOND_G(encoder_depth); ++i) {
			smart_str_appendl(buf, "    ", 4);
		}
	}
}
/* }}} */

static void php_json_encode_double(smart_str *buf, double d TSRMLS_DC) /* {{{ */
{
	if (!zend_isinf(d) && !zend_isnan(d)) {
		char num[PHP_JSON_DOUBLE_MAX_LENGTH + 1];
		size_t len;
		php_gcvt(d, EG(precision), '.', 'e', &num[0]);
		len = strlen(num);
		if (strchr(num, '.') == NULL) {
			memcpy(&num[len], ".0", sizeof(".0"));
			len += 2;
		}
		smart_str_appendl(buf, num, len);
	} else {
		JSOND_G(error_code) = PHP_JSON_ERROR_INF_OR_NAN;
		smart_str_appendc(buf, '0');
	}
}
/* }}} */

static void php_json_escape_string(smart_str *buf, char *s, int len, int options TSRMLS_DC) /* {{{ */
{
	size_t count;
	size_t newlen;
	int codepoint;
	int state = 0;
	size_t oldlen;

	if (len == 0) {
		smart_str_appendl(buf, "\"\"", 2);
		return;
	}

	if (options & PHP_JSON_NUMERIC_CHECK) {
		double d;
		int type;
		long p;

		if ((type = is_numeric_string(s, len, &p, &d, 0)) != 0) {
			if (type == IS_LONG) {
				smart_str_append_long(buf, p);
			} else if (type == IS_DOUBLE) {
				php_json_encode_double(buf, d TSRMLS_CC);
			}
			return;
		}
	}

	oldlen = buf->len;

	/* pre-allocate for string length plus 2 quotes */
	smart_str_alloc(buf, len+2, 0);
	smart_str_appendc(buf, '"');

	for (count = 0; count < len; count++, s++) {
		if (php_json_utf8_decode(&state, &codepoint, (unsigned char) *s)) {
			continue;
		}

		switch (codepoint) {
			case '"':
				if (options & PHP_JSON_HEX_QUOT) {
					smart_str_appendl(buf, "\\u0022", 6);
				} else {
					smart_str_appendl(buf, "\\\"", 2);
				}
				break;

			case '\\':
				smart_str_appendl(buf, "\\\\", 2);
				break;

			case '/':
				if (options & PHP_JSON_UNESCAPED_SLASHES) {
					smart_str_appendc(buf, '/');
				} else {
					smart_str_appendl(buf, "\\/", 2);
				}
				break;

			case '\b':
				smart_str_appendl(buf, "\\b", 2);
				break;

			case '\f':
				smart_str_appendl(buf, "\\f", 2);
				break;

			case '\n':
				smart_str_appendl(buf, "\\n", 2);
				break;

			case '\r':
				smart_str_appendl(buf, "\\r", 2);
				break;

			case '\t':
				smart_str_appendl(buf, "\\t", 2);
				break;

			case '<':
				if (options & PHP_JSON_HEX_TAG) {
					smart_str_appendl(buf, "\\u003C", 6);
				} else {
					smart_str_appendc(buf, '<');
				}
				break;

			case '>':
				if (options & PHP_JSON_HEX_TAG) {
					smart_str_appendl(buf, "\\u003E", 6);
				} else {
					smart_str_appendc(buf, '>');
				}
				break;

			case '&':
				if (options & PHP_JSON_HEX_AMP) {
					smart_str_appendl(buf, "\\u0026", 6);
				} else {
					smart_str_appendc(buf, '&');
				}
				break;

			case '\'':
				if (options & PHP_JSON_HEX_APOS) {
					smart_str_appendl(buf, "\\u0027", 6);
				} else {
					smart_str_appendc(buf, '\'');
				}
				break;

			default:
				if (codepoint >= ' ' && ((options & PHP_JSON_UNESCAPED_UNICODE) || codepoint <= 0x7f)) {
					if (codepoint <= 0x7f) {
						smart_str_appendc(buf, (unsigned char) codepoint);
					} else if (codepoint <= 0x7ff) {
						smart_str_appendc(buf, (unsigned char) (0xc0 + (codepoint >> 6)));
						smart_str_appendc(buf, (unsigned char) (0x80 + (codepoint & 0x3f)));
					} else if (codepoint <= 0xffff) {
						smart_str_appendc(buf, (unsigned char) (0xe0 + (codepoint >> 12)));
						smart_str_appendc(buf, (unsigned char) (0x80 + ((codepoint >> 6) & 0x3f)));
						smart_str_appendc(buf, (unsigned char) (0x80 + (codepoint & 0x3f)));
					} else if (codepoint <= 0x1ffff) {
						smart_str_appendc(buf, (unsigned char) (0xf0 + (codepoint >> 18)));
						smart_str_appendc(buf, (unsigned char) (0x80 + ((codepoint >> 12) & 0x3f)));
						smart_str_appendc(buf, (unsigned char) (0x80 + ((codepoint >> 6) & 0x3f)));
						smart_str_appendc(buf, (unsigned char) (0x80 + (codepoint & 0x3f)));
					}
				} else {
					if (codepoint <= 0xffff) {
						smart_str_appendl(buf, "\\u", 2);
						smart_str_appendc(buf, digits[(codepoint & 0xf000) >> 12]);
						smart_str_appendc(buf, digits[(codepoint & 0xf00)  >> 8]);
						smart_str_appendc(buf, digits[(codepoint & 0xf0)   >> 4]);
						smart_str_appendc(buf, digits[(codepoint & 0xf)]);
					} else {
						smart_str_appendl(buf, "\\u", 2);
						smart_str_appendc(buf, digits[((0xD7C0 + (codepoint >> 10)) & 0xf000) >> 12]);
						smart_str_appendc(buf, digits[((0xD7C0 + (codepoint >> 10)) & 0xf00)  >> 8]);
						smart_str_appendc(buf, digits[((0xD7C0 + (codepoint >> 10)) & 0xf0)   >> 4]);
						smart_str_appendc(buf, digits[((0xD7C0 + (codepoint >> 10)) & 0xf)]);
						smart_str_appendl(buf, "\\u", 2);
						smart_str_appendc(buf, digits[((0xDC00 + (codepoint & 0x3FF)) & 0xf000) >> 12]);
						smart_str_appendc(buf, digits[((0xDC00 + (codepoint & 0x3FF)) & 0xf00)  >> 8]);
						smart_str_appendc(buf, digits[((0xDC00 + (codepoint & 0x3FF)) & 0xf0)   >> 4]);
						smart_str_appendc(buf, digits[((0xDC00 + (codepoint & 0x3FF)) & 0xf)]);
					}
				}
				break;
		}
	}

	if (state != PHP_JSON_UTF8_ACCEPT) {
		buf->len = oldlen;
		JSOND_G(error_code) = PHP_JSON_ERROR_UTF8;
		smart_str_appendl(buf, "null", 4);
		return;
	}

	smart_str_appendc(buf, '"');
}
/* }}} */

static void php_json_encode_array(smart_str *buf, zval **val, int options TSRMLS_DC) /* {{{ */
{
	int i, r;
	HashTable *myht;

	if (Z_TYPE_PP(val) == IS_ARRAY) {
		myht = HASH_OF(*val);
		r = (options & PHP_JSON_FORCE_OBJECT) ? PHP_JSON_OUTPUT_OBJECT : php_json_determine_array_type(val TSRMLS_CC);
	} else {
		myht = Z_OBJPROP_PP(val);
		r = PHP_JSON_OUTPUT_OBJECT;
	}

	if (myht && myht->nApplyCount > 1) {
		JSOND_G(error_code) = PHP_JSON_ERROR_RECURSION;
		smart_str_appendl(buf, "null", 4);
		return;
	}

	if (r == PHP_JSON_OUTPUT_ARRAY) {
		smart_str_appendc(buf, '[');
	} else {
		smart_str_appendc(buf, '{');
	}

	php_json_pretty_print_char(buf, options, '\n' TSRMLS_CC);
	++JSOND_G(encoder_depth);

	i = myht ? zend_hash_num_elements(myht) : 0;

	if (i > 0) {
		char *key;
		zval **data;
		ulong index;
		uint key_len;
		HashPosition pos;
		HashTable *tmp_ht;
		int need_comma = 0;

		zend_hash_internal_pointer_reset_ex(myht, &pos);
		for (;; zend_hash_move_forward_ex(myht, &pos)) {
			i = zend_hash_get_current_key_ex(myht, &key, &key_len, &index, 0, &pos);
			if (i == HASH_KEY_NON_EXISTENT)
				break;

			if (zend_hash_get_current_data_ex(myht, (void **) &data, &pos) == SUCCESS) {
				tmp_ht = HASH_OF(*data);
				if (tmp_ht) {
					tmp_ht->nApplyCount++;
				}

				if (r == PHP_JSON_OUTPUT_ARRAY) {
					if (need_comma) {
						smart_str_appendc(buf, ',');
						php_json_pretty_print_char(buf, options, '\n' TSRMLS_CC);
					} else {
						need_comma = 1;
					}

					php_json_pretty_print_indent(buf, options TSRMLS_CC);
					php_json_encode_zval(buf, *data, options TSRMLS_CC);
				} else if (r == PHP_JSON_OUTPUT_OBJECT) {
					if (i == HASH_KEY_IS_STRING) {
						if (key[0] == '\0' && Z_TYPE_PP(val) == IS_OBJECT) {
							/* Skip protected and private members. */
							if (tmp_ht) {
								tmp_ht->nApplyCount--;
							}
							continue;
						}

						if (need_comma) {
							smart_str_appendc(buf, ',');
							php_json_pretty_print_char(buf, options, '\n' TSRMLS_CC);
						} else {
							need_comma = 1;
						}

						php_json_pretty_print_indent(buf, options TSRMLS_CC);

						php_json_escape_string(buf, key, key_len - 1, options & ~PHP_JSON_NUMERIC_CHECK TSRMLS_CC);
						smart_str_appendc(buf, ':');

						php_json_pretty_print_char(buf, options, ' ' TSRMLS_CC);

						php_json_encode_zval(buf, *data, options TSRMLS_CC);
					} else {
						if (need_comma) {
							smart_str_appendc(buf, ',');
							php_json_pretty_print_char(buf, options, '\n' TSRMLS_CC);
						} else {
							need_comma = 1;
						}

						php_json_pretty_print_indent(buf, options TSRMLS_CC);

						smart_str_appendc(buf, '"');
						smart_str_append_long(buf, (long) index);
						smart_str_appendc(buf, '"');
						smart_str_appendc(buf, ':');

						php_json_pretty_print_char(buf, options, ' ' TSRMLS_CC);

						php_json_encode_zval(buf, *data, options TSRMLS_CC);
					}
				}

				if (tmp_ht) {
					tmp_ht->nApplyCount--;
				}
			}
		}
	}

	if (JSOND_G(encoder_depth) > JSOND_G(encode_max_depth)) {
		JSOND_G(error_code) = PHP_JSON_ERROR_DEPTH;
	}
	--JSOND_G(encoder_depth);
	php_json_pretty_print_char(buf, options, '\n' TSRMLS_CC);
	php_json_pretty_print_indent(buf, options TSRMLS_CC);

	if (r == PHP_JSON_OUTPUT_ARRAY) {
		smart_str_appendc(buf, ']');
	} else {
		smart_str_appendc(buf, '}');
	}
}
/* }}} */


static void php_json_encode_serializable_object(smart_str *buf, zval *val, int options TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce = Z_OBJCE_P(val);
	zval *retval = NULL, fname;
	HashTable* myht;

	if (Z_TYPE_P(val) == IS_ARRAY) {
		myht = HASH_OF(val);
	} else {
		myht = Z_OBJPROP_P(val);
	}

	if (myht && myht->nApplyCount > 1) {
		JSOND_G(error_code) = PHP_JSON_ERROR_RECURSION;
		smart_str_appendl(buf, "null", 4);
		return;
	}

	ZVAL_STRING(&fname, "jsonSerialize", 0);

	if (FAILURE == call_user_function_ex(EG(function_table), &val, &fname, &retval, 0, NULL, 1, NULL TSRMLS_CC) || !retval) {
		zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "Failed calling %s::jsonSerialize()", ce->name);
		smart_str_appendl(buf, "null", sizeof("null") - 1);
		return;
    }

	if (EG(exception)) {
		/* Error already raised */
		zval_ptr_dtor(&retval);
		smart_str_appendl(buf, "null", sizeof("null") - 1);
		return;
	}

	if ((Z_TYPE_P(retval) == IS_OBJECT) &&
		(Z_OBJ_HANDLE_P(retval) == Z_OBJ_HANDLE_P(val))) {
		/* Handle the case where jsonSerialize does: return $this; by going straight to encode array */
		php_json_encode_array(buf, &retval, options TSRMLS_CC);
	} else {
		/* All other types, encode as normal */
		php_json_encode_zval(buf, retval, options TSRMLS_CC);
	}

	zval_ptr_dtor(&retval);
}
/* }}} */

void php_json_encode_zval(smart_str *buf, zval *val, int options TSRMLS_DC) /* {{{ */
{
	switch (Z_TYPE_P(val)) {
		case IS_NULL:
			smart_str_appendl(buf, "null", 4);
			break;

		case IS_BOOL:
			if (Z_BVAL_P(val)) {
				smart_str_appendl(buf, "true", 4);
			} else {
				smart_str_appendl(buf, "false", 5);
			}
			break;

		case IS_LONG:
			smart_str_append_long(buf, Z_LVAL_P(val));
			break;

		case IS_DOUBLE:
			php_json_encode_double(buf, Z_DVAL_P(val) TSRMLS_CC);
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
			php_json_encode_array(buf, &val, options TSRMLS_CC);
			break;

		default:
			JSOND_G(error_code) = PHP_JSON_ERROR_UNSUPPORTED_TYPE;
			smart_str_appendl(buf, "null", 4);
			break;
	}

	return;
}
/* }}} */
