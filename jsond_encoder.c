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
  | Author: Omar Kilani <omar@php.net>                                   |
  |         Jakub Zelenka <bukka@php.net>                                |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/html.h"
#include "php_jsond.h"
#include "php_jsond_compat.h"
#include "php_jsond_encoder.h"
#include "php_jsond_dtoa.h"
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

static int php_json_determine_array_type(zval *val)
{
	int i;
	HashTable *myht = Z_ARRVAL_P(val);

	i = myht ? zend_hash_num_elements(myht) : 0;
	if (i > 0) {
		zend_ulong index, idx = 0;
		zend_string *key;

		if (HT_IS_PACKED(myht) && HT_IS_WITHOUT_HOLES(myht)) {
			return PHP_JSON_OUTPUT_ARRAY;
		}

		ZEND_HASH_FOREACH_KEY(myht, index, key) {
			if (key || index != idx) {
				return PHP_JSON_OUTPUT_OBJECT;
			}
			idx++;
		} ZEND_HASH_FOREACH_END();
	}

	return PHP_JSON_OUTPUT_ARRAY;
}

/* Pretty printing support functions */

static inline void php_json_pretty_print_char(
		php_json_buffer *buf, int options, char c)
{
	if (options & PHP_JSON_PRETTY_PRINT) {
		PHP_JSON_BUF_APPEND_CHAR(buf, c);
	}
}

static inline void php_json_pretty_print_indent(
		php_json_buffer *buf, int options, php_json_encoder *encoder)
{
	int i;

	if (options & PHP_JSON_PRETTY_PRINT) {
		for (i = 0; i < encoder->depth; ++i) {
			PHP_JSON_BUF_APPEND_STRING(buf, "    ", 4);
		}
	}
}

/* Double encoding */

static inline int php_json_is_valid_double(double d)
{
	return !zend_isinf(d) && !zend_isnan(d);
}

static inline void php_json_encode_double(
		php_json_buffer *buf, double d, int options)
{
	size_t len;
	PHP_JSON_BUF_DOUBLE_BLOCK_INIT(buf, num, PHP_JSON_DOUBLE_MAX_LENGTH);
	php_jsond_gcvt(d, PG(serialize_precision), '.', 'e', &num[0]);
	len = strlen(num);
	if ((options & PHP_JSON_PRESERVE_ZERO_FRACTION) && strchr(num, '.') == NULL) {
		memcpy(&num[len], ".0", sizeof(".0"));
		len += 2;
	}
	PHP_JSON_BUF_DOUBLE_BLOCK_CLOSE(buf, num, len);
}

/* String encoding */

static inline char *php_json_escape_string_flush_ex(
		php_json_buffer *buf, char *mark, char *s, char *esc,
		int esc_len, int extralen)
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

static inline char *php_json_escape_string_flush(
		php_json_buffer *buf, char *mark, char *s, char *esc, int esc_len)
{
	return php_json_escape_string_flush_ex(buf, mark, s, esc, esc_len, 0);
}

