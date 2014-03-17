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

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/html.h"
#include "ext/standard/php_smart_str.h"
#include "php_jsond.h"
#include "php_jsond_parser.h"
#include <zend_exceptions.h>



static PHP_MINFO_FUNCTION(jsond);
static PHP_JSOND_FUNCTION(encode);
static PHP_JSOND_FUNCTION(decode);
static PHP_JSOND_FUNCTION(last_error);
static PHP_JSOND_FUNCTION(last_error_msg);

static const char digits[] = "0123456789abcdef";

PHP_JSOND_API zend_class_entry *PHP_JSOND_NAME(serializable_ce);

ZEND_DECLARE_MODULE_GLOBALS(jsond)

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_jsond_encode, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, depth)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_jsond_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, json)
	ZEND_ARG_INFO(0, assoc)
	ZEND_ARG_INFO(0, depth)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_jsond_last_error, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_jsond_last_error_msg, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ json_functions[] */
static const zend_function_entry jsond_functions[] = {
	PHP_JSOND_FE(encode, arginfo_jsond_encode)
	PHP_JSOND_FE(decode, arginfo_jsond_decode)
	PHP_JSOND_FE(last_error, arginfo_jsond_last_error)
	PHP_JSOND_FE(last_error_msg, arginfo_jsond_last_error_msg)
	PHP_FE_END
};
/* }}} */

/* {{{ JsonSerializable methods */
ZEND_BEGIN_ARG_INFO(jsond_serialize_arginfo, 0)
	/* No arguments */
ZEND_END_ARG_INFO();

static const zend_function_entry jsond_serializable_interface[] = {
	PHP_ABSTRACT_ME(PHP_JSOND_SERIALIZABLE_INTERFACE, PHP_JSOND_SERIALIZABLE_METHOD, jsond_serialize_arginfo)
	PHP_FE_END
};
/* }}} */

