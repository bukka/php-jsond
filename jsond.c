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
PHP_JSOND_API zend_class_entry *php_jsond_schema_exception_ce;
PHP_JSOND_API zend_class_entry *php_jsond_schema_ce;
static zend_object_handlers php_jsond_schema_object_handlers;

PHP_JSOND_API ZEND_DECLARE_MODULE_GLOBALS(jsond)

#define PHP_JSOND_REGISTER_LONG_CONSTANT(name, lval) \
	do { \
		if (NULL == zend_hash_str_find( \
				EG(zend_constants), PHP_JSOND_CONSTANT"_"name, strlen(PHP_JSOND_CONSTANT"_"name))) { \
			REGISTER_LONG_CONSTANT(PHP_JSOND_CONSTANT"_"name, lval, CONST_CS | CONST_PERSISTENT); \
		} \
	} while(0)

#define PHP_JSOND_REGISTER_SCHEMA_CONSTANT(_name, _jso_const) \
	PHP_JSOND_REGISTER_LONG_CONSTANT(_name, (int) (_jso_const) + 1000)

#define PHP_JSOND_SCHEMA_OBJ_FROM_ZOBJ php_jsond_schema_object_from_zend_object

static void php_jsond_schema_free_object_storage(zend_object *obj) /* {{{ */
{
	php_jsond_schema_object *intern = PHP_JSOND_SCHEMA_OBJ_FROM_ZOBJ(obj);

	jso_schema_free(intern->schema);
	zend_object_std_dtor(&intern->std);
}

static inline zend_object *php_jsond_schema_create_object_ex(zend_class_entry *ce, jso_schema **schema)
{
	php_jsond_schema_object *intern = zend_object_alloc(sizeof(php_jsond_schema_object), ce);

	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);

	intern->schema = jso_schema_alloc();
	*schema = intern->schema;

	return &intern->std;
}

static zend_object *php_jsond_schema_create_object(zend_class_entry *ce)
{
	jso_schema *schema;
	return php_jsond_schema_create_object_ex(ce, &schema);
}

/* MINIT */
static PHP_MINIT_FUNCTION(jsond)
{
	/* register jsond function */
	if (zend_register_functions(NULL, ext_functions, NULL, 0) == FAILURE) {
		zend_error(E_CORE_WARNING,"jsond: Unable to register functions");
		return FAILURE;
	}

	/* register JSONd serializable class */
	php_jsond_serializable_ce = register_class_JsondSerializable();

	/* register JSONd exception class */
	php_jsond_exception_ce = register_class_JsondException(zend_ce_exception);

	/* register JSONd schema exception class */
	php_jsond_schema_exception_ce = register_class_JsondSchemaException(php_jsond_exception_ce);

	/* register JSONd schema */
	php_jsond_schema_ce = register_class_JsondSchema();
	php_jsond_schema_ce->create_object = php_jsond_schema_create_object;
	php_jsond_schema_ce->default_object_handlers = &php_jsond_schema_object_handlers;

	memcpy(&php_jsond_schema_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_jsond_schema_object_handlers.offset = XtOffsetOf(php_jsond_schema_object, std);
	php_jsond_schema_object_handlers.free_obj = php_jsond_schema_free_object_storage;

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
	PHP_JSOND_REGISTER_LONG_CONSTANT("ERROR_NON_BACKED_ENUM", PHP_JSOND_ERROR_NON_BACKED_ENUM);

	/* schema error constant */
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_ID", JSO_SCHEMA_ERROR_ID);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_KEYWORD_ALLOC", JSO_SCHEMA_ERROR_KEYWORD_ALLOC);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_KEYWORD_PREP", JSO_SCHEMA_ERROR_KEYWORD_PREP);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_KEYWORD_REQUIRED", JSO_SCHEMA_ERROR_KEYWORD_REQUIRED);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_KEYWORD_TYPE", JSO_SCHEMA_ERROR_KEYWORD_TYPE);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_REFERENCE_ALLOC", JSO_SCHEMA_ERROR_REFERENCE_ALLOC);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_REFERENCE_POINTER", JSO_SCHEMA_ERROR_REFERENCE_POINTER);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_REFERENCE_RESOLVE", JSO_SCHEMA_ERROR_REFERENCE_RESOLVE);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_REFERENCE_EXTERNAL", JSO_SCHEMA_ERROR_REFERENCE_EXTERNAL);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_REFERENCE_RECURSIVE", JSO_SCHEMA_ERROR_REFERENCE_RECURSIVE);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_ROOT_DATA_TYPE", JSO_SCHEMA_ERROR_ROOT_DATA_TYPE);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_STACK_ALLOC", JSO_SCHEMA_ERROR_STACK_ALLOC);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_TYPE_INVALID", JSO_SCHEMA_ERROR_TYPE_INVALID);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_URI_ALLOC", JSO_SCHEMA_ERROR_URI_ALLOC);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_URI_INVALID", JSO_SCHEMA_ERROR_URI_INVALID);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_VALIDATION_ALLOC", JSO_SCHEMA_ERROR_VALIDATION_ALLOC);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_VALIDATION_COMPOSITION", JSO_SCHEMA_ERROR_VALIDATION_COMPOSITION);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_VALIDATION_KEYWORD", JSO_SCHEMA_ERROR_VALIDATION_KEYWORD);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_VALIDATION_TYPE", JSO_SCHEMA_ERROR_VALIDATION_TYPE);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_VALIDATION_FALSE", JSO_SCHEMA_ERROR_VALIDATION_FALSE);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_VALUE_ALLOC", JSO_SCHEMA_ERROR_VALUE_ALLOC);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_VALUE_DATA_ALLOC", JSO_SCHEMA_ERROR_VALUE_DATA_ALLOC);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_VALUE_DATA_DEPS", JSO_SCHEMA_ERROR_VALUE_DATA_DEPS);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_VALUE_DATA_TYPE", JSO_SCHEMA_ERROR_VALUE_DATA_TYPE);
	PHP_JSOND_REGISTER_SCHEMA_CONSTANT("ERROR_SCHEMA_VERSION", JSO_SCHEMA_ERROR_VERSION);

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
		case PHP_JSOND_ERROR_NON_BACKED_ENUM:
			return "Non-backed enums have no default serialization";
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

