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
#include <zend_exceptions.h>

/* double limits */
#include <float.h>
#define PHP_JSON_DOUBLE_MAX_LENGTH (3 + DBL_MANT_DIG - DBL_MIN_EXP)

/* PHP init and user functions */
static PHP_MINFO_FUNCTION(jsond);
static PHP_FUNCTION(jsond_encode);
static PHP_FUNCTION(jsond_decode);
static PHP_FUNCTION(jsond_last_error);
static PHP_FUNCTION(jsond_last_error_msg);

static const char digits[] = "0123456789abcdef";

PHP_JSOND_API zend_class_entry *php_jsond_serializable_ce;

PHP_JSOND_API ZEND_DECLARE_MODULE_GLOBALS(jsond)

/* arginfo */
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


/* jsond_functions[] */
static zend_function_entry jsond_functions[] = {
	PHP_FE(jsond_encode, arginfo_jsond_encode)
	PHP_FE(jsond_decode, arginfo_jsond_decode)
	PHP_FE(jsond_last_error, arginfo_jsond_last_error)
	PHP_FE(jsond_last_error_msg, arginfo_jsond_last_error_msg)
	PHP_FE_END
};


/* JsonSerializable methods */
ZEND_BEGIN_ARG_INFO(jsond_serialize_arginfo, 0)
	/* No arguments */
ZEND_END_ARG_INFO()

static zend_function_entry jsond_serializable_interface[] = {
	PHP_ABSTRACT_ME(PHP_JSOND_SERIALIZABLE_INTERFACE, jsonSerialize, jsond_serialize_arginfo)
	PHP_FE_END
};


/* php_json_already_exists */
static inline zend_bool php_json_already_exists()
{
	return !strncmp(PHP_JSOND_PREFIX_STRING, "json", 5) &&
			zend_hash_str_exists(&module_registry, "json", strlen("json"));
}


/* php_json_register_serializable_interface */
static inline void php_jsond_register_serializable_interface()
{
	zend_class_entry ce;

	/* register JSON serializable class */
	INIT_CLASS_ENTRY(ce, PHP_JSOND_SERIALIZABLE_INTERFACE_STRING, jsond_serializable_interface);
	PHP_JSOND_NAME(serializable_ce) = zend_register_internal_interface(&ce);
}


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
	if (!php_json_already_exists()) {
		/* register jsond function */
		if (zend_register_functions(NULL, jsond_functions, NULL, 0) == FAILURE) {
			zend_error(E_CORE_WARNING,"jsond: Unable to register functions");
			return FAILURE;
		}

		php_jsond_register_serializable_interface();
	}

	/* decoding options */
	PHP_JSOND_REGISTER_LONG_CONSTANT("OBJECT_AS_ARRAY",  PHP_JSON_OBJECT_AS_ARRAY);
	PHP_JSOND_REGISTER_LONG_CONSTANT("BIGINT_AS_STRING", PHP_JSON_BIGINT_AS_STRING);

	/* encoding options */
	PHP_JSOND_REGISTER_LONG_CONSTANT("HEX_TAG",  PHP_JSON_HEX_TAG);
	PHP_JSOND_REGISTER_LONG_CONSTANT("HEX_AMP",  PHP_JSON_HEX_AMP);
	PHP_JSOND_REGISTER_LONG_CONSTANT("HEX_APOS", PHP_JSON_HEX_APOS);
	PHP_JSOND_REGISTER_LONG_CONSTANT("HEX_QUOT", PHP_JSON_HEX_QUOT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("FORCE_OBJECT", PHP_JSON_FORCE_OBJECT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("NUMERIC_CHECK", PHP_JSON_NUMERIC_CHECK);
	PHP_JSOND_REGISTER_LONG_CONSTANT("UNESCAPED_SLASHES", PHP_JSON_UNESCAPED_SLASHES);
	PHP_JSOND_REGISTER_LONG_CONSTANT("PRETTY_PRINT", PHP_JSON_PRETTY_PRINT);
	PHP_JSOND_REGISTER_LONG_CONSTANT("UNESCAPED_UNICODE", PHP_JSON_UNESCAPED_UNICODE);
	PHP_JSOND_REGISTER_LONG_CONSTANT("PARTIAL_OUTPUT_ON_ERROR", PHP_JSON_PARTIAL_OUTPUT_ON_ERROR);
	PHP_JSOND_REGISTER_LONG_CONSTANT("PRESERVE_ZERO_FRACTION", PHP_JSON_PRESERVE_ZERO_FRACTION);
	PHP_JSOND_REGISTER_LONG_CONSTANT("UNESCAPED_LINE_TERMINATORS", PHP_JSON_UNESCAPED_LINE_TERMINATORS);

	/* error constants */
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_NONE", PHP_JSON_ERROR_NONE);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_DEPTH", PHP_JSON_ERROR_DEPTH);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_STATE_MISMATCH", PHP_JSON_ERROR_STATE_MISMATCH);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_CTRL_CHAR", PHP_JSON_ERROR_CTRL_CHAR);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_SYNTAX", PHP_JSON_ERROR_SYNTAX);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_UTF8", PHP_JSON_ERROR_UTF8);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_RECURSION", PHP_JSON_ERROR_RECURSION);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_INF_OR_NAN", PHP_JSON_ERROR_INF_OR_NAN);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_UNSUPPORTED_TYPE", PHP_JSON_ERROR_UNSUPPORTED_TYPE);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_INVALID_PROPERTY_NAME", PHP_JSON_ERROR_INVALID_PROPERTY_NAME);
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_UTF16", PHP_JSON_ERROR_UTF16);

	return SUCCESS;
}

