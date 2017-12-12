%code top {
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
  | Author: Jakub Zelenka <bukka@php.net>                                |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_JSOND_BUF_TYPE_NATIVE
#define PHPC_SMART_CSTR_INCLUDE 1
#endif

#include "php.h"
#include "phpc/phpc.h"
#include "php_jsond.h"
#include "php_jsond_parser.h"

#define YYDEBUG 0

#if YYDEBUG
int json_yydebug = 1;
#endif

#ifdef _MSC_VER
#define YYMALLOC malloc
#define YYFREE free
#endif

#define PHP_JSON_USE(uv) ((void) (uv))
#define PHP_JSON_USE_1(uvr, uv1) PHP_JSON_USE(uvr); PHP_JSON_USE(uv1)
#define PHP_JSON_USE_2(uvr, uv1, uv2) \
	PHP_JSON_USE(uvr); PHP_JSON_USE(uv1); PHP_JSON_USE(uv2)

#define PHP_JSON_DEPTH_DEC --parser->depth
#define PHP_JSON_DEPTH_INC \
	if (parser->max_depth && parser->depth >= parser->max_depth) { \
		parser->scanner.errcode = PHP_JSON_ERROR_DEPTH; \
		YYERROR; \
	} \
	++parser->depth

}

%pure-parser
%name-prefix "php_json_yy"
%lex-param  { php_json_parser *parser  }
%parse-param { php_json_parser *parser }

%union {
	zval value;
	struct {
		PHPC_STR_DECLARE(key);
		zval val;
	} pair;
}


%token <value> PHP_JSON_T_NUL
%token <value> PHP_JSON_T_TRUE
%token <value> PHP_JSON_T_FALSE
%token <value> PHP_JSON_T_INT
%token <value> PHP_JSON_T_DOUBLE
%token <value> PHP_JSON_T_STRING
%token <value> PHP_JSON_T_ESTRING
%token <value> PHP_JSON_T_EOI
%token <value> PHP_JSON_T_ERROR

%type <value> start object key value array errlex
%type <value> members member elements element
%type <pair> pair

%destructor { zval_dtor(&$$); } <value>
%destructor { PHPC_STR_RELEASE($$.key); zval_dtor(&$$.val); } <pair>

%code {
int php_json_yylex(union YYSTYPE *value, php_json_parser *parser);
void php_json_yyerror(php_json_parser *parser, char const *msg);

}

%% /* Rules */

start:
		value PHP_JSON_T_EOI
			{
				$$ = $1;
				PHPC_PZVAL_COPY_INIT(parser->return_value, &$1);
				PHP_JSON_USE($2);
				YYACCEPT;
			}
	|	value errlex
			{
				PHP_JSON_USE_2($$, $1, $2);
			}
;

object:
		'{' {
				PHP_JSON_DEPTH_INC;
				if (parser->methods.object_start && FAILURE == parser->methods.object_start(parser)) {
					YYERROR;
				}
			}
		members object_end
			{
				PHP_JSON_DEPTH_DEC;
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
				parser->scanner.errcode = PHP_JSON_ERROR_STATE_MISMATCH;
				YYERROR;
			}
;

members:
		/* empty */
			{
				parser->methods.object_create(parser, &$$);
			}
	|	member
;

member:
		pair
			{
				parser->methods.object_create(parser, &$$);
				if (parser->methods.object_update(parser, &$$, PHPC_STR_PASS($1.key), &$1.val) == FAILURE)
					YYERROR;
			}
	|	member ',' pair
			{
				if (parser->methods.object_update(parser, &$$, PHPC_STR_PASS($3.key), &$3.val) == FAILURE)
					YYERROR;
				$$ = $1;
			}
	|	member errlex
			{
				PHP_JSON_USE_2($$, $1, $2);
			}
;

pair:
		key ':' value
			{
				PHPC_STR_FROM_ZVAL($$.key, $1);
				$$.val = $3;
			}
	|	key errlex
			{
				PHP_JSON_USE_2($$, $1, $2);
			}
;

array:
		'['
			{
				PHP_JSON_DEPTH_INC;
				if (parser->methods.array_start && FAILURE == parser->methods.array_start(parser)) {
					YYERROR;
				}
			}
		elements array_end
			{
				PHP_JSON_DEPTH_DEC;
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
				parser->scanner.errcode = PHP_JSON_ERROR_STATE_MISMATCH;
				YYERROR;
			}
;

elements:
		/* empty */
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
	|	element errlex
			{
				PHP_JSON_USE_2($$, $1, $2);
			}
;

key:
		PHP_JSON_T_STRING
	|	PHP_JSON_T_ESTRING
;