static int php_json_escape_string(
		php_json_buffer *buf, char *s, size_t len, int options,
		php_json_encoder *encoder)
{
	size_t count;
	int codepoint;
	int state = 0;
	int codelen = 0;
	char *mark = s;
	PHP_JSON_BUF_MARK_DECLARE(buf);

	if (len == 0) {
		PHP_JSON_BUF_APPEND_STRING(buf, "\"\"", 2);
		return SUCCESS;
	}

	if (options & PHP_JSON_NUMERIC_CHECK) {
		double d;
		int type;
		long p;

		if ((type = is_numeric_string(s, len, &p, &d, 0)) != 0) {
			if (type == IS_LONG) {
				PHP_JSON_BUF_APPEND_LONG(buf, p);
				return SUCCESS;
			}
			if (type == IS_DOUBLE && php_json_is_valid_double(d)) {
				php_json_encode_double(buf, d, options);
				return SUCCESS;
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
				if (codepoint < ' ' || (
						codepoint > 0x7f && (
							!(options & PHP_JSON_UNESCAPED_UNICODE) ||
							((codepoint == 0x2028 || codepoint == 0x2029) && !(options & PHP_JSON_UNESCAPED_LINE_TERMINATORS))
						)
					)
				) {
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
		encoder->error_code = PHP_JSON_ERROR_UTF8;
		if (options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) {
			PHP_JSON_BUF_RESET(buf);
			PHP_JSON_BUF_APPEND_STRING(buf, "null", 4);
		}
		return FAILURE;
	} else {
		if (mark < s) {
			PHP_JSON_BUF_APPEND_STRING(buf, mark, s - mark);
		}
		PHP_JSON_BUF_APPEND_CHAR(buf, '"');
	}
	PHP_JSON_BUF_MARK_DELETE(buf);

	return SUCCESS;
}

/* Array encoding */

#define PHP_JSON_HASH_PROTECT_RECURSION(_tmp_ht) \
	do { \
		if (_tmp_ht && PHP_JSON_APPLY_PROTECTION(_tmp_ht)) { \
			PHP_JSON_INC_APPLY_COUNT(_tmp_ht); \
		} \
	} while (0)

#define PHP_JSON_HASH_UNPROTECT_RECURSION(_tmp_ht) \
	do { \
		if (_tmp_ht && PHP_JSON_APPLY_PROTECTION(_tmp_ht)) { \
			PHP_JSON_DEC_APPLY_COUNT(_tmp_ht); \
		} \
	} while (0)

static int php_json_encode_array(
		php_json_buffer *buf, zval *val, int options,
		php_json_encoder *encoder)
{
	int i, r, need_comma = 0;
	HashTable *myht;

	if (Z_TYPE_P(val) == IS_ARRAY) {
		myht = Z_ARRVAL_P(val);
		r = (options & PHP_JSON_FORCE_OBJECT) ? PHP_JSON_OUTPUT_OBJECT : php_json_determine_array_type(val);
	} else {
		myht = Z_OBJPROP_P(val);
		r = PHP_JSON_OUTPUT_OBJECT;
	}

	if (myht && PHP_JSON_HAS_APPLY_COUNT(myht)) {
		encoder->error_code = PHP_JSON_ERROR_RECURSION;
		if (options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) {
			PHP_JSON_BUF_APPEND_STRING(buf, "null", 4);
		}
		return FAILURE;
	}

	PHP_JSON_HASH_PROTECT_RECURSION(myht);

	if (r == PHP_JSON_OUTPUT_ARRAY) {
		PHP_JSON_BUF_APPEND_CHAR(buf, '[');
	} else {
		PHP_JSON_BUF_APPEND_CHAR(buf, '{');
	}

	++encoder->depth;

	i = myht ? zend_hash_num_elements(myht) : 0;

	if (i > 0) {
		zend_string *key;
		zval *data;
		zval *z_data;
		zend_ulong index;
		HashTable *tmp_ht;

		ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, z_data) {
			ZVAL_DEREF(z_data);

			if (r == PHP_JSON_OUTPUT_ARRAY) {
				if (need_comma) {
					PHP_JSON_BUF_APPEND_CHAR(buf, ',');
				} else {
					need_comma = 1;
				}

				php_json_pretty_print_char(buf, options, '\n');
				php_json_pretty_print_indent(buf, options, encoder);
			} else if (r == PHP_JSON_OUTPUT_OBJECT) {
				/* append key */
				if (key) {
					if (ZSTR_VAL(key)[0] == '\0' && ZSTR_LEN(key) > 0 && Z_TYPE_P(val) == IS_OBJECT) {
						/* Skip protected and private members. */
						continue;
					}

					if (need_comma) {
						PHP_JSON_BUF_APPEND_CHAR(buf, ',');
					} else {
						need_comma = 1;
					}

					php_json_pretty_print_char(buf, options, '\n');
					php_json_pretty_print_indent(buf, options, encoder);

					php_json_escape_string(buf, ZSTR_VAL(key), ZSTR_LEN(key),
							options & ~PHP_JSON_NUMERIC_CHECK, encoder);
				} else {
					if (need_comma) {
						PHP_JSON_BUF_APPEND_CHAR(buf, ',');
					} else {
						need_comma = 1;
					}

					php_json_pretty_print_char(buf, options, '\n');
					php_json_pretty_print_indent(buf, options, encoder);

					PHP_JSON_BUF_APPEND_CHAR(buf, '"');
					PHP_JSON_BUF_APPEND_LONG(buf, (long) index);
					PHP_JSON_BUF_APPEND_CHAR(buf, '"');
				}
				PHP_JSON_BUF_APPEND_CHAR(buf, ':');
				/* append value */
				php_json_pretty_print_char(buf, options, ' ');
			}

			if (php_json_encode_zval(buf, z_data, options, encoder) == FAILURE &&
					!(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR)) {
				PHP_JSON_HASH_UNPROTECT_RECURSION(myht);
				return FAILURE;
			};
		} ZEND_HASH_FOREACH_END();

	}

	PHP_JSON_HASH_UNPROTECT_RECURSION(myht);

	if (encoder->depth > encoder->max_depth) {
		encoder->error_code = PHP_JSON_ERROR_DEPTH;
		if (!(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR)) {
			return FAILURE;
		}
	}
	--encoder->depth;

	/* Only keep closing bracket on same line for empty arrays/objects */
	if (need_comma) {
		php_json_pretty_print_char(buf, options, '\n');
		php_json_pretty_print_indent(buf, options, encoder);
	}

	if (r == PHP_JSON_OUTPUT_ARRAY) {
		PHP_JSON_BUF_APPEND_CHAR(buf, ']');
	} else {
		PHP_JSON_BUF_APPEND_CHAR(buf, '}');
	}

	return SUCCESS;
}

/* Serializable interface */

static int php_json_encode_serializable_object(
		php_json_buffer *buf, zval *val, int options,
		php_json_encoder *encoder)
{
	zend_class_entry *ce = Z_OBJCE_P(val);
	zval fname;
	zval retval;
	HashTable* myht;
	int return_code;

	if (Z_TYPE_P(val) == IS_ARRAY) {
		myht = Z_ARRVAL_P(val);
	} else {
		myht = Z_OBJPROP_P(val);
	}

	if (myht && PHP_JSON_HAS_APPLY_COUNT(myht)) {
		encoder->error_code = PHP_JSON_ERROR_RECURSION;
		if (options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) {
			PHP_JSON_BUF_APPEND_STRING(buf, "null", 4);
		}
		return FAILURE;
	}

	PHP_JSON_HASH_PROTECT_RECURSION(myht);

	ZVAL_STRING(&fname, "jsonSerialize");

	if (FAILURE == call_user_function(EG(function_table), val, &fname, &retval, 0, NULL)
			|| Z_ISUNDEF(retval)) {
		if (!EG(exception)) {
			zend_throw_exception_ex(NULL, 0, "Failed calling %s::jsonSerialize()", ZSTR_VAL(ce->name));
		}
		if (options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) {
			PHP_JSON_BUF_APPEND_STRING(buf, "null", sizeof("null") - 1);
		}
		zval_dtor(&fname);
		PHP_JSON_HASH_UNPROTECT_RECURSION(myht);
		return FAILURE;
	}

	if (EG(exception)) {
		/* Error already raised */
		zval_ptr_dtor(&retval);
		zval_dtor(&fname);
		if (options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) {
			PHP_JSON_BUF_APPEND_STRING(buf, "null", sizeof("null") - 1);
		}
		PHP_JSON_HASH_UNPROTECT_RECURSION(myht);
		return FAILURE;
	}

	if ((Z_TYPE(retval) == IS_OBJECT) && (Z_OBJ_HANDLE(retval) == Z_OBJ_HANDLE_P(val))) {
		/* Handle the case where jsonSerialize does: return $this; by going straight to encode array */
		PHP_JSON_HASH_UNPROTECT_RECURSION(myht);
		return_code = php_json_encode_array(buf, &retval, options, encoder);
	} else {
		/* All other types, encode as normal */
		return_code = php_json_encode_zval(buf, &retval, options, encoder);
		PHP_JSON_HASH_UNPROTECT_RECURSION(myht);
	}

	zval_ptr_dtor(&retval);
	zval_dtor(&fname);

	return return_code;
}

/* ZVAL encoding */

int php_json_encode_zval(php_json_buffer *buf, zval *val, int options,php_json_encoder *encoder)
{
again:
	switch (Z_TYPE_P(val)) {
		case IS_NULL:
			PHP_JSON_BUF_APPEND_STRING(buf, "null", 4);
			break;

		case IS_LONG:
			PHP_JSON_BUF_APPEND_LONG(buf, Z_LVAL_P(val));
			break;

		case IS_DOUBLE:
			if (php_json_is_valid_double(Z_DVAL_P(val))) {
				php_json_encode_double(buf, Z_DVAL_P(val), options);
			} else {
				encoder->error_code = PHP_JSON_ERROR_INF_OR_NAN;
				PHP_JSON_BUF_APPEND_CHAR(buf, '0');
			}
			break;

		case IS_STRING:
			return php_json_escape_string(
					buf, Z_STRVAL_P(val), Z_STRLEN_P(val), options, encoder);

		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(val), php_jsond_serializable_ce)) {
				return php_json_encode_serializable_object(buf, val, options, encoder);
			}
			/* fallthrough -- Non-serializable object */
		case IS_ARRAY:
			return php_json_encode_array(buf, val, options, encoder);
		case IS_TRUE:
			PHP_JSON_BUF_APPEND_STRING(buf, "true", 4);
			break;

		case IS_FALSE:
			PHP_JSON_BUF_APPEND_STRING(buf, "false", 5);
			break;

		case IS_REFERENCE:
			val = Z_REFVAL_P(val);
			goto again;

		default:
			encoder->error_code = PHP_JSON_ERROR_UNSUPPORTED_TYPE;
			if (options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) {
				PHP_JSON_BUF_APPEND_STRING(buf, "null", 4);
			}
			return FAILURE;
	}

	return SUCCESS;
}
