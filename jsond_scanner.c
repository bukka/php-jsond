/* Generated by re2c 0.13.6 */
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
#include "php_jsond_scanner.h"
#include "php_jsond_scanner_defs.h"
#include "jsond_parser.tab.h"

#ifndef PHP_WIN32
# include <stddef.h>
#endif

#define	YYCTYPE     php_json_ctype
#define	YYCURSOR    s->cursor
#define	YYLIMIT     s->limit
#define	YYMARKER    s->marker
#define	YYCTXMARKER s->ctxmarker

#define YYGETCONDITION()        s->state
#define YYSETCONDITION(yystate) s->state = yystate

#define	YYFILL(n)

#define PHP_JSON_CONDITION_SET(condition) YYSETCONDITION(yyc##condition)
#define PHP_JSON_CONDITION_GOTO(condition) goto yyc_##condition

#define PHP_JSON_SCANNER_COPY_ESC() php_json_scanner_copy_string(s, 0)
#define PHP_JSON_SCANNER_COPY_UTF() php_json_scanner_copy_string(s, 5)
#define PHP_JSON_SCANNER_COPY_UTF_SP() php_json_scanner_copy_string(s, 11)


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
	php_json_ctype *pc = s->cursor - start;
	for (i = 0; i < size; i++) {
		code |= php_json_hex_to_int(*(pc--)) << (i * 4);
	}
	return code;
}

static int php_json_ucs2_to_int(php_json_scanner *s, int size)
{
	return php_json_ucs2_to_int_ex(s, size, 1);
}

void php_json_scanner_init(php_json_scanner *s, char *str, int str_len, long options)
{
	s->cursor = (php_json_ctype *) str;
	s->limit = (php_json_ctype *) str + str_len;
	s->options = options;
	PHP_JSON_CONDITION_SET(JS);
}