value:
		object
	|	array
	|	PHP_JSON_T_STRING
	|	PHP_JSON_T_ESTRING
	|	PHP_JSON_T_INT
	|	PHP_JSON_T_DOUBLE
	|	PHP_JSON_T_NUL
	|	PHP_JSON_T_TRUE
	|	PHP_JSON_T_FALSE
	|	errlex
;

errlex:
		PHP_JSON_T_ERROR
			{
				PHP_JSON_USE_1($$, $1);
				YYERROR;
			}
;
	
%% /* Functions */

static int php_json_parser_array_create(php_json_parser *parser, zval *array)
{
	array_init(array);
	return SUCCESS;
}


static int php_json_parser_array_append(php_json_parser *parser, zval *array, zval *zvalue)
{
	zval *data;

	PHPC_PZVAL_MAKE(data);
	PHPC_PZVAL_SET(data, zvalue);

	PHPC_HASH_NEXT_INDEX_INSERT(Z_ARRVAL_P(array), PHPC_PZVAL_CAST_TO_PVAL(data));
	return SUCCESS;
}

static int php_json_parser_object_create(php_json_parser *parser, zval *object)
{
	if (parser->scanner.options & PHP_JSON_OBJECT_AS_ARRAY) {
		array_init(object);
	} else {
		TSRMLS_FETCH_FROM_CTX(parser->zts_ctx);
		object_init(object);
	}
}

static int php_json_parser_object_update(php_json_parser *parser, zval *object, PHPC_STR_ARG(key), zval *zvalue)
{
	zval *data;

	PHPC_PZVAL_MAKE(data);
	PHPC_PZVAL_SET(data, zvalue);

	/* if JSON_OBJECT_AS_ARRAY is set */
	if (Z_TYPE_P(object) == IS_ARRAY) {
		PHPC_SYMTABLE_UPDATE(Z_ARRVAL_P(object), key, data);
	} else {
		zval zkey;
		TSRMLS_FETCH_FROM_CTX(parser->zts_ctx);

		if (PHPC_STR_LEN(key) == 0) {
			PHPC_STR_RELEASE(key);
			PHPC_STR_INIT(key, "_empty_", sizeof("_empty_") - 1);
		} else if (PHPC_STR_VAL(key)[0] == '\0') {
			parser->scanner.errcode = PHP_JSON_ERROR_INVALID_PROPERTY_NAME;
			PHPC_STR_RELEASE(key);
			zval_dtor(zvalue);
			zval_dtor(object);
			PHPC_PZVAL_FREE(data);
			return FAILURE;
		}
		PHPC_ZVAL_NEW_STR(zkey, key);
		PHPC_OBJ_STD_WRITE_PROPERTY(object, &zkey, data);

		if (PHPC_REFCOUNTED_P(data)) {
			Z_DELREF_P(data);
		}
	}
	PHPC_STR_RELEASE(key);

	return SUCCESS;
}

int php_json_yylex(union YYSTYPE *value, php_json_parser *parser)
{
	int token = php_json_scan(&parser->scanner);
	value->value = parser->scanner.value;
	return token;
}

void php_json_yyerror(php_json_parser *parser, char const *msg)
{
	if (!parser->scanner.errcode) {
		parser->scanner.errcode = PHP_JSON_ERROR_SYNTAX;
	}
}

PHP_JSOND_API php_json_error_code PHP_JSOND_NAME(parser_error_code)(const php_json_parser *parser)
{
	return parser->scanner.errcode;
}

static const php_json_parser_methods default_parser_methods =
{
	php_json_parser_array_create,
	php_json_parser_array_append,
	NULL,
	NULL,
	php_json_parser_object_create,
	php_json_parser_object_update,
	NULL,
	NULL,
};

PHP_JSOND_API void PHP_JSOND_NAME(parser_init_ex)(
		php_json_parser *parser, zval *return_value,
		char *str, size_t str_len,
		int options, int max_depth,
		const php_json_parser_methods *parser_methods TSRMLS_DC)
{
	memset(parser, 0, sizeof(php_json_parser));
	php_json_scanner_init(&parser->scanner, str, str_len, options);
	parser->depth = 1;
	parser->max_depth = max_depth;
	parser->return_value = return_value;
	TSRMLS_SET_CTX(parser->zts_ctx);
}

PHP_JSOND_API void PHP_JSOND_NAME(parser_init)(
		php_json_parser *parser, zval *return_value,
		char *str, size_t str_len,
		int options, int max_depth TSRMLS_DC)
{
	PHP_JSOND_NAME(parser_init_ex)(
			parser,
			return_value,
			str,
			str_len,
			options,
			max_depth,
			&default_parser_methods TSRMLS_CC);
}


PHP_JSOND_API int PHP_JSOND_NAME(parse)(php_json_parser *parser)
{
	return php_json_yyparse(parser);
}
