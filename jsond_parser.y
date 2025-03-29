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

}

%define api.prefix {php_jsond_yy}
%define api.pure full
%param { php_jsond_parser *parser }

%union {
	zval value;
	struct {
		zend_string *key;
		zval val;
	} pair;
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

%type <value> start object key value array
%type <value> members member elements element
%type <pair> pair

%destructor { zval_ptr_dtor_nogc(&$$); } <value>
%destructor { PHP_JSOND_RELEASE_STRING($$.key); zval_ptr_dtor_nogc(&$$.val); } <pair>

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
		pair
			{
				parser->methods.object_create(parser, &$$);
				if (parser->methods.object_update(parser, &$$, $1.key, &$1.val) == FAILURE)
					YYERROR;
			}
	|	member ',' pair
			{
				if (parser->methods.object_update(parser, &$$, $3.key, &$3.val) == FAILURE)
					YYERROR;
				$$ = $1;
			}
;

pair:
		key ':' value
			{
				$$.key = Z_STR($1);
				$$.val = $3;
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

static int php_jsond_parser_array_create(php_jsond_parser *parser, zval *array)
{
	array_init(array);
	return SUCCESS;
}


static int php_jsond_parser_array_append(php_jsond_parser *parser, zval *array, zval *zvalue)
{
	zend_hash_next_index_insert(Z_ARRVAL_P(array), zvalue);
	return SUCCESS;
}

static int php_jsond_parser_object_create(php_jsond_parser *parser, zval *object)
{
	if (parser->scanner.options & PHP_JSOND_OBJECT_AS_ARRAY) {
		array_init(object);
	} else {
		object_init(object);
	}
	return SUCCESS;
}

static int php_jsond_parser_object_update(php_jsond_parser *parser, zval *object, zend_string *key, zval *zvalue)
{

	/* if JSON_OBJECT_AS_ARRAY is set */
	if (Z_TYPE_P(object) == IS_ARRAY) {
		zend_symtable_update(Z_ARRVAL_P(object), key, zvalue);
	} else {
		zval zkey;

		if (ZSTR_LEN(key) > 0 && ZSTR_VAL(key)[0] == '\0') {
			parser->scanner.errcode = PHP_JSOND_ERROR_INVALID_PROPERTY_NAME;
			PHP_JSOND_RELEASE_STRING(key);
			zval_ptr_dtor_nogc(zvalue);
			zval_ptr_dtor_nogc(object);
			return FAILURE;
		}
		PHP_JSOND_WRITE_PROPERTY(object, key, zvalue);
		Z_TRY_DELREF_P(zvalue);
	}
	PHP_JSOND_RELEASE_STRING(key);

	return SUCCESS;
}

int php_jsond_yylex(union YYSTYPE *value, php_jsond_parser *parser)
{
	int token = php_jsond_scan(&parser->scanner);
	value->value = parser->scanner.value;
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

static const php_jsond_parser_methods default_parser_methods =
{
	php_jsond_parser_array_create,
	php_jsond_parser_array_append,
	NULL,
	NULL,
	php_jsond_parser_object_create,
	php_jsond_parser_object_update,
	NULL,
	NULL,
};

PHP_JSOND_API void php_jsond_parser_init_ex(
		php_jsond_parser *parser, zval *return_value,
		const char *str, size_t str_len,
		int options, int max_depth,
		const php_jsond_parser_methods *parser_methods)
{
	memset(parser, 0, sizeof(php_jsond_parser));
	php_jsond_scanner_init(&parser->scanner, str, str_len, options);
	parser->depth = 1;
	parser->max_depth = max_depth;
	parser->return_value = return_value;
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
			&default_parser_methods);
}


PHP_JSOND_API int php_jsond_parse(php_jsond_parser *parser)
{
	return php_jsond_yyparse(parser);
}