int php_json_scan(php_json_scanner *s)
{
	ZVAL_NULL(&s->value);
	
std:
	s->token = s->cursor;


	{
		YYCTYPE yych;
		unsigned int yyaccept = 0;
		if (YYGETCONDITION() < 1) {
			goto yyc_JS;
		} else {
			if (YYGETCONDITION() < 2) {
				goto yyc_STR_P1;
			} else {
				goto yyc_STR_P2;
			}
		}
/* *********************************** */
yyc_JS:
		{
			static const unsigned char yybm[] = {
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0, 128,   0,   0,   0, 128,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				128,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				 64,  64,  64,  64,  64,  64,  64,  64, 
				 64,  64,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
				  0,   0,   0,   0,   0,   0,   0,   0, 
			};
			yych = *YYCURSOR;
			if (yych <= '9') {
				if (yych <= ' ') {
					if (yych <= '\n') {
						if (yych <= 0x00) goto yy3;
						if (yych <= 0x08) goto yy5;
						if (yych <= '\t') goto yy7;
						goto yy9;
					} else {
						if (yych == '\r') goto yy10;
						if (yych <= 0x1F) goto yy5;
						goto yy7;
					}
				} else {
					if (yych <= ',') {
						if (yych == '"') goto yy11;
						if (yych <= '+') goto yy5;
						goto yy13;
					} else {
						if (yych <= '-') goto yy15;
						if (yych <= '/') goto yy5;
						if (yych <= '0') goto yy16;
						goto yy18;
					}
				}
			} else {
				if (yych <= 'm') {
					if (yych <= '\\') {
						if (yych <= ':') goto yy19;
						if (yych == '[') goto yy21;
						goto yy5;
					} else {
						if (yych <= ']') goto yy23;
						if (yych == 'f') goto yy25;
						goto yy5;
					}
				} else {
					if (yych <= 'z') {
						if (yych <= 'n') goto yy26;
						if (yych == 't') goto yy27;
						goto yy5;
					} else {
						if (yych <= '{') goto yy28;
						if (yych == '}') goto yy30;
						goto yy5;
					}
				}
			}
yy3:
			++YYCURSOR;
			{
		if (s->limit < s->cursor) {
			return PHP_JSON_T_EOI;
		} else {
			s->errcode = PHP_JSON_ERROR_SYNTAX;
			return PHP_JSON_T_ERROR;
		}
	}
yy5:
			++YYCURSOR;
yy6:
			{
		s->errcode = PHP_JSON_ERROR_SYNTAX;
		return PHP_JSON_T_ERROR;
	}
yy7:
			++YYCURSOR;
			yych = *YYCURSOR;
			goto yy58;
yy8:
			{ goto std; }
yy9:
			yych = *++YYCURSOR;
			goto yy8;
yy10:
			yych = *++YYCURSOR;
			if (yych == '\n') goto yy59;
			goto yy58;
yy11:
			++YYCURSOR;
			{
		s->str_start = s->cursor;
		s->str_esc = 0;
		PHP_JSON_CONDITION_SET(STR_P1);
		PHP_JSON_CONDITION_GOTO(STR_P1);
	}
yy13:
			++YYCURSOR;
			{ return ','; }
yy15:
			yych = *++YYCURSOR;
			if (yych <= '/') goto yy6;
			if (yych <= '0') goto yy56;
			if (yych <= '9') goto yy46;
			goto yy6;
yy16:
			yyaccept = 0;
			yych = *(YYMARKER = ++YYCURSOR);
			if (yych <= 'D') {
				if (yych == '.') goto yy48;
			} else {
				if (yych <= 'E') goto yy49;
				if (yych == 'e') goto yy49;
			}
yy17:
			{
		zend_bool bigint = 0, negative = s->token[0] == '-';
		ptrdiff_t digits = s->cursor - s->token - negative;
		ptrdiff_t max_digits = MAX_LENGTH_OF_LONG - 1;
		if (digits >= max_digits) {
			if (digits == max_digits) {
				int cmp = strncmp((char *) (s->token + negative), long_min_digits, max_digits);
				if (!(cmp < 0 || (cmp == 0 && negative))) {
					bigint = 1;
				}
			} else {
				bigint = 1;
			}
		}
		if (!bigint) {
			ZVAL_LONG(&s->value, strtol((char *) s->token, NULL, 10));
			return PHP_JSON_T_INT;
		} else if (s->options & PHP_JSON_BIGINT_AS_STRING) {
			ZVAL_STRINGL(&s->value, (char *) s->token, s->cursor - s->token, 1);
			return PHP_JSON_T_STRING;
		} else {
			ZVAL_DOUBLE(&s->value, zend_strtod((char *) s->token, NULL));
			return PHP_JSON_T_DOUBLE;
		}
	}
yy18:
			yyaccept = 0;
			yych = *(YYMARKER = ++YYCURSOR);
			goto yy47;
yy19:
			++YYCURSOR;
			{ return ':'; }
yy21:
			++YYCURSOR;
			{ return '['; }
yy23:
			++YYCURSOR;
			{ return ']'; }
yy25:
			yyaccept = 1;
			yych = *(YYMARKER = ++YYCURSOR);
			if (yych == 'a') goto yy41;
			goto yy6;
yy26:
			yyaccept = 1;
			yych = *(YYMARKER = ++YYCURSOR);
			if (yych == 'u') goto yy37;
			goto yy6;
yy27:
			yyaccept = 1;
			yych = *(YYMARKER = ++YYCURSOR);
			if (yych == 'r') goto yy32;
			goto yy6;
yy28:
			++YYCURSOR;
			{ return '{'; }
yy30:
			++YYCURSOR;
			{ return '}'; }
yy32:
			yych = *++YYCURSOR;
			if (yych == 'u') goto yy34;
yy33:
			YYCURSOR = YYMARKER;
			if (yyaccept <= 1) {
				if (yyaccept == 0) {
					goto yy17;
				} else {
					goto yy6;
				}
			} else {
				goto yy53;
			}
yy34:
			yych = *++YYCURSOR;
			if (yych != 'e') goto yy33;
			++YYCURSOR;
			{
		ZVAL_TRUE(&s->value);
		return PHP_JSON_T_TRUE;
	}
yy37:
			yych = *++YYCURSOR;
			if (yych != 'l') goto yy33;
			yych = *++YYCURSOR;
			if (yych != 'l') goto yy33;
			++YYCURSOR;
			{
		ZVAL_NULL(&s->value);
		return PHP_JSON_T_NUL;
	}
yy41:
			yych = *++YYCURSOR;
			if (yych != 'l') goto yy33;
			yych = *++YYCURSOR;
			if (yych != 's') goto yy33;
			yych = *++YYCURSOR;
			if (yych != 'e') goto yy33;
			++YYCURSOR;
			{
		ZVAL_FALSE(&s->value);
		return PHP_JSON_T_FALSE;
	}
yy46:
			yyaccept = 0;
			YYMARKER = ++YYCURSOR;
			yych = *YYCURSOR;
yy47:
			if (yybm[0+yych] & 64) {
				goto yy46;
			}
			if (yych <= 'D') {
				if (yych != '.') goto yy17;
			} else {
				if (yych <= 'E') goto yy49;
				if (yych == 'e') goto yy49;
				goto yy17;
			}
yy48:
			yych = *++YYCURSOR;
			if (yych <= '/') goto yy33;
			if (yych <= '9') goto yy54;
			goto yy33;
yy49:
			yych = *++YYCURSOR;
			if (yych <= ',') {
				if (yych != '+') goto yy33;
			} else {
				if (yych <= '-') goto yy50;
				if (yych <= '/') goto yy33;
				if (yych <= '9') goto yy51;
				goto yy33;
			}
yy50:
			yych = *++YYCURSOR;
			if (yych <= '/') goto yy33;
			if (yych >= ':') goto yy33;
yy51:
			++YYCURSOR;
			yych = *YYCURSOR;
			if (yych <= '/') goto yy53;
			if (yych <= '9') goto yy51;
yy53:
			{
		ZVAL_DOUBLE(&s->value, zend_strtod((char *) s->token, NULL));
		return PHP_JSON_T_DOUBLE;
	}
yy54:
			yyaccept = 2;
			YYMARKER = ++YYCURSOR;
			yych = *YYCURSOR;
			if (yych <= 'D') {
				if (yych <= '/') goto yy53;
				if (yych <= '9') goto yy54;
				goto yy53;
			} else {
				if (yych <= 'E') goto yy49;
				if (yych == 'e') goto yy49;
				goto yy53;
			}
yy56:
			yyaccept = 0;
			yych = *(YYMARKER = ++YYCURSOR);
			if (yych <= 'D') {
				if (yych == '.') goto yy48;
				goto yy17;
			} else {
				if (yych <= 'E') goto yy49;
				if (yych == 'e') goto yy49;
				goto yy17;
			}
yy57:
			++YYCURSOR;
			yych = *YYCURSOR;
yy58:
			if (yybm[0+yych] & 128) {
				goto yy57;
			}
			goto yy8;
yy59:
			++YYCURSOR;
			yych = *YYCURSOR;
			goto yy8;
		}
/* *********************************** */
yyc_STR_P1:
		yych = *YYCURSOR;
		if (yych <= 0xDF) {
			if (yych <= '[') {
				if (yych <= 0x1F) goto yy62;
				if (yych == '"') goto yy66;
				goto yy64;
			} else {
				if (yych <= '\\') goto yy68;
				if (yych <= 0x7F) goto yy64;
				if (yych <= 0xC1) goto yy70;
				goto yy72;
			}
		} else {
			if (yych <= 0xEF) {
				if (yych <= 0xE0) goto yy73;
				if (yych <= 0xEC) goto yy74;
				if (yych <= 0xED) goto yy75;
				goto yy76;
			} else {
				if (yych <= 0xF0) goto yy77;
				if (yych <= 0xF3) goto yy78;
				if (yych <= 0xF4) goto yy79;
				goto yy70;
			}
		}
yy62:
		++YYCURSOR;
		{
		s->errcode = PHP_JSON_ERROR_CTRL_CHAR;
		return PHP_JSON_T_ERROR;
	}
yy64:
		++YYCURSOR;
yy65:
		{ PHP_JSON_CONDITION_GOTO(STR_P1); }
yy66:
		++YYCURSOR;
		{
		char *str;
		size_t len = s->cursor - s->str_start - s->str_esc - 1;
		if (len == 0) {
			PHP_JSON_CONDITION_SET(JS);
			ZVAL_EMPTY_STRING(&s->value);
			return PHP_JSON_T_ESTRING;
		}
		str = emalloc(len + 1);
		str[len] = 0;
		ZVAL_STRINGL(&s->value, str, len, 0);
		if (s->str_esc) {
			s->pstr = (php_json_ctype *) Z_STRVAL(s->value);
			s->cursor = s->str_start;
			PHP_JSON_CONDITION_SET(STR_P2);
			PHP_JSON_CONDITION_GOTO(STR_P2);
		} else {
			memcpy(Z_STRVAL(s->value), s->str_start, len);
			PHP_JSON_CONDITION_SET(JS);
			return PHP_JSON_T_STRING;
		}
	}
yy68:
		yyaccept = 0;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych <= 'e') {
			if (yych <= '/') {
				if (yych == '"') goto yy92;
				if (yych >= '/') goto yy92;
			} else {
				if (yych <= '\\') {
					if (yych >= '\\') goto yy92;
				} else {
					if (yych == 'b') goto yy92;
				}
			}
		} else {
			if (yych <= 'q') {
				if (yych <= 'f') goto yy92;
				if (yych == 'n') goto yy92;
			} else {
				if (yych <= 's') {
					if (yych <= 'r') goto yy92;
				} else {
					if (yych <= 't') goto yy92;
					if (yych <= 'u') goto yy94;
				}
			}
		}
yy69:
		{
		s->errcode = PHP_JSON_ERROR_SYNTAX;
		return PHP_JSON_T_ERROR;
	}
yy70:
		++YYCURSOR;
yy71:
		{
		s->errcode = PHP_JSON_ERROR_UTF8;
		return PHP_JSON_T_ERROR;
	}
yy72:
		yych = *++YYCURSOR;
		if (yych <= 0x7F) goto yy71;
		if (yych <= 0xBF) goto yy83;
		goto yy71;
yy73:
		yyaccept = 1;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych <= 0x9F) goto yy71;
		if (yych <= 0xBF) goto yy91;
		goto yy71;
