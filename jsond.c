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

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/html.h"
#include "php_jsond.h"
#include "php_jsond_encoder.h"
#include "php_jsond_buffer.h"
#include "php_jsond_parser.h"
#include "jsond_arginfo.h"
#include <zend_exceptions.h>

/* PHP init and user functions */
static PHP_MINFO_FUNCTION(jsond);

static const char digits[] = "0123456789abcdef";

PHP_JSOND_API zend_class_entry *php_jsond_serializable_ce;
PHP_JSOND_API zend_class_entry *php_jsond_exception_ce;

PHP_JSOND_API ZEND_DECLARE_MODULE_GLOBALS(jsond)

#define PHP_JSOND_REGISTER_LONG_CONSTANT(name, lval) \
	do { \
		if (NULL == zend_hash_str_find( \
				EG(zend_constants), PHP_JSOND_CONSTANT"_"name, strlen(PHP_JSOND_CONSTANT"_"name))) { \
			REGISTER_LONG_CONSTANT(PHP_JSOND_CONSTANT"_"name, lval, CONST_CS | CONST_PERSISTENT); \
		} \
	} while(0)

/* MINIT */
static PHP_MINIT_FUNCTION(jsond)
{
	zend_class_entry ce;

	/* register jsond function */
	if (zend_register_functions(NULL, ext_functions, NULL, 0) == FAILURE) {
		zend_error(E_CORE_WARNING,"jsond: Unable to register functions");
		return FAILURE;
	}

	/* register JSON serializable class */
	INIT_CLASS_ENTRY(ce, PHP_JSOND_SERIALIZABLE_INTERFACE_STRING, class_JsondSerializable_methods);
	php_jsond_serializable_ce = zend_register_internal_interface(&ce);

	/* register JSON exception class */
	INIT_CLASS_ENTRY(ce, "JsondException", NULL);
	php_jsond_exception_ce = zend_register_internal_class_ex(&ce, zend_ce_exception);

	/* decoding options */
	PHP_JSOND_REGISTER_LONG_CONSTANT("OBJECT_AS_ARRAY",  PHP_JSOND_OBJECT_AS_ARRAY);
	PHP_JSOND_REGISTER_LONG_CONSTANT("BIGINT_AS_STRING", PHP_JSOND_BIGINT_AS_STRING);

	/* encoding options */
	PHP_JSOND_REGISTER_LONG_CONSTANT("HEX_TAG",  PHP_JSOND_HEX_TAG);
	PHP_JSOND_REGISTER_LONG_CONSTANT("HEX_AMP",  PHP_JSOND_HEX_AMP);
	PHP_JSOND_REGISTER_LONG_CONSTANT("HEX_APOS", PHP_JSOND_HEX_APOS);
	PHP_JSOND_REGISTER_LONG_CONSTANT("HEX_QUOT", PHP_JSOND_HEX_QUOT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("FORCE_OBJECT", PHP_JSOND_FORCE_OBJECT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("NUMERIC_CHECK", PHP_JSOND_NUMERIC_CHECK);
	PHP_JSOND_REGISTER_LONG_CONSTANT("UNESCAPED_SLASHES", PHP_JSOND_UNESCAPED_SLASHES);
	PHP_JSOND_REGISTER_LONG_CONSTANT("PRETTY_PRINT", PHP_JSOND_PRETTY_PRINT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("UNESCAPED_UNICODE", PHP_JSOND_UNESCAPED_UNICODE);
	PHP_JSOND_REGISTER_LONG_CONSTANT("PARTIAL_OUTPUT_ON_ERROR", PHP_JSOND_PARTIAL_OUTPUT_ON_ERROR);
	PHP_JSOND_REGISTER_LONG_CONSTANT("PRESERVE_ZERO_FRACTION", PHP_JSOND_PRESERVE_ZERO_FRACTION);
	PHP_JSOND_REGISTER_LONG_CONSTANT("UNESCAPED_LINE_TERMINATORS", PHP_JSOND_UNESCAPED_LINE_TERMINATORS);

	/* common options */
	PHP_JSOND_REGISTER_LONG_CONSTANT("INVALID_UTF8_IGNORE", PHP_JSOND_INVALID_UTF8_IGNORE);
	PHP_JSOND_REGISTER_LONG_CONSTANT("INVALID_UTF8_SUBSTITUTE", PHP_JSOND_INVALID_UTF8_SUBSTITUTE);
	PHP_JSOND_REGISTER_LONG_CONSTANT("THROW_ON_ERROR", PHP_JSOND_THROW_ON_ERROR);

	/* error constants */
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_NONE", PHP_JSOND_ERROR_NONE);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_DEPTH", PHP_JSOND_ERROR_DEPTH);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_STATE_MISMATCH", PHP_JSOND_ERROR_STATE_MISMATCH);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_CTRL_CHAR", PHP_JSOND_ERROR_CTRL_CHAR);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_SYNTAX", PHP_JSOND_ERROR_SYNTAX);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_UTF8", PHP_JSOND_ERROR_UTF8);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_RECURSION", PHP_JSOND_ERROR_RECURSION);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_INF_OR_NAN", PHP_JSOND_ERROR_INF_OR_NAN);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_UNSUPPORTED_TYPE", PHP_JSOND_ERROR_UNSUPPORTED_TYPE);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_INVALID_PROPERTY_NAME", PHP_JSOND_ERROR_INVALID_PROPERTY_NAME);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_UTF16", PHP_JSOND_ERROR_UTF16);

	return SUCCESS;
}