/* {{{ MINIT */
static PHP_MINIT_FUNCTION(jsond)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, PHP_JSOND_SERIALIZABLE_INTERFACE_STRING, jsond_serializable_interface);
	PHP_JSOND_NAME(serializable_ce) = zend_register_internal_interface(&ce TSRMLS_CC);

	PHP_JSOND_REGISTER_LONG_CONSTANT("HEX_TAG",  PHP_JSON_HEX_TAG,  CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("HEX_AMP",  PHP_JSON_HEX_AMP,  CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("HEX_APOS", PHP_JSON_HEX_APOS, CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("HEX_QUOT", PHP_JSON_HEX_QUOT, CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("FORCE_OBJECT", PHP_JSON_FORCE_OBJECT, CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("NUMERIC_CHECK", PHP_JSON_NUMERIC_CHECK, CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("UNESCAPED_SLASHES", PHP_JSON_UNESCAPED_SLASHES, CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("PRETTY_PRINT", PHP_JSON_PRETTY_PRINT, CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("UNESCAPED_UNICODE", PHP_JSON_UNESCAPED_UNICODE, CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("PARTIAL_OUTPUT_ON_ERROR", PHP_JSON_PARTIAL_OUTPUT_ON_ERROR, CONST_CS | CONST_PERSISTENT);
	
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_NONE", PHP_JSON_ERROR_NONE, CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_DEPTH", PHP_JSON_ERROR_DEPTH, CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_STATE_MISMATCH", PHP_JSON_ERROR_STATE_MISMATCH, CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_CTRL_CHAR", PHP_JSON_ERROR_CTRL_CHAR, CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_SYNTAX", PHP_JSON_ERROR_SYNTAX, CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_UTF8", PHP_JSON_ERROR_UTF8, CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_UTF16", PHP_JSON_ERROR_UTF16, CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_RECURSION", PHP_JSON_ERROR_RECURSION, CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_INF_OR_NAN", PHP_JSON_ERROR_INF_OR_NAN, CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_UNSUPPORTED_TYPE", PHP_JSON_ERROR_UNSUPPORTED_TYPE, CONST_CS | CONST_PERSISTENT);

	PHP_JSOND_REGISTER_LONG_CONSTANT("OBJECT_AS_ARRAY",		PHP_JSON_OBJECT_AS_ARRAY,		CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("BIGINT_AS_STRING",		PHP_JSON_BIGINT_AS_STRING,		CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION
*/
static PHP_GINIT_FUNCTION(jsond)
{
	jsond_globals->encoder_depth = 0;
	jsond_globals->error_code = 0;
	jsond_globals->encode_max_depth = 0;
}
/* }}} */


/* {{{ jsond_module_entry
 */
zend_module_entry jsond_module_entry = {
	STANDARD_MODULE_HEADER,
	PHP_JSOND_PREFIX_STRING,
	jsond_functions,
	PHP_MINIT(jsond),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(jsond),
	PHP_JSOND_VERSION,
	PHP_MODULE_GLOBALS(jsond),
	PHP_GINIT(jsond),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_JSOND
ZEND_GET_MODULE(jsond)
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
static PHP_MINFO_FUNCTION(jsond)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "jsond support", "enabled");
	php_info_print_table_row(2, "jsond version", PHP_JSOND_VERSION);
	php_info_print_table_end();
}
/* }}} */

static void json_escape_string(smart_str *buf, char *s, int len, int options TSRMLS_DC);

static int json_determine_array_type(zval **val TSRMLS_DC) /* {{{ */
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
				return PHP_JSON_OUTPUT_OBJECT;
			} else {
				if (index != idx) {
					return PHP_JSON_OUTPUT_OBJECT;
				}
			}
			idx++;
		}
	}

	return PHP_JSON_OUTPUT_ARRAY;
}
/* }}} */

/* {{{ Pretty printing support functions */

static inline void json_pretty_print_char(smart_str *buf, int options, char c TSRMLS_DC) /* {{{ */
{
	if (options & PHP_JSON_PRETTY_PRINT) {
		smart_str_appendc(buf, c);
	}
}
/* }}} */

static inline void json_pretty_print_indent(smart_str *buf, int options TSRMLS_DC) /* {{{ */
{
	int i;

	if (options & PHP_JSON_PRETTY_PRINT) {
		for (i = 0; i < JSOND_G(encoder_depth); ++i) {
			smart_str_appendl(buf, "    ", 4);
		}
	}
}
/* }}} */

/* }}} */

static void json_encode_array(smart_str *buf, zval **val, int options TSRMLS_DC) /* {{{ */
{
	int i, r;
	HashTable *myht;

	if (Z_TYPE_PP(val) == IS_ARRAY) {
		myht = HASH_OF(*val);
		r = (options & PHP_JSON_FORCE_OBJECT) ? PHP_JSON_OUTPUT_OBJECT : json_determine_array_type(val TSRMLS_CC);
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

	json_pretty_print_char(buf, options, '\n' TSRMLS_CC);
	++JSOND_G(encoder_depth);

	i = myht ? zend_hash_num_elements(myht) : 0;

	if (i > 0)
	{
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
						json_pretty_print_char(buf, options, '\n' TSRMLS_CC);
					} else {
						need_comma = 1;
					}

					json_pretty_print_indent(buf, options TSRMLS_CC);
					PHP_JSOND_NAME(encode)(buf, *data, options TSRMLS_CC);
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
							json_pretty_print_char(buf, options, '\n' TSRMLS_CC);
						} else {
							need_comma = 1;
						}

						json_pretty_print_indent(buf, options TSRMLS_CC);

						json_escape_string(buf, key, key_len - 1, options & ~PHP_JSON_NUMERIC_CHECK TSRMLS_CC);
						smart_str_appendc(buf, ':');

						json_pretty_print_char(buf, options, ' ' TSRMLS_CC);

						PHP_JSOND_NAME(encode)(buf, *data, options TSRMLS_CC);
					} else {
						if (need_comma) {
							smart_str_appendc(buf, ',');
							json_pretty_print_char(buf, options, '\n' TSRMLS_CC);
						} else {
							need_comma = 1;
						}

						json_pretty_print_indent(buf, options TSRMLS_CC);

						smart_str_appendc(buf, '"');
						smart_str_append_long(buf, (long) index);
						smart_str_appendc(buf, '"');
						smart_str_appendc(buf, ':');

						json_pretty_print_char(buf, options, ' ' TSRMLS_CC);

						PHP_JSOND_NAME(encode)(buf, *data, options TSRMLS_CC);
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
	json_pretty_print_char(buf, options, '\n' TSRMLS_CC);
	json_pretty_print_indent(buf, options TSRMLS_CC);

	if (r == PHP_JSON_OUTPUT_ARRAY) {
		smart_str_appendc(buf, ']');
	} else {
		smart_str_appendc(buf, '}');
	}
}
/* }}} */

static int json_utf8_to_utf16(unsigned short *utf16, char utf8[], int len) /* {{{ */
{
	size_t pos = 0, us;
	int j, status;

	if (utf16) {
		/* really convert the utf8 string */
		for (j=0 ; pos < len ; j++) {
			us = php_next_utf8_char((const unsigned char *)utf8, len, &pos, &status);
			if (status != SUCCESS) {
				return -1;
			}
			/* From http://en.wikipedia.org/wiki/UTF16 */
			if (us >= 0x10000) {
				us -= 0x10000;
				utf16[j++] = (unsigned short)((us >> 10) | 0xd800);
				utf16[j] = (unsigned short)((us & 0x3ff) | 0xdc00);
			} else {
				utf16[j] = (unsigned short)us;
			}
		}
	} else {
		/* Only check if utf8 string is valid, and compute utf16 lenght */
		for (j=0 ; pos < len ; j++) {
			us = php_next_utf8_char((const unsigned char *)utf8, len, &pos, &status);
			if (status != SUCCESS) {
				return -1;
			}
			if (us >= 0x10000) {
				j++;
			}
		}
	}
	return j;
}
/* }}} */


static void json_escape_string(smart_str *buf, char *s, int len, int options TSRMLS_DC) /* {{{ */
{
	int pos = 0, ulen = 0;
	unsigned short us;
	unsigned short *utf16;
	size_t newlen;

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
				if (!zend_isinf(d) && !zend_isnan(d)) {
					char *tmp;
					int l = spprintf(&tmp, 0, "%.*k", (int) EG(precision), d);
					smart_str_appendl(buf, tmp, l);
					efree(tmp);
				} else {
					JSOND_G(error_code) = PHP_JSON_ERROR_INF_OR_NAN;
					smart_str_appendc(buf, '0');
				}
			}
			return;
		}

	}

	utf16 = (options & PHP_JSON_UNESCAPED_UNICODE) ? NULL : (unsigned short *) safe_emalloc(len, sizeof(unsigned short), 0);
	ulen = json_utf8_to_utf16(utf16, s, len);
	if (ulen <= 0) {
		if (utf16) {
			efree(utf16);
		}
		if (ulen < 0) {
			JSOND_G(error_code) = PHP_JSON_ERROR_UTF8;
			smart_str_appendl(buf, "null", 4);
		} else {
			smart_str_appendl(buf, "\"\"", 2);
		}
		return;
	}
	if (!(options & PHP_JSON_UNESCAPED_UNICODE)) {
		len = ulen;
	}

	/* pre-allocate for string length plus 2 quotes */
	smart_str_alloc(buf, len+2, 0);
	smart_str_appendc(buf, '"');

	while (pos < len)
	{
		us = (options & PHP_JSON_UNESCAPED_UNICODE) ? s[pos++] : utf16[pos++];

		switch (us)
		{
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
				if (us >= ' ' && ((options & PHP_JSON_UNESCAPED_UNICODE) || (us & 127) == us)) {
					smart_str_appendc(buf, (unsigned char) us);
				} else {
					smart_str_appendl(buf, "\\u", 2);
					smart_str_appendc(buf, digits[(us & 0xf000) >> 12]);
					smart_str_appendc(buf, digits[(us & 0xf00)  >> 8]);
					smart_str_appendc(buf, digits[(us & 0xf0)   >> 4]);
					smart_str_appendc(buf, digits[(us & 0xf)]);
				}
				break;
		}
	}

	smart_str_appendc(buf, '"');
	if (utf16) {
		efree(utf16);
	}
}
/* }}} */


static void json_encode_serializable_object(smart_str *buf, zval *val, int options TSRMLS_DC) /* {{{ */
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
		json_encode_array(buf, &retval, options TSRMLS_CC);
	} else {
		/* All other types, encode as normal */
		PHP_JSOND_NAME(encode)(buf, retval, options TSRMLS_CC);
	}

	zval_ptr_dtor(&retval);
}
/* }}} */

