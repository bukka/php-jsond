/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_JSO_YY_JSO_PARSER_TAB_H_INCLUDED
# define YY_JSO_YY_JSO_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef JSO_YYDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define JSO_YYDEBUG 1
#  else
#   define JSO_YYDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define JSO_YYDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined JSO_YYDEBUG */
#if JSO_YYDEBUG
extern int jso_yydebug;
#endif

/* Token kinds.  */
#ifndef JSO_YYTOKENTYPE
# define JSO_YYTOKENTYPE
  enum jso_yytokentype
  {
    JSO_YYEMPTY = -2,
    JSO_YYEOF = 0,                 /* "end of file"  */
    JSO_YYerror = 256,             /* error  */
    JSO_YYUNDEF = 257,             /* "invalid token"  */
    JSO_T_NUL = 258,               /* JSO_T_NUL  */
    JSO_T_TRUE = 259,              /* JSO_T_TRUE  */
    JSO_T_FALSE = 260,             /* JSO_T_FALSE  */
    JSO_T_LONG = 261,              /* JSO_T_LONG  */
    JSO_T_DOUBLE = 262,            /* JSO_T_DOUBLE  */
    JSO_T_STRING = 263,            /* JSO_T_STRING  */
    JSO_T_ESTRING = 264,           /* JSO_T_ESTRING  */
    JSO_T_ENOMEM = 265,            /* JSO_T_ENOMEM  */
    JSO_T_EOI = 266,               /* JSO_T_EOI  */
    JSO_T_ERROR = 267              /* JSO_T_ERROR  */
  };
  typedef enum jso_yytokentype jso_yytoken_kind_t;
#endif
/* Token kinds.  */
#define JSO_YYEMPTY -2
#define JSO_YYEOF 0
#define JSO_YYerror 256
#define JSO_YYUNDEF 257
#define JSO_T_NUL 258
#define JSO_T_TRUE 259
#define JSO_T_FALSE 260
#define JSO_T_LONG 261
#define JSO_T_DOUBLE 262
#define JSO_T_STRING 263
#define JSO_T_ESTRING 264
#define JSO_T_ENOMEM 265
#define JSO_T_EOI 266
#define JSO_T_ERROR 267

/* Value type.  */
#if ! defined JSO_YYSTYPE && ! defined JSO_YYSTYPE_IS_DECLARED
union JSO_YYSTYPE
{

	jso_value value;
	struct {
		jso_value key;
		jso_value val;
	} pair;
	jso_object *object;
	jso_array *array;


};
typedef union JSO_YYSTYPE JSO_YYSTYPE;
# define JSO_YYSTYPE_IS_TRIVIAL 1
# define JSO_YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined JSO_YYLTYPE && ! defined JSO_YYLTYPE_IS_DECLARED
typedef struct JSO_YYLTYPE JSO_YYLTYPE;
struct JSO_YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define JSO_YYLTYPE_IS_DECLARED 1
# define JSO_YYLTYPE_IS_TRIVIAL 1
#endif




int jso_yyparse (jso_parser *parser);


#endif /* !YY_JSO_YY_JSO_PARSER_TAB_H_INCLUDED  */