yy74:
		yyaccept = 1;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych <= 0x7F) goto yy71;
		if (yych <= 0xBF) goto yy90;
		goto yy71;
yy75:
		yyaccept = 1;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych <= 0x7F) goto yy71;
		if (yych <= 0x9F) goto yy89;
		goto yy71;
yy76:
		yyaccept = 1;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych <= 0x7F) goto yy71;
		if (yych <= 0xBF) goto yy88;
		goto yy71;
yy77:
		yyaccept = 1;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych <= 0x8F) goto yy71;
		if (yych <= 0xBF) goto yy86;
		goto yy71;
yy78:
		yyaccept = 1;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych <= 0x7F) goto yy71;
		if (yych <= 0xBF) goto yy84;
		goto yy71;
yy79:
		yyaccept = 1;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych <= 0x7F) goto yy71;
		if (yych >= 0x90) goto yy71;
		yych = *++YYCURSOR;
		if (yych <= 0x7F) goto yy81;
		if (yych <= 0xBF) goto yy82;
yy81:
		YYCURSOR = YYMARKER;
		if (yyaccept <= 1) {
			if (yyaccept == 0) {
				goto yy69;
			} else {
				goto yy71;
			}
		} else {
			goto yy103;
		}
yy82:
		yych = *++YYCURSOR;
		if (yych <= 0x7F) goto yy81;
		if (yych >= 0xC0) goto yy81;