PHP_JSOND_API void PHP_JSOND_NAME(encode)(smart_str *buf, zval *val, int options TSRMLS_DC) /* {{{ */
{
	switch (Z_TYPE_P(val))
	{
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
			{
				char *d = NULL;
				int len;
				double dbl = Z_DVAL_P(val);

				if (!zend_isinf(dbl) && !zend_isnan(dbl)) {
					len = spprintf(&d, 0, "%.*k", (int) EG(precision), dbl);
					smart_str_appendl(buf, d, len);
					efree(d);
				} else {
					JSOND_G(error_code) = PHP_JSON_ERROR_INF_OR_NAN;
					smart_str_appendc(buf, '0');
				}
			}
			break;

		case IS_STRING:
			json_escape_string(buf, Z_STRVAL_P(val), Z_STRLEN_P(val), options TSRMLS_CC);
			break;

		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(val), PHP_JSOND_NAME(serializable_ce) TSRMLS_CC)) {
				json_encode_serializable_object(buf, val, options TSRMLS_CC);
				break;
			}
			/* fallthrough -- Non-serializable object */
		case IS_ARRAY:
			json_encode_array(buf, &val, options TSRMLS_CC);
			break;

		default:
			JSOND_G(error_code) = PHP_JSON_ERROR_UNSUPPORTED_TYPE;
			smart_str_appendl(buf, "null", 4);
			break;
	}

	return;
}
/* }}} */

