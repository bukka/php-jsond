%require "3.0"
%code top {
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
#include "php_jsond.h"
#include "php_jsond_compat.h"
#include "php_jsond_parser.h"
#include "jso_schema.h"

#define YYDEBUG 0

#if YYDEBUG
int json_yydebug = 1;
#endif

#ifdef _MSC_VER
#define YYMALLOC malloc
#define YYFREE free
#endif

#define PHP_JSOND_DEPTH_DEC --parser->depth
#define PHP_JSOND_DEPTH_INC \
	if (parser->max_depth && parser->depth >= parser->max_depth) { \
		parser->scanner.errcode = PHP_JSOND_ERROR_DEPTH; \
		YYERROR; \
	} \
	++parser->depth
#define PHP_JSOND_CONV_JSO_RC(_call_rc) _call_rc == JSO_SUCCESS ? SUCCESS : FAILURE 

}

%define api.prefix {php_jsond_yy}
%define api.pure full
%header "jsond_parser.tab.h"
%param { php_jsond_parser *parser }


%union {
	zval value;
}


%token <value> PHP_JSOND_T_NUL
%token <value> PHP_JSOND_T_TRUE
%token <value> PHP_JSOND_T_FALSE
%token <value> PHP_JSOND_T_INT
%token <value> PHP_JSOND_T_DOUBLE
%token <value> PHP_JSOND_T_STRING
%token <value> PHP_JSOND_T_ESTRING
%token PHP_JSOND_T_EOI
%token PHP_JSOND_T_ERROR

%type <value> start object key value values array
%type <value> members member elements element

%destructor { zval_ptr_dtor_nogc(&$$); } <value>

%code {
int php_jsond_yylex(union YYSTYPE *value, php_jsond_parser *parser);
void php_jsond_yyerror(php_jsond_parser *parser, char const *msg);

}

%% /* Rules */

start:
		value PHP_JSOND_T_EOI
			{
				$$ = $1;
				ZVAL_COPY_VALUE(parser->return_value, &$1);
				(void) php_jsond_yynerrs;
				YYACCEPT;
			}
;

object:
		'{' {
				PHP_JSOND_DEPTH_INC;
				if (parser->methods.object_start && FAILURE == parser->methods.object_start(parser)) {
					YYERROR;
				}
			}
		members object_end
			{
				PHP_JSOND_DEPTH_DEC;
				$$ = $3;
				if (parser->methods.object_end && FAILURE == parser->methods.object_end(parser, &$$)) {
					YYERROR;
				}
			}
;

object_end:
		'}'
	|	']'
			{
				parser->scanner.errcode = PHP_JSOND_ERROR_STATE_MISMATCH;
				YYERROR;
			}
;

members:
		%empty
			{
				parser->methods.object_create(parser, &$$);
			}
	|	member
;

member:
		key 
			{
				if (parser->methods.object_key && FAILURE == parser->methods.object_key(parser, Z_STR($1))) {
					YYERROR;
				}
			}
		':' value
			{
				parser->methods.object_create(parser, &$$);
				if (parser->methods.object_update(parser, &$$, Z_STR($1), &$4) == FAILURE)
					YYERROR;
			}
	|	member ',' key
			{
				if (parser->methods.object_key && FAILURE == parser->methods.object_key(parser, Z_STR($3))) {
					YYERROR;
				}
			}
	':' value
			{
				if (parser->methods.object_update(parser, &$$, Z_STR($3), &$6) == FAILURE)
					YYERROR;
				$$ = $1;
			}
;

array:
		'['
			{
				PHP_JSOND_DEPTH_INC;
				if (parser->methods.array_start && FAILURE == parser->methods.array_start(parser)) {
					YYERROR;
				}
			}
		elements array_end
			{
				PHP_JSOND_DEPTH_DEC;
				$$ = $3;
				if (parser->methods.array_end && FAILURE == parser->methods.array_end(parser, &$$)) {
					YYERROR;
				}
			}