yy83:
		yych = *++YYCURSOR;
		goto yy65;
yy84:
		yych = *++YYCURSOR;
		if (yych <= 0x7F) goto yy81;
		if (yych >= 0xC0) goto yy81;
		yych = *++YYCURSOR;
		if (yych <= 0x7F) goto yy81;
		if (yych <= 0xBF) goto yy83;
		goto yy81;
yy86:
		yych = *++YYCURSOR;
		if (yych <= 0x7F) goto yy81;
		if (yych >= 0xC0) goto yy81;
		yych = *++YYCURSOR;
		if (yych <= 0x7F) goto yy81;
		if (yych <= 0xBF) goto yy83;
		goto yy81;
yy88:
		yych = *++YYCURSOR;
		if (yych <= 0x7F) goto yy81;
		if (yych <= 0xBF) goto yy83;
		goto yy81;
yy89:
		yych = *++YYCURSOR;
		if (yych <= 0x7F) goto yy81;
		if (yych <= 0xBF) goto yy83;
		goto yy81;
yy90:
		yych = *++YYCURSOR;
		if (yych <= 0x7F) goto yy81;
		if (yych <= 0xBF) goto yy83;
		goto yy81;
yy91:
		yych = *++YYCURSOR;
		if (yych <= 0x7F) goto yy81;
		if (yych <= 0xBF) goto yy83;
		goto yy81;