PHP_JSOND_API zend_result php_jsond_encode(php_jsond_buffer *buf, zval *val, int options)
{
	return php_jsond_encode_ex(buf, val, options, JSOND_G(encode_max_depth));
}

PHP_JSOND_API zend_result php_jsond_decode_ex(
		zval *return_value, const char *str, size_t str_len, int options, int depth)
{
	php_jsond_parser parser;

	php_jsond_parser_init(&parser, return_value, str, str_len, options, depth);

	if (php_jsond_parse(&parser)) {
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

PHP_JSOND_API bool php_jsond_validate_ex(const char *str, size_t str_len, zend_long options, zend_long depth)
{
	php_jsond_parser parser;
	zval tmp;
	const php_jsond_parser_methods* parser_validate_methods = php_jsond_get_validate_methods(NULL);
	php_jsond_parser_init_ex(&parser, &tmp, str, str_len, (int)options, (int)depth, NULL, parser_validate_methods);

	if (php_jsond_parse(&parser)) {
		php_jsond_error_code error_code = php_jsond_parser_error_code(&parser);
		JSOND_G(error_code) = error_code;
		return false;
	}

	return true;
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
	bool assoc = 0; /* return JS objects as PHP objects by default */
	bool assoc_null = 1;
	zend_long depth = PHP_JSOND_PARSER_DEFAULT_DEPTH;
	zend_long options = 0;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL_OR_NULL(assoc, assoc_null)
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

/* Validates if a string contains a valid json */
PHP_FUNCTION(jsond_validate)
{
	char *str;
	size_t str_len;
	zend_long depth = PHP_JSOND_PARSER_DEFAULT_DEPTH;
	zend_long options = 0;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(depth)
		Z_PARAM_LONG(options)
	ZEND_PARSE_PARAMETERS_END();


	if ((options != 0) && (options != PHP_JSOND_INVALID_UTF8_IGNORE)) {
		zend_argument_value_error(3, "must be a valid flag (allowed flags: JSOND_INVALID_UTF8_IGNORE)");
		RETURN_THROWS();
	}

	if (!str_len) {
		JSOND_G(error_code) = PHP_JSOND_ERROR_SYNTAX;
		RETURN_FALSE;
	}

	JSOND_G(error_code) = PHP_JSOND_ERROR_NONE;

	if (depth <= 0) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	}

	if (depth > INT_MAX) {
		zend_argument_value_error(2, "must be less than %d", INT_MAX);
		RETURN_THROWS();
	}

	RETURN_BOOL(php_jsond_validate_ex(str, str_len, options, depth));
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

PHP_METHOD(JsondSchema, __construct)
{
	zend_throw_error(NULL, "Cannot directly construct JsondSchema, use create methods instead");
}

PHP_METHOD(JsondSchema, createFromString)
{
	const char *source;
	size_t source_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &source, &source_len) == FAILURE) {
		RETURN_THROWS();
	}

	// TODO: jso load data to jso value, create object, parse its schema and return object
}