;

array_end:
		']'
	|	'}'
			{
				parser->scanner.errcode = PHP_JSOND_ERROR_STATE_MISMATCH;
				YYERROR;
			}
;

elements:
		%empty
			{
				parser->methods.array_create(parser, &$$);
			}
	|	element
;

element:
		value
			{
				parser->methods.array_create(parser, &$$);
				parser->methods.array_append(parser, &$$, &$1);
			}
	|	element ',' value
			{
				parser->methods.array_append(parser, &$1, &$3);
				$$ = $1;
			}
;

key:
		PHP_JSOND_T_STRING
	|	PHP_JSOND_T_ESTRING
;

value:
		values
			{
				if (parser->methods.scalar_value && FAILURE == parser->methods.scalar_value(parser, &$1)) {
					YYERROR;
				}
				$$ = $1;
			}

values:
		object
	|	array
	|	PHP_JSOND_T_STRING
	|	PHP_JSOND_T_ESTRING
	|	PHP_JSOND_T_INT
	|	PHP_JSOND_T_DOUBLE
	|	PHP_JSOND_T_NUL
	|	PHP_JSOND_T_TRUE
	|	PHP_JSOND_T_FALSE
;
	
%% /* Functions */

/* DECODE BASIC */

static zend_result php_jsond_parser_decode_array_create(php_jsond_parser *parser, zval *array)
{
	array_init(array);
	return SUCCESS;
}


static zend_result php_jsond_parser_decode_array_append(php_jsond_parser *parser, zval *array, zval *zvalue)
{
	zend_hash_next_index_insert(Z_ARRVAL_P(array), zvalue);
	return SUCCESS;
}

static zend_result php_jsond_parser_decode_object_create(php_jsond_parser *parser, zval *object)
{
	if (parser->scanner.options & PHP_JSOND_OBJECT_AS_ARRAY) {
		array_init(object);
	} else {
		object_init(object);
	}
	return SUCCESS;
}

static zend_result php_jsond_parser_decode_object_update(php_jsond_parser *parser, zval *object, zend_string *key, zval *zvalue)
{
	/* if JSON_OBJECT_AS_ARRAY is set */
	if (Z_TYPE_P(object) == IS_ARRAY) {
		zend_symtable_update(Z_ARRVAL_P(object), key, zvalue);
	} else {
		zval zkey;

		if (ZSTR_LEN(key) > 0 && ZSTR_VAL(key)[0] == '\0') {
			parser->scanner.errcode = PHP_JSOND_ERROR_INVALID_PROPERTY_NAME;
			zend_string_release_ex(key, 0);
			zval_ptr_dtor_nogc(zvalue);
			zval_ptr_dtor_nogc(object);
			return FAILURE;
		}
		zend_std_write_property(Z_OBJ_P(object), key, zvalue, NULL);
		Z_TRY_DELREF_P(zvalue);
	}
	zend_string_release_ex(key, 0);

	return SUCCESS;
}

/* DECODE SCHEMA */

static zend_result php_jsond_parser_decode_schema_array_create(php_jsond_parser *parser, zval *array)
{
	array_init(array);
	return SUCCESS;
}


static zend_result php_jsond_parser_decode_schema_array_append(php_jsond_parser *parser, zval *array, zval *zvalue)
{
	if (jso_schema_validation_stream_array_append(parser->schema_stream, array, zvalue) == JSO_FAILURE) {
		return FAILURE;
	}

	zend_hash_next_index_insert(Z_ARRVAL_P(array), zvalue);
	return SUCCESS;
}

static zend_result php_jsond_parser_decode_schema_array_start(php_jsond_parser *parser)
{
	return PHP_JSOND_CONV_JSO_RC(jso_schema_validation_stream_array_start(parser->schema_stream));
}

static zend_result php_jsond_parser_decode_schema_array_end(php_jsond_parser *parser, zval *array)
{
	return PHP_JSOND_CONV_JSO_RC(jso_schema_validation_stream_array_end(parser->schema_stream));
}