/* RINIT */
PHP_RINIT_FUNCTION(jsond)
{
	JSOND_G(error_code) = 0;

	return SUCCESS;
}

/* PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(jsond)
{
#if defined(ZTS) && defined(COMPILE_DL_JSOND)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	jsond_globals->encoder_depth = 0;
	jsond_globals->error_code = PHP_JSOND_ERROR_NONE;
	jsond_globals->encode_max_depth = PHP_JSOND_PARSER_DEFAULT_DEPTH;
}


/* jsond_module_entry */
zend_module_entry jsond_module_entry = {
	STANDARD_MODULE_HEADER,
	"jsond",
	NULL,
	PHP_MINIT(jsond),
	NULL,
	PHP_RINIT(jsond),
	NULL,
	PHP_MINFO(jsond),
	PHP_JSOND_VERSION,
	PHP_MODULE_GLOBALS(jsond),
	PHP_GINIT(jsond),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_JSOND
ZEND_GET_MODULE(jsond)
#endif

/* PHP_MINFO_FUNCTION */
static PHP_MINFO_FUNCTION(jsond)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "jsond support", "enabled");
	php_info_print_table_row(2, "jsond version", PHP_JSOND_VERSION);
	php_info_print_table_end();
}

static const char *php_jsond_get_error_msg(php_jsond_error_code error_code) /* {{{ */
{
	switch(error_code) {
		case PHP_JSOND_ERROR_NONE:
			return "No error";
		case PHP_JSOND_ERROR_DEPTH:
			return "Maximum stack depth exceeded";
		case PHP_JSOND_ERROR_STATE_MISMATCH:
			return "State mismatch (invalid or malformed JSON)";
		case PHP_JSOND_ERROR_CTRL_CHAR:
			return "Control character error, possibly incorrectly encoded";
		case PHP_JSOND_ERROR_SYNTAX:
			return "Syntax error";
		case PHP_JSOND_ERROR_UTF8:
			return "Malformed UTF-8 characters, possibly incorrectly encoded";
		case PHP_JSOND_ERROR_RECURSION:
			return "Recursion detected";
		case PHP_JSOND_ERROR_INF_OR_NAN:
			return "Inf and NaN cannot be JSON encoded";
		case PHP_JSOND_ERROR_UNSUPPORTED_TYPE:
			return "Type is not supported";
		case PHP_JSOND_ERROR_INVALID_PROPERTY_NAME:
			return "The decoded property name is invalid";
		case PHP_JSOND_ERROR_UTF16:
			return "Single unpaired UTF-16 surrogate in unicode escape";
		default:
			return "Unknown error";
	}
}

PHP_JSOND_API zend_result php_jsond_encode_ex(php_jsond_buffer *buf, zval *val, int options, zend_long depth)
{
	php_jsond_encoder encoder;
	zend_result return_code;

	php_jsond_encode_init(&encoder);
	encoder.max_depth = depth;

	return_code = php_jsond_encode_zval(buf, val, options, &encoder);
	JSOND_G(error_code) = encoder.error_code;

	return return_code;
}

PHP_JSOND_API int php_jsond_encode(php_jsond_buffer *buf, zval *val, int options)
{
	return php_jsond_encode_ex(buf, val, options, JSOND_G(encode_max_depth));
}