#define PHP_JSON_REPLACE_FN(_orig, _name) \
	_orig = zend_hash_str_find_ptr(EG(function_table), "json_"#_name, strlen("json_"#_name)); \
	_orig->internal_function.handler = PHP_JSOND_FN(_name);


/* RINIT */
PHP_RINIT_FUNCTION(jsond)
{
	zend_function *orig;

	if (php_json_already_exists()) {
		PHP_JSON_REPLACE_FN(orig, encode);
		PHP_JSON_REPLACE_FN(orig, decode);
		PHP_JSON_REPLACE_FN(orig, last_error);
		PHP_JSON_REPLACE_FN(orig, last_error_msg);

		zend_string *class_name = zend_string_init(
				PHP_JSOND_SERIALIZABLE_INTERFACE_STRING,
				sizeof(PHP_JSOND_SERIALIZABLE_INTERFACE_STRING) - 1,
				0
		);
		zend_class_entry *the_ce = zend_lookup_class(class_name);
		zend_string_release(class_name);
		if (!the_ce) {
			return FAILURE;
		}
		PHP_JSOND_NAME(serializable_ce) = the_ce;
	}

	return SUCCESS;
}


/* PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(jsond)
{
#if defined(ZTS) && defined(COMPILE_DL_JSOND)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	jsond_globals->encoder_depth = 0;
	jsond_globals->error_code = PHP_JSON_ERROR_NONE;
	jsond_globals->encode_max_depth = 0;
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

PHP_JSOND_API int php_jsond_encode(php_json_buffer *buf, zval *val, int options)
{
	php_json_encoder encoder;
	int return_code;

	php_json_encode_init(&encoder);
	encoder.max_depth = JSOND_G(encode_max_depth);
	encoder.error_code = PHP_JSON_ERROR_NONE;

	return_code = php_json_encode_zval(buf, val, options, &encoder);
	JSOND_G(error_code) = encoder.error_code;

	return return_code;
}

PHP_JSOND_API int php_jsond_decode_ex(
		zval *return_value, char *str, size_t str_len, int options, int depth)
{
	php_json_parser parser;

	PHP_JSOND_NAME(parser_init)(&parser, return_value, str, str_len, options, depth);

	if (php_json_yyparse(&parser)) {
		JSOND_G(error_code) = PHP_JSOND_NAME(parser_error_code)(&parser);
		RETVAL_NULL();
		return FAILURE;
	}

	return SUCCESS;
}


/* proto string json_encode(mixed data [, int options[, int depth]])
   Returns the JSON representation of a value */
static PHP_FUNCTION(jsond_encode)
{
	zval *parameter;
	php_json_buffer buf;
	zend_long options = 0;
	zend_long depth = PHP_JSON_PARSER_DEFAULT_DEPTH;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|ll", &parameter, &options, &depth) == FAILURE) {
		return;
	}

	JSOND_G(error_code) = PHP_JSON_ERROR_NONE;

	JSOND_G(encode_max_depth) = depth;

	PHP_JSON_BUF_INIT(&buf);
	php_jsond_encode(&buf, parameter, (int)options);

	if ((JSOND_G(error_code) != PHP_JSON_ERROR_NONE && !(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR)) ||
			PHP_JSON_BUF_LENGTH(buf) > LONG_MAX) {
		ZVAL_FALSE(return_value);
		PHP_JSON_BUF_DESTROY(&buf);
	} else {
		PHP_JSON_BUF_RETURN(buf, return_value);
	}
}