static zend_result php_jsond_parser_decode_schema_object_create(php_jsond_parser *parser, zval *object)
{
	if (parser->scanner.options & PHP_JSOND_OBJECT_AS_ARRAY) {
		array_init(object);
	} else {
		object_init(object);
	}
	return SUCCESS;
}

static zend_result php_jsond_parser_decode_schema_object_key(php_jsond_parser *parser, zend_string *key)
{
	return PHP_JSOND_CONV_JSO_RC(jso_schema_validation_stream_object_key(parser->schema_stream, key));
}

static zend_result php_jsond_parser_decode_schema_object_update(php_jsond_parser *parser, zval *object, zend_string *key, zval *zvalue)
{
	if (jso_schema_validation_stream_object_update(parser->schema_stream, object, key, zvalue) == JSO_FAILURE) {
		return FAILURE;
	}

	/* if JSON_OBJECT_AS_ARRAY is set */
	if (Z_TYPE_P(object) == IS_ARRAY) {
		zend_symtable_update(Z_ARRVAL_P(object), key, zvalue);
	} else {
		zval zkey;

		if (ZSTR_LEN(key) > 0 && ZSTR_VAL(key)[0] == '\0') {
			parser->scanner.errcode = PHP_JSOND_ERROR_INVALID_PROPERTY_NAME;
			zend_string_release_ex(key, 0);
			zval_ptr_dtor_nogc(zvalue);
			zval_ptr_dtor_nogc(object);
			return FAILURE;
		}
		zend_std_write_property(Z_OBJ_P(object), key, zvalue, NULL);
		Z_TRY_DELREF_P(zvalue);
	}
	zend_string_release_ex(key, 0);

	return SUCCESS;
}

static zend_result php_jsond_parser_decode_schema_object_start(php_jsond_parser *parser)
{
	return PHP_JSOND_CONV_JSO_RC(jso_schema_validation_stream_object_start(parser->schema_stream));
}

static zend_result php_jsond_parser_decode_schema_object_end(php_jsond_parser *parser, zval *object)
{
	return PHP_JSOND_CONV_JSO_RC(jso_schema_validation_stream_object_end(parser->schema_stream));
}

static zend_result php_jsond_parser_decode_schema_scalar_value(php_jsond_parser *parser, zval *value)
{
	return PHP_JSOND_CONV_JSO_RC(jso_schema_validation_stream_value(parser->schema_stream, value));
}

/* VALIDATE BASIC */

static zend_result php_jsond_parser_validate_array_create(php_jsond_parser *parser, zval *array)
{
	ZVAL_NULL(array);
	return SUCCESS;
}

static zend_result php_jsond_parser_validate_array_append(php_jsond_parser *parser, zval *array, zval *zvalue)
{
	return SUCCESS;
}

static zend_result php_jsond_parser_validate_object_create(php_jsond_parser *parser, zval *object)
{
	ZVAL_NULL(object);
	return SUCCESS;
}

static zend_result php_jsond_parser_validate_object_update(php_jsond_parser *parser, zval *object, zend_string *key, zval *zvalue)
{
	return SUCCESS;
}

/* LEXING AND PARSING */

int php_jsond_yylex(union YYSTYPE *value, php_jsond_parser *parser)
{
	int token = php_jsond_scan(&parser->scanner);
	value->value = parser->scanner.value;

	bool pure_validate = parser->methods.array_create == php_jsond_parser_validate_array_create;

	if (pure_validate) {
		zval_ptr_dtor_str(&(parser->scanner.value));
		ZVAL_UNDEF(&value->value);
	} else {
		value->value = parser->scanner.value;
	}

	return token;
}

void php_jsond_yyerror(php_jsond_parser *parser, char const *msg)
{
	if (!parser->scanner.errcode) {
		parser->scanner.errcode = PHP_JSOND_ERROR_SYNTAX;
	}
}