PHP_JSOND_API int php_jsond_decode_ex(
		zval *return_value, const char *str, size_t str_len, int options, int depth)
{
	php_jsond_parser parser;

	php_jsond_parser_init(&parser, return_value, str, str_len, options, depth);

	if (php_jsond_yyparse(&parser)) {
		php_jsond_error_code error_code = php_jsond_parser_error_code(&parser);
		if (!(options & PHP_JSOND_THROW_ON_ERROR)) {
			JSOND_G(error_code) = error_code;
		} else {
			zend_throw_exception(php_jsond_exception_ce,
					php_jsond_get_error_msg(error_code), error_code);
		}
		RETVAL_NULL();
		return FAILURE;
	}

	return SUCCESS;
}


/* Returns the JSON representation of a value */
PHP_FUNCTION(jsond_encode)
{
	zval *parameter;
	php_jsond_encoder encoder;
	php_jsond_buffer buf;
	zend_long options = 0;
	zend_long depth = PHP_JSOND_PARSER_DEFAULT_DEPTH;
	php_jsond_error_code prev_code = JSOND_G(error_code);

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_ZVAL(parameter)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(options)
		Z_PARAM_LONG(depth)
	ZEND_PARSE_PARAMETERS_END();

	PHP_JSOND_BUF_INIT(&buf);
	php_jsond_encode_init(&encoder);
	encoder.max_depth = (int)depth;
	php_jsond_encode_zval(&buf, parameter, (int)options, &encoder);

	if (!(options & PHP_JSOND_THROW_ON_ERROR) || (options & PHP_JSOND_PARTIAL_OUTPUT_ON_ERROR)) {
		JSOND_G(error_code) = encoder.error_code;
		if (encoder.error_code != PHP_JSOND_ERROR_NONE && !(options & PHP_JSOND_PARTIAL_OUTPUT_ON_ERROR)) {
			PHP_JSOND_BUF_DESTROY(&buf);
			RETURN_FALSE;
		}
	} else {
		if (encoder.error_code != PHP_JSOND_ERROR_NONE) {
			PHP_JSOND_BUF_DESTROY(&buf);
			zend_throw_exception(php_jsond_exception_ce,
					php_jsond_get_error_msg(encoder.error_code), encoder.error_code);
			RETURN_THROWS();
		}
	}

	PHP_JSOND_BUF_RETURN(buf, return_value);
}

/* Decodes the JSON representation into a PHP value */
PHP_FUNCTION(jsond_decode)
{
	char *str;
	size_t str_len;
	zend_bool assoc = 0; /* return JS objects as PHP objects by default */
	zend_bool assoc_null = 1;
	zend_long depth = PHP_JSOND_PARSER_DEFAULT_DEPTH;
	zend_long options = 0;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL_EX(assoc, assoc_null, 1, 0)
		Z_PARAM_LONG(depth)
		Z_PARAM_LONG(options)
	ZEND_PARSE_PARAMETERS_END();

	if (!(options & PHP_JSOND_THROW_ON_ERROR)) {
		JSOND_G(error_code) = PHP_JSOND_ERROR_NONE;
	}

	if (!str_len) {
		if (!(options & PHP_JSOND_THROW_ON_ERROR)) {
			JSOND_G(error_code) = PHP_JSOND_ERROR_SYNTAX;
			RETURN_NULL();
		} else {
			zend_throw_exception(php_jsond_exception_ce,
					php_jsond_get_error_msg(PHP_JSOND_ERROR_SYNTAX), PHP_JSOND_ERROR_SYNTAX);
			RETURN_THROWS();
		}
	}

	if (depth <= 0) {
		zend_argument_value_error(3, "must be greater than zero");
		RETURN_THROWS();
	}

	if (depth > INT_MAX) {
		zend_argument_value_error(3, "must be less than %d", INT_MAX);
		RETURN_THROWS();
	}

	/* For BC reasons, the bool $assoc overrides the long $options bit for PHP_JSOND_OBJECT_AS_ARRAY */
	if (!assoc_null) {
		if (assoc) {
			options |=  PHP_JSOND_OBJECT_AS_ARRAY;
		} else {
			options &= ~PHP_JSOND_OBJECT_AS_ARRAY;
		}
	}

	php_jsond_decode_ex(return_value, str, (size_t) str_len, (int) options, (int) depth);
}

/* Returns the error code of the last json_encode() or json_decode() call. */
PHP_FUNCTION(jsond_last_error)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG(JSOND_G(error_code));
}

#define PHP_JSOND_ERROR_MSG_RETURN(_msg) \
	RETURN_STRINGL(_msg, sizeof(_msg) - 1)

/* Returns the error string of the last json_encode() or json_decode() call. */
PHP_FUNCTION(jsond_last_error_msg)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_STRING(php_jsond_get_error_msg(JSOND_G(error_code)));
}