yy92:
		++YYCURSOR;
		{
		s->str_esc++;
		PHP_JSON_CONDITION_GOTO(STR_P1);
	}
yy94:
		yych = *++YYCURSOR;
		if (yych <= 'D') {
			if (yych <= '9') {
				if (yych <= '/') goto yy81;
				if (yych >= '1') goto yy96;
			} else {
				if (yych <= '@') goto yy81;
				if (yych <= 'C') goto yy96;
				goto yy97;
			}
		} else {
			if (yych <= 'c') {
				if (yych <= 'F') goto yy96;
				if (yych <= '`') goto yy81;
				goto yy96;
			} else {
				if (yych <= 'd') goto yy97;
				if (yych <= 'f') goto yy96;
				goto yy81;
			}
		}
		yych = *++YYCURSOR;
		if (yych <= '9') {
			if (yych <= '/') goto yy81;
			if (yych <= '0') goto yy116;
			if (yych <= '7') goto yy117;
			goto yy98;
		} else {
			if (yych <= 'F') {
				if (yych <= '@') goto yy81;
				goto yy98;
			} else {
				if (yych <= '`') goto yy81;
				if (yych <= 'f') goto yy98;
				goto yy81;
			}
		}
yy96:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy81;
			if (yych <= '9') goto yy98;
			goto yy81;
		} else {
			if (yych <= 'F') goto yy98;
			if (yych <= '`') goto yy81;
			if (yych <= 'f') goto yy98;
			goto yy81;
		}
yy97:
		yych = *++YYCURSOR;
		if (yych <= 'B') {
			if (yych <= '7') {
				if (yych <= '/') goto yy81;
			} else {
				if (yych <= '9') goto yy99;
				if (yych <= '@') goto yy81;
				goto yy99;
			}
		} else {
			if (yych <= '`') {
				if (yych <= 'F') goto yy100;
				goto yy81;
			} else {
				if (yych <= 'b') goto yy99;
				if (yych <= 'f') goto yy100;
				goto yy81;
			}
		}
yy98:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy81;
			if (yych <= '9') goto yy113;
			goto yy81;
		} else {
			if (yych <= 'F') goto yy113;
			if (yych <= '`') goto yy81;
			if (yych <= 'f') goto yy113;
			goto yy81;
		}
yy99:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy81;
			if (yych <= '9') goto yy104;
			goto yy81;
		} else {
			if (yych <= 'F') goto yy104;
			if (yych <= '`') goto yy81;
			if (yych <= 'f') goto yy104;
			goto yy81;
		}