PHP_JSOND_API php_jsond_error_code php_jsond_parser_error_code(const php_jsond_parser *parser)
{
	return parser->scanner.errcode;
}

static const php_jsond_parser_methods decode_parser_methods =
{
	php_jsond_parser_decode_array_create,
	php_jsond_parser_decode_array_append,
	NULL,
	NULL,
	php_jsond_parser_decode_object_create,
	NULL,
	php_jsond_parser_decode_object_update,
	NULL,
	NULL,
	NULL,
};


static const php_jsond_parser_methods decode_schema_parser_methods =
{
	php_jsond_parser_decode_schema_array_create,
	php_jsond_parser_decode_schema_array_append,
	php_jsond_parser_decode_schema_array_start,
	php_jsond_parser_decode_schema_array_end,
	php_jsond_parser_decode_schema_object_create,
	php_jsond_parser_decode_schema_object_key,
	php_jsond_parser_decode_schema_object_update,
	php_jsond_parser_decode_schema_object_start,
	php_jsond_parser_decode_schema_object_end,
	php_jsond_parser_decode_schema_scalar_value,
};

static const php_jsond_parser_methods validate_parser_methods =
{
	php_jsond_parser_validate_array_create,
	php_jsond_parser_validate_array_append,
	NULL,
	NULL,
	php_jsond_parser_validate_object_create,
	NULL,
	php_jsond_parser_validate_object_update,
	NULL,
	NULL,
	NULL,
};

// same as decode for now
static const php_jsond_parser_methods validate_schema_parser_methods =
{
	php_jsond_parser_decode_schema_array_create,
	php_jsond_parser_decode_schema_array_append,
	php_jsond_parser_decode_schema_array_start,
	php_jsond_parser_decode_schema_array_end,
	php_jsond_parser_decode_schema_object_create,
	php_jsond_parser_decode_schema_object_key,
	php_jsond_parser_decode_schema_object_update,
	php_jsond_parser_decode_schema_object_start,
	php_jsond_parser_decode_schema_object_end,
	php_jsond_parser_decode_schema_scalar_value,
};

PHP_JSOND_API void php_jsond_parser_init_ex(
		php_jsond_parser *parser, zval *return_value,
		const char *str, size_t str_len,
		int options, int max_depth, jso_schema *schema,
		const php_jsond_parser_methods *parser_methods)
{
	memset(parser, 0, sizeof(php_jsond_parser));
	php_jsond_scanner_init(&parser->scanner, str, str_len, options);
	parser->depth = 1;
	parser->max_depth = max_depth;
	parser->return_value = return_value;
	parser->schema = schema;
	memcpy(&parser->methods, parser_methods, sizeof(php_jsond_parser_methods));
}

PHP_JSOND_API void php_jsond_parser_init(
		php_jsond_parser *parser, zval *return_value,
		const char *str, size_t str_len,
		int options, int max_depth)
{
	php_jsond_parser_init_ex(
			parser,
			return_value,
			str,
			str_len,
			options,
			max_depth,
			NULL,
			&decode_parser_methods);
}


PHP_JSOND_API int php_jsond_parse(php_jsond_parser *parser)
{
	jso_schema_validation_stream stream;

	if (parser->schema != NULL) {
		parser->schema_stream = &stream;
		if (jso_schema_validation_stream_init(parser->schema, parser->schema_stream, 32) == JSO_FAILURE) {
			return -1;
		}
	}

	int rc = php_jsond_yyparse(parser);

	if (parser->schema_stream != NULL) {
		jso_schema_validation_stream_clear(parser->schema_stream);
	}

	return rc;
}


const php_jsond_parser_methods* php_jsond_get_decode_methods(jso_schema *schema)
{
	return schema == NULL ? &decode_parser_methods : &decode_schema_parser_methods;
}

const php_jsond_parser_methods* php_jsond_get_validate_methods(jso_schema *schema)
{
	return schema == NULL ? &validate_parser_methods : &validate_schema_parser_methods;
}
