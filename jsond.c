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

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/html.h"
#include "php_jsond.h"
#include "php_jsond_encoder.h"
#include "php_jsond_buffer.h"
#include "php_jsond_parser.h"
#include <zend_exceptions.h>

/* double limits */
#include <float.h>
#define PHP_JSON_DOUBLE_MAX_LENGTH (3 + DBL_MANT_DIG - DBL_MIN_EXP)

/* PHP init and user functions */
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
static zend_function_entry jsond_functions[] = {
	PHP_JSOND_FE(encode, arginfo_jsond_encode)
	PHP_JSOND_FE(decode, arginfo_jsond_decode)
	PHP_JSOND_FE(last_error, arginfo_jsond_last_error)
	PHP_JSOND_FE(last_error_msg, arginfo_jsond_last_error_msg)
	PHP_JSOND_FE_END
};
/* }}} */

/* {{{ JsonSerializable methods */
ZEND_BEGIN_ARG_INFO(jsond_serialize_arginfo, 0)
	/* No arguments */
ZEND_END_ARG_INFO();

static zend_function_entry jsond_serializable_interface[] = {
	PHP_ABSTRACT_ME(PHP_JSOND_SERIALIZABLE_INTERFACE, jsonSerialize, jsond_serialize_arginfo)
	PHP_JSOND_FE_END
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
	PHP_JSOND_REGISTER_LONG_CONSTANT("PRESERVE_FRACTIONAL_PART", PHP_JSON_PRESERVE_FRACTIONAL_PART, CONST_CS | CONST_PERSISTENT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("VALID_ESCAPED_UNICODE", PHP_JSON_VALID_ESCAPED_UNICODE, CONST_CS | CONST_PERSISTENT);

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

PHP_JSOND_API void PHP_JSOND_NAME(encode)(php_json_buffer *buf, zval *val, int options TSRMLS_DC) /* {{{ */
{
	php_json_encode_zval(buf, val, options TSRMLS_CC);
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
	php_json_buffer buf;
	long options = 0;
    long depth = PHP_JSON_PARSER_DEFAULT_DEPTH;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|ll", &parameter, &options, &depth) == FAILURE) {
		return;
	}

	JSOND_G(error_code) = PHP_JSON_ERROR_NONE;

	JSOND_G(encode_max_depth) = depth;

	PHP_JSON_BUF_INIT(&buf);
	PHP_JSOND_NAME(encode)(&buf, parameter, options TSRMLS_CC);

	if ((JSOND_G(error_code) != PHP_JSON_ERROR_NONE && !(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR)) ||
			PHP_JSON_BUF_LENGTH(buf) > LONG_MAX) {
		ZVAL_FALSE(return_value);
		PHP_JSON_BUF_DESTROY(&buf);
	} else {
		PHP_JSON_BUF_RETURN(buf, return_value);
	}
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
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	RETURN_LONG(JSOND_G(error_code));
}
/* }}} */

#define PHP_JSON_ERROR_NONE_MSG "No error"
#define PHP_JSON_ERROR_DEPTH_MSG "Maximum stack depth exceeded"
#define PHP_JSON_ERROR_STATE_MISMATCH_MSG "State mismatch (invalid or malformed JSON)"
#define PHP_JSON_ERROR_CTRL_CHAR_MSG "Control character error, possibly incorrectly encoded"
#define PHP_JSON_ERROR_SYNTAX_MSG "Syntax error"
#define PHP_JSON_ERROR_UTF8_MSG "Malformed UTF-8 characters, possibly incorrectly encoded"
#define PHP_JSON_ERROR_UTF16_MSG "Invalid unicode escape code"
#define PHP_JSON_ERROR_RECURSION_MSG "Recursion detected"
#define PHP_JSON_ERROR_INF_OR_NAN_MSG "Inf and NaN cannot be JSON encoded"
#define PHP_JSON_ERROR_UNSUPPORTED_TYPE_MSG "Type is not supported"
#define PHP_JSON_ERROR_UNKNOWN_MSG "Unknown error"

/* {{{ proto string json_last_error_msg()
   Returns the error string of the last json_encode() or json_decode() call. */
static PHP_JSOND_FUNCTION(last_error_msg)
{
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	switch(JSOND_G(error_code)) {
		case PHP_JSON_ERROR_NONE:
			RETURN_STRINGL(PHP_JSON_ERROR_NONE_MSG, sizeof(PHP_JSON_ERROR_NONE_MSG) - 1, 1);
		case PHP_JSON_ERROR_DEPTH:
			RETURN_STRINGL(PHP_JSON_ERROR_DEPTH_MSG, sizeof(PHP_JSON_ERROR_DEPTH_MSG) - 1, 1);
		case PHP_JSON_ERROR_STATE_MISMATCH:
			RETURN_STRINGL(PHP_JSON_ERROR_STATE_MISMATCH_MSG, sizeof(PHP_JSON_ERROR_STATE_MISMATCH_MSG) - 1, 1);
		case PHP_JSON_ERROR_CTRL_CHAR:
			RETURN_STRINGL(PHP_JSON_ERROR_CTRL_CHAR_MSG, sizeof(PHP_JSON_ERROR_CTRL_CHAR_MSG) - 1, 1);
		case PHP_JSON_ERROR_SYNTAX:
			RETURN_STRINGL(PHP_JSON_ERROR_SYNTAX_MSG, sizeof(PHP_JSON_ERROR_SYNTAX_MSG) - 1, 1);
		case PHP_JSON_ERROR_UTF8:
			RETURN_STRINGL(PHP_JSON_ERROR_UTF8_MSG, sizeof(PHP_JSON_ERROR_UTF8_MSG) - 1, 1);
		case PHP_JSON_ERROR_UTF16:
			RETURN_STRINGL(PHP_JSON_ERROR_UTF16_MSG, sizeof(PHP_JSON_ERROR_UTF16_MSG) - 1, 1);
		case PHP_JSON_ERROR_RECURSION:
			RETURN_STRINGL(PHP_JSON_ERROR_RECURSION_MSG, sizeof(PHP_JSON_ERROR_RECURSION_MSG) - 1, 1);
		case PHP_JSON_ERROR_INF_OR_NAN:
			RETURN_STRINGL(PHP_JSON_ERROR_INF_OR_NAN_MSG, sizeof(PHP_JSON_ERROR_INF_OR_NAN_MSG) - 1, 1);
		case PHP_JSON_ERROR_UNSUPPORTED_TYPE:
			RETURN_STRINGL(PHP_JSON_ERROR_UNSUPPORTED_TYPE_MSG, sizeof(PHP_JSON_ERROR_UNSUPPORTED_TYPE_MSG) - 1, 1);
		default:
			RETURN_STRINGL(PHP_JSON_ERROR_UNKNOWN_MSG, sizeof(PHP_JSON_ERROR_UNKNOWN_MSG) - 1, 1);
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