yy100:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy81;
			if (yych >= ':') goto yy81;
		} else {
			if (yych <= 'F') goto yy101;
			if (yych <= '`') goto yy81;
			if (yych >= 'g') goto yy81;
		}
yy101:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy81;
			if (yych >= ':') goto yy81;
		} else {
			if (yych <= 'F') goto yy102;
			if (yych <= '`') goto yy81;
			if (yych >= 'g') goto yy81;
		}
yy102:
		++YYCURSOR;
yy103:
		{
		s->errcode = PHP_JSON_ERROR_UTF16;
		return PHP_JSON_T_ERROR;
	}
yy104:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy81;
			if (yych >= ':') goto yy81;
		} else {
			if (yych <= 'F') goto yy105;
			if (yych <= '`') goto yy81;
			if (yych >= 'g') goto yy81;
		}
yy105:
		yyaccept = 2;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych != '\\') goto yy103;
		yych = *++YYCURSOR;
		if (yych != 'u') goto yy81;
		yych = *++YYCURSOR;
		if (yych == 'D') goto yy108;
		if (yych != 'd') goto yy81;
yy108:
		yych = *++YYCURSOR;
		if (yych <= 'B') goto yy81;
		if (yych <= 'F') goto yy109;
		if (yych <= 'b') goto yy81;
		if (yych >= 'g') goto yy81;
yy109:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy81;
			if (yych >= ':') goto yy81;
		} else {
			if (yych <= 'F') goto yy110;
			if (yych <= '`') goto yy81;
			if (yych >= 'g') goto yy81;
		}
yy110:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy81;
			if (yych >= ':') goto yy81;
		} else {
			if (yych <= 'F') goto yy111;
			if (yych <= '`') goto yy81;
			if (yych >= 'g') goto yy81;
		}
yy111:
		++YYCURSOR;
		{
		s->str_esc += 8;
		PHP_JSON_CONDITION_GOTO(STR_P1);
	}
yy113:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy81;
			if (yych >= ':') goto yy81;
		} else {
			if (yych <= 'F') goto yy114;
			if (yych <= '`') goto yy81;
			if (yych >= 'g') goto yy81;
		}
yy114:
		++YYCURSOR;
		{
		s->str_esc += 3;
		PHP_JSON_CONDITION_GOTO(STR_P1);
	}
yy116:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy81;
			if (yych <= '7') goto yy121;
			if (yych <= '9') goto yy118;
			goto yy81;
		} else {
			if (yych <= 'F') goto yy118;
			if (yych <= '`') goto yy81;
			if (yych <= 'f') goto yy118;
			goto yy81;
		}
yy117:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy81;
			if (yych >= ':') goto yy81;
		} else {
			if (yych <= 'F') goto yy118;
			if (yych <= '`') goto yy81;
			if (yych >= 'g') goto yy81;
		}
yy118:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy81;
			if (yych >= ':') goto yy81;
		} else {
			if (yych <= 'F') goto yy119;
			if (yych <= '`') goto yy81;
			if (yych >= 'g') goto yy81;
		}
yy119:
		++YYCURSOR;
		{
		s->str_esc += 4;
		PHP_JSON_CONDITION_GOTO(STR_P1);
	}
yy121:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy81;
			if (yych >= ':') goto yy81;
		} else {
			if (yych <= 'F') goto yy122;
			if (yych <= '`') goto yy81;
			if (yych >= 'g') goto yy81;
		}
yy122:
		++YYCURSOR;
		{
		s->str_esc += 5;
		PHP_JSON_CONDITION_GOTO(STR_P1);
	}
/* *********************************** */
yyc_STR_P2:
		yych = *YYCURSOR;
		if (yych == '"') goto yy128;
		if (yych == '\\') goto yy130;
		++YYCURSOR;
		{ PHP_JSON_CONDITION_GOTO(STR_P2); }
