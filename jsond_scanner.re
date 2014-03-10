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

#include "php_jsond_scanner.h"

/* #include "jsond_scanner_defs.h" */

#define	YYCTYPE     char
#define	YYCURSOR    s->cursor
#define	YYLIMIT     s->limit
#define	YYMARKER    s->marker
#define	YYCTXMARKER s->ctxmarker

#define YYGETCONDITION()        s->state
#define YYSETCONDITION(yystate) s->state = yystate

#define	YYFILL(n)

#define PHP_JSON_TOKEN(token)  PHP_JSON_T_##token
#define	PHP_JSON_TOKEN_RETURN(token)  return PHP_JSON_TOKEN(token)
#define PHP_JSON_CONDITION_SET(condition) YYSETCONDITION(yyc##condition)
#define PHP_JSON_CONDITION_GOTO(condition) goto yyc_##condition

#define PHP_JSON_SCANNER_COPY_ESC() php_json_scanner_copy_string(s, 0)
#define PHP_JSON_SCANNER_COPY_UTF() php_json_scanner_copy_string(s, 5)


static void php_json_scanner_copy_string(php_json_scanner *s, int esc_size)
{
	size_t len = s->cursor - s->str_start - esc_size - 1;
	if (len) {
		memcpy(s->pstr, s->str_start, len);
		s->pstr += len;
	}
}

static int php_json_hex_to_int(char c)
{
	if (c >= '0' && c <= '9')
	{
		return c - '0';
	}
	else if (c >= 'A' && c <= 'F')
	{
		return c - ('A' - 10);
	}
	else if (c >= 'a' && c <= 'f')
	{
		return c - ('a' - 10);
	}
	else
	{
		/* this should never happened */
		return -1;
	}
}

static int php_json_ucs2_to_int_ex(php_json_scanner *s, int size, int start)
{
	int i, code = 0;
	char *pc = s->cursor - start;
	for (i = 0; i < size; i++) {
		code |= php_json_hex_to_int(*(pc--)) << (i * 4);
	}
	return code;
}

static int php_json_ucs2_to_int(php_json_scanner *s, int size)
{
	return php_json_ucs2_to_int_ex(s, size, 1);
}

void php_json_scanner_init(php_json_scanner *s, const char *str, int str_len)
{
	s->cursor = str;
	s->limit = str + str_len;
	PHP_JSON_CONDITION_SET(JS);
}

int php_json_scan(php_json_scanner *s)
{

std:
	s->token = s->cursor;

/*!re2c
	re2c:indent:top = 1;
	re2c:yyfill:enable = 0;

	DIGIT   = [0-9] ;
	DIGITNZ = [1-9] ;
	UINT    = "0" | ( DIGITNZ DIGIT* ) ;
	INT     = "-"? UINT ;
	HEX     = DIGIT | [a-fA-F] ;
	HEXNZ   = DIGITNZ | [a-fA-F] ;
	HEX7    = [0-7] ;
	HEXC    = DIGIT | [a-cA-C] ;
	FLOAT   = INT "." UINT ;
	EXP     = ( INT | FLOAT ) [eE] [+-]? UINT ;
	WS      = [ \t]+ ;
	NL      = "\r"? "\n" ;
	EOI     = "\000";
	CTRL    = [\x00-\x1F] ;
	UTF8T   = [\x80-\xBF] ;
	UTF8_1  = [\x00-\x7F] ;
	UTF8_2  = [\xC2-\xDF] UTF8T ;
	UTF8_3A = "\xE0" [\xA0-\xBF] UTF8T ;
	UTF8_3B = [\xE1-\xEC] UTF8T{2} ;
	UTF8_3C = "\xED" [\x80-\x9F] UTF8T ;
	UTF8_3D = [\xEE-\xEF] UTF8T{2} ;
	UTF8_3  = UTF8_3A | UTF8_3B | UTF8_3C | UTF8_3D ;
	UTF8_4A = "\xF0"[\x90-\xBF] UTF8T{2} ;
	UTF8_4B = [\xF1-\xF3] UTF8T{3} ;
	UTF8_4C = "\xF4" [\x80-\x8F] UTF8T{2} ;
	UTF8_4  = UTF8_4A | UTF8_4B | UTF8_4C ;
	UTF8    = UTF8_1 | UTF8_2 | UTF8_3 | UTF8_4 ;
	ANY     = [^] ;
	ESCPREF = "\\" ;
	ESCSYM  = ( "\"" | "\\" | "/" | [bfnrt] ) ;
	ESC     = ESCPREF ESCSYM ;
	UTFSYM  = "u" ;
	UTFPREF = ESCPREF UTFSYM ;
	UCS2    = UTFPREF HEX{4} ;
	UTF16_1 = UTFPREF "00" HEX7 HEX ;
	UTF16_2 = UTFPREF "0" HEX7 HEX{2} ;
	UTF16_3 = UTFPREF ( ( ( HEXC | [efEF] ) HEX ) | ( [dD] HEX7 ) ) HEX{2} ;
	UTF16_4 = UTFPREF [dD] [89abAB] HEX{2} UTFPREF [dD] [c-fC-F] HEX{2} ;


*/

}