/* proto mixed json_decode(string json [, bool assoc [, long depth]])
   Decodes the JSON representation into a PHP value */
static PHP_FUNCTION(jsond_decode)
{
	char *str;
	size_t str_len;
	zend_bool assoc = 0; /* return JS objects as PHP objects by default */
	zend_long depth = PHP_JSON_PARSER_DEFAULT_DEPTH;
	zend_long options = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|bll", &str, &str_len, &assoc, &depth, &options) == FAILURE) {
		return;
	}

	JSOND_G(error_code) = 0;

	if (!str_len) {
		JSOND_G(error_code) = PHP_JSON_ERROR_SYNTAX;
		RETURN_NULL();
	}

	if (depth <= 0) {
		php_error_docref(NULL, E_WARNING, "Depth must be greater than zero");
		RETURN_NULL();
	}

	if (depth > INT_MAX) {
		php_error_docref(NULL, E_WARNING, "Depth must be lower than %d", INT_MAX);
		RETURN_NULL();
	}

	/* For BC reasons, the bool $assoc overrides the long $options bit for PHP_JSON_OBJECT_AS_ARRAY */
	if (assoc) {
		options |=  PHP_JSON_OBJECT_AS_ARRAY;
	} else {
		options &= ~PHP_JSON_OBJECT_AS_ARRAY;
	}

	php_jsond_decode_ex(return_value, str, (size_t) str_len, (int) options, (int) depth);
}

/* proto int json_last_error()
   Returns the error code of the last json_encode() or json_decode() call. */
static PHP_FUNCTION(jsond_last_error)
{
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "") == FAILURE) {
		return;
	}

	RETURN_LONG(JSOND_G(error_code));
}

#define PHP_JSON_ERROR_MSG_RETURN(_msg) \
	RETURN_STRINGL(_msg, sizeof(_msg) - 1)

/* proto string json_last_error_msg()
   Returns the error string of the last json_encode() or json_decode() call. */
static PHP_FUNCTION(jsond_last_error_msg)
{
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "") == FAILURE) {
		return;
	}

	switch(JSOND_G(error_code)) {
		case PHP_JSON_ERROR_NONE:
			PHP_JSON_ERROR_MSG_RETURN("No error");
		case PHP_JSON_ERROR_DEPTH:
			PHP_JSON_ERROR_MSG_RETURN("Maximum stack depth exceeded");
		case PHP_JSON_ERROR_STATE_MISMATCH:
			PHP_JSON_ERROR_MSG_RETURN("State mismatch (invalid or malformed JSON)");
		case PHP_JSON_ERROR_CTRL_CHAR:
			PHP_JSON_ERROR_MSG_RETURN("Control character error, possibly incorrectly encoded");
		case PHP_JSON_ERROR_SYNTAX:
			PHP_JSON_ERROR_MSG_RETURN("Syntax error");
		case PHP_JSON_ERROR_UTF8:
			PHP_JSON_ERROR_MSG_RETURN("Malformed UTF-8 characters, possibly incorrectly encoded");
		case PHP_JSON_ERROR_RECURSION:
			PHP_JSON_ERROR_MSG_RETURN("Recursion detected");
		case PHP_JSON_ERROR_INF_OR_NAN:
			PHP_JSON_ERROR_MSG_RETURN("Inf and NaN cannot be JSON encoded");
		case PHP_JSON_ERROR_UNSUPPORTED_TYPE:
			PHP_JSON_ERROR_MSG_RETURN("Type is not supported");
		case PHP_JSON_ERROR_INVALID_PROPERTY_NAME:
			PHP_JSON_ERROR_MSG_RETURN("The decoded property name is invalid");
		case PHP_JSON_ERROR_UTF16:
			PHP_JSON_ERROR_MSG_RETURN("Single unpaired UTF-16 surrogate in unicode escape");
		default:
			PHP_JSON_ERROR_MSG_RETURN("Unknown error");
	}

}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