yy128:
		++YYCURSOR;
		YYSETCONDITION(yycJS);
		{
		PHP_JSON_SCANNER_COPY_ESC();
		return PHP_JSON_T_STRING;
	}
yy130:
		yyaccept = 0;
		yych = *(YYMARKER = ++YYCURSOR);
		if (yych == 'u') goto yy132;
yy131:
		{
		char esc;
		PHP_JSON_SCANNER_COPY_ESC();
		switch (*s->cursor) {
			case 'b':
				esc = '\b';
				break;
			case 'f':
				esc = '\f';
				break;
			case 'n':
				esc = '\n';
				break;
			case 'r':
				esc = '\r';
				break;
			case 't':
				esc = '\t';
				break;
			case '\\':
			case '/':
			case '"':
				esc = *s->cursor;
				break;
			default:
				s->errcode = PHP_JSON_ERROR_SYNTAX;
				return PHP_JSON_T_ERROR;
		}
		*(s->pstr++) = esc;
		++YYCURSOR;
		s->str_start = s->cursor;
		PHP_JSON_CONDITION_GOTO(STR_P2);
	}
yy132:
		yych = *++YYCURSOR;
		if (yych <= 'D') {
			if (yych <= '9') {
				if (yych <= '/') goto yy133;
				if (yych <= '0') goto yy134;
				goto yy135;
			} else {
				if (yych <= '@') goto yy133;
				if (yych <= 'C') goto yy135;
				goto yy136;
			}
		} else {
			if (yych <= 'c') {
				if (yych <= 'F') goto yy135;
				if (yych >= 'a') goto yy135;
			} else {
				if (yych <= 'd') goto yy136;
				if (yych <= 'f') goto yy135;
			}
		}
yy133:
		YYCURSOR = YYMARKER;
		goto yy131;
yy134:
		yych = *++YYCURSOR;
		if (yych <= '9') {
			if (yych <= '/') goto yy133;
			if (yych <= '0') goto yy151;
			if (yych <= '7') goto yy152;
			goto yy138;
		} else {
			if (yych <= 'F') {
				if (yych <= '@') goto yy133;
				goto yy138;
			} else {
				if (yych <= '`') goto yy133;
				if (yych <= 'f') goto yy138;
				goto yy133;
			}
		}
yy135:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy133;
			if (yych <= '9') goto yy138;
			goto yy133;
		} else {
			if (yych <= 'F') goto yy138;
			if (yych <= '`') goto yy133;
			if (yych <= 'f') goto yy138;
			goto yy133;
		}
yy136:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy133;
			if (yych <= '7') goto yy138;
			if (yych >= ':') goto yy133;
		} else {
			if (yych <= 'B') goto yy137;
			if (yych <= '`') goto yy133;
			if (yych >= 'c') goto yy133;
		}
yy137:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy133;
			if (yych <= '9') goto yy142;
			goto yy133;
		} else {
			if (yych <= 'F') goto yy142;
			if (yych <= '`') goto yy133;
			if (yych <= 'f') goto yy142;
			goto yy133;
		}
yy138:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy133;
			if (yych >= ':') goto yy133;
		} else {
			if (yych <= 'F') goto yy139;
			if (yych <= '`') goto yy133;
			if (yych >= 'g') goto yy133;
		}
yy139:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy133;
			if (yych >= ':') goto yy133;
		} else {
			if (yych <= 'F') goto yy140;
			if (yych <= '`') goto yy133;
			if (yych >= 'g') goto yy133;
		}
yy140:
		++YYCURSOR;
		{
		int utf16 = php_json_ucs2_to_int(s, 4);
		PHP_JSON_SCANNER_COPY_UTF();
		*(s->pstr++) = (char) (0xe0 | (utf16 >> 12));
		*(s->pstr++) = (char) (0x80 | ((utf16 >> 6) & 0x3f));
		*(s->pstr++) = (char) (0x80 | (utf16 & 0x3f));
		s->str_start = s->cursor;
		PHP_JSON_CONDITION_GOTO(STR_P2);
	}
