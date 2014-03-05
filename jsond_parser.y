%code top {
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

#include "php.h"
#include "php_jsond.h"
#include "php_jsond_parser.h"

#define YYDEBUG 0

#if YYDEBUG
int json_yydebug = 1;
#endif

#define PHP_JSON_USE(uv) ((void) (uv))
#define PHP_JSON_USE_2(uvr, uv1, uv2) PHP_JSON_USE(uvr); PHP_JSON_USE(uv1); PHP_JSON_USE(uv2)

}

%pure-parser
%name-prefix "php_json_yy"
%lex-param  { php_json_parser *parser  }
%parse-param { php_json_parser *parser }

%union {
	zval value;
	struct {
		zval key;
		zval val;
	} pair;
	HashTable *ht;
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
%type <ht> members member elements element
%type <pair> pair

%destructor { zval_ptr_dtor(&$$); } <value>
%destructor { zend_hash_destroy($$); FREE_HASHTABLE($$); } <ht>
%destructor { zval_ptr_dtor(&$$.key); zval_ptr_dtor(&$$.val); } <pair>

%code {
int php_json_yylex(union YYSTYPE *value, php_json_parser *parser);
void php_json_yyerror(php_json_parser *parser, char const *msg);
void php_json_object_set(zval *zv, HashTable *ht);

#define PHP_JSON_DEPTH_DEC --parser->depth
#define PHP_JSON_DEPTH_INC \
	if (parser->max_depth && parser->depth >= parser->max_depth) { \
		/* PHP_JSON_VALUE_SET_ERROR(parser->result, PHP_JSON_ERROR_DEPTH); */ \
		YYERROR; \
	} \
	++parser->depth
}


%% /* Rules */

start:
		value PHP_JSON_T_EOI    { $$ = $1; parser->result = $1; PHP_JSON_USE($2); YYACCEPT; }
	|	value errlex            { PHP_JSON_USE_2($$, $1, $2); }
;

object:
		'{' { PHP_JSON_DEPTH_INC; } members '}' { PHP_JSON_DEPTH_DEC; php_json_object_set(&$$, $3); }
;

members:
		/* empty */             { ALLOC_HASHTABLE($$); zend_hash_init($$, 0, NULL, ZVAL_PTR_DTOR, 0); }
	|	member
;

member:
		pair                    { 
									zval *data;
									MAKE_STD_ZVAL(data);
									ZVAL_ZVAL(data, &$1.value, 1, 0);
									ALLOC_HASHTABLE($$); 
									zend_hash_init($$, 4, NULL, ZVAL_PTR_DTOR, 0); 
									zend_symtable_update($$, Z_STRVAL($1.key), Z_STRLEN($1.key), &data, sizeof(zval *), NULL); 
								}
	|	member ',' pair         {
									zval *data;
									MAKE_STD_ZVAL(data);
									ZVAL_ZVAL(data, &$3.value, 1, 0);
									zend_symtable_update($1, Z_STRVAL($3.key), Z_STRLEN($3.key), &data, sizeof(zval *), NULL);
									$$ = $1;
								}
	|	member errlex           { PHP_JSON_USE_2($$, $1, $2); }
;

pair:
		key ':' value           { $$.key = $1; $$.val = $3; }
	|	key errlex              { PHP_JSON_USE_2($$, $1, $2); }
;

array:
		'[' { PHP_JSON_DEPTH_INC; } elements ']' { PHP_JSON_DEPTH_DEC; Z_TYPE($$) = IS_ARRAY; Z_ARRVAL($$) = $3; }
;

elements:
		/* empty */             { ALLOC_HASHTABLE($$); zend_hash_init($$, 0, NULL, ZVAL_PTR_DTOR, 0); }
	|	element
;

element:
		value                   {
									zval *data;
									MAKE_STD_ZVAL(data);
									ZVAL_ZVAL(data, &$1, 1, 0);
									ALLOC_HASHTABLE($$); 
									zend_hash_init($$, 4, NULL, ZVAL_PTR_DTOR, 0); 
									zend_hash_next_index_insert($$, &data, sizeof(zval *), NULL); 
								}
	|	element ',' value       {
									zval *data;
									MAKE_STD_ZVAL(data);
									ZVAL_ZVAL(data, &$3, 1, 0);
									zend_hash_next_index_insert($1, &data, sizeof(zval *), NULL);
									$$ = $1;
								}
	|	element errlex          { PHP_JSON_USE_2($$, $1, $2); }
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
		PHP_JSON_T_ERROR             { parser->result = $1; PHP_JSON_USE($$); YYERROR; }
;
	
%% /* Functions */
	
int php_json_yylex(union YYSTYPE *value, php_json_parser *parser)
{
	int token = php_json_scan(&parser->scanner);
	value->value = parser->scanner.value;
	return token;
}

void php_json_yyerror(php_json_parser *parser, char const *msg)
{
	
}

void php_json_object_set(zval *zv, HashTable *ht)
{
	
}