PHP_JSOND_API void PHP_JSOND_NAME(decode_ex)(zval *return_value, char *str, int str_len, long options, long depth TSRMLS_DC) /* {{{ */
{
	php_json_parser parser;
	
	php_json_parser_init(&parser, return_value, str, str_len, options, depth TSRMLS_CC);
	
	if (php_json_yyparse(&parser)) {
		JSOND_G(error_code) = php_json_parser_error_code(&parser);
		RETURN_NULL();
	}
}
/* }}} */


/* {{{ proto string json_encode(mixed data [, int options[, int depth]])
   Returns the JSON representation of a value */
static PHP_JSOND_FUNCTION(encode)
{
	zval *parameter;
	smart_str buf = {0};
	long options = 0;
    long depth = PHP_JSON_PARSER_DEFAULT_DEPTH;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|ll", &parameter, &options, &depth) == FAILURE) {
		return;
	}

	JSOND_G(error_code) = PHP_JSON_ERROR_NONE;

	JSOND_G(encode_max_depth) = depth;

	PHP_JSOND_NAME(encode)(&buf, parameter, options TSRMLS_CC);

	if (JSOND_G(error_code) != PHP_JSON_ERROR_NONE && !(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR)) {
		ZVAL_FALSE(return_value);
	} else {
		ZVAL_STRINGL(return_value, buf.c, buf.len, 1);
	}

	smart_str_free(&buf);
}
/* }}} */

/* {{{ proto mixed json_decode(string json [, bool assoc [, long depth]])
   Decodes the JSON representation into a PHP value */
static PHP_JSOND_FUNCTION(decode)
{
	char *str;
	int str_len;
	zend_bool assoc = 0; /* return JS objects as PHP objects by default */
	long depth = PHP_JSON_PARSER_DEFAULT_DEPTH;
	long options = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|bll", &str, &str_len, &assoc, &depth, &options) == FAILURE) {
		return;
	}

	JSOND_G(error_code) = 0;

	if (!str_len) {
		RETURN_NULL();
	}

	/* For BC reasons, the bool $assoc overrides the long $options bit for PHP_JSON_OBJECT_AS_ARRAY */
	if (assoc) {
		options |=  PHP_JSON_OBJECT_AS_ARRAY;
	} else {
		options &= ~PHP_JSON_OBJECT_AS_ARRAY;
	}

	PHP_JSOND_NAME(decode_ex)(return_value, str, str_len, options, depth TSRMLS_CC);
}
/* }}} */

/* {{{ proto int json_last_error()
   Returns the error code of the last json_encode() or json_decode() call. */
static PHP_JSOND_FUNCTION(last_error)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(JSOND_G(error_code));
}
/* }}} */

/* {{{ proto string json_last_error_msg()
   Returns the error string of the last json_encode() or json_decode() call. */
static PHP_JSOND_FUNCTION(last_error_msg)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	switch(JSOND_G(error_code)) {
		case PHP_JSON_ERROR_NONE:
			RETURN_STRING("No error", 1);
		case PHP_JSON_ERROR_DEPTH:
			RETURN_STRING("Maximum stack depth exceeded", 1);
		case PHP_JSON_ERROR_STATE_MISMATCH:
			RETURN_STRING("State mismatch (invalid or malformed JSON)", 1);
		case PHP_JSON_ERROR_CTRL_CHAR:
			RETURN_STRING("Control character error, possibly incorrectly encoded", 1);
		case PHP_JSON_ERROR_SYNTAX:
			RETURN_STRING("Syntax error", 1);
		case PHP_JSON_ERROR_UTF8:
			RETURN_STRING("Malformed UTF-8 characters, possibly incorrectly encoded", 1);
		case PHP_JSON_ERROR_UTF16:
			RETURN_STRING("Invalid unicode escape code", 1);
		case PHP_JSON_ERROR_RECURSION:
			RETURN_STRING("Recursion detected", 1);
		case PHP_JSON_ERROR_INF_OR_NAN:
			RETURN_STRING("Inf and NaN cannot be JSON encoded", 1);
		case PHP_JSON_ERROR_UNSUPPORTED_TYPE:
			RETURN_STRING("Type is not supported", 1);
		default:
			RETURN_STRING("Unknown error", 1);
	}

}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