yy142:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy133;
			if (yych >= ':') goto yy133;
		} else {
			if (yych <= 'F') goto yy143;
			if (yych <= '`') goto yy133;
			if (yych >= 'g') goto yy133;
		}
yy143:
		yych = *++YYCURSOR;
		if (yych != '\\') goto yy133;
		yych = *++YYCURSOR;
		if (yych != 'u') goto yy133;
		yych = *++YYCURSOR;
		if (yych == 'D') goto yy146;
		if (yych != 'd') goto yy133;
yy146:
		yych = *++YYCURSOR;
		if (yych <= 'B') goto yy133;
		if (yych <= 'F') goto yy147;
		if (yych <= 'b') goto yy133;
		if (yych >= 'g') goto yy133;
yy147:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy133;
			if (yych >= ':') goto yy133;
		} else {
			if (yych <= 'F') goto yy148;
			if (yych <= '`') goto yy133;
			if (yych >= 'g') goto yy133;
		}
yy148:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy133;
			if (yych >= ':') goto yy133;
		} else {
			if (yych <= 'F') goto yy149;
			if (yych <= '`') goto yy133;
			if (yych >= 'g') goto yy133;
		}
yy149:
		++YYCURSOR;
		{
		int utf32, utf16_hi, utf16_lo;
		utf16_hi = php_json_ucs2_to_int(s, 4);
		utf16_lo = php_json_ucs2_to_int_ex(s, 4, 7);
		utf32 = ((utf16_lo & 0x3FF) << 10) + (utf16_hi & 0x3FF) + 0x10000;
		PHP_JSON_SCANNER_COPY_UTF_SP();
		*(s->pstr++) = (char) (0xf0 | (utf32 >> 18));
		*(s->pstr++) = (char) (0x80 | ((utf32 >> 12) & 0x3f));
		*(s->pstr++) = (char) (0x80 | ((utf32 >> 6) & 0x3f));
		*(s->pstr++) = (char) (0x80 | (utf32 & 0x3f));
		s->str_start = s->cursor;
		PHP_JSON_CONDITION_GOTO(STR_P2);
	}
yy151:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy133;
			if (yych <= '7') goto yy156;
			if (yych <= '9') goto yy153;
			goto yy133;
		} else {
			if (yych <= 'F') goto yy153;
			if (yych <= '`') goto yy133;
			if (yych <= 'f') goto yy153;
			goto yy133;
		}
yy152:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy133;
			if (yych >= ':') goto yy133;
		} else {
			if (yych <= 'F') goto yy153;
			if (yych <= '`') goto yy133;
			if (yych >= 'g') goto yy133;
		}
yy153:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy133;
			if (yych >= ':') goto yy133;
		} else {
			if (yych <= 'F') goto yy154;
			if (yych <= '`') goto yy133;
			if (yych >= 'g') goto yy133;
		}
yy154:
		++YYCURSOR;
		{
		int utf16 = php_json_ucs2_to_int(s, 3);
		PHP_JSON_SCANNER_COPY_UTF();
		*(s->pstr++) = (char) (0xc0 | (utf16 >> 6));
		*(s->pstr++) = (char) (0x80 | (utf16 & 0x3f));
		s->str_start = s->cursor;
		PHP_JSON_CONDITION_GOTO(STR_P2);
	}
yy156:
		yych = *++YYCURSOR;
		if (yych <= '@') {
			if (yych <= '/') goto yy133;
			if (yych >= ':') goto yy133;
		} else {
			if (yych <= 'F') goto yy157;
			if (yych <= '`') goto yy133;
			if (yych >= 'g') goto yy133;
		}
yy157:
		++YYCURSOR;
		{
		int utf16 = php_json_ucs2_to_int(s, 2);
		PHP_JSON_SCANNER_COPY_UTF();
		*(s->pstr++) = (char) utf16;
		s->str_start = s->cursor;
		PHP_JSON_CONDITION_GOTO(STR_P2);
	}
	}


}

